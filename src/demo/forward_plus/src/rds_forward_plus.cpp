#include "rds_forward_plus-pch.h"

#include "rds_forward_plus.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"

#include "rds_render/pass_feature/geometry/rdsRpfPreDepth.h"

namespace rds
{
namespace shaderInterop
{
#include "../../../built-in/shader/geometry/rdsGeometryPrimitive.hlsl"
}
}

namespace rds
{

#define RDS_DEBUG_MATERIAL 0


#if 0
#pragma mark --- rdsForwardPlus-Impl ---
#endif // 0
#if 1

static constexpr int N = 100;
static int arr[N];

void 
ForwardPlus::onCreate()
{
	#if 0
	_rdPassPipelines.emplace_back(makeUPtr<RenderPassPipeline>());
	if (true)
	{
		return;
	}
	#endif // 0

	Base::onCreate();

	auto& rdPassPipeline = *_rdPassPipelines[0];

	_rpfPreDepth	= rdPassPipeline.addRenderPassFeature<RpfPreDepth>();
	_rpfForwardPlus = rdPassPipeline.addRenderPassFeature<RpfForwardPlus>();

	// just for test address sanitizers
	#if 0
	for (size_t i = 0; i < N; i++)
	{
		arr[i] = sCast<int>(i);
}
	arr[1000] = 0;
	#endif // 0
}

void 
ForwardPlus::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);

	#if 1
	{
		auto& scene = *oScene;
		auto* ent = scene.addEntity("Debug Frustum"); RDS_UNUSED(ent);

		//auto* rdableMesh = ent->addComponent<CRenderableMesh>();
		//rdableMesh->material	= _rfpForwardPlus->mtlFwdp;
		//rdableMesh->meshAsset	= meshAssets().fullScreenTriangle;

		//auto* transform	= ent->getComponent<CTransform>();
		//transform->setLocalPosition(startPos.x + step.x * c, 0.0f, startPos.y + step.y * r);
	}
	{
		auto& scene = *oScene;
		auto* ent = scene.addEntity("fwdp culling"); RDS_UNUSED(ent);
		auto* rdableMesh = ent->addComponent<CRenderableMesh>();
		rdableMesh->material	= _rpfForwardPlus->_mtlFwdp;
	}
	#endif // 0

	createDefaultScene(oScene, nullptr, meshAssets().plane, Vec3u::s_one());
	for (auto& e : scene().entities())
	{
		auto& transform = e->transform();
		float scaleFactor = 64.0f; RDS_UNUSED(scaleFactor);
		transform.setLocalScale(Vec3f{ scaleFactor, 1.0, scaleFactor });
	}

	{
		auto& camera = app().mainWindow().camera();
		//camera.setPos(0, 10, 35);
		camera.setPos(-0.423f, 4.870f, 4.728f);
		camera.setPos(-0.348f, 4.011f, 3.894f);
		//camera.setPos(-0.572f, 6.589f, 6.397f); // threshold to reproduce the bug

		camera.setFov(45.0f);
		camera.setAim(0, 0, 0);
		camera.setNearClip(0.1f);
		camera.setFarClip(1000.0f);

		#if 0
		Vec3f scale;
		Quat4f rotation;
		Vec3f translation;
		Vec3f skew;
		Vec4f perspective;
		glm::decompose(camera.viewMatrix(), scale, rotation, translation, skew, perspective);
		RDS_DUMP_VAR(scale, rotation, translation, skew, perspective);
		RDS_DUMP_VAR(scale, rotation, translation, skew, perspective);
		#endif // 0
	}
}

void 
ForwardPlus::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);
}

