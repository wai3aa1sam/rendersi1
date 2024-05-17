#include "rds_forward_plus-pch.h"

#include "rds_forward_plus.h"

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
	Base::onCreate();

	createMaterial(&_shaderForwardPlus, &_mtlForwardPlus, "asset/shader/demo/hello_triangle/hello_triangle.shader"
		, [&](Material* mtl) {mtl->setParam("texture0", texUvChecker()); });

	createMaterial(&_shaderPostProcess, &_mtlPostProcess, "asset/shader/present.shader");

	_rfpForwardPlus = makeUPtr<RfpForwardPlus>();

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

	{
		auto& scene = *oScene;
		auto* ent = scene.addEntity("");

		auto* rdableMesh = ent->addComponent<CRenderableMesh>();
		rdableMesh->material	= _rfpForwardPlus->mtlFwdp;
		//rdableMesh->meshAsset	= meshAssets().fullScreenTriangle;

		//auto* transform	= ent->getComponent<CTransform>();
		//transform->setLocalPosition(startPos.x + step.x * c, 0.0f, startPos.y + step.y * r);

		TempString buf;
		fmtTo(buf, "Debug Frustum-{}", sCast<u64>(ent->id()));
		ent->setName(buf);
	}

	createDefaultScene(oScene, nullptr, meshAssets().plane, 1);
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
		//camera.setPos(-0.572f, 6.589f, 6.397f); // threshold to reproduce the bug

		camera.setFov(45.0f);
		camera.setAim(0, 0, 0);
		camera.setNearClip(0.1f);
		camera.setFarClip(1000.0f);

		/*Vec3f scale;
		Quat4f rotation;
		Vec3f translation;
		Vec3f skew;
		Vec4f perspective;
		glm::decompose(camera.viewMatrix(), scale, rotation, translation, skew, perspective);
		RDS_DUMP_VAR(scale, rotation, translation, skew, perspective);
		RDS_DUMP_VAR(scale, rotation, translation, skew, perspective);*/
	}
}

void 
ForwardPlus::onPrepareRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	Base::onPrepareRender(oRdGraph, drawData);
}

void 
ForwardPlus::onExecuteRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	Base::onExecuteRender(oRdGraph, drawData);

	auto*	rdGraph		= oRdGraph;
	auto	screenSize	= Vec2u::s_cast(Vec2f(drawData->resolution())).toTuple2();

	//RdgBufferHnd bufFrustums = _fwpMakeFrustums->onExecuteRender(oRdGraph, drawData);
	//Renderer::rdDev()->waitIdle();

#if 0
	{
		Mat4f viewMatrix = drawData->camera->viewMatrix();
		{
			Vec4f v = viewMatrix * Vec4f(0.0f, 1.0f, 0.0f, 1.0f);
			RDS_DUMP_VAR(v);
			v = viewMatrix * Vec4f(3.0f, 1.0f, 0.0f, 1.0f);
			RDS_DUMP_VAR(v);
		}

		{
			Vec3f v = viewMatrix.mulPoint4x3(Vec3f(0.0f, 1.0f, 0.0f));
			RDS_DUMP_VAR(v);
			v = viewMatrix.mulPoint4x3(Vec3f(3.0f, 1.0f, 0.0f));
			RDS_DUMP_VAR(v);
		}
	}
#endif // 0

	RdgTextureHnd rtColor		= rdGraph->createTexture("forward_plus_rtColor",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf			= rdGraph->createTexture("forward_plus_dsBuf",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferSrc});
	RdgTextureHnd texDepth		= rdGraph->createTexture("forward_plus_texDepth",	Texture2D_CreateDesc{ screenSize, ColorType::Rf,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource /*| TextureUsageFlags::TransferDst*/});

	addPreDepthPass(rdGraph, drawData, &dsBuf, &texDepth, Color4f{1.0f, 0.0f, 0.0f, 1.0f});

	auto		bufFwdpFrsutums			= _rfpForwardPlus->addMakeFrustumsPass(rdGraph, drawData);
	auto		fwdpCullingResult		= _rfpForwardPlus->addLightCullingPass(rdGraph, drawData, bufFwdpFrsutums, texDepth);
	RdgPass*	passFwdpLighting		= _rfpForwardPlus->addLightingPass(rdGraph, drawData, rtColor, dsBuf, fwdpCullingResult, true, true);

	// TODO: remove, just use it for transit then compute use, later add a flag in VkStageAccess to determine the stage
	#if 0
	{
		RdgPass*	passFwdpLightCulling	= fwdpCullingResult.lightCullingPass;

		auto& pass = rdGraph->addPass("TODO: remove, just use it for transit then compute use", RdgPassTypeFlags::Graphics /*| RdgPassTypeFlags::Transfer*/, RdgPassFlags::NoRenderTarget);
		pass.readTexture(texDepth);
		//pass.readTexture(dsBuf);
		//pass.writeTexture(texDepth);
		//pass.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				//rdReq.copyTexture(RDS_SRCLOC, texDepth.renderResource(), dsBuf.renderResource(), 0, 0, 0, 0);
			}
		);
		if (passFwdpLightCulling)
			passFwdpLightCulling->runAfter(&pass);
	}
	#endif // 1

	//if (passFwdpLighting)
	//	addSkyboxPass(rdGraph, drawData, skyboxDefault(), rtColor, dsBuf);
	//addPostProcessPass(rdGraph, drawData, "debug_fwdp", rtColor,)

	//_rfpForwardPlus->renderDebugMakeFrustums(rdGraph, drawData, rtColor);
	if (passFwdpLighting)
		addDrawLightOutlinePass(rdGraph, drawData, rtColor, nullptr);	

	drawData->oTexPresent = passFwdpLighting ? rtColor : RdgTextureHnd{};
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
#pragma mark --- rdsRfpForwardPlus-Impl ---
#endif // 0
#if 1

