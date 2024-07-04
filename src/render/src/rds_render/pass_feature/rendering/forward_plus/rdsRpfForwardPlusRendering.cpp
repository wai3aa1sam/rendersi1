#include "rds_render-pch.h"
#include "rdsRpfForwardPlusRendering.h"

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
#pragma mark --- rdsRpfForwardPlusRendering-Impl ---
#endif // 0
#if 1

void 
RpfForwardPlusRendering::getMakeFrustumsThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 blockSize)
{
	Vec2f nThreads_f	= resolution_ / sCast<float>(blockSize);
	Vec2f nThreadGrps_f	= nThreads_f  / sCast<float>(blockSize);

	*oNThreads		= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreads_f.x),	math::ceilToInt(nThreads_f.y)});
	*oNThreadGrps	= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreadGrps_f.x),	math::ceilToInt(nThreadGrps_f.y)});
}

void 
RpfForwardPlusRendering::getLightCulllingThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 blockSize)
{
	Vec2f nThreadGrps_f	= resolution_ / sCast<float>(blockSize);

	*oNThreadGrps	= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreadGrps_f.x),	math::ceilToInt(nThreadGrps_f.y)});
	*oNThreads		= *oNThreadGrps * blockSize;
}

RpfForwardPlusRendering::RpfForwardPlusRendering()
{

}

RpfForwardPlusRendering::~RpfForwardPlusRendering()
{
	destroy();
}

void 
RpfForwardPlusRendering::create()
{
	RenderUtil::createMaterial(&_shaderMakeFrustums,	&_mtlMakeFrustums,	"asset/shader/pass_feature/rendering/forward_plus/rdsFwdp_MakeFrustums.shader");
	RenderUtil::createMaterial(&_shaderLightsCulling,	&_mtlLightsCulling,	"asset/shader/pass_feature/rendering/forward_plus/rdsFwdp_LightsCulling.shader");
	RenderUtil::createMaterial(&_shaderLighting,		&_mtlLighting,		"asset/shader/pass_feature/rendering/forward_plus/rdsFwdp_Lighting.shader");

	RenderUtil::createShader(&_shaderClearBuffer,	"asset/shader/pass_feature/rendering/forward_plus/rdsFwdp_ClearBuffer.shader");
	RenderUtil::createShader(&_shaderDebugBuffer,	"asset/shader/pass_feature/rendering/forward_plus/rdsFwdp_DebugBuffer.shader");
}

void 
RpfForwardPlusRendering::destroy()
{
	RenderUtil::destroyShaderMaterial(_shaderMakeFrustums,	_mtlMakeFrustums);
	RenderUtil::destroyShaderMaterial(_shaderLightsCulling, _mtlLightsCulling);
	RenderUtil::destroyShaderMaterial(_shaderLighting,		_mtlLighting);
	RenderUtil::destroyShaderMaterial(_shaderDebugBuffer,	_mtlDebugBuffer);

	RenderUtil::destroyShader(_shaderClearBuffer);

	RenderUtil::destroyMaterial(_mtlClearOpaqueLightGrid);
	RenderUtil::destroyMaterial(_mtlClearOpaqueLightIndexCounter);
	RenderUtil::destroyMaterial(_mtlClearOpaqueLightIndexList);
	RenderUtil::destroyMaterial(_mtlClearTransparentLightGrid);
	RenderUtil::destroyMaterial(_mtlClearTransparentLightIndexCounter);
	RenderUtil::destroyMaterial(_mtlClearTransparentLightIndexList);

	RenderUtil::destroyMaterial(_mtlClearDebugBuffer0);
	RenderUtil::destroyMaterial(_mtlClearDebugBuffer1);
}

