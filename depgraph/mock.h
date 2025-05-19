
#pragma once
// 模拟 ONNX Module 和 Parameter
#include <unordered_map>
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
    class ONNX_OP{};

/**
 * Mock function
 */
std::shared_ptr<PruningFunc> get_pruner_of_module(const ONNX_MODULE& module);


};//namespace Mock