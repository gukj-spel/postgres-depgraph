#pragma once
#include <vector>
#include <memory>
#include "imp.h"
#include "mock.h"

// OPTYPE
enum class OPTYPE {
    CONCAT,
    SPLIT,
    ELEMENTWISE,
    RESHAPE,
    UNBIND,
    EXPAND,
    CUSTOMIZED,
    SLICE
};

// 模拟 PruningFunc
class PruningFunc {
public:
    virtual void prune_in_channels(Mock::ONNX_MODULE* module, const std::vector<int>& idxs) = 0;
    virtual void prune_out_channels(Mock::ONNX_MODULE* module, const std::vector<int>& idxs) = 0;
    virtual ~PruningFunc() = default;
    static const std::unordered_map<OPTYPE, std::shared_ptr<PruningFunc>> PrunerBox;
};


// 模拟不同类型的 Pruner
class ConcatPruner : public PruningFunc {
public:
    void prune_in_channels(Mock::ONNX_MODULE* module, const std::vector<int>& idxs) override {
        // 实现拼接层 override的剪枝逻辑
        // 这里需要根据具体的模块类型实现不同的剪枝方式
    }
    void prune_out_channels(Mock::ONNX_MODULE* module, const std::vector<int>& idxs) override {
        // 实现拼接层的剪枝逻辑
        // 这里需要根据具体的模块类型实现不同的剪枝方式
    }
};

class SplitPruner : public PruningFunc {
public:
    void prune_in_channels(Mock::ONNX_MODULE* module, const std::vector<int>& idxs) override {}
    void prune_out_channels(Mock::ONNX_MODULE* module, const std::vector<int>& idxs) override {}
};


inline const std::unordered_map<OPTYPE, std::shared_ptr<PruningFunc>> PruningFunc::PrunerBox = {
    {OPTYPE::CONCAT, std::make_shared<ConcatPruner>()},
    {OPTYPE::SPLIT, std::make_shared<SplitPruner>()},
    // 其他 Pruner 实例...
};