RdgPass* 
RpfForwardPlusRendering::addMakeFrustumsPass(RdgBufferHnd& oBufFrustums)
{
	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passMakeFrustums	= nullptr;

	using Frustum			= shaderInterop::Frustum;
	using DebugFrustumPts	= shaderInterop::DebugFrustrumPts;

	RdgBufferHnd& bufFrustums = oBufFrustums;
	if (!(isInvalidate(drawData->resolution()) && drawData->drawParamIdx == 0) && true)
		return nullptr;

	resolution = drawData->resolution();

	Vec2u nThreads, nThreadGrps;
	getMakeFrustumsThreadParamTo(&nThreads, &nThreadGrps, resolution, s_kBlockSize);

	auto n = nThreads.x * nThreads.y;
	bufFrustums = rdGraph->createBuffer("fwdp_buf_frustums"
		, RenderGpuBuffer_CreateDesc{ sizeof(Frustum) * n, sizeof(Frustum), RenderGpuBufferTypeFlags::Compute});

	RdgBufferHnd bufDebugFrustumsPts = rdGraph->createBuffer("fwdp_buf_debugFrustumsPts"
		, RenderGpuBuffer_CreateDesc{ sizeof(DebugFrustumPts) * n, sizeof(DebugFrustumPts), RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex});

	Material* mtl = _mtlMakeFrustums;
	{
		auto& pass = rdGraph->addPass("fwdp_make_frustums", RdgPassTypeFlags::Compute);
		pass.writeBuffer(bufFrustums);
		pass.writeBuffer(bufDebugFrustumsPts);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				mtl->setParam("nThreads",			Vec3u{nThreads,		1});
				mtl->setParam("nThreadGroups",		Vec3u{nThreadGrps,	1});
				mtl->setParam("oFrustums",			bufFrustums.renderResource());
				mtl->setParam("oDebugFrustumsPts",	bufDebugFrustumsPts.renderResource());
				drawData->setupMaterial(mtl);
				rdReq.dispatch(RDS_SRCLOC, mtl, 0, Vec3u{nThreadGrps, 1});
			}
		);
		passMakeFrustums = &pass;
	}

	rdGraph->exportBuffer(&gpuFrustums,			bufFrustums,			RenderGpuBufferTypeFlags::Compute);
	rdGraph->exportBuffer(&debugFrustumsPts,	bufDebugFrustumsPts,	RenderGpuBufferTypeFlags::Vertex);

	{
		auto& indices	= s_debugFrustumIndices;
		auto idxCount	= arraySize(indices);

		using IdxType = RpfForwardPlusRendering::DebugIndexType;
		Vector<IdxType> indiceData;
		indiceData.resize(idxCount * n);
		for (u32 i = 0; i < n; i++)
		{
			for (u32 iIdx = 0; iIdx < idxCount; iIdx++)
			{
				IdxType idxOffset				= i * sCast<IdxType>(idxCount);
				IdxType idxValueOffset			= i * sCast<IdxType>(ArraySize<decltype(DebugFrustumPts::pts) >);
				indiceData[idxOffset + iIdx]	= indices[iIdx] + idxValueOffset;
			}
		}

		auto data = indiceData.byteSpan();
		auto cDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
		cDesc.bufSize	= data.size();
		cDesc.stride	= sizeof(IdxType);
		cDesc.typeFlags = RenderGpuBufferTypeFlags::Index;
		debugFrustumsIdxBuf = rdGraph->renderContext()->renderDevice()->createRenderGpuBuffer(cDesc);
		debugFrustumsIdxBuf->setDebugName("debugFrustumsIdxBuf");
		debugFrustumsIdxBuf->uploadToGpu(data);
	}

	return passMakeFrustums;
}

