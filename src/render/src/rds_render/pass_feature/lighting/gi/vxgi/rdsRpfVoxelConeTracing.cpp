#include "rds_render-pch.h"
#include "rdsRpfVoxelConeTracing.h"
#include "../../../geometry/rdsRpfGeometryBuffer.h"
#include "../../../shadow/cascaded_shadow_maps/rdsRpfCascadedShadowMaps.h"

#define RDS_VCT_USE_6_FACES_CLIPMAP 0

namespace rds
{

#if 0
#pragma mark --- rdsVoxelClipmapRegion-Decl ---
#endif // 0
#if 1

struct VoxelClipmapRegion
{
public:
	using T			= float;
	using Tuple3	= Tuple3<T>;
	using Vec3		= Vec3<T>;

public:
	Tuple3i	minPosLocal	= Vec3i::s_zero();
	Tuple3i	extentLocal	= Vec3i::s_zero();
	T		voxelSize	= T(0);				// half extent
};

struct VctVoxelClipmapUtil
{
public:
	using SizeType	= RenderTraits::SizeType;

	using T			= float;
	using Tuple3	= Tuple3<T>;
	using Vec3		= Vec3<T>;
	using AABBox3	= AABBox3<T>;

public:
	static T computeVoxelSize(int level, T extentWsL0, SizeType voxelResoultion)	{ return (extentWsL0 * math::pow2(sCast<T>(level))) / sCast<T>(voxelResoultion) / T(2); }

	static u32 computeVoxelResolutionWithBorder(SizeType voxelResoultion)			{ return sCast<u32>(voxelResoultion) + RpfVoxelConeTracing::s_kBorderWidth * 2; }

	static Vec3 getMinPosWs(VoxelClipmapRegion v)						{ return Vec3::s_cast(Vec3i{v.minPosLocal}) * v.voxelSize; }
	static Vec3 getMaxPosWs(VoxelClipmapRegion v)						{ return Vec3::s_cast(Vec3i{v.minPosLocal} + Vec3i{v.extentLocal}) * v.voxelSize; }
	static Vec3 getExtentWs(VoxelClipmapRegion v)						{ return Vec3::s_cast(Vec3i{v.extentLocal}) * v.voxelSize; }
	static Vec3 getCenter(	VoxelClipmapRegion v, Vec3 cameraPos)		{ auto texelSize = v.voxelSize * T(2.0); return Vec3::s_cast(Vec3{math::floor(cameraPos.x / texelSize), math::floor(cameraPos.y / texelSize), math::floor(cameraPos.z / texelSize)} * texelSize); }

	static AABBox3 computeAABBox(VoxelClipmapRegion v, Vec3 cameraPos, SizeType voxelResoultion)	
	{ 
		AABBox3 o;
		auto center				= getCenter(v, cameraPos);
		auto worldHalfExtent	= voxelResoultion * v.voxelSize;
		o.reset(center - worldHalfExtent, center + worldHalfExtent);
		return o;
	}

