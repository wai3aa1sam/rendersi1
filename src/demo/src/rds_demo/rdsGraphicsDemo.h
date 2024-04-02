#pragma once

#include "rds_demo/common/rds_demo_common.h"


namespace rds
{

class DemoEditorLayer;
class GraphicsDemo;
extern UPtr<GraphicsDemo> makeDemo();

struct RenderData
{
	SceneView*		sceneView	= nullptr;
	math::Camera3f*	camera		= nullptr;

	RdgTextureHnd oTexPresent;
};

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
	virtual void onPrepareRender(RenderGraph* oRdGraph, RenderData& rdData);
	virtual void onExecuteRender(RenderGraph* oRdGraph, RenderData& rdData);

public:
	EngineContext& engineContext();

private:
	DemoEditorLayer* _demoLayer = nullptr;
};

#endif

}