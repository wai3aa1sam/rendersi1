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

class VulkanEditorApp : public EditorApp
{
protected:
	virtual void VulkanEditorApp::onCreate	(const CreateDesc_Base& cd) override
	{
		auto thisCDesc = sCast<const CreateDesc&>(cd);
		//Base::onCreate(thisCDesc);

		JobSystem::init();
		Renderer::init();
		
		{
			auto cDesc = Renderer::makeCDesc();
			Renderer::instance()->create(cDesc);
		}

		{
			auto cDesc = _mainWin.makeCDesc();
			cDesc.isMainWindow = true;
			_mainWin.create(cDesc);
			_mainWin.setWindowTitle("rds Test Vulkan Editor");
		}

		pushLayer(makeUPtr<VulkanLayer>());
	}

	virtual void willQuit() override
	{
		Renderer::terminate();
		JobSystem::terminate();
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




