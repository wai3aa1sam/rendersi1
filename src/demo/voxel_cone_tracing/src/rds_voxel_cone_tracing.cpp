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

static EntityId s_entVctVoxelVisualizationId = 0;

#if 0
#pragma mark --- rdsVoxelConeTracing-Impl ---
#endif // 0
#if 1

void 
VoxelConeTracing::onCreate()
{
	Base::onCreate();

	createMaterial(&_shaderPostProcess, &_mtlPostProcess, "asset/shader/present.shader");

	_rpfVct = makeUPtr<RpfVoxelConeTracing>();
}

void 
VoxelConeTracing::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);
	#if 0
	/*{
	auto& scene_ = scene();
	auto* ent = scene_.addEntity("");

	auto* rdableMesh = ent->addComponent<CRenderableMesh>();
	rdableMesh->material	= nullptr;
	rdableMesh->meshAsset	= meshAssets().plane;

	auto* transform	= ent->getComponent<CTransform>();
	transform->setLocalPosition(0.0f, 0.0f, -10.0f);
	transform->setLocalRotation(Quat4f::s_eulerDegX(90.0f));
	transform->setLocalScale(Vec3f::s_one() * 32.0f);

	TempString buf;
	fmtTo(buf, "Test", sCast<u64>(ent->id()));
	ent->setName(buf);
	}*/

	#endif // 0

	{
		auto& meshes = meshAssets();

		#if 1
		{
			HiResTimer t;
			meshes.loadSponza();
			RDS_LOG("load sponza used: {}s", t.get());
		}
		#endif // 0


		//createDefaultScene(oScene, nullptr, meshes.box, 1);
		createDefaultScene(oScene, nullptr, meshes.suzanne, 4, Tuple2f {0.0f, 5.0f}, Vec2f::s_one() * 10.0f);
		for (auto& ent : scene().entities())
		{
			auto* transform = ent->getComponent<CTransform>();
			transform->setLocalScale(Vec3f{5.0, 5.0, 5.0});
		}

		if (meshes.sponza)
		{
			meshes.sponza->addToScene(oScene, Mat4f::s_scale(Vec3f{0.25f, 0.25f, 0.25f} / 2.0f));
			for (auto& ent : scene().entities())
			{
				auto* transform = ent->getComponent<CTransform>();
				transform->setLocalScale(Vec3f{0.25f, 0.25f, 0.25f} / 2.0f);
			}
		}
	}

	{
		auto* ent = scene().addEntity("VctVoxelVisualization");
		auto* rdableMesh = ent->addComponent<CRenderableMesh>();
		rdableMesh->material	= _rpfVct->mtlVoxelVisualization;;
		//rdableMesh->meshAsset	= meshAsset;
		s_entVctVoxelVisualizationId = ent->id();
	}
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

	RdgTextureHnd rtColor		= rdGraph->createTexture("vct_rtColor",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf			= rdGraph->createTexture("vct_dsBuf",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferSrc});
	RdgTextureHnd texDepth		= rdGraph->createTexture("vct_texDepth",	Texture2D_CreateDesc{ screenSize, ColorType::Rf,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource /*| TextureUsageFlags::TransferDst*/});

	#if 1
	{
		auto& pass = rdGraph->addPass("temp", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf,		RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::DontCare);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = _rpfVct->mtlVct;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();
				//drawData->drawScene(rdReq, mtl);
			}
		);
	}
	#endif // 1

	_rpfVct->mtlVct->setParam("tex2D", texUvChecker());
	_rpfVct->mtlVoxelizationDebug->setParam("tex2D", texUvChecker());
	_rpfVct->mtlVoxelVisualization->setParam("tex2D", texUvChecker());
	_rpfVct->mtlVoxelization->setParam("tex2D", texUvChecker());

	RdgPass* passVctLighting = nullptr;

	addPreDepthPass(rdGraph, drawData, &dsBuf, &texDepth, Color4f{1.0f, 0.0f, 0.0f, 1.0f});
	_rpfVct->addVoxelizationPass(rdGraph, drawData);
	_rpfVct->addAnisotropicMipmappingPass(rdGraph, drawData);

	passVctLighting = _rpfVct->addLightingPass(rdGraph, drawData, rtColor, dsBuf);
	_rpfVct->addVoxelizationDebugPass(rdGraph, drawData, rtColor, dsBuf);

	if (passVctLighting)
	{
		addSkyboxPass(rdGraph, drawData, skyboxDefault(), rtColor, dsBuf);
		//addPostProcessPass(rdGraph, drawData, "debug_fwdp", rtColor,)

		addDrawLightOutlinePass(rdGraph, drawData, rtColor, nullptr);
		//addDisplayNormalPass(rdGraph, drawData, rtColor);
	}
	passVctLighting = _rpfVct->addVoxelVisualizationPass(rdGraph, drawData, rtColor, dsBuf);

	drawData->oTexPresent = passVctLighting ? rtColor : RdgTextureHnd{};
}