void 
RfpForwardPlus::getMakeFrustumsThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 blockSize)
{
	/*
	*	1 block for 1 furstum == 1 thread for 1 frustum
	*/

	Vec2f nThreads_f	= resolution_ / sCast<float>(blockSize);
	Vec2f nThreadGrps_f	= nThreads_f  / sCast<float>(blockSize);

	*oNThreads		= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreads_f.x),	math::ceilToInt(nThreads_f.y)});
	*oNThreadGrps	= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreadGrps_f.x),	math::ceilToInt(nThreadGrps_f.y)});
}

void 
RfpForwardPlus::getLightCulllingThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 blockSize)
{
	Vec2f nThreadGrps_f	= resolution_ / sCast<float>(blockSize);

	*oNThreadGrps	= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreadGrps_f.x),	math::ceilToInt(nThreadGrps_f.y)});
	*oNThreads		= *oNThreadGrps * blockSize;
}

RfpForwardPlus::RfpForwardPlus()
{
	create();
}

RfpForwardPlus::~RfpForwardPlus()
{
	destroy();
}

void 
RfpForwardPlus::create()
{
	GraphicsDemo::createMaterial(&shaderFwdpMakeFrustum,	&mtlFwdpMakeFrustum,	"asset/shader/demo/forward_plus/forward_plus_makeFrustums.shader");
	GraphicsDemo::createMaterial(&shaderFwdp,				&mtlFwdp,				"asset/shader/demo/forward_plus/forward_plus.shader");
	GraphicsDemo::createMaterial(&shaderFwdpLighting,		&mtlFwdpLighting,		"asset/shader/demo/forward_plus/forward_plus_lighting.shader");
}

void 
RfpForwardPlus::destroy()
{
	shaderFwdpMakeFrustum.reset(	nullptr);
	mtlFwdpMakeFrustum.reset(	nullptr);
}


RdgBufferHnd 
RfpForwardPlus::addMakeFrustumsPass(RenderGraph* oRdGraph, DrawData* drawData)
{
	using Frustum			= shaderInterop::Frustum;
	using DebugFrustumPts	= shaderInterop::DebugFrustrumPts;

	RdgBufferHnd bufFrustums;
	if (!(isInvalidate(drawData->resolution()) && drawData->drawParamIdx == 0) && false)
		return bufFrustums;

	resolution = drawData->resolution();

	auto* rdGraph		= oRdGraph;

	Vec2u nThreads, nThreadGrps;
	getMakeFrustumsThreadParamTo(&nThreads, &nThreadGrps, resolution, s_kBlockSize);

	auto n = nThreads.x * nThreads.y;
	bufFrustums = rdGraph->createBuffer("fwdp_buf_frustums"
		, RenderGpuBuffer_CreateDesc{ sizeof(Frustum) * n, sizeof(Frustum), RenderGpuBufferTypeFlags::Compute});

	RdgBufferHnd bufDebugFrustumsPts = rdGraph->createBuffer("fwdp_buf_debugFrustumsPts"
		, RenderGpuBuffer_CreateDesc{ sizeof(DebugFrustumPts) * n, sizeof(DebugFrustumPts), RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex});

	auto& passFwdpMakeFrustums = rdGraph->addPass("fwdp_make_frustums", RdgPassTypeFlags::Compute);
	passFwdpMakeFrustums.writeBuffer(bufFrustums);
	passFwdpMakeFrustums.writeBuffer(bufDebugFrustumsPts);
	passFwdpMakeFrustums.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			auto mtl = mtlFwdpMakeFrustum;
			rdReq.reset(rdGraph->renderContext(), *drawData);

			mtl->setParam("nThreads",			Vec3u{nThreads,		1});
			mtl->setParam("nThreadGroups",		Vec3u{nThreadGrps,	1});
			mtl->setParam("oFrustums",			bufFrustums.renderResource());
			mtl->setParam("oDebugFrustumsPts",	bufDebugFrustumsPts.renderResource());
			drawData->setupMaterial(mtl);
			rdReq.dispatch(RDS_SRCLOC, mtl, 0, Vec3u{nThreadGrps, 1});
		}
	);
	rdGraph->exportBuffer(&gpuFrustums,			bufFrustums,			RenderGpuBufferTypeFlags::Compute);
	rdGraph->exportBuffer(&debugFrustumsPts,	bufDebugFrustumsPts,	RenderGpuBufferTypeFlags::Vertex);

	{
		auto& indices	= s_debugFrustumIndices;
		auto idxCount	= arraySize(indices);

		using IdxType = RfpForwardPlus::DebugIndexType;
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

	return bufFrustums;
}

