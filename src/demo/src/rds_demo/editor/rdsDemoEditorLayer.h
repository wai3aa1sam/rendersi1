#pragma once

#include "rds_demo/common/rds_demo_common.h"
#include "rds_engine/draw/rdsSceneView.h"

#include "rds_render_api_layer/thread/rdsRenderThread.h"

namespace rds
{

class GraphicsDemo;
class DemoEditorApp;
class DemoEditorMainWindow;

class SceneView;

struct MeshAssets;

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

	MeshAssets&				meshAssets();

protected:
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onRender() override;

	virtual void onUiMouseEvent(    UiMouseEvent& ev)		override;
	virtual void onUiKeyboardEvent( UiKeyboardEvent& ev)	override;

	void drawUI(RenderContext& rdCtx, CRenderableSystem& rdableSys);
	void drawEditorUI(EditorUiDrawRequest& uiDrawReq, RdgTextureHnd texHndPresent);		// should be copied from ImGui

private:
	CRenderableSystem& renderableSystem();

protected:
	void prepare_SingleThreadMode();
	void submitRenderJob(RenderDevice* rdDev);

private:
	void _todoList();

private:
	EngineContext	_egCtx;
	EditorContext	_edtCtx;

	EditorViewportWindow	_edtViewportWnd;
	EditorHierarchyWindow	_edtHierarchyWnd;
	EditorInspectorWindow	_edtInspectorWnd;
	EditorProjectWindow		_edtProjectWnd;
	EditorConsoleWindow		_edtConsoleWnd;

	Scene			_scene;
	SceneView		_sceneView;

	UPtr<GraphicsDemo>	_gfxDemo;
	RdgTextureHnd		_texHndPresent;

	UPtr<MeshAssets> _meshAssets;

	bool _isFullScreen = false;

	RenderThread		_rdThread;
	RenderThreadQueue	_rdThreadQueue;
};

inline EngineContext&		DemoEditorLayer::engineContext()	{ return _egCtx; }

inline CRenderableSystem&	DemoEditorLayer::renderableSystem() { return engineContext().renderableSystem(); }

inline EditorContext&		DemoEditorLayer::editorContext()	{ return _edtCtx; }

inline Scene&				DemoEditorLayer::scene()			{ return _scene; }

inline MeshAssets&			DemoEditorLayer::meshAssets()		{ return *_meshAssets; }


#endif


}