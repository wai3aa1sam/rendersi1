#include "rds_editor_test-pch.h"

#include "rds_render_api_layer.h"
#include "rds_editor.h"

namespace rds 
{


class VulkanLayer : public EditorLayer
{
public:
	VulkanLayer() = default;
	virtual ~VulkanLayer() = default;

protected:

};

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
	~VulkanEditorApp()
	{
	}

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