	static VoxelClipmapRegion makeClipmapRegion(int level, T extentWsL0, SizeType voxelResoultion)
	{
		int resolution = sCast<int>(voxelResoultion);
		VoxelClipmapRegion o;
		o.minPosLocal = -Vec3i{resolution, resolution, resolution} / 2;
		o.extentLocal =  Vec3i{resolution, resolution, resolution};
		o.voxelSize	  = computeVoxelSize(level, extentWsL0, voxelResoultion);
		return o;
	}

};

#endif


#if 0
#pragma mark --- rdsRpfVoxelConeTracing-Impl ---
#endif // 0
#if 1

RpfVoxelConeTracing::RpfVoxelConeTracing()
{

}

RpfVoxelConeTracing::~RpfVoxelConeTracing()
{
	destroy();
}

void 
RpfVoxelConeTracing::create()
{
	destroy();

	RenderUtil::createMaterial(&_shaderVoxelConeTracing,				&_mtlVoxelConeTracing,			"asset/shader/demo/voxel_cone_tracing/rdsVct_VoxelConeTracing.shader");
	RenderUtil::createMaterial(&_shaderVoxelization,					&_mtlVoxelization,				"asset/shader/demo/voxel_cone_tracing/rdsVct_Voxelization.shader");
	RenderUtil::createMaterial(&_shaderOpacityAlpha,					&_mtlOpacityAlpha,				"asset/shader/demo/voxel_cone_tracing/rdsVct_OpacityAlpha.shader");
	RenderUtil::createMaterial(&_shaderCheckAlpha,						&_mtlCheckAlpha,				"asset/shader/demo/voxel_cone_tracing/rdsVct_CheckAlpha.shader");

	RenderUtil::createMaterial(&_shaderVoxelizationDebug,				&_mtlVoxelizationDebug,			"asset/shader/demo/voxel_cone_tracing/rdsVct_Voxelization_Debug.shader");
	RenderUtil::createMaterial(&_shaderVoxelVisualization,				&_mtlVoxelVisualization,		"asset/shader/demo/voxel_cone_tracing/rdsVct_VoxelVisualization.shader");

	RenderUtil::createMaterial(&_shaderClearVoxelClipmap,				&_mtlClearVoxelClipmap,			"asset/shader/demo/voxel_cone_tracing/rdsVct_ClearVoxelClipmap.shader");

	RenderUtil::createMaterial(&_shaderLighting,						&_mtlLighting,					"asset/shader/demo/voxel_cone_tracing/rdsVct_Lighting.shader");

	_mtlsAnisotropicMipmapping.resize(clipmapMaxLevel);
	RenderUtil::createMaterials(&_shaderAnisotropicMipmapping,			_mtlsAnisotropicMipmapping,		"asset/shader/demo/voxel_cone_tracing/rdsVct_AnisotropicMipmapping.shader");

	_mtlClearImage2Ds.resize(2);

	{
		Vec3u clipmapSize3u;
		Vec2u dummmySize;

		#if VCT_USE_6_FACES_CLIPMAP
		auto voxelResolutionWithBorder = VctVoxelClipmapUtil::computeVoxelResolutionWithBorder(voxelResolution);
		clipmapSize3u	= Vec3u{voxelResolutionWithBorder * sCast<u32>(TextureCube::s_kFaceCount), voxelResolutionWithBorder * clipmapMaxLevel, voxelResolutionWithBorder};
		dummmySize		= Vec2u{voxelResolutionWithBorder, voxelResolutionWithBorder};
		#else
		clipmapSize3u	= Vec3u{voxelResolution, voxelResolution, voxelResolution};
		dummmySize		= Vec2u{voxelResolution, voxelResolution};
		#endif // 0
		auto cDesc = Texture3D::makeCDesc(RDS_SRCLOC);
		cDesc = Texture3D_CreateDesc{ clipmapSize3u,	ColorType::RGBAb,	TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst | TextureUsageFlags::TransferSrc};
		_voxelTexRadiance = Renderer::rdDev()->createTexture3D(cDesc);
		_voxelTexRadiance->setDebugName("vct_voxelTexRadiance");
	}

	voxelClipmaps.resize(s_kMaxClipmapLevel);
	voxelClipmaps.setDebugName("VoxelClipmaps");
}

void 
RpfVoxelConeTracing::destroy()
{
	RenderUtil::destroyShaderMaterial(_shaderVoxelization, _mtlVoxelization);
	RenderUtil::destroyShaderMaterial(_shaderOpacityAlpha, _mtlOpacityAlpha);
	RenderUtil::destroyShaderMaterial(_shaderVoxelConeTracing, _mtlVoxelConeTracing);

	RenderUtil::destroyShaderMaterial(_shaderVoxelizationDebug, _mtlVoxelizationDebug);
	RenderUtil::destroyShaderMaterial(_shaderVoxelVisualization, _mtlVoxelVisualization);
	RenderUtil::destroyShaderMaterial(_shaderClearVoxelClipmap, _mtlClearVoxelClipmap);

	RenderUtil::destroyShaderMaterials(_shaderAnisotropicMipmapping, _mtlsAnisotropicMipmapping);

	RenderUtil::destroyMaterials(_mtlClearImage2Ds);
}

RdgPass* 
RpfVoxelConeTracing::addVoxelizationPass(const DrawSettings& drawSettings_, const RpfCascadedShadowMaps::Result& csmResult)
{
	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passVoxelization	= nullptr;

	u32 level = curLevel;
	updateClipmap(level, drawData);

	auto	screenSize	= drawData->resolution2u();

	Vec3u clipmapSize3u;
	Vec2u dummmySize;

	#if VCT_USE_6_FACES_CLIPMAP
	auto voxelResolutionWithBorder = VctVoxelClipmapUtil::computeVoxelResolutionWithBorder(voxelResolution);
	clipmapSize3u	= Vec3u{voxelResolutionWithBorder * sCast<u32>(TextureCube::s_kFaceCount), voxelResolutionWithBorder * clipmapMaxLevel, voxelResolutionWithBorder};
	dummmySize		= Vec2u{voxelResolutionWithBorder, voxelResolutionWithBorder};
	#else
	clipmapSize3u	= Vec3u{voxelResolution, voxelResolution, voxelResolution};
	dummmySize		= Vec2u{voxelResolution, voxelResolution};
	#endif // 0

	// TODO: remove
	RdgTextureHnd rtDummy		= rdGraph->createTexture("vct_dummy",			Texture2D_CreateDesc{ dummmySize,		ColorType::Rb,		TextureUsageFlags::RenderTarget});
	//RdgTextureHnd voxelTexRadiance = rdGraph->createTexture("vct_voxelTexRadiance",	Texture3D_CreateDesc{ clipmapSize3u,	ColorType::RGBAb,	TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	//RdgTextureHnd voxelTexRadiance = rdGraph->importTexture("vct_voxelTexRadiance", _voxelTexRadiance);

	#if 1
	#if VCT_USE_6_FACES_CLIPMAP

	RdgTextureHnd voxelTexRadiance = rdGraph->importTexture("vct_voxelTexRadiance", _voxelTexRadiance);

	#if 0
	auto clipmap6FacesSize			= Vec3u{voxelResolutionWithBorder * sCast<u32>(TextureCube::s_kFaceCount), voxelResolutionWithBorder * clipmapMaxLevel, voxelResolutionWithBorder};

	RdgTextureHnd voxelTexRadiancePrev = rdGraph->importTexture("vct_voxelTexRadiancePrev",	_voxelTexRadianceRaw ? _voxelTexRadianceRaw : _voxelTexRadiance.ptr());
	RdgTextureHnd voxelTexRadiance		= rdGraph->createTexture("vct_voxelTexRadiance",		Texture3D_CreateDesc{ clipmap6FacesSize,	ColorType::RGBAb,	TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst | TextureUsageFlags::TransferSrc});
	rdGraph->exportTexture(&_voxelTexRadianceRaw, voxelTexRadiance, TextureUsageFlags::TransferSrc);

	if (s_frame != 0)
	{
		auto& passCopy = rdGraph->addPass("vct_copy_voxel_tex", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Transfer);
		passCopy.readTexture(voxelTexRadiancePrev);
		passCopy.writeTexture(voxelTexRadiance);
		passCopy.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.copyTexture(RDS_SRCLOC, voxelTexRadiance.renderResource(), voxelTexRadiancePrev.renderResource(), 0, 0, 0, 0);
			}
		);
	}
	#endif // 0

	#else
	RdgTextureHnd voxelTexRadiance = rdGraph->importTexture("vct_voxelTexRadiance", _voxelTexRadiance);
	#endif // 0
	#endif // 0

	result.voxelTexRadiance = voxelTexRadiance;
	//RDS_LOG("------------ addVoxelizationPass {}", result.voxelTexRadiance.name());

	{
		#if VCT_USE_6_FACES_CLIPMAP

		Material* mtl = _mtlClearVoxelClipmap;
		{
			//auto& pass = rdGraph->addPass("vct_clearImage3D", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Transfer);
			//pass.writeTexture(voxelTexRadiance, TextureUsageFlags::TransferDst);
			auto& pass = rdGraph->addPass("vct_clearVoxelClipmap", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
			pass.writeTexture(voxelTexRadiance, TextureUsageFlags::UnorderedAccess);
			pass.setExecuteFunc(
				[=](RenderRequest& rdReq)
				{
					bool clearAll		= false;
					auto imageExtent	= clearAll ? voxelTexRadiance.texture3D()->size() : Vec3u{ voxelResolutionWithBorder, voxelResolutionWithBorder, voxelResolutionWithBorder };

					mtl->setParam("image_extent",			imageExtent);
					mtl->setParam("image_extent_offset",	Vec3u::s_zero());
					mtl->setParam("clear_value",			Vec4f::s_zero());
					mtl->setImage("image",					voxelTexRadiance.texture3D(), 0);
					setupCommonParam(mtl, level);
					rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, imageExtent);
				}
			);
		}
		#else
		{
			_addClearImage3DPass(_mtlClearImage3D, voxelTexRadiance);
		}
		#endif
	}

	String passName = "vct_voxelization";
	{
		fmtToNew(passName, "{}-l{}", "vct_voxelization", level);
		auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Graphics/*, RdgPassFlags::NoRenderTarget*/);
		if (csmResult.shadowMap)
			pass.readTexture(csmResult.shadowMap);
		pass.writeTexture(voxelTexRadiance, TextureUsageFlags::UnorderedAccess, ShaderStageFlag::Pixel);
		pass.setRenderTarget(rtDummy, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				//auto mtl = mtlVoxelization;
				rdReq.reset(rdGraph->renderContext(), drawData);
				rdReq.setViewport(		Tuple2f::s_zero(), Vec2f::s_cast(dummmySize));
				rdReq.setScissorRect(	Tuple2f::s_zero(), Vec2f::s_cast(dummmySize));

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				DrawSettings drawSettings = drawSettings_;
				drawSettings.setMaterialFn = [=](Material* mtl_)
					{
						setupCommonParam(mtl_, level);
						mtl_->setImage("voxel_tex_radiance",	voxelTexRadiance.texture3D(), 0);

						if (csmResult.shadowMap)
						{
							mtl_->setArray("csm_matrices",		csmResult.lightMatrices);
							mtl_->setArray("csm_plane_dists",	csmResult.cascadePlaneDists);
							mtl_->setParam("csm_level_count",	sCast<u32>(csmResult.cascadePlaneDists.size()));
							mtl_->setParam("csm_shadowMap",		csmResult.shadowMap.texture2DArray());
							mtl_->setParam("csm_shadowMap",		SamplerState::makeNearestClampToEdge());
						}
					};

				auto clipmapRegion = VctVoxelClipmapUtil::makeClipmapRegion(level, clipmapRegionWorldExtentL0, voxelResolution);
				drawSettings.cullingSetting.setAABBox(VctVoxelClipmapUtil::computeAABBox(clipmapRegion, drawData->camera->pos(), voxelResolution));

				constCast(drawSettings).overrideShader = _shaderVoxelization;
				drawData->drawScene(rdReq, drawSettings);
			}
		);
		passVoxelization = &pass;
	}

	return passVoxelization;
}