RfpForwardPlus::LightCullingResult
RfpForwardPlus::addLightCullingPass(RenderGraph* oRdGraph, DrawData* drawData, RdgBufferHnd frustumsHnd, RdgTextureHnd texDepth)
{
	if (!frustumsHnd && !gpuFrustums)
		return {};

	auto* rdGraph		= oRdGraph;

	Vec2u nThreads, nThreadGrps;
	getLightCulllingThreadParamTo(&nThreads, &nThreadGrps, resolution, s_kBlockSize);

	auto uintSize = RenderDataTypeUtil::getByteSize(RenderDataType::UInt32);
	//auto n		= nThreads.x	* nThreads.y;
	auto nGrps	= nThreadGrps.x * nThreadGrps.y;

	RdgBufferHnd	opaque_lightIndexCounter		= rdGraph->createBuffer( "fwdp_opaque_lightIndexCounter",		RenderGpuBuffer_CreateDesc{ 1		* uintSize, uintSize, RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex});
	RdgBufferHnd	opaque_lightIndexList			= rdGraph->createBuffer( "fwdp_opaque_lightIndexList",			RenderGpuBuffer_CreateDesc{ nGrps	* uintSize, uintSize, RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex});
	RdgTextureHnd	opaque_lightGrid				= rdGraph->createTexture("fwdp_opaque_lightGrid",				Texture2D_CreateDesc{		nThreadGrps, ColorType::RGu, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::RenderTarget});

	RdgBufferHnd	transparent_lightIndexCounter	= rdGraph->createBuffer( "fwdp_transparent_lightIndexCounter",	RenderGpuBuffer_CreateDesc{ 1		* uintSize, uintSize, RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex});
	RdgBufferHnd	transparent_lightIndexList		= rdGraph->createBuffer( "fwdp_transparent_lightIndexList",		RenderGpuBuffer_CreateDesc{ nGrps	* uintSize, uintSize, RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex});
	RdgTextureHnd	transparent_lightGrid			= rdGraph->createTexture("fwdp_transparent_lightGrid",			Texture2D_CreateDesc{		nThreadGrps, ColorType::RGu, TextureUsageFlags::UnorderedAccess | TextureUsageFlags::ShaderResource | TextureUsageFlags::RenderTarget});

	#if 1
	{
		auto& pass = rdGraph->addPass("fwdp_set_default", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(opaque_lightGrid,		RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setRenderTarget(transparent_lightGrid, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = mtlFwdp;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				/*Vector<u8> data;
				data.resize(opaque_lightIndexCounter.renderResource()->bufSize());
				opaque_lightIndexCounter.renderResource()->uploadToGpu(data.byteSpan());
				transparent_lightIndexCounter.renderResource()->uploadToGpu(data.byteSpan());

				data.resize(opaque_lightIndexList.renderResource()->bufSize());
				opaque_lightIndexList.renderResource()->uploadToGpu(data.byteSpan());
				transparent_lightIndexList.renderResource()->uploadToGpu(data.byteSpan());*/

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(Color4f{0.0f, 0.0f, 0.0f, 0.0f});
			}
		);
	}
	#endif // 0

	RdgPass* lightCullingPass = nullptr;
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
				auto mtl = mtlFwdp;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				mtl->setParam("nThreads",			Vec3u{nThreads,		1});
				mtl->setParam("nThreadGroups",		Vec3u{nThreadGrps,	1});

				mtl->setParam("texDepth", texDepth.texture2D());
				mtl->setParam("frustums", frustumsHnd ? frustumsHnd.renderResource() : gpuFrustums);

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

				mtl->setParam("opaque_lightIndexCounter",		opaque_lightIndexCounter.renderResource());
				mtl->setParam("opaque_lightIndexList",			opaque_lightIndexList.renderResource());
				mtl->setParam("opaque_lightGrid",				opaque_lightGrid.renderResource());

				mtl->setParam("transparent_lightIndexCounter",	transparent_lightIndexCounter.renderResource());
				mtl->setParam("transparent_lightIndexList",		transparent_lightIndexList.renderResource());
				mtl->setParam("transparent_lightGrid",			transparent_lightGrid.renderResource());

				drawData->setupMaterial(mtl);
				rdReq.dispatch(RDS_SRCLOC, mtl, 0, Vec3u{nThreadGrps, 1});
			}
		);
		lightCullingPass = &pass;
	}
	LightCullingResult ret;
	ret.lightCullingPass			= lightCullingPass;
	ret.opaque_lightIndexList		= opaque_lightIndexList;
	ret.opaque_lightGrid			= opaque_lightGrid;
	ret.transparent_lightIndexList	= transparent_lightIndexList;
	ret.transparent_lightGrid		= transparent_lightGrid;

	return ret;
}

