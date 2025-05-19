
#pragma once
#include <vector>
#include <memory>
#include "mock.h"
#include "dep.h"
#include "imp.h"
#include "graph.h"
//base pruner
// 剪枝组类
class PruningGroup {
public:
    PruningGroup() = default;
    
    void add_pruning_action(std::function<void()> action) {
        pruning_actions.push_back(action);
    }
    
    void prune() {
        for (auto& action : pruning_actions) {
            action();
        }
    }
    
private:
    std::vector<std::function<void()>> pruning_actions;
};

// 基础剪枝器
class BasePruner {
public:
    BasePruner(
              Mock::ONNX_MODEL& model, 
              std::shared_ptr<Importance> importance = std::shared_ptr<MagnitudeImportance>(),
              float pruning_ratio = 0.25,
              std::unordered_map<Mock::ONNX_MODULE*, int> num_heads = {},
              const std::vector<Mock::ONNX_MODULE*>& ignored_layers = {},
              std::function<Mock::ONNX_OP(const Mock::ONNX_OP&)> output_transform = nullptr)
        : model(model), 
          importance(importance),
          pruning_ratio(pruning_ratio),
          ignored_layers(ignored_layers),
          output_transform(output_transform) {
        
        initialize();
    }
    
    // 初始化依赖图和剪枝器
    void initialize() {
        // 收集所有可剪枝的模块
        // collect_prunable_modules();
        
        // 计算基准指标
        this->DG = DependencyGraph().build_dependency(
            &this->model,
            ignored_layers,
            true
        );
        
        std::cout << "Base MACs: " << base_macs / 1e6 << " M, Base Params: " << base_params / 1e6 << " M" << std::endl;
    }
    
    // 执行一步剪枝
    std::vector<PruningGroup> step(bool interactive = false) {
        std::vector<PruningGroup> groups;
        /**
         * @todo: 这里的剪枝逻辑需要根据实际情况实现
         */
        
        // // 为每个可剪枝模块创建剪枝组
        // for (auto& module : prunable_modules) {
        //     if (is_ignored(module)) {
        //         continue;
        //     }
            
        //     // 检查是否是注意力层的投影
        //     auto it = num_heads.find(module);
        //     if (it != num_heads.end()) {
        //         // 注意力头剪枝
        //         if (prune_num_heads) {
        //             PruningGroup group = create_head_pruning_group(module, it->second);
        //             groups.push_back(group);
        //         }
        //     } else {
        //         // 普通通道剪枝
        //         PruningGroup group = create_channel_pruning_group(module);
        //         groups.push_back(group);
        //     }
        // }
        
        // // 如果是交互式剪枝，返回剪枝组让用户决定
        // if (interactive) {
        //     return groups;
        // }
        
        // // 否则直接执行剪枝
        // for (auto& group : groups) {
        //     group.prune();
        // }
        
        return groups;
    }
    
    // 创建通道剪枝组
    PruningGroup create_channel_pruning_group(Mock::ONNX_MODULE* module) {
        PruningGroup group;
        /**
         * @todo: 这里的剪枝逻辑需要根据实际情况实现
         */
        return group;
    }
    
    // 创建注意力头剪枝组
    PruningGroup create_head_pruning_group(Mock::ONNX_MODULE* module, int n_heads) {
        PruningGroup group;
        /**
         * @todo: 这里的剪枝逻辑需要根据实际情况实现
         */
        return group;
    }
    
    
    // 检查模块是否被忽略
    bool is_ignored(Mock::ONNX_MODULE* module) {
        for (auto ignored : ignored_layers) {
            if (module == ignored) {
                return true;
            }
        }
        return false;
    }


    
private:
    DependencyGraph DG;
private:
    Mock::ONNX_MODEL& model;
    std::shared_ptr<Importance> importance;
    std::function<Mock::ONNX_OP(const Mock::ONNX_OP&)> output_transform;
    std::vector<Mock::ONNX_MODULE*> ignored_layers;
    int iterative_steps;
    float pruning_ratio;
    bool prune_head_dims;
    bool prune_num_heads;
    float head_pruning_ratio;
    int64_t base_macs;
    int64_t base_params;
};