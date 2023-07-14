#include "rds_editor_test-pch.h"

#include "rds_render_api_layer.h"
#include "rds_editor.h"

#include <rds_render_api_layer/vertex/rdsVertex.h>
#include <rds_render_api_layer/vertex/rdsVertexLayoutManager.h>
#include <rds_render_api_layer/mesh/rdsEditMesh.h>

namespace rds 
{

void testEditMesh()
{
	{
		using T = Vertex_PosColorUvNTB<2, 2, 2, 2, 2>;
		const auto* p = VertexLayoutManager::instance()->registerLayout<T>();
		EditMeshC<1, 2, 2, 2, 2, 2> em;
		p->stride();

		{
			auto& data = em.positions[0];
			data.emplace_back(1.0, 0.5, 0.6);
			data.emplace_back(6.0, 6.5, 6.6);
		}
		{
			auto& data = em.colors[0];
			data.emplace_back(255, 128, 0, 0);
			data.emplace_back(10, 20, 128, 255);
		}
		{
			auto& data = em.colors[1];
			data.emplace_back(5, 4, 5, 6);
			data.emplace_back(11, 12, 24, 65);
		}
		{
			auto& data = em.uvs[0];
			data.emplace_back(1.0, 0.6);
			data.emplace_back(6.0, 1.6);
		}
		{
			auto& data = em.uvs[1];
			data.emplace_back(1.0, 0.6);
			data.emplace_back(6.0, 1.6);
		}
		{
			auto& data = em.normals[0];
			data.emplace_back(1.0, 0.5, 0.6);
		}
		{
			auto& data = em.normals[1];
			data.emplace_back(-5, -6, -7);
		}
		{
			auto& data = em.tangents[0];
			data.emplace_back(1.0, 0.5, 0.6);
		}
		{
			auto& data = em.tangents[1];
			data.emplace_back(-5, -6, -7);
		}
		{
			auto& data = em.binormals[0];
			data.emplace_back(1.0, 0.5, 0.6);
		}
		{
			auto& data = em.binormals[1];
			data.emplace_back(-5, -6, -7);
		}
		auto vtx = em.makePackedVtxData();
		auto* pData = reinCast<T*>(vtx.data());
		for (size_t i = 0; i < em.positions[0].size(); i++)
		{
			auto& e = pData[i];
			RDS_LOG("i: {} --- pos[0]: {}, colors[0]: {} colors[1]: {}, uvs[0]: {}, uvs[1]: {}, normals[0]: {}, normals[1]: {}, tangents[0]: {}, tangents[1]: {}, binromals[0]: {}, binromals[1]: {}"
				, i, e.position, e.colors[0], e.colors[1], e.uvs[0], e.uvs[1], e.normals[0], e.normals[1], e.tangents[0], e.tangents[1], e.binormals[0], e.binormals[1]);
		}
	}
}

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




