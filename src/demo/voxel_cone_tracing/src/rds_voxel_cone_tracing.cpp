#include "rds_voxel_cone_tracing-pch.h"
#include "rds_voxel_cone_tracing.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"

namespace rds
{
namespace shaderInterop
{
#include "../../../built-in/shader/geometry/rdsGeometryPrimitive.hlsl"
}
}


namespace rds
{

#if 0
#pragma mark --- rdsVoxelConeTracing-Impl ---
#endif // 0
#if 1

void 
VoxelConeTracing::onCreate()
{
	Base::onCreate();

	createMaterial(&_shaderPostProcess, &_mtlPostProcess, "asset/shader/present.shader");

	_rpfVxgi = makeUPtr<RpfVoxelConeTracing>();
}

void 
VoxelConeTracing::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);

	{
		auto& scene = *oScene;
		auto* ent = scene.addEntity("");

		auto* rdableMesh = ent->addComponent<CRenderableMesh>();
		rdableMesh->material	= _rpfVxgi->mtlVxgi;
		//rdableMesh->meshAsset	= meshAssets().fullScreenTriangle;

		//auto* transform	= ent->getComponent<CTransform>();
		//transform->setLocalPosition(startPos.x + step.x * c, 0.0f, startPos.y + step.y * r);

		TempString buf;
		fmtTo(buf, "Debug Frustum-{}", sCast<u64>(ent->id()));
		ent->setName(buf);
	}

	createDefaultScene(oScene, nullptr, meshAssets().plane, 1);
}

void 
VoxelConeTracing::onPrepareRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	Base::onPrepareRender(oRdGraph, drawData);
}

void 
VoxelConeTracing::onExecuteRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	Base::onExecuteRender(oRdGraph, drawData);

	auto*	rdGraph		= oRdGraph;
	auto	screenSize	= Vec2u::s_cast(Vec2f(drawData->resolution())).toTuple2();

	//RdgBufferHnd bufFrustums = _fwpMakeFrustums->onExecuteRender(oRdGraph, drawData);
	//Renderer::rdDev()->waitIdle();

	RdgTextureHnd rtColor		= rdGraph->createTexture("vxgi_rtColor",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf			= rdGraph->createTexture("vxgi_dsBuf",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferSrc});
	RdgTextureHnd texDepth		= rdGraph->createTexture("vxgi_texDepth",	Texture2D_CreateDesc{ screenSize, ColorType::Rf,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource /*| TextureUsageFlags::TransferDst*/});

	addPreDepthPass(rdGraph, drawData, &dsBuf, &texDepth, Color4f{1.0f, 0.0f, 0.0f, 1.0f});

	_rpfVxgi->mtlVxgi->setParam("texture0", texUvChecker());
	RdgPass* passVxgiLighting = _rpfVxgi->addLightingPass(rdGraph, drawData, rtColor, dsBuf);

	if (passVxgiLighting)
		addSkyboxPass(rdGraph, drawData, skyboxDefault(), rtColor, dsBuf);
	//addPostProcessPass(rdGraph, drawData, "debug_fwdp", rtColor,)

	//_rfpVoxelConeTracing->renderDebugMakeFrustums(rdGraph, drawData, rtColor);
	if (passVxgiLighting)
		addDrawLightOutlinePass(rdGraph, drawData, rtColor, nullptr);	

	drawData->oTexPresent = passVxgiLighting ? rtColor : RdgTextureHnd{};
}

void 
VoxelConeTracing::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{

}

void 
VoxelConeTracing::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);
}

void 
VoxelConeTracing::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);
}
#endif

#if 0
#pragma mark --- rdsRpfVoxelConeTracing-Impl ---
#endif // 0
#if 1

RpfVoxelConeTracing::RpfVoxelConeTracing()
{
	create();
}

RpfVoxelConeTracing::~RpfVoxelConeTracing()
{
	destroy();
}

void 
RpfVoxelConeTracing::create()
{
	GraphicsDemo::createMaterial(&shaderVxgi,	&mtlVxgi,	"asset/shader/demo/voxel_cone_tracing/voxel_cone_tracing.shader");
}

void 
RpfVoxelConeTracing::destroy()
{
	shaderVxgi.reset(	nullptr);
	mtlVxgi.reset(		nullptr);
}

RdgPass* 
RpfVoxelConeTracing::addLightingPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, RdgTextureHnd dsBuf)
{
	auto*		rdGraph			= oRdGraph;
	RdgPass*	lightingPass	= nullptr;
	{
		auto& pass = rdGraph->addPass("vxgi_lighting", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(rtColor, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf,	RdgAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = mtlVxgi;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				drawData->drawScene(rdReq, mtl);
			}
		);
		lightingPass = &pass;
	}
	return lightingPass;
}

bool RpfVoxelConeTracing::isInvalidate(const Vec2f& resoluton_) const { return !math::equals(resolution, resoluton_) && !math::equals0(resoluton_); }

#endif


}