RdgPass* 
RfpForwardPlus::addLightingPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, RdgTextureHnd dsBuf, LightCullingResult& lightCulling, bool isOpaque, bool isClearColor)
{
	if (!lightCulling.lightCullingPass)
		return nullptr;

	auto*		rdGraph			= oRdGraph;
	RdgPass*	lightingPass	= nullptr;
	{
		auto& pass = rdGraph->addPass("fwdp_lighting", RdgPassTypeFlags::Graphics);

		RdgBufferHnd	lightIndexList	= lightCulling.lightIndexList(isOpaque);
		RdgTextureHnd	lightGrid		= lightCulling.lightGrid(isOpaque);

		//RDS_CORE_LOG_WARN("--- --- before RdGraph Execute frame: {}", rdGraph->iFrame());

		pass.readBuffer(lightCulling.lightIndexList(isOpaque));
		pass.readTexture(lightCulling.lightGrid(isOpaque));
		pass.setRenderTarget(rtColor,	isClearColor ? RenderTargetLoadOp::Clear : RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf,	RdgAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = mtlFwdpLighting;
				rdReq.reset(rdGraph->renderContext(), *drawData);

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
		lightingPass = &pass;
	}
	return lightingPass;
}

void 
RfpForwardPlus::renderDebugMakeFrustums(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor)
{
#if 1
	{
		if (!debugFrustumsPts || !debugFrustumsIdxBuf)
			return;

		using DebugFrustumPts	= shaderInterop::DebugFrustrumPts;
		//auto& debugFrustumsPts = _fwpMakeFrustums->debugFrustumsPts;

		auto*	rdGraph		= oRdGraph;
		auto*	rdCtx		= rdGraph->renderContext();
		auto	screenSize	= Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

		Vec2u nThreads, nThreadGrps;
		getMakeFrustumsThreadParamTo(&nThreads, &nThreadGrps, resolution, s_kBlockSize);
		auto n = nThreads.x * nThreads.y;

		auto& pass = rdGraph->addPass("fwdp_render_debug_frustums", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(rtColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		//pass.setDepthStencil(texDepth, RdgAccess::Write,	RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = mtlFwdpMakeFrustum;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				auto* drawCall = rdReq.addDrawCall();
				drawCall->vertexLayout			= VertexT_Pos<Tuple4f>::vertexLayout();
				drawCall->indexType				= RenderDataTypeUtil::get<RfpForwardPlus::DebugIndexType>();
				drawCall->vertexCount			= ArraySize<decltype(DebugFrustumPts::pts)> * n;
				drawCall->indexCount			= arraySize(s_debugFrustumIndices) * n;
				drawCall->vertexBuffer			= debugFrustumsPts;
				drawCall->indexBuffer			= debugFrustumsIdxBuf;

				drawCall->vertexOffset			= 0;
				drawCall->indexOffset			= 0;

				drawCall->renderPrimitiveType	= RenderPrimitiveType::Line;
				drawCall->setMaterial(mtl);

				drawData->setupMaterial(mtl);

				RDS_CORE_ASSERT(drawCall->vertexCount <= NumLimit<RfpForwardPlus::DebugIndexType>::max(), "vertex count > index max count");
			}
		);
	}
	#endif // 1
}

bool RfpForwardPlus::isInvalidate(const Vec2f& resoluton_) const { return !math::equals(resolution, resoluton_) && !math::equals0(resoluton_); }

#endif


}