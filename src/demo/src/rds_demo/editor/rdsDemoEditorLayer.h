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

	EditorContext&			editorContext();

	Scene&					scene();

protected:
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onRender() override;

	void drawEditorUi(RdgTextureHnd texHndPresent);		// should be copied from ImGui

private:
	CRenderableSystem& renderableSystem();

protected:
	RenderMesh		_fullScreenTriangle;
	SPtr<Shader>	_shaderPresent;
	SPtr<Material>	_mtlPresent;

private:
	EngineContext	_egCtx;;
	EditorContext	_edtCtx;;

	EditorHierarchyWindow _edtHierarchyWnd;
	EditorInspectorWindow _edtInspectorWnd;

	Scene			_scene;
	SceneView		_sceneView;

	UPtr<GraphicsDemo>	_gfxDemo;
	RdgTextureHnd		_texHndPresent;
};

inline EngineContext&		DemoEditorLayer::engineContext()	{ return _egCtx; }

inline CRenderableSystem&	DemoEditorLayer::renderableSystem() { return engineContext().renderableSystem(); }

inline EditorContext&		DemoEditorLayer::editorContext()	{ return _edtCtx; }

inline Scene&				DemoEditorLayer::scene()			{ return _scene; }


#endif

}