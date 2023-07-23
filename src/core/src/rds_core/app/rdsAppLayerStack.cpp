#include "rds_core-pch.h"

#include "rdsAppLayerStack.h"
#include "rdsAppLayer.h"


namespace rds
{
#if 0
#pragma mark --- rdsAppLayerStack-Impl ---
#endif // 0
#if 1

AppLayerStack::~AppLayerStack()
{
	clear();
}

void 
AppLayerStack::pushLayer(UPtr<AppLayer> p)
{
	p->create();
    _stack.emplace_back(rds::move(p));
}

void 
AppLayerStack::popLayer()
{
    _stack.back()->destroy();
    _stack.pop_back();
}

void 
AppLayerStack::clear()
{
	for (auto& e : _stack)
	{
		e->destroy();
	}
	_stack.clear();
}

#endif
}