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
    AppLayer_Base()             = default;
    virtual ~AppLayer_Base()    = default;

    void create();
    void destroy();
    void update();
    void render();

    void setName(StrView name);

    const String& name() const;

protected:
    virtual void onCreate()     = 0;
    virtual void onDestroy()    = 0;
    virtual void onUpdate()     = 0;
    virtual void onRender()     = 0;

    virtual void onUiMouseEvent(    UiMouseEvent& ev)       {};
    virtual void onUiKeyboardEvent( UiKeyboardEvent& ev)    {};

protected:
    String _name;
};

class AppLayer : public AppLayer_Base
{
public:
    AppLayer()          = default;
    virtual ~AppLayer() = default;

private:

};


#endif

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

inline void 
AppLayer_Base::setName(StrView name)
{
    _name = name;
}

inline const String& 
AppLayer_Base::name() const
{
    return _name;
}

#endif

}