RdgPass* 
RpfVoxelConeTracing::addOpacityAlphaPass(RpfVoxelConeTracing_Result* oResult)
{
	auto*		rdGraph					= renderGraph();
	auto*		drawData				= drawDataBase();
	RdgPass*	passOpacityAlphaPass	= nullptr;

	auto* result_ = oResult;

	u32				level						= curLevel;
	u32				voxelResolutionWithBorder	= VctVoxelClipmapUtil::computeVoxelResolutionWithBorder(voxelResolution);
	RdgTextureHnd	voxelTexRadiance			= result_->voxelTexRadiance;

	Material* mtl = _mtlOpacityAlpha;
	TempString passName;
	{
		fmtToNew(passName, "vct_opacityAlpha-l{}", level);
		auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Compute);
		pass.writeTexture(voxelTexRadiance);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				auto imageExtent = Vec3u{ voxelResolutionWithBorder, voxelResolutionWithBorder, voxelResolutionWithBorder };
				mtl->setParam("image_extent",			imageExtent);
				mtl->setParam("image_extent_offset",	Vec3u::s_zero());
				mtl->setImage("voxel_tex_radiance",		voxelTexRadiance.texture3D(), 0);

				setupCommonParam(mtl, level);
				rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, 0, imageExtent);
			}
		);
		passOpacityAlphaPass = &pass;
	}
	return passOpacityAlphaPass;
}

