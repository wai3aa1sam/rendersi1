#include "rds_render-pch.h"
#include "rdsRpfVoxelConeTracing.h"

#define RDS_VCT_USE_6_FACES_CLIPMAP 0

namespace rds
{

#if 0
#pragma mark --- rdsVoxelClipmapRegion-Decl ---
#endif // 0
#if 0

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

struct VoxelClipmapUtil
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
#if 0

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

	createMaterial(&shaderVoxelConeTracing,			&mtlVoxelConeTracing,			"asset/shader/demo/voxel_cone_tracing/rdsVct_VoxelConeTracing.shader");
	createMaterial(&shaderVoxelization,				&mtlVoxelization,				"asset/shader/demo/voxel_cone_tracing/rdsVct_Voxelization.shader");

	createMaterial(&shaderVoxelizationDebug,			&mtlVoxelizationDebug,		"asset/shader/demo/voxel_cone_tracing/rdsVct_Voxelization_Debug.shader");
	createMaterial(&shaderVoxelVisualization,			&mtlVoxelVisualization,		"asset/shader/demo/voxel_cone_tracing/rdsVct_VoxelVisualization.shader");

	createMaterial(&shaderClearVoxelClipmap,			&mtlClearVoxelClipmap,		"asset/shader/demo/voxel_cone_tracing/rdsVctClearVoxelClipmap.shader");

	createMaterial(&shaderClearImage3D,				&mtlClearImage3D,				"asset/shader/util/rdsClearImage3D.shader");

	mtlClearImage2D.resize(2);
	for (auto& e : mtlClearImage2D)
	{
		createMaterial(&shaderClearImage2D,			&e,								"asset/shader/util/rdsClearImage2D.shader");
	}

	mtlAnisotropicMipmappings.resize(clipmapMaxLevel);
	for (auto& e : mtlAnisotropicMipmappings)
	{
		createMaterial(&shaderAnisotropicMipmapping,	&e,							"asset/shader/demo/voxel_cone_tracing/rdsVct_AnisotropicMipmapping.shader");
	}

	mtlVoxelizations.resize(clipmapMaxLevel);
	for (auto& e : mtlVoxelizations)
	{
		createMaterial(&shaderVoxelization,			&e,								"asset/shader/demo/voxel_cone_tracing/rdsVct_Voxelization.shader");
	}

	{
		Vec3u clipmapSize3u;
		Vec2u dummmySize;

		#if RDS_VCT_USE_6_FACES_CLIPMAP
		auto voxelResolutionWithBorder = VctVoxelClipmapUtil::computeVoxelResolutionWithBorder(voxelResolution);
		clipmapSize3u	= Vec3u{voxelResolutionWithBorder * sCast<u32>(TextureCube::s_kFaceCount), voxelResolutionWithBorder * clipmapMaxLevel, voxelResolutionWithBorder};
		dummmySize		= Vec2u{voxelResolutionWithBorder, voxelResolutionWithBorder};
		#else
		clipmapSize3u	= Vec3u{voxelResolution, voxelResolution, voxelResolution};
		dummmySize		= Vec2u{voxelResolution, voxelResolution};
		#endif // 0
		auto cDesc = Texture3D::makeCDesc(RDS_SRCLOC);
		cDesc = Texture3D_CreateDesc{ clipmapSize3u,	ColorType::RGBAb,	TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst };
		voxelTexRadiance = Renderer::rdDev()->createTexture3D(cDesc);
	}
}

void 
RpfVoxelConeTracing::destroy()
{
	shaderVoxelConeTracing.reset(nullptr);
	mtlVoxelConeTracing.reset(	 nullptr);
}

