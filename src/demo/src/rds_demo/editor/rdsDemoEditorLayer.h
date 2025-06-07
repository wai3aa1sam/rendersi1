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

struct TestEngine;

#if 0
#pragma mark --- rdsDemoEditorLayer-Decl ---
#endif // 0
#if 1

class DemoEditorLayer : public EditorLayer
{
public:
	DemoEditorLayer();
	virtual ~DemoEditorLayer();

public:
	void init(UPtr<GraphicsDemo> gfxDemo);

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

	void drawUI(RenderContext* rdCtx);
	void drawEditorUI(EditorUiDrawRequest& uiDrawReq, RdgTextureHnd texHndPresent);		// should be copied from ImGui

private:
	CRenderableSystem& renderableSystem();

private:
	void _logForResumeDevelopMustWatchFirst();
	void _todoList();
	void _testEngineCode();

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

	bool			_isFullScreen = false;
	SPtr<Material>	_mtlScreenQuad;

	UPtr<TestEngine> _testEngine;
};

inline EngineContext&		DemoEditorLayer::engineContext()	{ return _egCtx; }

inline CRenderableSystem&	DemoEditorLayer::renderableSystem() { return engineContext().renderableSystem(); }

inline EditorContext&		DemoEditorLayer::editorContext()	{ return _edtCtx; }

inline Scene&				DemoEditorLayer::scene()			{ return _scene; }

inline MeshAssets&			DemoEditorLayer::meshAssets()		{ return *_meshAssets; }


#endif

#if 0
#pragma mark --- rdsTestEngine-Decl ---
#endif // 0
#if 1

struct TestEngine
{
	SPtr<RenderGpuBuffer>		_testBuffer;
	SPtr<RenderGpuMultiBuffer>	_testMultiBuffer;
	SPtr<Texture2D>				_testTex2D;

	void testRenderResource()
	{
		auto fn =
			[]
			(SPtr<RenderGpuBuffer>& testBuffer, SPtr<RenderGpuMultiBuffer>& testMultiBuffer, SPtr<Texture2D>& testTex2D)
			{
				{
					if (Renderer::renderDevice()->engineFrameCount() % 2 == 0)
					{
						/*
						SPtr<RenderGpuBuffer>		_testBuffer;
						SPtr<RenderGpuMultiBuffer>	_testMultiBuffer;
						SPtr<Texture2D>				_testTex2D;
						*/
						{
							auto cDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
							cDesc.bufSize = 16;
							testBuffer = Renderer::renderDevice()->createRenderGpuBuffer(cDesc);

							Vector<u8, 16> data;
							data.resize(16);

							testBuffer->setDebugName("_testBuffer");
							//testBuffer->uploadToGpu(data);

							testMultiBuffer = Renderer::renderDevice()->createRenderGpuMultiBuffer(cDesc);
							testMultiBuffer->setDebugName("_testMultiBuffer");
							//testMultiBuffer->uploadToGpu(data);
						}

						{
							auto cDesc = Texture2D::makeCDesc(RDS_SRCLOC);
							Vector<u8, 16 * 16 * sizeof(ColorRGBAb)> data;
							data.resize(16 * 16 * sizeof(ColorRGBAb));
							cDesc.create(data.data(), 16, 16, ColorType::RGBAb);
							testTex2D = Renderer::renderDevice()->createTexture2D(cDesc);
							testTex2D->setDebugName("_testTex2D");
							//testTex2D->uploadToGpu(cDesc);
						}
					}
				}
			};

		{
			SPtr<RenderGpuBuffer>		testBuffer;
			SPtr<RenderGpuMultiBuffer>	testMultiBuffer;
			SPtr<Texture2D>				testTex2D;

			fn(testBuffer, testMultiBuffer, testTex2D);
		}
			
			
		{
			fn(_testBuffer, _testMultiBuffer, _testTex2D);
		}
	}
};

#endif

}