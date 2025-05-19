#pragma once
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <unordered_map>
#include <memory>
#include <unordered_set>
#include "pruner.h"
#include "mock.h"
// 前向声明
class Node;
class Dependency;
class Group;
class DependencyGraph;
class PruningFunc;




// Dependency 类
class Dependency {
public:
    Dependency(const std::function<void(Mock::ONNX_MODULE*, const std::vector<int>&)>& trigger,
               const std::function<void(Mock::ONNX_MODULE*, const std::vector<int>&)>& handler,
               const std::shared_ptr<Node>& source,
               const std::shared_ptr<Node>& target)
        : trigger(trigger), handler(handler), source(source), target(target) {}

    void operator()(const std::vector<int>& idxs);

    std::string str() const;

    bool is_triggered_by(const std::function<void(Mock::ONNX_MODULE*, const std::vector<int>&)>& pruning_fn) const;

    bool operator==(const Dependency& other) const;

    std::function<void(Mock::ONNX_MODULE*, const std::vector<int>&)> trigger;
    std::function<void(Mock::ONNX_MODULE*, const std::vector<int>&)> handler;
    std::shared_ptr<Node> source;
    std::shared_ptr<Node> target;
};

// Node 类
class Node {
public:
    Node(std::shared_ptr<Mock::ONNX_MODULE> module, const std::string& name = "")
        : module(module), _name(name) {}

    std::string name() const;

    void add_input(const std::shared_ptr<Node>& node);

    void add_output(const std::shared_ptr<Node>& node);

    std::string str() const;

    std::string details() const;

    std::vector<std::shared_ptr<Node>> inputs;
    std::vector<std::shared_ptr<Node>> outputs;
    std::shared_ptr<Mock::ONNX_MODULE> module;
    std::string _name;
    std::vector<std::shared_ptr<Dependency>> dependencies;
    bool enable_index_mapping = true;
    int pruning_dim = -1;
};


// GroupItem 类
class GroupItem {
public:
    GroupItem(const std::shared_ptr<Dependency>& dep, const std::vector<int>& idxs)
        : dep(dep), idxs(idxs) {}

    std::shared_ptr<Dependency> dep;
    std::vector<int> idxs;
};

// Group 类
class Group {
public:
    Group() = default;

    void prune(const std::vector<int>* idxs = nullptr, bool record_history = true) {
        if (idxs) {
            // 这里简化处理，没有实现完整的 get_pruning_group 逻辑
            std::cout << "Pruning with user-specified indices" << std::endl;
        } else {
            for (const auto& item : _group) {
                (*item.dep)(item.idxs);
            }
        }
        if (record_history) {
            std::cout << "Recording pruning history" << std::endl;
        }
    }

    void add_dep(const std::shared_ptr<Dependency>& dep, const std::vector<int>& idxs) {
        _group.emplace_back(dep, idxs);
    }

    GroupItem& operator[](size_t k) {
        return _group[k];
    }

    bool has_dep(const std::shared_ptr<Dependency>& dep) const {
        for (const auto& item : _group) {
            if (*item.dep == *dep) {
                return true;
            }
        }
        return false;
    }

    size_t size() const {
        return _group.size();
    }

    void add_and_merge(const std::shared_ptr<Dependency>& dep, const std::vector<int>& idxs) {
        for (auto& item : _group) {
            if (*item.dep == *dep) {
                std::unordered_set<int> visited_idxs;
                std::vector<int> merged_idxs;
                for (int index : item.idxs) {
                    visited_idxs.insert(index);
                    merged_idxs.push_back(index);
                }
                for (int index : idxs) {
                    if (visited_idxs.find(index) == visited_idxs.end()) {
                        merged_idxs.push_back(index);
                        visited_idxs.insert(index);
                    }
                }
                item.idxs = merged_idxs;
                return;
            }
        }
        add_dep(dep, idxs);
    }

    std::string str() const {
        std::string fmt = "\n--------------------------------\n";
        fmt += "          Pruning Group\n";
        fmt += "--------------------------------\n";
        for (size_t i = 0; i < _group.size(); ++i) {
            fmt += "[" + std::to_string(i) + "] " + _group[i].dep->str() + ", len(idxs)=" + std::to_string(_group[i].idxs.size()) + "\n";
        }
        fmt += "--------------------------------\n";
        return fmt;
    }

    std::string details() const {
        std::string fmt = "\n--------------------------------\n";
        fmt += "          Pruning Group\n";
        fmt += "--------------------------------\n";
        for (size_t i = 0; i < _group.size(); ++i) {
            if (i == 0) {
                fmt += "[" + std::to_string(i) + "] " + _group[i].dep->str() + ", idxs (" + std::to_string(_group[i].idxs.size()) + ") = {";
                for (int idx : _group[i].idxs) {
                    fmt += std::to_string(idx) + ", ";
                }
                fmt += "}  (Pruning Root)\n";
            } else {
                fmt += "[" + std::to_string(i) + "] " + _group[i].dep->str() + ", idxs (" + std::to_string(_group[i].idxs.size()) + ") = {";
                for (int idx : _group[i].idxs) {
                    fmt += std::to_string(idx) + ", ";
                }
                fmt += "}\n";
            }
        }
        fmt += "--------------------------------\n";
        return fmt;
    }

    void exec() {
        std::cout << "Group.exec() is deprecated. Use Group.prune() instead." << std::endl;
        prune();
    }

    void operator()() {
        prune();
    }

private:
    std::vector<GroupItem> _group;
    std::shared_ptr<DependencyGraph> _DG;
};