RdgPass*
RpfForwardPlusRendering::addLightCullingPass(Result& oResult, RdgBufferHnd frustumsHnd, RdgTextureHnd texDepth)
{
	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passLightCulling	= nullptr;

	if (!frustumsHnd && !gpuFrustums)
		return passLightCulling;

	Vec2u nThreads, nThreadGrps;
	getLightCulllingThreadParamTo(&nThreads, &nThreadGrps, resolution, s_kBlockSize);

	auto uintSize = RenderDataTypeUtil::getByteSize(RenderDataType::UInt32);
	//auto n		= nThreads.x	* nThreads.y;
	auto nGrps				= nThreadGrps.x * nThreadGrps.y;
	auto lightIdxListSize	= s_kEstLightPerGrid * nGrps;

	RdgBufferHnd	opaque_lightIndexCounter		= rdGraph->createBuffer( "fwdp_opaque_lightIndexCounter",		RenderGpuBuffer_CreateDesc{ 1					* uintSize, uintSize, RenderGpuBufferTypeFlags::Compute});
	RdgBufferHnd	opaque_lightIndexList			= rdGraph->createBuffer( "fwdp_opaque_lightIndexList",			RenderGpuBuffer_CreateDesc{ lightIdxListSize	* uintSize, uintSize, RenderGpuBufferTypeFlags::Compute});
	RdgTextureHnd	opaque_lightGrid				= rdGraph->createTexture("fwdp_opaque_lightGrid",				Texture2D_CreateDesc{		nThreadGrps, ColorType::RGu, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::RenderTarget});

	RdgBufferHnd	transparent_lightIndexCounter	= rdGraph->createBuffer( "fwdp_transparent_lightIndexCounter",	RenderGpuBuffer_CreateDesc{ 1					* uintSize, uintSize, RenderGpuBufferTypeFlags::Compute});
	RdgBufferHnd	transparent_lightIndexList		= rdGraph->createBuffer( "fwdp_transparent_lightIndexList",		RenderGpuBuffer_CreateDesc{ lightIdxListSize	* uintSize, uintSize, RenderGpuBufferTypeFlags::Compute});
	RdgTextureHnd	transparent_lightGrid			= rdGraph->createTexture("fwdp_transparent_lightGrid",			Texture2D_CreateDesc{		nThreadGrps, ColorType::RGu, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::RenderTarget});

	_addClearImage2DPass(	_mtlClearOpaqueLightGrid,				opaque_lightGrid);
	addClearBufferPass(		_mtlClearOpaqueLightIndexCounter,		opaque_lightIndexCounter);
	addClearBufferPass(		_mtlClearOpaqueLightIndexList,			opaque_lightIndexList);
	_addClearImage2DPass(	_mtlClearTransparentLightGrid,			transparent_lightGrid);
	addClearBufferPass(		_mtlClearTransparentLightIndexCounter,	transparent_lightIndexCounter);
	addClearBufferPass(		_mtlClearTransparentLightIndexList,		transparent_lightIndexList);

	Material* mtl = _mtlLightsCulling;
	{
		auto& pass = rdGraph->addPass("fwdp_light_culling", RdgPassTypeFlags::Compute);
		if (frustumsHnd)
			pass.readBuffer(frustumsHnd);

		//pass.readTexture(texDepth);
		pass.readTexture(texDepth, TextureUsageFlags::ShaderResource, ShaderStageFlag::Compute);
		pass.writeBuffer(opaque_lightIndexCounter);
		pass.writeBuffer(opaque_lightIndexList);
		pass.writeTexture(opaque_lightGrid);
		pass.writeBuffer(transparent_lightIndexCounter);
		pass.writeBuffer(transparent_lightIndexList);
		pass.writeTexture(transparent_lightGrid);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				mtl->setParam("nThreads",			Vec3u{nThreads,		1});
				mtl->setParam("nThreadGroups",		Vec3u{nThreadGrps,	1});

				mtl->setParam("texDepth", texDepth.texture2D());
				mtl->setParam("frustums", frustumsHnd ? frustumsHnd.renderResource() : gpuFrustums);

				#if 0
				{
					RDS_TODO("temporary solution for set data to 0");
					Vector<u8, 64> data;
					data.resize(opaque_lightIndexCounter.renderResource()->bufSize());
					opaque_lightIndexCounter.renderResource()->uploadToGpu(data.byteSpan());
					transparent_lightIndexCounter.renderResource()->uploadToGpu(data.byteSpan());

					data.resize(opaque_lightIndexList.renderResource()->bufSize());
					for (auto& e : data)
					{
						RDS_CORE_ASSERT(e == 0, "init failed");
					}
					opaque_lightIndexList.renderResource()->uploadToGpu(data.byteSpan());
					transparent_lightIndexList.renderResource()->uploadToGpu(data.byteSpan());
				}
				#endif // 0

				mtl->setParam("opaque_lightIndexCounter",		opaque_lightIndexCounter.renderResource());
				mtl->setParam("opaque_lightIndexList",			opaque_lightIndexList.renderResource());
				mtl->setImage("opaque_lightGrid",				opaque_lightGrid.renderResource(), 0);

				mtl->setParam("transparent_lightIndexCounter",	transparent_lightIndexCounter.renderResource());
				mtl->setParam("transparent_lightIndexList",		transparent_lightIndexList.renderResource());
				mtl->setImage("transparent_lightGrid",			transparent_lightGrid.renderResource(), 0);

				drawData->setupMaterial(mtl);
				rdReq.dispatch(RDS_SRCLOC, mtl, 0, Vec3u{nThreadGrps, 1});
			}
		);
		passLightCulling = &pass;
	}

	Result& result = oResult;
	result.lightCullingPass				= passLightCulling;
	result.opaque_lightIndexList		= opaque_lightIndexList;
	result.opaque_lightGrid				= opaque_lightGrid;
	result.transparent_lightIndexList	= transparent_lightIndexList;
	result.transparent_lightGrid		= transparent_lightGrid;

	return passLightCulling;
}

