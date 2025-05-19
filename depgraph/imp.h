#pragma once
#include <vector>
#include <string>
#include <mock.h>
// 重要性评估器接口
class Importance {
public:
    // @todo: 这里的 ONNX_OP 需要根据实际情况定义, 待实现
    virtual Mock::ONNX_OP compute_importance(const std::vector<Mock::ONNX_OP>& parameters) = 0;
    virtual ~Importance() = default;
};

// L2 范数重要性评估器
class MagnitudeImportance : public Importance {
public:
    MagnitudeImportance(int p = 2, const std::string& group_reduction = "mean")
        {}
    
    Mock::ONNX_OP compute_importance(const std::vector<Mock::ONNX_OP>& parameters) override {
        return Mock::ONNX_OP(); // 这里需要实现具体的计算逻辑
    }
    
private:
};