void 
VoxelConeTracing::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	auto wnd = uiDrawReq.makeWindow("Debug");
	uiDrawReq.dragFloat("clipmapRegionWorldExtentL0",	&_rpfVct->clipmapRegionWorldExtentL0);

	int curLevel = sCast<int>(_rpfVct->curLevel);
	uiDrawReq.dragInt("curLevel",						&curLevel);
	_rpfVct->curLevel = sCast<u32>(curLevel);
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
	destroy();

	GraphicsDemo::createMaterial(&shaderVct,						&mtlVct,					"asset/shader/demo/voxel_cone_tracing/voxel_cone_tracing.shader");
	GraphicsDemo::createMaterial(&shaderVoxelization,				&mtlVoxelization,			"asset/shader/demo/voxel_cone_tracing/rdsVct_Voxelization.shader");

	GraphicsDemo::createMaterial(&shaderVoxelizationDebug,			&mtlVoxelizationDebug,		"asset/shader/demo/voxel_cone_tracing/rdsVct_Voxelization_Debug.shader");
	GraphicsDemo::createMaterial(&shaderVoxelVisualization,			&mtlVoxelVisualization,		"asset/shader/demo/voxel_cone_tracing/rdsVct_VoxelVisualization.shader");

	GraphicsDemo::createMaterial(&shaderClearImage3D,				&mtlClearImage3D,			"asset/shader/util/rdsClearImage3D.shader");
	
	mtlAnisotropicMipmappings.resize(clipmapMaxLevel);
	for (auto& e : mtlAnisotropicMipmappings)
	{
		GraphicsDemo::createMaterial(&shaderAnisotropicMipmapping,	&e,							"asset/shader/demo/voxel_cone_tracing/rdsVct_AnisotropicMipmapping.shader");
	}
}

void 
RpfVoxelConeTracing::destroy()
{
	shaderVct.reset(	nullptr);
	mtlVct.reset(		nullptr);
}