RdgPass*
RpfForwardPlusRendering::addLightCullingPass(Result& oResult, RdgTextureHnd texDepth)
{
	RdgPass*	passLightCulling	= nullptr;

	RdgBufferHnd bufFrustums;
	addMakeFrustumsPass(bufFrustums);
	passLightCulling = addLightCullingPass(oResult, bufFrustums, texDepth);
	return passLightCulling;
}

RdgPass*
RpfForwardPlusRendering::addLightingPass(RdgTextureHnd rtColor, RdgTextureHnd dsBuf, Result& result, bool isOpaque, bool isClearColor)
{
	auto*		rdGraph			= renderGraph();
	auto*		drawData		= drawDataBase();
	RdgPass*	passLighting	= nullptr;

	if (!result.lightCullingPass)
		return passLighting;


	Material* mtl = _mtlLighting;
	{
		auto& pass = rdGraph->addPass("fwdp_lighting", RdgPassTypeFlags::Graphics);

		RdgBufferHnd	lightIndexList	= result.lightIndexList(isOpaque);
		RdgTextureHnd	lightGrid		= result.lightGrid(isOpaque);

		//RDS_CORE_LOG_WARN("--- --- before RdGraph Execute frame: {}", rdGraph->iFrame());

		pass.readBuffer(result.lightIndexList(isOpaque));
		pass.readTexture(result.lightGrid(isOpaque));
		pass.setRenderTarget(rtColor,	isClearColor ? RenderTargetLoadOp::Clear : RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf,	RdgAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				//clearValue->setClearDepth();

				#if RDS_DEBUG_MATERIAL
				RDS_CORE_LOG_WARN("--- --- after RdGraph Execute frame: {}", rdGraph->iFrame());

				{
					auto* mtlPass = mtl->passes()[0].ptr();
					RDS_CORE_LOG_WARN("--- --- before mtlPass set, frame: {}", mtlPass->iFrame());

					auto* lightIndexList_	= mtlPass->shaderResources().findParamT<u32>("lightIndexList");
					auto* lightGrid_		= mtlPass->shaderResources().findParamT<u32>("lightGrid");

					if (lightIndexList_ && lightGrid_)
					{
						RDS_DUMP_VAR(*lightIndexList_, *lightGrid_);
					}
				}
				#endif // 0

				mtl->setParam("lightIndexList",		lightIndexList.renderResource());
				mtl->setParam("lightGrid",			lightGrid.renderResource());

				#if RDS_DEBUG_MATERIAL
				{
					auto* mtlPass = mtl->passes()[0].ptr();
					RDS_CORE_LOG_WARN("--- after mtlPass set, frame: {}", mtlPass->iFrame());

					auto* lightIndexList_	= mtlPass->shaderResources().findParamT<u32>("lightIndexList");
					auto* lightGrid_		= mtlPass->shaderResources().findParamT<u32>("lightGrid");

					if (lightIndexList_ && lightGrid_)
					{
						RDS_DUMP_VAR(*lightIndexList_, *lightGrid_);
					}
				}
				#endif // RDS_DEBUG_MATERIAL

				drawData->drawScene(rdReq, mtl);
			}
		);
		passLighting = &pass;
	}
	return passLighting;
}

