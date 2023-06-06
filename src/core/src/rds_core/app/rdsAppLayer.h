#pragma once

#include "rds_core/common/rds_core_common.h"

namespace rds
{
#if 0
#pragma mark --- rdsAppLayer-Decl ---
#endif // 0
#if 1

class AppLayer_Base : public NonCopyable
{
public:
    virtual ~AppLayer_Base = default;

    void create();
    void destroy();
    void update();
    void render();

    void setName();

    const String& name() const;

protected:
    virtual void onCreate();
    virtual void onDestroy();
    virtual void onUpdate();
    virtual void onRender();

protected:
    String _name;
}

class AppLayer : public AppLayer_Base
{
public:


private:

}


#endif
}

#if 0
#pragma mark --- rdsAppLayer-Impl ---
#endif // 0
#if 1

inline void 
AppLayer_Base::create()
{
    onCreate();
}

inline void
AppLayer_Base::destroy()
{
    onDestroy();
}

inline void
AppLayer_Base::update()
{
    onUpdate();
}

inline void
AppLayer_Base::render()
{
    onRender();
}

#endif