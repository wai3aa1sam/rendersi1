#include "rds_core-pch.h"

#include "rdsAppLayerStack.h"


namespace rds
{
#if 0
#pragma mark --- rdsAppLayerStack-Impl ---
#endif // 0
#if 1

AppLayerStack::~AppLayerStack()
{
    for (auto& e : _stack)
    {
        e->destroy();
    }
}

void AppLayerStack::pushLayer(UPtr<AppLayer> p)
{
    _stack.emplace_back(rds::move(p));
}

void AppLayerStack::popLayer()
{
    _stack.back()->destroy();
    _stack.pop_back();
}

#endif
}