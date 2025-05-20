#pragma once
#include <vector>
#include <string>
#include <mock.h>
#include <cmath>
#include <numeric>
#include <algorithm>
// 重要性评估器接口
class Importance {
public:
    // 计算参数的重要性分数
    virtual Mock::ONNX_OP compute_importance(const std::vector<Mock::ONNX_OP>& parameters) = 0;
    virtual ~Importance() = default;
};

// L2 范数重要性评估器
class MagnitudeImportance : public Importance {
public:
    MagnitudeImportance(int p = 2, const std::string& group_reduction = "mean")
        : p_(p), group_reduction_(group_reduction) {}
    
    Mock::ONNX_OP compute_importance(const std::vector<Mock::ONNX_OP>& parameters) override {
        // 处理空参数情况
        if (parameters.empty()) {
            return Mock::ONNX_OP();
        }
        
        // 计算每个参数的L2范数
        std::vector<float> l2_norms;
        for (const auto& param : parameters) {
            float l2_norm = calculate_l2_norm(param);
            l2_norms.push_back(l2_norm);
        }
        
        // 根据reduction方法聚合分数
        float final_score = aggregate_scores(l2_norms);
        
        // 创建并返回包含重要性分数的ONNX_OP
        return create_importance_op(final_score);
    }
    
private:
    int p_; // 范数的阶数，默认2表示L2范数
    std::string group_reduction_; // 组内分数聚合方法：mean, sum, max
    
    // 计算单个参数的L2范数
    float calculate_l2_norm(const Mock::ONNX_OP& param) const {
        // 这里需要根据Mock::ONNX_OP的具体实现来访问其数据
        // 假设Mock::ONNX_OP有一个方法get_weights()返回参数数据
        const auto& weights = param.get_weights();
        
        // 计算L2范数：sqrt(sum(x_i^2))
        float sum_squares = 0.0f;
        for (float value : weights) {
            sum_squares += value * value;
        }
        
        return std::sqrt(sum_squares);
    }
    
    // 聚合多个参数的分数
    float aggregate_scores(const std::vector<float>& scores) const {
        if (scores.empty()) return 0.0f;
        
        if (group_reduction_ == "mean") {
            // 计算平均值
            float sum = std::accumulate(scores.begin(), scores.end(), 0.0f);
            return sum / scores.size();
        } 
        else if (group_reduction_ == "sum") {
            // 计算总和
            return std::accumulate(scores.begin(), scores.end(), 0.0f);
        } 
        else if (group_reduction_ == "max") {
            // 计算最大值
            return *std::max_element(scores.begin(), scores.end());
        } 
        else {
            // 默认使用平均值
            float sum = std::accumulate(scores.begin(), scores.end(), 0.0f);
            return sum / scores.size();
        }
    }
    
    // 创建包含重要性分数的ONNX_OP
    Mock::ONNX_OP create_importance_op(float score) const {
        // 这里需要根据Mock::ONNX_OP的具体实现来创建一个表示分数的OP
        // 假设Mock::ONNX_OP有一个构造函数接受名称和分数值
        return Mock::ONNX_OP("ImportanceScore", {score});
    }
};    