RdgPass* 
RpfVoxelConeTracing::addVoxelizationPass(RenderGraph* oRdGraph, DrawData* drawData)
{
	auto*		rdGraph				= oRdGraph;
	RdgPass*	voxelizationPass	= nullptr;

	Vec3u clipmapSize3u;
	Vec2u clipmapSize2u;
	clipmapSize3u.setAll(voxelResolution);
	clipmapSize2u = clipmapSize3u.toVec2();
	auto	screenSize	= Vec2u::s_cast(Vec2f(drawData->resolution())).toTuple2();

	// TODO: remove
	RdgTextureHnd rtDummy		= rdGraph->createTexture("vct_dummy",			Texture2D_CreateDesc{ clipmapSize2u, ColorType::Rb,		TextureUsageFlags::RenderTarget});
	RdgTextureHnd voxelTexColor = rdGraph->createTexture("vct_voxelTexColor",	Texture3D_CreateDesc{ clipmapSize3u, ColorType::RGBAb,	TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	result.voxelTexColor		= voxelTexColor;
	//RDS_LOG("------------ addVoxelizationPass {}", result.voxelTexColor.name());

	{
		auto& pass = rdGraph->addPass("vct_clearImage3D", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Transfer);
		pass.writeTexture(voxelTexColor, TextureUsageFlags::TransferDst);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = mtlClearImage3D;

				auto	imageExtent		= voxelTexColor.texture3D()->size();
				Vec3u	threadGroup		= (Vec3u::s_one() * 8);
				Vec3f   dispatchGroup	= Vec3f::s_cast(imageExtent) / Vec3f::s_cast(threadGroup);

				mtl->setParam("image_extent",			imageExtent);
				mtl->setParam("image_extent_offset",	Vec3u::s_zero());
				mtl->setParam("clear_value",			Vec4f::s_zero());
				mtl->setImage("image",					voxelTexColor.texture3D(), 0);
				rdReq.dispatch(RDS_SRCLOC, mtl, Vec3u::s_cast(Vec3f{math::ceil(dispatchGroup.x), math::ceil(dispatchGroup.y), math::ceil(dispatchGroup.z)}));
			}
		);
	}

	{
		auto& pass = rdGraph->addPass("vct_voxelization", RdgPassTypeFlags::Graphics/*, RdgPassFlags::NoRenderTarget*/);
		pass.writeTexture(voxelTexColor, TextureUsageFlags::UnorderedAccess, ShaderStageFlag::Pixel);
		pass.setRenderTarget(rtDummy, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = mtlVoxelization;
				rdReq.reset(rdGraph->renderContext(), *drawData);
				rdReq.setViewport(		Tuple2f::s_zero(), Vec2f::s_cast(clipmapSize2u));
				rdReq.setScissorRect(	Tuple2f::s_zero(), Vec2f::s_cast(clipmapSize2u));

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				#if 0

				{
					auto	center		= drawData->camera->pos();
					auto	size		= Vec3f{ clipmapRegionWorldExtentL0, clipmapRegionWorldExtentL0, clipmapRegionWorldExtentL0 };
					//float	halfSize	= clipRegionWorldExtentL0 * math::pow2(0.0f);

					//auto projX = Mat4f::s_ortho(0.0f, 

					auto voxelMatViewX = Mat4f::s_lookAt(center, center + Vec3f{1.0f, 0.0f, 0.0f}, Vec3f{0.0f, 1.0f,  0.0f});
					auto voxelMatViewY = Mat4f::s_lookAt(center, center + Vec3f{0.0f, 1.0f, 0.0f}, Vec3f{0.0f, 0.0f, -1.0f});
					auto voxelMatViewZ = Mat4f::s_lookAt(center, center + Vec3f{0.0f, 0.0f, 1.0f}, Vec3f{0.0f, 1.0f,  0.0f});

					auto voxelMatProjX = Mat4f::s_ortho(0.0f, size.z, 0.0f, size.y, 0.0f, size.x);
					auto voxelMatProjY = Mat4f::s_ortho(0.0f, size.x, 0.0f, size.z, 0.0f, size.y);
					auto voxelMatProjZ = Mat4f::s_ortho(0.0f, size.x, 0.0f, size.y, 0.0f, size.x);

					mtl->setParam("voxel_matrix_vp_x", voxelMatProjX * voxelMatViewX);
					mtl->setParam("voxel_matrix_vp_y", voxelMatProjY * voxelMatViewY);
					mtl->setParam("voxel_matrix_vp_z", voxelMatProjZ * voxelMatViewZ);
				}

				{
					int iLevel = 0;
					float dimensions	= 800;
					float offset		= dimensions * math::pow2(iLevel) / 2.0f;

					auto	center	= drawData->camera->pos();
					float	m		= math::pow2(iLevel) * sCast<float>(dimensions) / voxelResolution;

					center -= Vec3f{glm::mod(center.x, m), glm::mod(center.x, m), glm::mod(center.x, m)};

					auto voxelProj = Mat4f::s_ortho(center.x - offset, center.x + offset, center.y - offset, center.y + offset, -center.z + offset, -center.z - offset);
					mtl->setParam("voxel_matrix_proj", voxelProj);
				}

				{
					mtl->setParam("rds_matrix_vp", Mat4f::s_perspective(45.0f, 1.0f, 0.1f, 1000.0f) * Mat4f::s_lookAt(drawData->camera->pos(), Vec3f::s_zero(), Vec3f::s_up()));
				}
				#endif // 0

				//RDS_LOG("setParam {} - idx: {}", voxelTexColor.name(), voxelTexColor.texture3D()->bindlessHandle().getResourceIndex());
				
				setCommonParam(mtl, curLevel, drawData);

				mtl->setImage("voxel_tex_albedo",	voxelTexColor.texture3D(), 0);
				//mtl->setImage("voxel_tex_albedo",	voxelTexColor.texture3D(), 1);

				drawData->drawScene(rdReq, mtl);
			}
		);
		voxelizationPass = &pass;
	}
	return voxelizationPass;
}

