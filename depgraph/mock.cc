#include "mock.h"
#include "pruner.h"

namespace Mock{
	std::shared_ptr<PruningFunc> get_pruner_of_module(const ONNX_MODULE &module)
	{
		return std::make_shared<PruningFunc>();
	}
}