RdgPass* 
RpfVoxelConeTracing::addVoxelizationPass(const DrawSettings& drawSettings_, RpfGeometryBuffer_Result& gBuf)
{
	auto*	rdGraph		= renderGraph();
	auto*	drawData	= getDrawData();

	RdgPass*	voxelizationPass	= nullptr;

	u32 level = curLevel;

	auto	screenSize	= Vec2u::s_cast(Vec2f(drawData->resolution())).toTuple2();

	Vec3u clipmapSize3u;
	Vec2u dummmySize;

	#if RDS_VCT_USE_6_FACES_CLIPMAP
	auto voxelResolutionWithBorder = VctVoxelClipmapUtil::computeVoxelResolutionWithBorder(voxelResolution);
	clipmapSize3u	= Vec3u{voxelResolutionWithBorder * sCast<u32>(TextureCube::s_kFaceCount), voxelResolutionWithBorder * clipmapMaxLevel, voxelResolutionWithBorder};
	dummmySize		= Vec2u{voxelResolutionWithBorder, voxelResolutionWithBorder};
	#else
	clipmapSize3u	= Vec3u{voxelResolution, voxelResolution, voxelResolution};
	dummmySize		= Vec2u{voxelResolution, voxelResolution};
	#endif // 0


	// TODO: remove
	RdgTextureHnd rtDummy		= rdGraph->createTexture("vct_dummy",			Texture2D_CreateDesc{ dummmySize,		ColorType::Rb,		TextureUsageFlags::RenderTarget});
	//RdgTextureHnd voxelTexColor = rdGraph->createTexture("vct_voxelTexColor",	Texture3D_CreateDesc{ clipmapSize3u,	ColorType::RGBAb,	TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst});
	RdgTextureHnd voxelTexColor = rdGraph->importTexture("vct_voxelTexColor", voxelTexRadiance);
	result.voxelTexColor		= voxelTexColor;
	//RDS_LOG("------------ addVoxelizationPass {}", result.voxelTexColor.name());

	{
		#if RDS_VCT_USE_6_FACES_CLIPMAP
		{
			//auto& pass = rdGraph->addPass("vct_clearImage3D", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Transfer);
			//pass.writeTexture(voxelTexColor, TextureUsageFlags::TransferDst);
			auto& pass = rdGraph->addPass("vct_clearVoxelClipmap", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
			pass.writeTexture(voxelTexColor, TextureUsageFlags::UnorderedAccess);
			pass.setExecuteFunc(
				[=](RenderRequest& rdReq)
				{
					auto mtl = mtlClearVoxelClipmap;

					bool clearAll = false;
					auto	imageExtent		= clearAll ? voxelTexColor.texture3D()->size() : Vec3u{ voxelResolutionWithBorder, voxelResolutionWithBorder, voxelResolutionWithBorder };
					Vec3u	threadGroup		= (Vec3u::s_one() * 8);
					Vec3f   dispatchGroup	= Vec3f::s_cast(imageExtent) / Vec3f::s_cast(threadGroup);

					mtl->setParam("image_extent",			imageExtent);
					mtl->setParam("image_extent_offset",	Vec3u::s_zero());
					mtl->setParam("clear_value",			Vec4f::s_zero());
					mtl->setImage("image",					voxelTexColor.texture3D(), 0);
					setupCommonParam(mtl, level, drawData);
					rdReq.dispatch(RDS_SRCLOC, mtl, Vec3u::s_cast(Vec3f{math::ceil(dispatchGroup.x), math::ceil(dispatchGroup.y), math::ceil(dispatchGroup.z)}));
				}
			);
		}
		#else
		{
			//auto& pass = rdGraph->addPass("vct_clearImage3D", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Transfer);
			//pass.writeTexture(voxelTexColor, TextureUsageFlags::TransferDst);
			auto& pass = rdGraph->addPass("vct_clearImage3D", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
			pass.writeTexture(voxelTexColor, TextureUsageFlags::UnorderedAccess);
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
					setupCommonParam(mtl, 0, drawData);
					rdReq.dispatch(RDS_SRCLOC, mtl, Vec3u::s_cast(Vec3f{math::ceil(dispatchGroup.x), math::ceil(dispatchGroup.y), math::ceil(dispatchGroup.z)}));
				}
			);
		}
		#endif
	}

	String passName = "vct_voxelization";
	{
		fmtToNew(passName, "{}-l{}", "vct_voxelization", level);
		auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Graphics/*, RdgPassFlags::NoRenderTarget*/);
		pass.writeTexture(voxelTexColor, TextureUsageFlags::UnorderedAccess, ShaderStageFlag::Pixel);
		pass.setRenderTarget(rtDummy, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				//auto mtl = mtlVoxelization;
				rdReq.reset(rdGraph->renderContext(), *drawData);
				rdReq.setViewport(		Tuple2f::s_zero(), Vec2f::s_cast(dummmySize));
				rdReq.setScissorRect(	Tuple2f::s_zero(), Vec2f::s_cast(dummmySize));

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				//auto uav = voxelTexColor.texture3D()->uavBindlessHandle().getResourceIndex(0);

				//RDS_LOG("setParam {} - idx: {}", voxelTexColor.name(), voxelTexColor.texture3D()->bindlessHandle().getResourceIndex());
				DrawSettings drawSettings = drawSettings_;
				drawSettings.setMaterialFn = [=](Material* mtl_)
					{
						setupCommonParam(mtl_, level, drawData);
						mtl_->setImage("voxel_tex_albedo",	voxelTexColor.texture3D(), 0);

						/*auto* vTex = mtl_->passes()[0]->shaderResources().findParamT<u32>("voxel_tex_albedo");
						auto mtlUav = vTex ? *vTex : NumLimit<u32>::max();
						if (mtlUav != uav )
						{
						RDS_CORE_LOG_WARN("uav: {} != mtlUav: {}", uav, mtlUav);
						}*/
						//RDS_CORE_LOG_WARN("uav: {}", voxelTexColor.texture3D()->uavBindlessHandle().getResourceIndex(0));
					};

				auto clipmapRegion = VctVoxelClipmapUtil::makeClipmapRegion(level, clipmapRegionWorldExtentL0, voxelResolution);
				drawSettings.cullingSetting.setAABBox(VctVoxelClipmapUtil::computeAABBox(clipmapRegion, drawData->camera->pos(), voxelResolution));

				//RDS_LOG_ERROR("Begin {}", passName);
				drawData->drawScene(rdReq, drawSettings);
				//RDS_LOG_ERROR("=== End {}", passName);
			}
		);
		voxelizationPass = &pass;
	}
	return voxelizationPass;
}

