#include <unordered_map>
#include <iostream>
#include "depgraph/base_pruner.h"
int main(){
// 准备示例输入
    
    // 收集注意力层信息
    std::unique_ptr<Mock::ONNX_MODEL> model;
    std::unordered_map<Mock::ONNX_MODULE*, int> num_heads;
    std::shared_ptr<Importance> imp = std::make_shared<MagnitudeImportance>(2, "mean");
    for (auto& child : model->named_children()) {
        // if (auto attention = dynamic_cast<*>(child.value.get())) {
        //     num_heads[attention->query_projection.get()] = attention->n_heads;
        //     num_heads[attention->key_projection.get()] = attention->n_heads;
        //     num_heads[attention->value_projection.get()] = attention->n_heads;
        // }
    }
    
    // 创建剪枝器
    std::vector<Mock::ONNX_MODULE*> ignored_layers = {
        model->enc_embedding.get(),
        model->dec_embedding.get()
    };
    
    BasePruner pruner(*model, imp, 0.25, num_heads, ignored_layers, nullptr);
    
    // 执行剪枝
    auto groups = pruner.step(true);
    for (auto& group : groups) {
        group.prune();
    }
    
    // 输出剪枝结果
    std::cout << "Model after pruning:" << std::endl;
}