#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <onnxruntime_cxx_api.h>

using namespace std;
using namespace Ort;

// 节点信息结构体
struct NodeInfo {
	lkfjdklsj
    string type; // 节点类型
    vector<string> predecessors; // 前驱节点名称
    vector<string> successors; // 后继节点名称
};

int test_onnx_main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <model_path>" << endl;
        return 1;
    }

    // 初始化环境
    Env env(ORT_LOGGING_LEVEL_WARNING, "NodeAnalyzer");
    SessionOptions session_options;
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_DISABLE_ALL);

    // 加载模型
    string model_path = argv[1];
    Session session(env, model_path.c_str(), session_options);
    const GraphViewer graph = session.GetModelMetadata().GetGraphViewer();

    // 用于存储节点名称到索引的映射
    unordered_map<string, size_t> node_name_to_index;
    // 存储所有节点信息
    vector<NodeInfo> node_infos;
    // 存储每个输出张量的生产者节点索引
    unordered_map<string, size_t> output_to_producer;

    // 第一步：收集节点信息和输出到生产者的映射
    size_t node_count = graph.GetNodeCount();
    node_infos.resize(node_count);

    for (size_t i = 0; i < node_count; ++i) {
        const Node* node = graph.GetNode(i);
        node_name_to_index[node->Name()] = i;
        
        NodeInfo& info = node_infos[i];
        info.type = node->OpType();
        
        // 记录该节点产生的输出对应的生产者为当前节点
        for (const auto& output : node->OutputDefs()) {
            output_to_producer[output->Name()] = i;
        }
    }

    // 第二步：分析前驱节点关系
    for (size_t i = 0; i < node_count; ++i) {
        const Node* node = graph.GetNode(i);
        NodeInfo& info = node_infos[i];
        
        // 遍历当前节点的所有输入
        for (const auto& input : node->InputDefs()) {
            string input_name = input->Name();
            // 查找该输入的生产者节点
            if (output_to_producer.find(input_name) != output_to_producer.end()) {
                size_t producer_index = output_to_producer[input_name];
                // 避免自循环
                if (producer_index != i) {
                    info.predecessors.push_back(graph.GetNode(producer_index)->Name());
                }
            }
        }
    }

    // 第三步：分析后继节点关系
    for (size_t i = 0; i < node_count; ++i) {
        const Node* node = graph.GetNode(i);
        NodeInfo& info = node_infos[i];
        
        // 遍历当前节点的所有输出
        for (const auto& output : node->OutputDefs()) {
            string output_name = output->Name();
            
            // 查找使用该输出作为输入的所有节点
            for (size_t j = 0; j < node_count; ++j) {
                if (i == j) continue; // 跳过自身
                
                const Node* other_node = graph.GetNode(j);
                for (const auto& other_input : other_node->InputDefs()) {
                    if (other_input->Name() == output_name) {
                        // 检查是否已经添加过该后继节点
                        bool already_added = false;
                        for (const auto& succ : info.successors) {
                            if (succ == other_node->Name()) {
                                already_added = true;
                                break;
                            }
                        }
                        if (!already_added) {
                            info.successors.push_back(other_node->Name());
                        }
                        break;
                    }
                }
            }
        }
    }

    // 输出分析结果
    cout << "ONNX模型计算图节点分析结果:" << endl;
    cout << "======================================" << endl;
    
    for (size_t i = 0; i < node_count; ++i) {
        const Node* node = graph.GetNode(i);
        const NodeInfo& info = node_infos[i];
        
        cout << "节点名称: " << node->Name() << endl;
        cout << "节点类型: " << info.type << endl;
        
        cout << "前驱节点: ";
        if (info.predecessors.empty()) {
            cout << "无";
        } else {
            for (size_t j = 0; j < info.predecessors.size(); ++j) {
                cout << info.predecessors[j];
                if (j < info.predecessors.size() - 1) {
                    cout << ", ";
                }
            }
        }
        cout << endl;
        
        cout << "后继节点: ";
        if (info.successors.empty()) {
            cout << "无";
        } else {
            for (size_t j = 0; j < info.successors.size(); ++j) {
                cout << info.successors[j];
                if (j < info.successors.size() - 1) {
                    cout << ", ";
                }
            }
        }
        cout << endl;
        cout << "--------------------------------------" << endl;
    }

    return 0;
}    