RdgPass* 
RpfVoxelConeTracing::addVoxelizationDebugPass(RdgTextureHnd rtColor, RdgTextureHnd dsBuf)
{
	auto*		rdGraph					= oRdGraph;
	RdgPass*	passVoxelizationDebug	= nullptr;

	auto	screenSize	= Vec2u::s_cast(Vec2f(drawData->resolution())).toTuple2();

	Tuple3u clipmapSizes;
	clipmapSizes.setAll(voxelResolution);
	clipmapSizes.set(screenSize.x, screenSize.y, 4);

	RdgTextureHnd voxelTexColor			= result.voxelTexColor;		// must copy here

	auto level = curLevel;

	{
		auto& pass = rdGraph->addPass("vct_voxelization_debug", RdgPassTypeFlags::Graphics);

		setupPassReadMipmap(pass, ShaderStageFlag::Pixel);
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

				setupCommonParam(mtl, level, drawData);
				setupMipmapParam(mtl);

				float scale = 32.0f;
				mtl->setParam("matrix_world",		Mat4f::s_TRS(Vec3f{0.0f, scale, -10.0f}, Quat4f::s_eulerDegX(90.0f), Vec3f::s_one() * scale));

				mtl->setParam("voxel_extent",		1.0f);
				mtl->setParam("voxel_resolution",	voxelResolution);
				mtl->setParam("voxel_tex3D",		voxelTexColor.texture3D());

				drawData->setupMaterial(mtl);
				rdReq.drawMesh(RDS_SRCLOC, drawData->meshAssets->plane->renderMesh, mtl);
			}
		);
		passVoxelizationDebug = &pass;
	}
	return passVoxelizationDebug;
}

RdgPass* 
RpfVoxelConeTracing::addVoxelVisualizationPass(RdgTextureHnd rtColor, RdgTextureHnd dsBuf)
{
	auto*		rdGraph					= oRdGraph;
	RdgPass*	passVoxelVisualization	= nullptr;

	auto	screenSize	= Vec2u::s_cast(Vec2f(drawData->resolution())).toTuple2();

	Tuple3u clipmapSizes;
	clipmapSizes.setAll(voxelResolution);
	clipmapSizes.set(screenSize.x, screenSize.y, 4);

	RdgTextureHnd voxelTexColor			= result.voxelTexColor;		// must copy here

	TempString passName = "vct_voxel_visualization";
	for (u32 i = 0; i < 1; ++i)
	{
		fmtToNew(passName, "{}-l{}", "vct_voxel_visualization", i);
		auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Graphics);
		pass.readTexture(voxelTexColor,			TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);
		setupPassReadMipmap(pass, ShaderStageFlag::Pixel);

		pass.setRenderTarget(rtColor,	RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		//pass.setDepthStencil(dsBuf,		RdgAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setDepthStencil(dsBuf,		RdgAccess::Write, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = mtlVoxelVisualization;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				setupCommonParam(mtl, visualizeLevel, drawData);

				mtl->setParam("voxel_tex3D",		voxelTexColor.texture3D());

				drawData->setupMaterial(mtl);
				setupMipmapParam(mtl);

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
			auto& pass = rdGraph->addPass("vct_anisotropic_pre_mipmapping", RdgPassTypeFlags::Compute | RdgPassTypeFlags::Graphics);
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

					setupCommonParam(mtl, mipLevel, drawData);
					rdReq.dispatch(RDS_SRCLOC, mtl, 0, Vec3u::s_cast(Vec3f{math::ceil(dispatchGroup.x), math::ceil(dispatchGroup.y), math::ceil(dispatchGroup.z)}));
				}
			);
			lastPass = &pass;
		}

		for (u32 i = 1; i < clipmapMaxLevel; i++)
		{
			fmtToNew(passName, "vct_anisotropic_mipmapping-mip{}", i);
			auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Compute | RdgPassTypeFlags::Graphics);
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

					setupCommonParam(mtl, mipLevel, drawData);
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

