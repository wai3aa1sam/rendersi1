#include "rds_hello_triangle-pch.h"

#include "rds_hello_triangle.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"

namespace rds
{

void 
HelloTriangle::onCreate()
{
	auto texCDesc = Texture2D::makeCDesc();

	texCDesc.create("asset/texture/uvChecker.png");
	_texUvChecker = Renderer::rdDev()->createTexture2D(texCDesc);
	_texUvChecker->setDebugName("uvChecker");

	{
		auto& shader	= _shaderHelloTriangle;
		auto& mtl		= _mtlHelloTriangle;

		shader	= Renderer::rdDev()->createShader("asset/shader/demo/hello_triangle/hello_triangle.shader");
		mtl		= Renderer::rdDev()->createMaterial();
		mtl->setShader(_shaderHelloTriangle);
		mtl->setParam("texture0", _texUvChecker);
	}
}

void 
HelloTriangle::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);

	auto& scene = *oScene;

	EditMesh mesh;
	WavefrontObjLoader::loadFile(mesh, "asset/mesh/box.obj");
	WavefrontObjLoader::loadFile(mesh, "asset/mesh/suzanne.obj");

	auto n = 25;
	auto row = math::sqrt(sCast<int>(n));
	auto col = row;
	auto stepPos	= Tuple2f{ 3.0f, 3.0f};
	auto startPos	= Tuple2f{-0.0f, 0.0f};
	for (size_t r = 0; r < row; r++)
	{
		for (size_t c = 0; c < col; c++)
		{
			auto* ent = scene.addEntity("");

			auto* rdableMesh = ent->addComponent<CRenderableMesh>();
			rdableMesh->material = _mtlHelloTriangle;
			rdableMesh->meshAsset = makeSPtr<MeshAsset>();
			rdableMesh->meshAsset->rdMesh.create(mesh);

			auto* transform	= ent->getComponent<CTransform>();
			transform->setLocalPosition(startPos.x + stepPos.x * c, 0.0f, startPos.y + stepPos.y * r);

			TempString buf;
			fmtTo(buf, "Entity-{}", sCast<u64>(ent->id()));
			ent->setName(buf);
		}
	}
}

void 
HelloTriangle::onPrepareRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	Base::onPrepareRender(oRdGraph, drawData);

}

void 
HelloTriangle::onExecuteRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	Base::onExecuteRender(oRdGraph, drawData);

	auto*	rdGraph		= oRdGraph;
	auto*	rdCtx		= rdGraph->renderContext();
	auto	screenSize	= Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

	RdgTextureHnd texColor	= rdGraph->createTexture("hello_triangle_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd texDepth	= rdGraph->createTexture("hello_triangle_depth",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

	auto& passHelloTriangle = rdGraph->addPass("hello_triangle", RdgPassTypeFlags::Graphics);
	passHelloTriangle.setRenderTarget(texColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
	passHelloTriangle.setDepthStencil(texDepth,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight
	passHelloTriangle.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			auto mtl = _mtlHelloTriangle;
			rdReq.reset(rdGraph->renderContext(), *drawData);

			auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor(Color4f{ 0.1f, 0.2f, 0.3f, 1.0f });
			clearValue->setClearDepth(1.0f);

			drawData->sceneView->drawScene(rdReq, mtl, drawData);
		}
	);

	drawData->oTexPresent = texColor;
}

void 
HelloTriangle::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	#if 0
	{
		ImGui::Begin("Permutation Test");

		static bool isCheckBox = false;
		static bool isENABLE_feature = false;
		static bool isClearPermutation = false;
		ImGui::Checkbox("ENABLE_permutation", &isCheckBox);
		ImGui::Checkbox("RDS_ENABLE_FEATURE_1", &isENABLE_feature);
		ImGui::Checkbox("isClearPermutation", &isClearPermutation);

		auto& mtl = _mtlHelloTriangle;

		if (isCheckBox)
		{
			if (isENABLE_feature)
				mtl->setPermutation("RDS_ENABLE_FEATURE_1", "1");
			else
				mtl->setPermutation("RDS_ENABLE_FEATURE_2", "0");
		}
		if (isClearPermutation)
		{
			Renderer::rdDev()->waitIdle();
			mtl->clearPermutation();
			isClearPermutation = false;
		}

		ImGui::End();
	}
	#endif // 0
}

void 
HelloTriangle::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);
}

void 
HelloTriangle::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);
}

}