RdgPass* 
RpfVoxelConeTracing::addCheckAlphaPass()
{
	auto*		rdGraph					= renderGraph();
	auto*		drawData				= drawDataBase();
	RdgPass*	passCheckAlphaPass	= nullptr;

	u32				level						= curLevel;
	RdgTextureHnd	voxelTexRadiance			= result.voxelTexRadiance;

	Material* mtl = _mtlCheckAlpha;
	TempString passName;
	{
		fmtToNew(passName, "vct_checkAlpha-l{}", level);
		auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Compute);
		pass.writeTexture(voxelTexRadiance);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				mtl->setParam("image_extent",			voxelTexRadiance.size());
				mtl->setParam("image_extent_offset",	Vec3u::s_zero());
				mtl->setImage("image",					voxelTexRadiance.texture3D(), 0);

				setupCommonParam(mtl, level);
				rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, 0, voxelTexRadiance.size());
			}
		);
		passCheckAlphaPass = &pass;
	}
	return passCheckAlphaPass;
}

RdgPass* 
RpfVoxelConeTracing::addVoxelizationDebugPass(RdgTextureHnd rtColor, RdgTextureHnd dsBuf)
{
	auto*		rdGraph					= renderGraph();
	auto*		drawData				= drawDataBase();
	RdgPass*	passVoxelizationDebug	= nullptr;

	auto	screenSize	= drawData->resolution2u();

	Tuple3u clipmapSizes;
	clipmapSizes.setAll(voxelResolution);
	clipmapSizes.set(screenSize.x, screenSize.y, 4);

	RdgTextureHnd voxelTexRadiance			= result.voxelTexRadiance;		// must copy here

	auto level = curLevel;

	Material* mtl = _mtlVoxelizationDebug;
	{
		auto& pass = rdGraph->addPass("vct_voxelization_debug", RdgPassTypeFlags::Graphics);

		setupPassReadMipmap(pass, ShaderStageFlag::Pixel);
		//pass.writeTexture(voxelTexRadiance,	TextureUsageFlags::UnorderedAccess, ShaderStageFlag::Pixel);
		//pass.writeTexture(tex2D_color,		TextureUsageFlags::UnorderedAccess, ShaderStageFlag::Pixel);

		pass.setRenderTarget(rtColor,	RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf,		RdgAccess::Write, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				//RDS_LOG("setParam {} - idx: {}", voxelTexRadiance.name(), voxelTexRadiance.texture3D()->bindlessHandle().getResourceIndex());

				setupCommonParam(mtl, level);
				setupMipmapParam(mtl);

				float scale = 32.0f;
				mtl->setParam("matrix_world",		Mat4f::s_TRS(Vec3f{0.0f, scale, -10.0f}, Quat4f::s_identity(), Vec3f::s_one() * scale)); // Quat4f::s_eulerDegX(90.0f)

				mtl->setParam("voxel_extent",		1.0f);
				mtl->setParam("voxel_resolution",	voxelResolution);
				mtl->setParam("voxel_tex_radiance",		voxelTexRadiance.texture3D());

				drawData->setupMaterial(mtl);
				rdReq.drawSceneQuad(RDS_SRCLOC, mtl);
			}
		);
		passVoxelizationDebug = &pass;
	}
	return passVoxelizationDebug;
}

