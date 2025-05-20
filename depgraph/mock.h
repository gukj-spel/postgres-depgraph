
#pragma once
// 模拟 ONNX Module 和 Parameter
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
class PruningFunc;
namespace Mock{
/**
 * Mock class
 */
    class ONNX_PARAMETER {};

    class ONNX_MODULE {
    public:
        const std::unordered_map<std::string, std::shared_ptr<ONNX_MODULE>>& named_children() const {
            return _children;
        }
    private:
        std::unordered_map<std::string, std::shared_ptr<ONNX_MODULE>> _children;
        std::unordered_map<std::string, std::shared_ptr<ONNX_PARAMETER>> _parameters;
    };


    class ONNX_MODEL{
    public:
        std::unordered_map<std::string, std::shared_ptr<ONNX_MODULE>> named_children() const {
            return {};
        }
        
        std::unordered_map<std::string, std::shared_ptr<ONNX_PARAMETER>> named_parameters() const {
            return {};
        }
        
        std::shared_ptr<ONNX_MODULE> enc_embedding;
        std::shared_ptr<ONNX_MODULE> dec_embedding;
    };

    //ONNX 算子
    class ONNX_OP{
        public:
        // 默认构造函数
        ONNX_OP() = default;
        // 带参数的构造函数
        ONNX_OP(const std::string& name, const std::vector<float>& weights)
            : name_(name), weights_(weights) {}
        
        // 获取参数数据
        const std::vector<float>& get_weights() const {
            return weights_;
        }
        
        // 获取OP名称
        const std::string& get_name() const {
            return name_;
        }
        
    private:
        std::string name_;         // OP名称
        std::vector<float> weights_; // 参数权重
    };

/**
 * Mock function
 */
std::shared_ptr<PruningFunc> get_pruner_of_module(const ONNX_MODULE& module);


};//namespace Mock