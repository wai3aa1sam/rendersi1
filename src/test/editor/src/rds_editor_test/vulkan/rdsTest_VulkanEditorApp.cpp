#include "rds_editor_test-pch.h"

#include "rds_render_api_layer.h"
#include "rds_editor.h"

#include <rds_render_api_layer/vertex/rdsVertex.h>
#include <rds_render_api_layer/vertex/rdsVertexLayoutManager.h>

namespace rds 
{


class VulkanLayer;
class VulkanEditorMainWindow : public EditorMainWindow
{
public:
	~VulkanEditorMainWindow()
	{

	}

	void destroy()
	{
	}

protected:
	virtual void onCreate(const CreateDesc_Base& cDesc) override
	{
		auto thisCDesc = sCast<const CreateDesc&>(cDesc);
		Base::onCreate(thisCDesc);

		auto* renderer = Renderer::instance();
		auto renderContextCDesc = RenderContext::makeCDesc();
		renderContextCDesc.window = this;
		_renderContext = renderer->createContext(renderContextCDesc);
	}

private:

};

class VulkanEditorApp : public EditorApp
{
public:
	static VulkanEditorApp* instance() { return sCast<VulkanEditorApp*>(s_instance); }

public:
	~VulkanEditorApp()
	{
	}

	VulkanEditorMainWindow* mainWin() { return &_vulkanMainWin; }

protected:

	virtual void VulkanEditorApp::onCreate	(const CreateDesc_Base& cDesc) override
	{
		auto thisCDesc = sCast<const CreateDesc&>(cDesc);
		//Base::onCreate(thisCDesc);

		JobSystem::init();
		Renderer::init();
		
		{
			auto rendererCDesc = Renderer::makeCDesc();
			Renderer::instance()->create(rendererCDesc);
		}

		{ Process sh = { "asset/shader/vulkan/compile_shader.bat" }; }

		{
			auto windowCDesc = _vulkanMainWin.makeCDesc();
			windowCDesc.isMainWindow = true;
			_vulkanMainWin.create(windowCDesc);
			_vulkanMainWin.setWindowTitle("rds Test Vulkan Editor");
		}

		pushLayer(makeUPtr<VulkanLayer>());
	}

	

	virtual void willQuit() override
	{
		_vulkanMainWin.~VulkanEditorMainWindow();
		Renderer::terminate();
		JobSystem::terminate();
	}

protected:
	VulkanEditorMainWindow _vulkanMainWin;
};


class VulkanLayer : public EditorLayer
{
public:
	VulkanLayer() = default;
	virtual ~VulkanLayer() = default;

	virtual void onCreate() override
	{
		{
			#if 1
			auto t = VertexTypeUtil::make(RenderDataType::Float32x3
				, RenderDataType::UNorm8x4, 1
				, RenderDataType::Float32x2, 1
				, RenderDataType::Float32x3, 1, 0, 0
			);
			;
			RDS_DUMP_VAR("{}", (u64)t);
			RDS_DUMP_VAR("{}", (u64)Vertex_PosColorUvNormal<1>::s_kVertexType);

			VertexLayoutManager::instance()->get(t);
			VertexSemantic::NORMAL0;
			VertexSemantic::TEXCOORD0;
			VertexSemantic::TEXCOORD1;
			#endif // 0


			for (size_t i = 0; i < 8; i++)
			{
				RDS_DUMP_VAR(TBM<VertexType>::offsets(i));
			}
		}
	}

	virtual void onRender() override
	{
		RDS_PROFILE_SCOPED();

		auto* mainWin	= VulkanEditorApp::instance()->mainWin();
		auto& rdCtx		= mainWin->renderContext();

		rdCtx.setFramebufferSize(mainWin->clientRect().size);
		rdCtx.beginRender();

		rdCtx.endRender();

	}

protected:

};

class Test_VulkanEditorApp : public UnitTest
{
public:
	void test()
	{
		VulkanEditorApp app;
		auto appCDesc = app.makeCDesc();
		app.run(appCDesc);
	}

	virtual void onSetup() override
	{
	}

	virtual void onTest() override
	{

	}

	virtual void onBenchmark() override
	{

	}

private:
};
RDS_REGISTER_UNIT_TEST_CLASS(Test_VulkanEditorApp);

}

void test_VulkanEditorApp()
{
	RDS_PROFILE_SECTION("Test_VulkanEditorApp");

	using namespace rds;
	RDS_TEST_CASE(Test_VulkanEditorApp, test());
}




