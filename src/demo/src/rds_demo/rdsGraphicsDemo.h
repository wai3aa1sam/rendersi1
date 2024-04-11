#pragma once

#include "rds_demo/common/rds_demo_common.h"


namespace rds
{

class DemoEditorLayer;
class GraphicsDemo;
extern UPtr<GraphicsDemo> makeDemo();


#if 0
#pragma mark --- rdsGraphicsDemo-Decl ---
#endif // 0
#if 1

class GraphicsDemo : public NonCopyable
{
	friend class DemoEditorLayer;
public:
	virtual ~GraphicsDemo();

	virtual void onCreate();
	virtual void onCreateScene(Scene* oScene);
	virtual void onPrepareRender(RenderGraph* oRdGraph, DrawData* drawData);
	virtual void onExecuteRender(RenderGraph* oRdGraph, DrawData* drawData);

	virtual void onUiMouseEvent(	UiMouseEvent&		ev);
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev);

public:
	EngineContext& engineContext();

private:
	DemoEditorLayer* _demoLayer = nullptr;
};

#endif

}