RdgPass* 
RpfVoxelConeTracing::addVoxelConeTracingPass(RpfGeometryBuffer_Result& gBuf, RdgTextureHnd depth)
{
	bool isUsingCompute = true;

	auto*		rdGraph					= oRdGraph;
	RdgPass*	passVoxelConeTracing	= nullptr;

	auto	screenSize	= Vec2u::s_cast(Vec2f(drawData->resolution())).toTuple2();

	result.texDiffuse	= rdGraph->createTexture("vct_diffuse",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAb,	TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::RenderTarget | TextureUsageFlags::TransferDst});
	result.texSpecular	= rdGraph->createTexture("vct_specular",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb,	TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::RenderTarget | TextureUsageFlags::TransferDst});

	auto texDiffuse		= result.texDiffuse;
	auto texSpecular	= result.texSpecular;

	{
		{
			//auto& pass = rdGraph->addPass(fmtAs_T<TempString>("clearImage2D_{}", texDiffuse.name()), RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Transfer);
			//pass.writeTexture(texDiffuse, TextureUsageFlags::TransferDst);
			auto& pass = rdGraph->addPass(fmtAs_T<TempString>("clearImage2D_{}", texDiffuse.name()), RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
			pass.writeTexture(texDiffuse, TextureUsageFlags::UnorderedAccess);
			pass.setExecuteFunc(
				[=](RenderRequest& rdReq)
				{
					auto mtl = mtlClearImage2D[0];

					auto	imageExtent		= texDiffuse.size();
					Vec3u	threadGroup		= (Vec3u::s_one() * 8);
					Vec3f   dispatchGroup	= Vec3f::s_cast(imageExtent) / Vec3f::s_cast(threadGroup);

					mtl->setParam("image_extent",			imageExtent);
					mtl->setParam("image_extent_offset",	Vec3u::s_zero());
					mtl->setParam("clear_value",			Vec4f::s_zero());
					mtl->setImage("image",					texDiffuse.texture2D(), 0);
					drawData->setupMaterial(mtl);
					rdReq.dispatch(RDS_SRCLOC, mtl, Vec3u::s_cast(Vec3f{math::ceil(dispatchGroup.x), math::ceil(dispatchGroup.y), math::ceil(dispatchGroup.z)}));
				}
			);
		}

		{
			//auto& pass = rdGraph->addPass(fmtAs_T<TempString>("clearImage2D_{}", texSpecular.name()), RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Transfer);
			//pass.writeTexture(texSpecular, TextureUsageFlags::TransferDst);
			auto& pass = rdGraph->addPass(fmtAs_T<TempString>("clearImage2D_{}", texSpecular.name()), RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
			pass.writeTexture(texSpecular, TextureUsageFlags::UnorderedAccess);
			pass.setExecuteFunc(
				[=](RenderRequest& rdReq)
				{
					auto mtl = mtlClearImage2D[1];

					auto	imageExtent		= texSpecular.size();
					Vec3u	threadGroup		= (Vec3u::s_one() * 8);
					Vec3f   dispatchGroup	= Vec3f::s_cast(imageExtent) / Vec3f::s_cast(threadGroup);

					mtl->setParam("image_extent",			imageExtent);
					mtl->setParam("image_extent_offset",	Vec3u::s_zero());
					mtl->setParam("clear_value",			Vec4f::s_zero());
					mtl->setImage("image",					texSpecular.texture2D(), 0);
					drawData->setupMaterial(mtl);
					rdReq.dispatch(RDS_SRCLOC, mtl, Vec3u::s_cast(Vec3f{math::ceil(dispatchGroup.x), math::ceil(dispatchGroup.y), math::ceil(dispatchGroup.z)}));
				}
			);
		}
	}

	{
		auto rdgPassTypeFlag = isUsingCompute ? RdgPassTypeFlags::Compute | RdgPassTypeFlags::Graphics : RdgPassTypeFlags::Compute | RdgPassTypeFlags::Graphics;
		auto shaderStageFlag = isUsingCompute ? ShaderStageFlag::Compute : ShaderStageFlag::Pixel;

		auto& pass = rdGraph->addPass("vct_voxel_cone_tracing", rdgPassTypeFlag);
		pass.readTexture(result.voxelTexColor,			TextureUsageFlags::ShaderResource, shaderStageFlag);

		#if !RDS_VCT_USE_6_FACES_CLIPMAP
		pass.readTexture(result.voxelTexColorMip_PosX,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(result.voxelTexColorMip_NegX,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(result.voxelTexColorMip_PosY,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(result.voxelTexColorMip_NegY,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(result.voxelTexColorMip_PosZ,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(result.voxelTexColorMip_NegZ,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		#endif // !RDS_VCT_USE_6_FACES_CLIPMAP

		pass.readTexture(gBuf.albedo,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(gBuf.normal,	TextureUsageFlags::ShaderResource, shaderStageFlag);
		pass.readTexture(depth,			TextureUsageFlags::ShaderResource, shaderStageFlag);

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
				auto mtl = mtlVoxelConeTracing;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				if (isUsingCompute)
				{
					mtl->setImage("out_indirect_diffuse",	texDiffuse.texture2D(),  0);
					mtl->setImage("out_indirect_specular",	texSpecular.texture2D(), 0);
				}

				mtl->setParam("voxel_tex3D",		result.voxelTexColor.texture3D());

				mtl->setParam("tex_depth",			depth.texture2D());
				mtl->setParam("gBuf_normal",		gBuf.normal.texture2D());
				mtl->setParam("gBuf_albedo",		gBuf.albedo.texture2D());
				mtl->setParam("gBuf_position",		gBuf.debugPosition.texture2D());


				Vec3u	threadGroup		= (Vec3u::s_one() * 8);
				Vec3f   dispatchGroup	= Vec3f::s_cast(Vec3u{screenSize, 1}) / Vec3f::s_cast(threadGroup);

				setupCommonParam(mtl, curLevel, drawData);
				setupMipmapParam(mtl);
				drawData->setupMaterial(mtl);
				if (isUsingCompute)
				{
					rdReq.dispatch(RDS_SRCLOC, mtl, 0, Vec3u::s_cast(Vec3f{math::ceil(dispatchGroup.x), math::ceil(dispatchGroup.y), math::ceil(dispatchGroup.z)}));
				}
				else
				{
					auto drawCall = rdReq.addDrawCall();
					drawCall->setDebugSrcLoc(RDS_SRCLOC);
					drawCall->vertexCount = 3;
					drawCall->setMaterial(mtl);
				}
			}
		);
		passVoxelConeTracing = &pass;
	}

	return passVoxelConeTracing;
}

void 
RpfVoxelConeTracing::setupCommonParam(Material* mtl, u32 level, DrawData* drawData)
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
}

void
RpfVoxelConeTracing::setupMipmapParam(Material* mtl)
{
	#if !RDS_VCT_USE_6_FACES_CLIPMAP
	mtl->setParam("voxel_tex_pos_x",	result.voxelTexColorMip_PosX.texture3D());
	mtl->setParam("voxel_tex_neg_x",	result.voxelTexColorMip_NegX.texture3D());
	mtl->setParam("voxel_tex_pos_y",	result.voxelTexColorMip_PosY.texture3D());
	mtl->setParam("voxel_tex_neg_y",	result.voxelTexColorMip_NegY.texture3D());
	mtl->setParam("voxel_tex_pos_z",	result.voxelTexColorMip_PosZ.texture3D());
	mtl->setParam("voxel_tex_neg_z",	result.voxelTexColorMip_NegZ.texture3D());
	#endif // !RDS_VCT_USE_6_FACES_CLIPMAP
}

void 
RpfVoxelConeTracing::setupPassReadMipmap(RdgPass& pass, ShaderStageFlag stage)
{
	#if !RDS_VCT_USE_6_FACES_CLIPMAP
	pass.readTexture(result.voxelTexColorMip_PosX,	TextureUsageFlags::ShaderResource, stage);
	pass.readTexture(result.voxelTexColorMip_NegX,	TextureUsageFlags::ShaderResource, stage);
	pass.readTexture(result.voxelTexColorMip_PosY,	TextureUsageFlags::ShaderResource, stage);
	pass.readTexture(result.voxelTexColorMip_NegY,	TextureUsageFlags::ShaderResource, stage);
	pass.readTexture(result.voxelTexColorMip_PosZ,	TextureUsageFlags::ShaderResource, stage);
	pass.readTexture(result.voxelTexColorMip_NegZ,	TextureUsageFlags::ShaderResource, stage);
	#endif
}

#endif

}