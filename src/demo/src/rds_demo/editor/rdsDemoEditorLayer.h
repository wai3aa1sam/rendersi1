#pragma once

#include "rds_demo/common/rds_demo_common.h"


namespace rds
{

class GraphicsDemo;
class DemoEditorApp;
class DemoEditorMainWindow;

#if 0
#pragma mark --- rdsDemoEditorLayer-Decl ---
#endif // 0
#if 1

class DemoEditorLayer : public EditorLayer
{
public:
	DemoEditorLayer(UPtr<GraphicsDemo> gfxDemo);
	virtual ~DemoEditorLayer();

public:
	EngineContext&			engineContext();
	DemoEditorApp&			app();
	DemoEditorMainWindow&	mainWindow();

protected:
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onRender() override;

private:
	CRenderableSystem& renderableSystem();

protected:
	RenderMesh		_fullScreenTriangle;
	SPtr<Shader>	_shaderPresent;
	SPtr<Material>	_mtlPresent;

private:
	EngineContext	_egCtx;;
	Scene			_scene;
	SceneView		_sceneView;

	UPtr<GraphicsDemo>	_gfxDemo;
};

inline EngineContext&		DemoEditorLayer::engineContext()	{ return _egCtx; }

inline CRenderableSystem&	DemoEditorLayer::renderableSystem() { return engineContext().renderableSystem(); }


#endif

}