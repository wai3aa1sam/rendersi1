#include "rds_forward_plus-pch.h"

#include "rds_forward_plus.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"

namespace rds
{
namespace shaderInterop
{
#include "../../../built-in/shader/common/geometry/rdsGeometryPrimitive.hlsl"
}
}


namespace rds
{

#if 0
#pragma mark --- rdsForwardPlus-Impl ---
#endif // 0
#if 1

void 
ForwardPlus::onCreate()
{
	Base::onCreate();

	createMaterial(&_shaderForwardPlus, &_mtlForwardPlus, "asset/shader/demo/forward_plus/forward_plus.shader"
					, [&](Material* mtl) {mtl->setParam("texture0", texUvChecker()); });

	createMaterial(&_shaderHelloTriangle, &_mtlHelloTriangle, "asset/shader/demo/hello_triangle/hello_triangle.shader"
		, [&](Material* mtl) {mtl->setParam("texture0", texUvChecker()); });

	_fwpMakeFrustums = makeUPtr<ForwardPlus_MakeFrustums>();
}

void 
ForwardPlus::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);

	createDefaultScene(oScene, _mtlForwardPlus, meshAssets().suzanne, 25);

	{
		auto& scene = *oScene;
		auto* ent = scene.addEntity("");

		auto* rdableMesh = ent->addComponent<CRenderableMesh>();
		rdableMesh->material	= _fwpMakeFrustums->mtlFwpMakeFrustum;
		//rdableMesh->meshAsset	= meshAsset;

		//auto* transform	= ent->getComponent<CTransform>();
		//transform->setLocalPosition(startPos.x + step.x * c, 0.0f, startPos.y + step.y * r);

		TempString buf;
		fmtTo(buf, "Debug Frustum-{}", sCast<u64>(ent->id()));
		ent->setName(buf);
	}
}

void 
ForwardPlus::onPrepareRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	Base::onPrepareRender(oRdGraph, drawData);

	#if 0
	auto*	rdGraph		= oRdGraph;
	auto*	rdCtx		= rdGraph->renderContext();
	auto	screenSize	= Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

	RdgTextureHnd texCompute	= rdGraph->createTexture("forward_plus_texCompute-onPrepareRender"
		,	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::UnorderedAccess	| TextureUsageFlags::ShaderResource});
	#if 1
	{
		auto& pass = rdGraph->addPass("forward_plus", RdgPassTypeFlags::Compute);
		pass.writeTexture(texCompute);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = _mtlForwardPlus;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				mtl->setParam("oImage", texCompute.renderResource());
				rdReq.dispatch(RDS_SRCLOC, mtl, 0, screenSize.x / 8, screenSize.y / 8, 1);
			}
		);
	}
	#endif // 1

	rdGraph->exportTexture(&_tex, texCompute, TextureUsageFlags::ShaderResource);
	#endif // 0
}

void 
ForwardPlus::onExecuteRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	Base::onExecuteRender(oRdGraph, drawData);

	auto*	rdGraph		= oRdGraph;
	auto*	rdCtx		= rdGraph->renderContext();
	auto	screenSize	= Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

	//RdgBufferHnd bufFrustums = _fwpMakeFrustums->onExecuteRender(oRdGraph, drawData);

	//Renderer::rdDev()->waitIdle();

	RdgTextureHnd texColor		= rdGraph->createTexture("forward_plus_color",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget		| TextureUsageFlags::ShaderResource});
	RdgTextureHnd texDepth		= rdGraph->createTexture("forward_plus_depth",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil		| TextureUsageFlags::ShaderResource});
	RdgTextureHnd texCompute	= rdGraph->createTexture("forward_plus_texCompute",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::UnorderedAccess	| TextureUsageFlags::ShaderResource});
	#if 1
	{
		auto& pass = rdGraph->addPass("forward_plus", RdgPassTypeFlags::Compute);
		pass.writeTexture(texCompute);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = _mtlForwardPlus;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				mtl->setParam("oImage", texCompute.renderResource());
				rdReq.dispatch(RDS_SRCLOC, mtl, 0, screenSize.x / 8, screenSize.y / 8, 1);
			}
		);
	}
	#endif // 1

	#if 1
	{
		auto& passHelloTriangle = rdGraph->addPass("hello_triangle", RdgPassTypeFlags::Graphics);
		passHelloTriangle.readTexture(texCompute);
		passHelloTriangle.setRenderTarget(texColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		passHelloTriangle.setDepthStencil(texDepth,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight
		passHelloTriangle.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = _mtlHelloTriangle;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				mtl->setParam("texture0", texCompute.renderResource());
				//mtl->setParam("texture0", _tex);
				drawData->sceneView->drawScene(rdReq, mtl, drawData);
			}
		);
	}
	#endif // 1

	addSkyboxPass(rdGraph, drawData, skyboxDefault(), texColor, texDepth);
	//_fwpMakeFrustums->renderDebugMakeFrustums(rdGraph, drawData, texColor);

	drawData->oTexPresent = texColor;
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
#pragma mark --- rdsForwardPlus-Impl ---
#endif // 0
#if 1

ForwardPlus_MakeFrustums::ForwardPlus_MakeFrustums()
{
	create();
}

ForwardPlus_MakeFrustums::~ForwardPlus_MakeFrustums()
{
	destroy();
}