RdgPass*
RpfForwardPlusRendering::addRenderDebugFrustumsPass(RdgTextureHnd rtColor)
{
	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passRenderFrustums	= nullptr;

	if (!debugFrustumsPts || !debugFrustumsIdxBuf)
		return passRenderFrustums;

	using DebugFrustumPts	= shaderInterop::DebugFrustrumPts;

	Vec2u nThreads, nThreadGrps;
	getMakeFrustumsThreadParamTo(&nThreads, &nThreadGrps, resolution, s_kBlockSize);
	auto n = nThreads.x * nThreads.y;

	Material* mtl = _mtlMakeFrustums;
	#if 1
	{
		auto& pass = rdGraph->addPass("fwdp_render_debug_frustums", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(rtColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				auto* drawCall = rdReq.addDrawCall();
				drawCall->vertexLayout			= VertexT_Pos<Tuple4f>::vertexLayout();
				drawCall->indexType				= RenderDataTypeUtil::get<RpfForwardPlusRendering::DebugIndexType>();
				drawCall->vertexCount			= ArraySize<decltype(DebugFrustumPts::pts)> * n;
				drawCall->indexCount			= arraySize(s_debugFrustumIndices) * n;
				drawCall->vertexBuffer			= debugFrustumsPts;
				drawCall->indexBuffer			= debugFrustumsIdxBuf;

				drawCall->vertexOffset			= 0;
				drawCall->indexOffset			= 0;

				drawCall->renderPrimitiveType	= RenderPrimitiveType::Line;
				drawCall->setMaterial(mtl);

				drawData->setupMaterial(mtl);
				RDS_CORE_ASSERT(drawCall->vertexCount <= NumLimit<RpfForwardPlusRendering::DebugIndexType>::max(), "vertex count > index max count");
			}
		);
		passRenderFrustums = &pass;
	}
	#endif // 1

	return passRenderFrustums;
}

RdgPass* 
RpfForwardPlusRendering::addClearBufferPass(SPtr<Material>& material, RdgBufferHnd buffer)
{
	RenderUtil::createMaterial(_shaderClearBuffer, &material);

	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passClearBuffer	= nullptr;

	Material* mtl = material;
	{
		auto passName = fmtAs_T<TempString>("clearBuffer-{}", buffer.name());
		auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
		pass.writeBuffer(buffer);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto buferElementCount = sCast<u32>(buffer.desc().bufSize / buffer.desc().stride);

				mtl->setParam("buffer_size",			buferElementCount);
				mtl->setParam("clear_value",			u32(0));
				mtl->setParam("buffer",					buffer.renderResource());
				drawData->setupMaterial(mtl);
				rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, Vec3u{buferElementCount, 1, 1});
			}
		);
		passClearBuffer = &pass;
	}

	return passClearBuffer;
}

RdgPass* 
RpfForwardPlusRendering::addFwdpDebugBufferPass()
{
	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passFwdpDebugBuffer	= nullptr;

	Material* mtl = _mtlDebugBuffer;

	Vec3u	dispatchGrp = Vec3u{4, 1, 1};
	u32		offsetCount = dispatchGrp.x * mtl->getPass(0)->computeStage()->info().numthreads().x;

	auto uintSize = RenderDataTypeUtil::getByteSize(RenderDataType::UInt32);
	RdgBufferHnd	counter		= rdGraph->createBuffer( "counter",		RenderGpuBuffer_CreateDesc{ 1			* uintSize, uintSize, RenderGpuBufferTypeFlags::Compute});
	RdgBufferHnd	offsets		= rdGraph->createBuffer( "offsets",		RenderGpuBuffer_CreateDesc{ offsetCount	* uintSize, uintSize, RenderGpuBufferTypeFlags::Compute});

	RenderUtil::createMaterial(_shaderClearBuffer, &_mtlClearDebugBuffer0);
	RenderUtil::createMaterial(_shaderClearBuffer, &_mtlClearDebugBuffer1);
	addClearBufferPass(_mtlClearDebugBuffer0, counter);
	addClearBufferPass(_mtlClearDebugBuffer1, offsets);

	{
		auto& pass = rdGraph->addPass("fwdp_debug", RdgPassTypeFlags::Compute);
		pass.writeBuffer(counter);
		pass.writeBuffer(offsets);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				mtl->setParam("counter",		counter.renderResource());
				mtl->setParam("offsets",		offsets.renderResource());

				drawData->setupMaterial(mtl);
				rdReq.dispatch(RDS_SRCLOC, mtl, 0, dispatchGrp);
			}
		);
		passFwdpDebugBuffer = &pass;
	}

	return passFwdpDebugBuffer;
}

bool RpfForwardPlusRendering::isInvalidate(const Vec2f& resoluton_) const { return !math::equals(resolution, resoluton_) && !math::equals0(resoluton_); }

#endif


}