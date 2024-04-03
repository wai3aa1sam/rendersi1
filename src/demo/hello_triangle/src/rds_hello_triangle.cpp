#include "rds_hello_triangle-pch.h"

#include "rds_hello_triangle.h"

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

	auto n = 1;
	for (size_t i = 0; i < n; i++)
	{
		auto* ent = scene.addEntity("");

		auto* rdableMesh = ent->addComponent<CRenderableMesh>();
		rdableMesh->material;
		rdableMesh->meshAsset = makeSPtr<MeshAsset>();
		rdableMesh->meshAsset->rdMesh.create(mesh);

		auto* transform	= ent->getComponent<CTransform>();
		transform->setLocalPosition(0, 0, 0);

		TempString buf;
		fmtTo(buf, "Entity-{}", sCast<u64>(ent->id()));
		ent->setName(buf);
	}
}

void 
HelloTriangle::onPrepareRender(RenderGraph* oRdGraph, RenderData& rdData)
{
	Base::onPrepareRender(oRdGraph, rdData);

}

void 
HelloTriangle::onExecuteRender(RenderGraph* oRdGraph, RenderData& rdData)
{
	Base::onExecuteRender(oRdGraph, rdData);

	auto*	rdGraph		= oRdGraph;
	auto*	rdCtx		= rdGraph->renderContext();
	auto	screenSize	= Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

	//auto& camera = ;

	RdgTextureHnd texColor	= rdGraph->createTexture("hello_triangle_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd texDepth	= rdGraph->createTexture("hello_triangle_depth",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

	auto& passHelloTriangle = rdGraph->addPass("hello_triangle", RdgPassTypeFlags::Graphics);
	passHelloTriangle.setRenderTarget(texColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
	passHelloTriangle.setDepthStencil(texDepth,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight
	passHelloTriangle.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			auto& camera = *rdData.camera;
			rdReq.reset(rdGraph->renderContext(), camera);

			auto mtl = _mtlHelloTriangle;

			//auto clientRect = Rect2f{ Vec2f::s_zero(), rdCtx->framebufferSize() };
			//_rdCmdBuf.setViewport	();
			//_rdCmdBuf.setScissorRect(Rect2f{ Vec2f::s_zero(), _rdCtx->framebufferSize()}); 

			auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor(Color4f{ 0.1f, 0.2f, 0.3f, 1.0f });
			clearValue->setClearDepth(1.0f);

			auto matrix_view   = camera.viewMatrix();
			auto matrix_proj   = camera.projMatrix();
			mtl->setParam("rds_matrix_vp", matrix_proj * matrix_view);

			rdData.sceneView->drawScene(rdReq, mtl);
		}
	);

	rdData.oTexPresent = texColor;
}

}