void 
ForwardPlus::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);

	#if 0
	if (true)
	{
		return;
	}
	#endif // 0

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData*>();
	auto	screenSize	= drawData->resolution2u();

	//Renderer::rdDev()->waitIdle();

	RdgTextureHnd rtColor		= rdGraph->createTexture("fwdp_rtColor",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf			= rdGraph->createTexture("fwdp_dsBuf",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferSrc});
	RdgTextureHnd texDepth		= rdGraph->createTexture("fwdp_texDepth",	Texture2D_CreateDesc{ screenSize, ColorType::Rf,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource /*| TextureUsageFlags::TransferDst*/});

	RdgPass* fwdpPassFLighting = nullptr;

	_rpfPreDepth->addPreDepthPass({}, dsBuf, &texDepth);

	_rpfForwardPlus->addFwdpDebugPass();

	#if 1
	RpfForwardPlus::Result fwdpResult;
	_rpfForwardPlus->addLightCullingPass(fwdpResult, texDepth);
	fwdpPassFLighting = _rpfForwardPlus->addLightingPass(rtColor, dsBuf, fwdpResult, true, true);
	#endif // 0

	//if (passFwdpLighting)
	//	addSkyboxPass(, skyboxDefault(), rtColor, dsBuf);
	//addPostProcessPass(, "debug_fwdp", rtColor,)

	//_rfpForwardPlus->renderDebugMakeFrustums(, rtColor);
	if (fwdpPassFLighting)
		addDrawLightOutlinePass(rdGraph, drawData, rtColor, nullptr);	

	drawData->oTexPresent = fwdpPassFLighting ? rtColor : RdgTextureHnd{};
}

void 
ForwardPlus::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{

}

void 
ForwardPlus::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);
}

void 
ForwardPlus::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);
}
#endif

#if 0
#pragma mark --- rdsRpfForwardPlus-Impl ---
#endif // 0
#if 1

void 
RpfForwardPlus::getMakeFrustumsThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 blockSize)
{
	Vec2f nThreads_f	= resolution_ / sCast<float>(blockSize);
	Vec2f nThreadGrps_f	= nThreads_f  / sCast<float>(blockSize);

	*oNThreads		= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreads_f.x),	math::ceilToInt(nThreads_f.y)});
	*oNThreadGrps	= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreadGrps_f.x),	math::ceilToInt(nThreadGrps_f.y)});
}

void 
RpfForwardPlus::getLightCulllingThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 blockSize)
{
	Vec2f nThreadGrps_f	= resolution_ / sCast<float>(blockSize);

	*oNThreadGrps	= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreadGrps_f.x),	math::ceilToInt(nThreadGrps_f.y)});
	*oNThreads		= *oNThreadGrps * blockSize;
}

RpfForwardPlus::RpfForwardPlus()
{
	
}

RpfForwardPlus::~RpfForwardPlus()
{
	destroy();
}

void 
RpfForwardPlus::create()
{
	createMaterial(&_shaderFwdpMakeFrustum,		&_mtlFwdpMakeFrustum,	"asset/shader/demo/forward_plus/forward_plus_makeFrustums.shader");
	createMaterial(&_shaderFwdp,				&_mtlFwdp,				"asset/shader/demo/forward_plus/forward_plus.shader");
	createMaterial(&_shaderFwdpLighting,		&_mtlFwdpLighting,		"asset/shader/demo/forward_plus/forward_plus_lighting.shader");

	createMaterial(&_shaderFwdpDebug,			&_mtlFwdpDebug,			"asset/shader/demo/forward_plus/forward_plus_debug.shader");

	createShader(&_shaderClearBuffer, "asset/shader/demo/forward_plus/rdsClearBuffer.shader");
}

void 
RpfForwardPlus::destroy()
{
	_shaderFwdpMakeFrustum.reset(nullptr);
	_mtlFwdpMakeFrustum.reset(nullptr);

	_shaderFwdp.reset(nullptr);
	_mtlFwdp.reset(nullptr);

	_shaderFwdpLighting.reset(nullptr);
	_mtlFwdpLighting.reset(nullptr);

	_mtlClearOpaqueLightGrid.reset(nullptr);
	_mtlClearTransparentLightGrid.reset(nullptr);
}

RdgPass* 
RpfForwardPlus::addFwdpDebugPass()
{
	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passFwdpDebug	= nullptr;

	Material* mtl = _mtlFwdpDebug;

	Vec3u	dispatchGrp = Vec3u{4, 1, 1};
	u32		offsetCount = dispatchGrp.x * mtl->getPass(0)->computeStage()->info().numthreads().x;

	auto uintSize = RenderDataTypeUtil::getByteSize(RenderDataType::UInt32);
	RdgBufferHnd	counter		= rdGraph->createBuffer( "counter",		RenderGpuBuffer_CreateDesc{ 1			* uintSize, uintSize, RenderGpuBufferTypeFlags::Compute});
	RdgBufferHnd	offsets		= rdGraph->createBuffer( "offsets",		RenderGpuBuffer_CreateDesc{ offsetCount	* uintSize, uintSize, RenderGpuBufferTypeFlags::Compute});

	createMaterial(_shaderClearBuffer, &_mtlDebug0);
	createMaterial(_shaderClearBuffer, &_mtlDebug1);
	addClearBufferPass( _mtlDebug0,		counter);
	addClearBufferPass( _mtlDebug1,		offsets);

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
		passFwdpDebug = &pass;
	}

	return passFwdpDebug;
}

