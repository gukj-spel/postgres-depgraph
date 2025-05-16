#pragma once
#include <vector>
#include <functional>
#include <string>
#include <memory>
#include "dep.h"
// DependencyGraph 类
class DependencyGraph {
public:
    DependencyGraph(): REGISTERED_PRUNERS(PruningFunc::PrunerBox.begin(), PruningFunc::PrunerBox.end()) {}

    void pruning_history()  {}
    void load_pruning_history(const std::vector<std::tuple<std::string, bool, std::vector<int>>>& pruning_history) {}

    DependencyGraph& build_dependency(ONNX_MODEL* model,
                                      const std::vector<ONNX_MODULE*>& ignored_layers = {},
                                      bool verbose = true){

        this->model = model;
        this->verbose = verbose;
        //设置 ignored_layers， 为空设置为默认的
        if(ignored_layers.empty()){
            this->IGNORED_LAYERS_IN_TRACING = {model->enc_embedding.get(), model->dec_embedding.get()};
        } else {
            this->IGNORED_LAYERS_IN_TRACING = ignored_layers;
        }


        //设置 _module2node
        this->_module2node = _trace(*this->model);

        //根据 _module2node 构建依赖
        this->_build_dependency();
        return *this;
    }

private:

    /**
     * @brief 根据 _module2node 构建依赖,主要构建层间依赖，对于层内依赖
     */
    bool _build_dependency(){
        for (const auto& pair : _module2node) {
            const std::unique_ptr<Node>& node = pair.second;
            // 处理输入节点的依赖
            for (auto in_node : node->inputs) {
                Pruner* handler_pruner = get_pruner_of_module(in_node->module);
                Pruner* trigger_pruner = get_pruner_of_module(node->module);
                
                // 假设Pruner类有这些方法
                void (*handler)(int) = [handler_pruner](int channels) {
                    handler_pruner->prune_out_channels(channels);
                };
                
                void (*trigger)(int) = [trigger_pruner](int channels) {
                    trigger_pruner->prune_in_channels(channels);
                };
                
                Dependency dep(trigger, handler, node, in_node);
                node->dependencies.push_back(dep);
            }
            
            // 处理输出节点的依赖
            for (Node* out_node : node->outputs) {
                Pruner* trigger_pruner = get_pruner_of_module(node->module);
                Pruner* handler_pruner = get_pruner_of_module(out_node->module);
                
                void (*trigger)(int) = [trigger_pruner](int channels) {
                    trigger_pruner->prune_out_channels(channels);
                };
                
                void (*handler)(int) = [handler_pruner](int channels) {
                    handler_pruner->prune_in_channels(channels);
                };
                
                Dependency dep(trigger, handler, node, out_node);
                node->dependencies.push_back(dep);
            }
        }
    }
    /**
     * @brief 记录, 遍历 ONNX_MODEL 遍历剪枝，设置 _module2name 的值
     */
    std::unordered_map<ONNX_MODULE*, std::unique_ptr<Node>> _trace(const ONNX_MODEL& model){
        std::unordered_map<ONNX_MODULE*, std::unique_ptr<Node>> module2node;
        for (auto& child : model.named_children()) {
            auto module = child.second.get();
            auto name = child.first;
            module2node[module] = std::make_unique<Node>(module, nullptr, name);
            if (module->named_children().size() > 0) {
                auto sub_module2node = trace(*module);
                module2node.insert(sub_module2node.begin(), sub_module2node.end());
            }
        }
        return module2node;
    }

    /**
     * @brief 记录, 遍历 ONNX_MODEL 遍历剪枝，设置 _module2name 的值
     */
    std::unordered_map<ONNX_MODULE*, std::string> _trace(const ONNX_MODULE& module){
        std::unordered_map<ONNX_MODULE*, std::string> module2name;
        for (auto& child : module.named_children()) {
            auto module = child.second.get();
            auto name = child.first;
            module2name[module] = name;
            if (module->named_children().size() > 0) {
                auto sub_module2name = trace(*module);
                module2name.insert(sub_module2name.begin(), sub_module2name.end());
            }
        }
        return module2name;
    }

    


private:
    std::unordered_map<ONNX_MODULE*, std::string> _module2name;
    std::unordered_map<ONNX_MODULE*, std::unique_ptr<Node>> _module2node;
    std::unordered_map<ONNX_PARAMETER*, std::string> _param_to_name;
    std::unordered_map<ONNX_MODULE*, std::shared_ptr<Node>> module2node;
    std::vector<std::tuple<std::string, bool, std::vector<int>>> _pruning_history;
    std::vector<ONNX_MODULE*> IGNORED_LAYERS_IN_TRACING;
    std::vector<ONNX_PARAMETER*> ignored_params;
    std::unordered_map<OPTYPE, std::shared_ptr<PruningFunc>> REGISTERED_PRUNERS;
    std::unordered_map<ONNX_MODULE*, std::shared_ptr<PruningFunc>> CUSTOMIZED_PRUNERS;
    bool verbose;
    ONNX_MODEL* model;
};