void 
ForwardPlus_MakeFrustums::create()
{
	GraphicsDemo::createMaterial(&shaderFwpMakeFrustum, &mtlFwpMakeFrustum, "asset/shader/demo/forward_plus/forward_plus_makeFrustums.shader");
}

void 
ForwardPlus_MakeFrustums::destroy()
{
	shaderFwpMakeFrustum.reset(	nullptr);
	mtlFwpMakeFrustum.reset(	nullptr);
}

RdgBufferHnd 
ForwardPlus_MakeFrustums::onExecuteRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	using Frustum			= shaderInterop::Frustum;
	using DebugFrustumPts	= shaderInterop::DebugFrustrumPts;

	RdgBufferHnd bufFrustums;
	if (!(isInvalidate(drawData->resolution()) && drawData->drawParamIdx == 0))
		return bufFrustums;

	resolution = drawData->resolution();

	auto* rdGraph		= oRdGraph;
	Vec2f nThreads_f	= resolution / s_kTileCount;
	Vec2f nThreadGrps_f	= nThreads_f / s_kTileCount;

	Vec2u nThreads		= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreads_f.x),	math::ceilToInt(nThreads_f.y)});
	Vec2u nThreadGrps	= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreadGrps_f.x),	math::ceilToInt(nThreadGrps_f.y)});

	auto n = nThreads.x * nThreads.y;
	bufFrustums = rdGraph->createBuffer("forward_plus_buf_frustums"
				, RenderGpuBuffer_CreateDesc{ sizeof(Frustum) * n, sizeof(Frustum), RenderGpuBufferTypeFlags::Compute});

	RdgBufferHnd bufDebugFrustumsPts = rdGraph->createBuffer("forward_plus_buf_debugFrustumsPts"
				, RenderGpuBuffer_CreateDesc{ sizeof(DebugFrustumPts) * n, sizeof(DebugFrustumPts), RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex});

	auto& passFwpMakeFrustums = rdGraph->addPass("forward_plus_make_frustums", RdgPassTypeFlags::Compute);
	passFwpMakeFrustums.writeBuffer(bufFrustums);
	passFwpMakeFrustums.writeBuffer(bufDebugFrustumsPts);
	passFwpMakeFrustums.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			auto mtl = mtlFwpMakeFrustum;
			rdReq.reset(rdGraph->renderContext(), *drawData);

			mtl->setParam("nThreads",			Vec3u{nThreads,		1});
			mtl->setParam("nThreadGroups",		Vec3u{nThreadGrps,	1});
			mtl->setParam("oFrustum",			bufFrustums.renderResource());
			mtl->setParam("oDebugFrustumsPts",	bufDebugFrustumsPts.renderResource());
			drawData->setupMaterial(mtl);
			rdReq.dispatch(RDS_SRCLOC, mtl, 0, Vec3u{nThreadGrps, 1});
		}
	);
	rdGraph->exportBuffer(&frustums,			bufFrustums,			RenderGpuBufferTypeFlags::Compute);
	rdGraph->exportBuffer(&debugFrustumsPts,	bufDebugFrustumsPts,	RenderGpuBufferTypeFlags::Vertex);

	{
		auto& indices	= s_debugFrustumIndices;
		auto idxCount	= arraySize(indices);

		using IdxType = ForwardPlus_MakeFrustums::DebugIndexType;
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

void 
ForwardPlus_MakeFrustums::renderDebugMakeFrustums(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd texColor)
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

		Vec2f nThreads_f	= resolution / s_kTileCount;
		Vec2f nThreadGrps_f	= nThreads_f / s_kTileCount;

		Vec2u nThreads		= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreads_f.x),	math::ceilToInt(nThreads_f.y)});
		Vec2u nThreadGrps	= Vec2u::s_cast(Vec2i{math::ceilToInt(nThreadGrps_f.x),	math::ceilToInt(nThreadGrps_f.y)});

		auto n = nThreads.x * nThreads.y;

		auto& pass = rdGraph->addPass("forward_plus_render_debug_frustums", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(texColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		//pass.setDepthStencil(texDepth, RdgAccess::Write,	RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto mtl = mtlFwpMakeFrustum;
				rdReq.reset(rdGraph->renderContext(), *drawData);

				auto* drawCall = rdReq.addDrawCall();
				drawCall->vertexLayout			= VertexT_Pos<Tuple4f>::vertexLayout();
				drawCall->indexType				= RenderDataTypeUtil::get<ForwardPlus_MakeFrustums::DebugIndexType>();
				drawCall->vertexCount			= ArraySize<decltype(DebugFrustumPts::pts)> * n;
				drawCall->indexCount			= arraySize(s_debugFrustumIndices) * n;
				drawCall->vertexBuffer			= debugFrustumsPts;
				drawCall->indexBuffer			= debugFrustumsIdxBuf;

				drawCall->vertexOffset			= 0;
				drawCall->indexOffset			= 0;

				drawCall->renderPrimitiveType	= RenderPrimitiveType::Line;
				drawCall->setMaterial(mtl);

				drawData->setupMaterial(mtl);

				RDS_CORE_ASSERT(drawCall->vertexCount <= NumLimit<ForwardPlus_MakeFrustums::DebugIndexType>::max(), "vertex count > index max count");
			}
		);
	}
	#endif // 1
}

bool ForwardPlus_MakeFrustums::isInvalidate(const Vec2f& resoluton_) const { return !math::equals(resolution, resoluton_) && !math::equals0(resoluton_); }

#endif


}