RdgPass* 
RpfForwardPlus::addMakeFrustumsPass(RdgBufferHnd& oBufFrustums)
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

	{
		auto& pass = rdGraph->addPass("fwdp_make_frustums", RdgPassTypeFlags::Compute);
		pass.writeBuffer(bufFrustums);
		pass.writeBuffer(bufDebugFrustumsPts);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = _mtlFwdpMakeFrustum;
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

		using IdxType = RpfForwardPlus::DebugIndexType;
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
RpfForwardPlus::addLightCullingPass(Result& oResult, RdgBufferHnd frustumsHnd, RdgTextureHnd texDepth)
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

	addClearImage2DPass(_mtlClearOpaqueLightGrid,				opaque_lightGrid);
	addClearBufferPass( _mtlClearOpaqueLightIndexCounter,		opaque_lightIndexCounter);
	addClearBufferPass( _mtlClearOpaqueLightIndexList,			opaque_lightIndexList);
	addClearImage2DPass(_mtlClearTransparentLightGrid,			transparent_lightGrid);
	addClearBufferPass( _mtlClearTransparentLightIndexCounter,	transparent_lightIndexCounter);
	addClearBufferPass( _mtlClearTransparentLightIndexList,		transparent_lightIndexList);

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
				auto mtl = _mtlFwdp;
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
RpfForwardPlus::addLightCullingPass(Result& oResult, RdgTextureHnd texDepth)
{
	RdgPass*	passLightCulling	= nullptr;

	RdgBufferHnd bufFrustums;
	addMakeFrustumsPass(bufFrustums);
	passLightCulling = addLightCullingPass(oResult, bufFrustums, texDepth);
	return passLightCulling;
}

RdgPass*
RpfForwardPlus::addLightingPass(RdgTextureHnd rtColor, RdgTextureHnd dsBuf, Result& result, bool isOpaque, bool isClearColor)
{
	auto*		rdGraph			= renderGraph();
	auto*		drawData		= drawDataBase();
	RdgPass*	passLighting	= nullptr;

	if (!result.lightCullingPass)
		return passLighting;

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
				auto mtl = _mtlFwdpLighting;
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
RpfForwardPlus::addRenderDebugFrustumsPass(RdgTextureHnd rtColor)
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

	#if 1
	{
		auto& pass = rdGraph->addPass("fwdp_render_debug_frustums", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(rtColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = _mtlFwdpMakeFrustum;
				rdReq.reset(rdGraph->renderContext(), drawData);

				auto* drawCall = rdReq.addDrawCall();
				drawCall->vertexLayout			= VertexT_Pos<Tuple4f>::vertexLayout();
				drawCall->indexType				= RenderDataTypeUtil::get<RpfForwardPlus::DebugIndexType>();
				drawCall->vertexCount			= ArraySize<decltype(DebugFrustumPts::pts)> * n;
				drawCall->indexCount			= arraySize(s_debugFrustumIndices) * n;
				drawCall->vertexBuffer			= debugFrustumsPts;
				drawCall->indexBuffer			= debugFrustumsIdxBuf;

				drawCall->vertexOffset			= 0;
				drawCall->indexOffset			= 0;

				drawCall->renderPrimitiveType	= RenderPrimitiveType::Line;
				drawCall->setMaterial(mtl);

				drawData->setupMaterial(mtl);
				RDS_CORE_ASSERT(drawCall->vertexCount <= NumLimit<RpfForwardPlus::DebugIndexType>::max(), "vertex count > index max count");
			}
		);
		passRenderFrustums = &pass;
	}
	#endif // 1

	return passRenderFrustums;
}

RdgPass* 
RpfForwardPlus::addClearBufferPass(SPtr<Material>& material, RdgBufferHnd buffer)
{
	createMaterial(_shaderClearBuffer, &material);

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

bool RpfForwardPlus::isInvalidate(const Vec2f& resoluton_) const { return !math::equals(resolution, resoluton_) && !math::equals0(resoluton_); }

#endif


}