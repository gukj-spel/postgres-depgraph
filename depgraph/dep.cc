#include "dep.h"


void Dependency::operator()(const std::vector<int>& idxs) {
	handler(target->module.get(), idxs);
}

std::string Dependency::str() const {
	return "Trigger on " + source->name() + " => Handler on " + target->name();
}

bool Dependency::is_triggered_by(const std::function<void(Mock::Mock::ONNX_MODULE*, const std::vector<int>&)>& pruning_fn) const {
	return pruning_fn.target_type() == trigger.target_type();
}

bool Dependency::operator==(const Dependency& other) const {
	return source == other.source && trigger.target_type() == other.trigger.target_type()
			&& handler.target_type() == other.handler.target_type() && target == other.target;
}


std::string Node::name() const {
	if (_name.empty()) {
		return "UnknownModule";
	}
	return _name;
}

void Node::add_input(const std::shared_ptr<Node>& node) {
	inputs.push_back(node);
}

void Node::add_output(const std::shared_ptr<Node>& node) {
	outputs.push_back(node);
}

std::string Node::str() const {
	return "<Node: (" + name() + ")>";
}

std::string Node::details() const {
	std::string fmt = "--------------------------------\n";
	fmt += "<Node: (" + name() + ")>\n";
	fmt += "    IN:\n";
	for (const auto& in_node : inputs) {
		fmt += "        " + in_node->str() + "\n";
	}
	fmt += "    OUT:\n";
	for (const auto& out_node : outputs) {
		fmt += "        " + out_node->str() + "\n";
	}
	fmt += "    DEP:\n";
	for (const auto& dep : dependencies) {
		fmt += "        " + dep->str() + "\n";
	}
	fmt += "    enable_index_mapping=true, pruning_dim=-1\n";
	fmt += "--------------------------------\n";
	return fmt;
}