RdgPass* 
RpfVoxelConeTracing::addVoxelVisualizationPass(RdgTextureHnd rtColor, RdgTextureHnd dsBuf)
{
	auto*		rdGraph					= renderGraph();
	auto*		drawData				= drawDataBase();
	RdgPass*	passVoxelVisualization	= nullptr;

	auto	screenSize	= drawData->resolution2u();

	Tuple3u clipmapSizes;
	clipmapSizes.setAll(voxelResolution);
	clipmapSizes.set(screenSize.x, screenSize.y, 4);

	RdgTextureHnd voxelTexRadiance			= result.voxelTexRadiance;		// must copy here

	TempString passName = "vct_voxel_visualization";
	for (u32 i = 0; i < 1; ++i)
	{
		Material* mtl = _mtlVoxelVisualization;
		setupCommonParam(mtl, curLevel);

		fmtToNew(passName, "{}-l{}", "vct_voxel_visualization", i);
		auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Graphics);
		pass.readTexture(voxelTexRadiance,			TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		setupPassReadMipmap(pass, ShaderStageFlag::Pixel);

		pass.setRenderTarget(rtColor,	RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		//pass.setDepthStencil(dsBuf,		RdgAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setDepthStencil(dsBuf,		RdgAccess::Write, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				mtl->setParam("voxel_tex_radiance",		voxelTexRadiance.texture3D());
				mtl->setParam("visualize_level",		clipmapMaxLevel);

				drawData->setupMaterial(mtl);
				setupMipmapParam(mtl);

				auto drawCall = rdReq.addDrawCall(sizeof(PerObjectParam));
				drawCall->setDebugSrcLoc(RDS_SRCLOC);
				drawCall->renderPrimitiveType = RenderPrimitiveType::Point;
				drawCall->vertexCount = voxelResolution * voxelResolution * voxelResolution * (clipmapMaxLevel * 2 / 3);
				drawCall->setMaterial(mtl);

				/*PerObjectParam objParam;
				objParam.id = sCast<decltype(PerObjectParam::id)>(s_entVctVoxelVisualizationId);
				drawCall->setExtraData(objParam);*/
			}
		);
		passVoxelVisualization = &pass;
	}

	return passVoxelVisualization;
}

