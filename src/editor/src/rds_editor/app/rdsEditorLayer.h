#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "base/rdsEditorLayer_Base.h"

namespace rds
{
#if 0
#pragma mark --- rdsEditorLayer-Decl ---
#endif // 0
#if 1

class EditorLayer : public EditorLayer_Base
{
public:
	using Base = EditorLayer_Base;

public:
    EditorLayer()           = default;
    virtual ~EditorLayer()  = default;

protected:
    virtual void onCreate();
    virtual void onDestroy();
    virtual void onUpdate();
    virtual void onRender();

private:

};


#endif
}