RdgPass* 
RpfVoxelConeTracing::addLightingPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, RdgTextureHnd dsBuf)
{
	auto*		rdGraph			= oRdGraph;
	RdgPass*	lightingPass	= nullptr;

	auto	screenSize	= Vec2u::s_cast(Vec2f(drawData->resolution())).toTuple2();

	Tuple3u clipmapSizes;
	clipmapSizes.setAll(voxelResolution);
	clipmapSizes.set(screenSize.x, screenSize.y, 4);

	RdgTextureHnd tex2D_color	= rdGraph->createTexture("vct_tex2D_color",			Texture2D_CreateDesc{ screenSize,		ColorType::RGBAb, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource});

	{
		auto& pass = rdGraph->addPass("vct_lighting", RdgPassTypeFlags::Graphics);
		pass.writeTexture(tex2D_color,		TextureUsageFlags::UnorderedAccess, ShaderStageFlag::Pixel);

		pass.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf,		RdgAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		//pass.setDepthStencil(dsBuf,		RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::DontCare);

		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = mtlVct;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				mtl->setParam("tex2D_albedo",		tex2D_color.texture2D());
				drawData->drawScene(rdReq, mtl);
			}
		);
		lightingPass = &pass;
	}
	return lightingPass;
}

RdgPass* 
RpfVoxelConeTracing::addVoxelizationDebugPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, RdgTextureHnd dsBuf)
{
	auto*		rdGraph					= oRdGraph;
	RdgPass*	passVoxelizationDebug	= nullptr;

	auto	screenSize	= Vec2u::s_cast(Vec2f(drawData->resolution())).toTuple2();

	Tuple3u clipmapSizes;
	clipmapSizes.setAll(voxelResolution);
	clipmapSizes.set(screenSize.x, screenSize.y, 4);

	RdgTextureHnd voxelTexColor			= result.voxelTexColor;		// must copy here

	RdgTextureHnd voxelTexColorMip_PosX = result.voxelTexColorMip_PosX;
	RdgTextureHnd voxelTexColorMip_NegX = result.voxelTexColorMip_NegX;
	RdgTextureHnd voxelTexColorMip_PosY = result.voxelTexColorMip_PosY;
	RdgTextureHnd voxelTexColorMip_NegY = result.voxelTexColorMip_NegY;
	RdgTextureHnd voxelTexColorMip_PosZ = result.voxelTexColorMip_PosZ;
	RdgTextureHnd voxelTexColorMip_NegZ = result.voxelTexColorMip_NegZ;

	// TODO: remove
	//RdgTextureHnd tex2D_color		= rdGraph->createTexture("vct_tex2D_color",			Texture2D_CreateDesc{ screenSize,		ColorType::RGBAb, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource});
	//RDS_LOG("addVoxelizationDebugPass {}", voxelTexColor.name());

	{
		auto& pass = rdGraph->addPass("vct_voxelization_debug", RdgPassTypeFlags::Graphics);
		pass.readTexture(voxelTexColor,			TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		pass.readTexture(voxelTexColorMip_PosX,	TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		pass.readTexture(voxelTexColorMip_NegX,	TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		pass.readTexture(voxelTexColorMip_PosY,	TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		pass.readTexture(voxelTexColorMip_NegY,	TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		pass.readTexture(voxelTexColorMip_PosZ,	TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		pass.readTexture(voxelTexColorMip_NegZ,	TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);

		//pass.writeTexture(voxelTexColor,	TextureUsageFlags::UnorderedAccess, ShaderStageFlag::Pixel);
		//pass.writeTexture(tex2D_color,		TextureUsageFlags::UnorderedAccess, ShaderStageFlag::Pixel);

		pass.setRenderTarget(rtColor,	RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf,		RdgAccess::Write, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = mtlVoxelizationDebug;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				//RDS_LOG("setParam {} - idx: {}", voxelTexColor.name(), voxelTexColor.texture3D()->bindlessHandle().getResourceIndex());

				setCommonParam(mtl, curLevel, drawData);

				mtl->setParam("matrix_world",		Mat4f::s_TRS(Vec3f{0.0f, 0.0f, -10.0f}, Quat4f::s_eulerDegX(90.0f), Vec3f::s_one() * 32.0f));

				mtl->setParam("voxel_extent",		1.0f);
				mtl->setParam("voxel_resolution",	voxelResolution);
				mtl->setParam("voxel_tex_albedo",	voxelTexColor.texture3D());
				mtl->setParam("voxel_tex_pos_x",	voxelTexColorMip_PosX.texture3D());
				mtl->setParam("voxel_tex_neg_x",	voxelTexColorMip_NegX.texture3D());
				mtl->setParam("voxel_tex_pos_y",	voxelTexColorMip_PosY.texture3D());
				mtl->setParam("voxel_tex_neg_y",	voxelTexColorMip_NegY.texture3D());
				mtl->setParam("voxel_tex_pos_z",	voxelTexColorMip_PosZ.texture3D());
				mtl->setParam("voxel_tex_neg_z",	voxelTexColorMip_NegZ.texture3D());

				//mtl->setParam("tex2D_albedo",		tex2D_color.texture2D());
				//RDS_LOG("voxel_tex_albedo: {}", *mtl->passes()[0]->shaderResources().constBufs()[0].findParamT<u32>("voxel_tex_albedo"));
				//glm::unpackUnorm4x8()
				drawData->setupMaterial(mtl);
				rdReq.drawMesh(RDS_SRCLOC, drawData->meshAssets->plane->renderMesh, mtl);
				//drawData->drawScene(rdReq, mtl);
			}
		);
		passVoxelizationDebug = &pass;
	}
	return passVoxelizationDebug;
}

RdgPass* 
RpfVoxelConeTracing::addVoxelVisualizationPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, RdgTextureHnd dsBuf)
{
	auto*		rdGraph					= oRdGraph;
	RdgPass*	passVoxelVisualization	= nullptr;

	auto	screenSize	= Vec2u::s_cast(Vec2f(drawData->resolution())).toTuple2();

	Tuple3u clipmapSizes;
	clipmapSizes.setAll(voxelResolution);
	clipmapSizes.set(screenSize.x, screenSize.y, 4);

	RdgTextureHnd voxelTexColor			= result.voxelTexColor;		// must copy here

	RdgTextureHnd voxelTexColorMip_PosX = result.voxelTexColorMip_PosX;
	RdgTextureHnd voxelTexColorMip_NegX = result.voxelTexColorMip_NegX;
	RdgTextureHnd voxelTexColorMip_PosY = result.voxelTexColorMip_PosY;
	RdgTextureHnd voxelTexColorMip_NegY = result.voxelTexColorMip_NegY;
	RdgTextureHnd voxelTexColorMip_PosZ = result.voxelTexColorMip_PosZ;
	RdgTextureHnd voxelTexColorMip_NegZ = result.voxelTexColorMip_NegZ;

	{
		auto& pass = rdGraph->addPass("vct_voxel_visualization", RdgPassTypeFlags::Graphics);
		pass.readTexture(voxelTexColor,			TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		pass.readTexture(voxelTexColorMip_PosX,	TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		pass.readTexture(voxelTexColorMip_NegX,	TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		pass.readTexture(voxelTexColorMip_PosY,	TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		pass.readTexture(voxelTexColorMip_NegY,	TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		pass.readTexture(voxelTexColorMip_PosZ,	TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		pass.readTexture(voxelTexColorMip_NegZ,	TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);

		pass.setRenderTarget(rtColor,	RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		//pass.setDepthStencil(dsBuf,		RdgAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setDepthStencil(dsBuf,		RdgAccess::Write, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = mtlVoxelVisualization;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				setCommonParam(mtl, curLevel, drawData);

				mtl->setParam("voxel_tex3D",		voxelTexColor.texture3D());
				mtl->setParam("voxel_tex_pos_x",	voxelTexColorMip_PosX.texture3D());
				mtl->setParam("voxel_tex_neg_x",	voxelTexColorMip_NegX.texture3D());
				mtl->setParam("voxel_tex_pos_y",	voxelTexColorMip_PosY.texture3D());
				mtl->setParam("voxel_tex_neg_y",	voxelTexColorMip_NegY.texture3D());
				mtl->setParam("voxel_tex_pos_z",	voxelTexColorMip_PosZ.texture3D());
				mtl->setParam("voxel_tex_neg_z",	voxelTexColorMip_NegZ.texture3D());

				drawData->setupMaterial(mtl);

				auto drawCall = rdReq.addDrawCall(sizeof(PerObjectParam));
				drawCall->setDebugSrcLoc(RDS_SRCLOC);
				drawCall->vertexCount = voxelResolution * voxelResolution * voxelResolution;
				drawCall->setMaterial(mtl);

				PerObjectParam objParam;
				objParam.id = sCast<decltype(PerObjectParam::id)>(s_entVctVoxelVisualizationId);
				drawCall->setExtraData(objParam);
			}
		);
		passVoxelVisualization = &pass;
	}

	return passVoxelVisualization;
}

RdgPass* 
RpfVoxelConeTracing::addAnisotropicMipmappingPass(RenderGraph* oRdGraph, DrawData* drawData)
{
	auto*		rdGraph						= oRdGraph;
	RdgPass*	passAnisotropicMipmapping	= nullptr;

	u32		mipDimension	= math::max<u32>(voxelResolution >> 1, 1);
	Vec3u	mipDimensions	= Tuple3u{ mipDimension, mipDimension, mipDimension };

	RdgTextureHnd voxelTexColorMip_PosX = rdGraph->createTexture("vct_voxelTexColorMip_PosX", Texture3D_CreateDesc{ mipDimensions, ColorType::RGBAb, clipmapMaxLevel, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	RdgTextureHnd voxelTexColorMip_NegX = rdGraph->createTexture("vct_voxelTexColorMip_NegX", Texture3D_CreateDesc{ mipDimensions, ColorType::RGBAb, clipmapMaxLevel, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	RdgTextureHnd voxelTexColorMip_PosY = rdGraph->createTexture("vct_voxelTexColorMip_PosY", Texture3D_CreateDesc{ mipDimensions, ColorType::RGBAb, clipmapMaxLevel, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	RdgTextureHnd voxelTexColorMip_NegY = rdGraph->createTexture("vct_voxelTexColorMip_NegY", Texture3D_CreateDesc{ mipDimensions, ColorType::RGBAb, clipmapMaxLevel, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	RdgTextureHnd voxelTexColorMip_PosZ = rdGraph->createTexture("vct_voxelTexColorMip_PosZ", Texture3D_CreateDesc{ mipDimensions, ColorType::RGBAb, clipmapMaxLevel, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	RdgTextureHnd voxelTexColorMip_NegZ = rdGraph->createTexture("vct_voxelTexColorMip_NegZ", Texture3D_CreateDesc{ mipDimensions, ColorType::RGBAb, clipmapMaxLevel, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});

	RdgTextureHnd voxelTexColor		= result.voxelTexColor;		// must copy here

	result.voxelTexColorMip_PosX = voxelTexColorMip_PosX;
	result.voxelTexColorMip_NegX = voxelTexColorMip_NegX;
	result.voxelTexColorMip_PosY = voxelTexColorMip_PosY;
	result.voxelTexColorMip_NegY = voxelTexColorMip_NegY;
	result.voxelTexColorMip_PosZ = voxelTexColorMip_PosZ;
	result.voxelTexColorMip_NegZ = voxelTexColorMip_NegZ;

	{
		TempString passName;
		RdgPass* lastPass = nullptr;

		{
			auto& pass = rdGraph->addPass("vct_anisotropic_pre_mipmapping", RdgPassTypeFlags::Compute);
			pass.readTexture(voxelTexColor);
			pass.writeTexture(voxelTexColorMip_PosX);
			pass.writeTexture(voxelTexColorMip_NegX);
			pass.writeTexture(voxelTexColorMip_PosY);
			pass.writeTexture(voxelTexColorMip_NegY);
			pass.writeTexture(voxelTexColorMip_PosZ);
			pass.writeTexture(voxelTexColorMip_NegZ);
			pass.setExecuteFunc(
				[=](RenderRequest& rdReq)
				{
					auto mtl = mtlAnisotropicMipmappings[0];
					rdReq.reset(rdGraph->renderContext(), *drawData);

					u32 mipLevel = 0;
					mtl->setParam("mip_dimemsions",		mipDimensions);
					mtl->setParam("mip_level",			mipLevel);

					mtl->setParam("src_image",			voxelTexColor.texture3D());

					//RDS_LOG("------------------");
					mtl->setImage("dst_image_pos_x",	voxelTexColorMip_PosX.texture3D(), mipLevel);
					mtl->setImage("dst_image_neg_x",	voxelTexColorMip_NegX.texture3D(), mipLevel);
					mtl->setImage("dst_image_pos_y",	voxelTexColorMip_PosY.texture3D(), mipLevel);
					mtl->setImage("dst_image_neg_y",	voxelTexColorMip_NegY.texture3D(), mipLevel);
					mtl->setImage("dst_image_pos_z",	voxelTexColorMip_PosZ.texture3D(), mipLevel);
					mtl->setImage("dst_image_neg_z",	voxelTexColorMip_NegZ.texture3D(), mipLevel);

					Vec3u	threadGroup		= (Vec3u::s_one() * 8);
					Vec3f   dispatchGroup	= Vec3f::s_cast(mipDimensions) / Vec3f::s_cast(threadGroup);

					drawData->setupMaterial(mtl);
					rdReq.dispatch(RDS_SRCLOC, mtl, 0, Vec3u::s_cast(Vec3f{math::ceil(dispatchGroup.x), math::ceil(dispatchGroup.y), math::ceil(dispatchGroup.z)}));
				}
			);
			lastPass = &pass;
		}
		
		for (u32 i = 1; i < clipmapMaxLevel; i++)
		{
			fmtToNew(passName, "vct_anisotropic_mipmapping-mip{}", i);
			auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Compute);
			pass.runAfter(lastPass);
			pass.writeTexture(voxelTexColorMip_PosX);
			pass.writeTexture(voxelTexColorMip_NegX);
			pass.writeTexture(voxelTexColorMip_PosY);
			pass.writeTexture(voxelTexColorMip_NegY);
			pass.writeTexture(voxelTexColorMip_PosZ);
			pass.writeTexture(voxelTexColorMip_NegZ);
			pass.setExecuteFunc(
				[=](RenderRequest& rdReq)
				{
					auto mtl = mtlAnisotropicMipmappings[i];
					rdReq.reset(rdGraph->renderContext(), *drawData);

					u32 mipLevel = i;
					mtl->setParam("mip_dimemsions",		mipDimensions);
					mtl->setParam("mip_level",			mipLevel);

					mtl->setImage("src_image_pos_x",	voxelTexColorMip_PosX.texture3D(), mipLevel - 1);
					mtl->setImage("src_image_neg_x",	voxelTexColorMip_NegX.texture3D(), mipLevel - 1);
					mtl->setImage("src_image_pos_y",	voxelTexColorMip_PosY.texture3D(), mipLevel - 1);
					mtl->setImage("src_image_neg_y",	voxelTexColorMip_NegY.texture3D(), mipLevel - 1);
					mtl->setImage("src_image_pos_z",	voxelTexColorMip_PosZ.texture3D(), mipLevel - 1);
					mtl->setImage("src_image_neg_z",	voxelTexColorMip_NegZ.texture3D(), mipLevel - 1);

					mtl->setImage("dst_image_pos_x",	voxelTexColorMip_PosX.texture3D(), mipLevel);
					mtl->setImage("dst_image_neg_x",	voxelTexColorMip_NegX.texture3D(), mipLevel);
					mtl->setImage("dst_image_pos_y",	voxelTexColorMip_PosY.texture3D(), mipLevel);
					mtl->setImage("dst_image_neg_y",	voxelTexColorMip_NegY.texture3D(), mipLevel);
					mtl->setImage("dst_image_pos_z",	voxelTexColorMip_PosZ.texture3D(), mipLevel);
					mtl->setImage("dst_image_neg_z",	voxelTexColorMip_NegZ.texture3D(), mipLevel);

					Vec3u	threadGroup		= (Vec3u::s_one() * 8);
					Vec3f   dispatchGroup	= Vec3f::s_cast(mipDimensions) / Vec3f::s_cast(threadGroup);

					drawData->setupMaterial(mtl);
					rdReq.dispatch(RDS_SRCLOC, mtl, 1, Vec3u::s_cast(Vec3f{math::ceil(dispatchGroup.x), math::ceil(dispatchGroup.y), math::ceil(dispatchGroup.z)}));
				}
			);
			mipDimension	= math::max<u32>(mipDimension >> 1, 1);
			mipDimensions.setAll(mipDimension);
			lastPass		= &pass;
		}
		passAnisotropicMipmapping = lastPass;
	}

	return passAnisotropicMipmapping;
}

void 
RpfVoxelConeTracing::setCommonParam(Material* mtl, u32 level, DrawData* drawData)
{
	auto clipmapRegion = VctVoxelClipmapUtil::makeClipmapRegion(level, clipmapRegionWorldExtentL0, voxelResolution);

	mtl->setParam("voxel_resolution",		voxelResolution);
	mtl->setParam("voxel_region_minPosWs",	VctVoxelClipmapUtil::getMinPosWs(clipmapRegion));
	mtl->setParam("voxel_region_maxPosWs",	VctVoxelClipmapUtil::getMaxPosWs(clipmapRegion));
	mtl->setParam("voxel_size",				clipmapRegion.voxelSize);
	//mtl->setParam("clipmap_center",			drawData->camera->pos());
	mtl->setParam("clipmap_level",			level);
	mtl->setParam("clipmap_center",			VctVoxelClipmapUtil::getCenter(clipmapRegion, drawData->camera->pos()));
}

#endif


}