RdgPass* 
RpfVoxelConeTracing::addAnisotropicMipmappingPass()
{
	auto*		rdGraph						= renderGraph();
	auto*		drawData					= drawDataBase();
	RdgPass*	passAnisotropicMipmapping	= nullptr;

	u32		mipDimension	= math::max<u32>(voxelResolution >> 1, 1);
	Vec3u	mipDimensions	= Tuple3u{ mipDimension, mipDimension, mipDimension };

	RdgTextureHnd voxelTexRadianceMip_PosX = rdGraph->createTexture("vct_voxelTexRadianceMip_PosX", Texture3D_CreateDesc{ mipDimensions, ColorType::RGBAb, clipmapMaxLevel, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	RdgTextureHnd voxelTexRadianceMip_NegX = rdGraph->createTexture("vct_voxelTexRadianceMip_NegX", Texture3D_CreateDesc{ mipDimensions, ColorType::RGBAb, clipmapMaxLevel, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	RdgTextureHnd voxelTexRadianceMip_PosY = rdGraph->createTexture("vct_voxelTexRadianceMip_PosY", Texture3D_CreateDesc{ mipDimensions, ColorType::RGBAb, clipmapMaxLevel, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	RdgTextureHnd voxelTexRadianceMip_NegY = rdGraph->createTexture("vct_voxelTexRadianceMip_NegY", Texture3D_CreateDesc{ mipDimensions, ColorType::RGBAb, clipmapMaxLevel, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	RdgTextureHnd voxelTexRadianceMip_PosZ = rdGraph->createTexture("vct_voxelTexRadianceMip_PosZ", Texture3D_CreateDesc{ mipDimensions, ColorType::RGBAb, clipmapMaxLevel, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	RdgTextureHnd voxelTexRadianceMip_NegZ = rdGraph->createTexture("vct_voxelTexRadianceMip_NegZ", Texture3D_CreateDesc{ mipDimensions, ColorType::RGBAb, clipmapMaxLevel, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});

	RdgTextureHnd voxelTexRadiance		= result.voxelTexRadiance;		// must copy here

	{
		TempString passName;
		RdgPass* lastPass = nullptr;

		{
			Material* mtl = _mtlsAnisotropicMipmapping[0];

			auto& pass = rdGraph->addPass("vct_anisotropic_pre_mipmapping", RdgPassTypeFlags::Compute | RdgPassTypeFlags::Graphics);
			pass.readTexture(voxelTexRadiance);
			pass.writeTexture(voxelTexRadianceMip_PosX);
			pass.writeTexture(voxelTexRadianceMip_NegX);
			pass.writeTexture(voxelTexRadianceMip_PosY);
			pass.writeTexture(voxelTexRadianceMip_NegY);
			pass.writeTexture(voxelTexRadianceMip_PosZ);
			pass.writeTexture(voxelTexRadianceMip_NegZ);
			pass.setExecuteFunc(
				[=](RenderRequest& rdReq)
				{
					rdReq.reset(rdGraph->renderContext(), drawData);

					u32 mipLevel = 0;
					mtl->setParam("mip_dimemsions",		mipDimensions);
					mtl->setParam("mip_level",			mipLevel);

					mtl->setParam("src_image",			voxelTexRadiance.texture3D());

					//RDS_LOG("------------------");
					mtl->setImage("dst_image_pos_x",	voxelTexRadianceMip_PosX.texture3D(), mipLevel);
					mtl->setImage("dst_image_neg_x",	voxelTexRadianceMip_NegX.texture3D(), mipLevel);
					mtl->setImage("dst_image_pos_y",	voxelTexRadianceMip_PosY.texture3D(), mipLevel);
					mtl->setImage("dst_image_neg_y",	voxelTexRadianceMip_NegY.texture3D(), mipLevel);
					mtl->setImage("dst_image_pos_z",	voxelTexRadianceMip_PosZ.texture3D(), mipLevel);
					mtl->setImage("dst_image_neg_z",	voxelTexRadianceMip_NegZ.texture3D(), mipLevel);

					setupCommonParam(mtl, mipLevel);
					rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, 0, mipDimensions);
				}
			);
			lastPass = &pass;
		}

		for (u32 i = 1; i < clipmapMaxLevel; i++)
		{
			Material* mtl = _mtlsAnisotropicMipmapping[i];

			fmtToNew(passName, "vct_anisotropic_mipmapping-mip{}", i);
			auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Compute | RdgPassTypeFlags::Graphics);
			pass.runAfter(lastPass);
			pass.writeTexture(voxelTexRadianceMip_PosX);
			pass.writeTexture(voxelTexRadianceMip_NegX);
			pass.writeTexture(voxelTexRadianceMip_PosY);
			pass.writeTexture(voxelTexRadianceMip_NegY);
			pass.writeTexture(voxelTexRadianceMip_PosZ);
			pass.writeTexture(voxelTexRadianceMip_NegZ);
			pass.setExecuteFunc(
				[=](RenderRequest& rdReq)
				{
					rdReq.reset(rdGraph->renderContext(), drawData);

					u32 mipLevel = i;
					mtl->setParam("mip_dimemsions",		mipDimensions);
					mtl->setParam("mip_level",			mipLevel);

					mtl->setImage("src_image_pos_x",	voxelTexRadianceMip_PosX.texture3D(), mipLevel - 1);
					mtl->setImage("src_image_neg_x",	voxelTexRadianceMip_NegX.texture3D(), mipLevel - 1);
					mtl->setImage("src_image_pos_y",	voxelTexRadianceMip_PosY.texture3D(), mipLevel - 1);
					mtl->setImage("src_image_neg_y",	voxelTexRadianceMip_NegY.texture3D(), mipLevel - 1);
					mtl->setImage("src_image_pos_z",	voxelTexRadianceMip_PosZ.texture3D(), mipLevel - 1);
					mtl->setImage("src_image_neg_z",	voxelTexRadianceMip_NegZ.texture3D(), mipLevel - 1);

					mtl->setImage("dst_image_pos_x",	voxelTexRadianceMip_PosX.texture3D(), mipLevel);
					mtl->setImage("dst_image_neg_x",	voxelTexRadianceMip_NegX.texture3D(), mipLevel);
					mtl->setImage("dst_image_pos_y",	voxelTexRadianceMip_PosY.texture3D(), mipLevel);
					mtl->setImage("dst_image_neg_y",	voxelTexRadianceMip_NegY.texture3D(), mipLevel);
					mtl->setImage("dst_image_pos_z",	voxelTexRadianceMip_PosZ.texture3D(), mipLevel);
					mtl->setImage("dst_image_neg_z",	voxelTexRadianceMip_NegZ.texture3D(), mipLevel);

					setupCommonParam(mtl, mipLevel);
					rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, 1, mipDimensions);
				}
			);
			mipDimension	= math::max<u32>(mipDimension >> 1, 1);
			mipDimensions.setAll(mipDimension);
			lastPass		= &pass;
		}
		passAnisotropicMipmapping = lastPass;
	}

	result.voxelTexRadianceMip_PosX = voxelTexRadianceMip_PosX;
	result.voxelTexRadianceMip_NegX = voxelTexRadianceMip_NegX;
	result.voxelTexRadianceMip_PosY = voxelTexRadianceMip_PosY;
	result.voxelTexRadianceMip_NegY = voxelTexRadianceMip_NegY;
	result.voxelTexRadianceMip_PosZ = voxelTexRadianceMip_PosZ;
	result.voxelTexRadianceMip_NegZ = voxelTexRadianceMip_NegZ;

	return passAnisotropicMipmapping;
}

RdgPass* 
RpfVoxelConeTracing::addVoxelConeTracingPass(RdgTextureHnd depth, RpfGeometryBuffer_Result& gBuf)
{
	auto*		rdGraph					= renderGraph();
	auto*		drawData				= drawDataBase();
	RdgPass*	passVoxelConeTracing	= nullptr;

	bool isUsingCompute = true;

	auto	screenSize	= drawData->resolution2u();

	result.texDiffuse	= rdGraph->createTexture("vct_diffuse",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAb,	TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::RenderTarget | TextureUsageFlags::TransferDst});
	result.texSpecular	= rdGraph->createTexture("vct_specular",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb,	TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::RenderTarget | TextureUsageFlags::TransferDst});

	auto level = curLevel;

	auto texDiffuse			= result.texDiffuse;
	auto texSpecular		= result.texSpecular;
	auto voxelTexRadiance	= result.voxelTexRadiance;

	_addClearImage2DPass(_mtlClearImage2Ds[0], texDiffuse);
	_addClearImage2DPass(_mtlClearImage2Ds[1], texSpecular);

	Material* mtl = _mtlVoxelConeTracing;
	{
		auto rdgPassTypeFlag = isUsingCompute ? RdgPassTypeFlags::Compute | RdgPassTypeFlags::Graphics : RdgPassTypeFlags::Compute | RdgPassTypeFlags::Graphics;
		auto shaderStageFlag = isUsingCompute ? ShaderStageFlag::Compute : ShaderStageFlag::Pixel;

		auto& pass = rdGraph->addPass("vct_voxel_cone_tracing", rdgPassTypeFlag);
		pass.readTexture(voxelTexRadiance,			TextureUsageFlags::ShaderResource, shaderStageFlag);

		#if !VCT_USE_6_FACES_CLIPMAP
		pass.readTexture(result.voxelTexRadianceMip_PosX,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(result.voxelTexRadianceMip_NegX,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(result.voxelTexRadianceMip_PosY,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(result.voxelTexRadianceMip_NegY,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(result.voxelTexRadianceMip_PosZ,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(result.voxelTexRadianceMip_NegZ,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		#endif // !VCT_USE_6_FACES_CLIPMAP

		gBuf.setupRdgPassForRead(pass, depth, shaderStageFlag);

		if (isUsingCompute)
		{
			pass.writeTexture(texDiffuse,	TextureUsageFlags::UnorderedAccess, shaderStageFlag);
			pass.writeTexture(texSpecular,	TextureUsageFlags::UnorderedAccess, shaderStageFlag);
		}
		else
		{
			pass.setRenderTarget(texDiffuse,	RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
			pass.setRenderTarget(texSpecular,	RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		}

		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				if (isUsingCompute)
				{
					mtl->setImage("out_indirect_diffuse",	texDiffuse.texture2D(),  0);
					mtl->setImage("out_indirect_specular",	texSpecular.texture2D(), 0);
				}

				mtl->setParam("voxel_tex_radiance",			voxelTexRadiance.texture3D());
				mtl->setParam("voxel_tex_radiance",			SamplerState::makeLinearClampToEdge());

				constCast(gBuf).setupMaterial(mtl, depth);

				setupCommonParam(mtl, level);
				setupMipmapParam(mtl);
				drawData->setupMaterial(mtl);
				if (isUsingCompute)
				{
					rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, 0, Vec3u{screenSize, 1});
				}
				else
				{
					rdReq.drawSceneQuad(RDS_SRCLOC, mtl);
				}
			}
		);
		passVoxelConeTracing = &pass;
	}

	return passVoxelConeTracing;
}

RdgPass* 
RpfVoxelConeTracing::addLightingPass(RdgTextureHnd rtColor, RdgTextureHnd depth, RpfGeometryBuffer_Result& gBuf)
{
	auto*		rdGraph			= renderGraph();
	auto*		drawData		= drawDataBase();
	RdgPass*	passVctLighting	= nullptr;

	auto	screenSize	= drawData->resolution2u();
	auto level = curLevel;

	auto vctResult = result;

	Material* mtl = _mtlLighting;
	{
		auto shaderStageFlag = ShaderStageFlag::Pixel;

		auto& pass = rdGraph->addPass("vct_lighting", RdgPassTypeFlags::Graphics);

		pass.readTexture(vctResult.texDiffuse,			TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(vctResult.texSpecular,			TextureUsageFlags::ShaderResource, shaderStageFlag);
		gBuf.setupRdgPassForRead(pass, depth, shaderStageFlag);

		pass.setRenderTarget(rtColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				mtl->setParam("indirect_diffuse",	vctResult.texDiffuse.texture2D());
				mtl->setParam("indirect_specular",	vctResult.texSpecular.texture2D());
				constCast(gBuf).setupMaterial(mtl, depth);

				setupCommonParam(mtl, level);
				drawData->setupMaterial(mtl);
				rdReq.drawSceneQuad(RDS_SRCLOC, mtl);
				//rdReq.drawMesh(RDS_SRCLOC, DemoEditorApp::instance()->gfxDemo->meshAssets().fullScreenTriangle->renderMesh, mtl);
			}
		);
		passVctLighting = &pass;
}

	return passVctLighting;
}

void 
RpfVoxelConeTracing::updateClipmap(u32 level, DrawData_Base* drawData)
{
	auto clipmapRegion = VctVoxelClipmapUtil::makeClipmapRegion(level, clipmapRegionWorldExtentL0, voxelResolution);

	auto& clipmap = voxelClipmaps.at(level);
	clipmap.center		= VctVoxelClipmapUtil::getCenter(clipmapRegion, drawData->camera->pos());
	clipmap.voxelSize	= clipmapRegion.voxelSize;
	voxelClipmaps.uploadToGpu();
}

void 
RpfVoxelConeTracing::setupCommonParam(Material* mtl, u32 level)
{
	auto clipmapRegion		= VctVoxelClipmapUtil::makeClipmapRegion(level, clipmapRegionWorldExtentL0, voxelResolution);
	auto clipmapRegionL0	= VctVoxelClipmapUtil::makeClipmapRegion(0,		clipmapRegionWorldExtentL0, voxelResolution);

	//mtl->setParam("voxel_region_minPosWs",	VctVoxelClipmapUtil::getMinPosWs(clipmapRegion));
	//mtl->setParam("voxel_region_maxPosWs",	VctVoxelClipmapUtil::getMaxPosWs(clipmapRegion));
	//mtl->setParam("voxel_size",				clipmapRegion.voxelSize);
	//mtl->setParam("clipmap_center",			VctVoxelClipmapUtil::getCenter(clipmapRegion, drawData->camera->pos()));
	//mtl->setParam("clipmap_center",			drawData->camera->pos());
	//mtl->setParam("clipmap_center",			VctVoxelClipmapUtil::getCenter(clipmapRegionL0, drawData->camera->pos()));

	mtl->setParam("voxel_resolution",		voxelResolution);
	mtl->setParam("voxel_sizeL0",			clipmapRegionL0.voxelSize);
	mtl->setParam("clipmap_level",			level);
	mtl->setParam("clipmap_maxLevel",		clipmapMaxLevel);

	mtl->setParam("clipmaps",				voxelClipmaps.gpuBuffer());
}

void
RpfVoxelConeTracing::setupMipmapParam(Material* mtl)
{
	#if !VCT_USE_6_FACES_CLIPMAP
	mtl->setParam("voxel_tex_pos_x",	result.voxelTexRadianceMip_PosX.texture3D());
	mtl->setParam("voxel_tex_neg_x",	result.voxelTexRadianceMip_NegX.texture3D());
	mtl->setParam("voxel_tex_pos_y",	result.voxelTexRadianceMip_PosY.texture3D());
	mtl->setParam("voxel_tex_neg_y",	result.voxelTexRadianceMip_NegY.texture3D());
	mtl->setParam("voxel_tex_pos_z",	result.voxelTexRadianceMip_PosZ.texture3D());
	mtl->setParam("voxel_tex_neg_z",	result.voxelTexRadianceMip_NegZ.texture3D());
	#endif // !VCT_USE_6_FACES_CLIPMAP
}

void 
RpfVoxelConeTracing::setupPassReadMipmap(RdgPass& pass, ShaderStageFlag stage)
{
	#if !VCT_USE_6_FACES_CLIPMAP
	pass.readTexture(result.voxelTexRadianceMip_PosX,	TextureUsageFlags::ShaderResource, stage);
	pass.readTexture(result.voxelTexRadianceMip_NegX,	TextureUsageFlags::ShaderResource, stage);
	pass.readTexture(result.voxelTexRadianceMip_PosY,	TextureUsageFlags::ShaderResource, stage);
	pass.readTexture(result.voxelTexRadianceMip_NegY,	TextureUsageFlags::ShaderResource, stage);
	pass.readTexture(result.voxelTexRadianceMip_PosZ,	TextureUsageFlags::ShaderResource, stage);
	pass.readTexture(result.voxelTexRadianceMip_NegZ,	TextureUsageFlags::ShaderResource, stage);
	#endif
}

#endif

}