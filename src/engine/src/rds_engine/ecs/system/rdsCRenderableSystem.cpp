#include "rds_engine-pch.h"
#include "rdsCRenderableSystem.h"
#include "rds_engine/ecs/component/rdsCRenderable.h"

#include "rds_engine/rdsEngineContext.h"
#include "rdsCLightSystem.h"


#include "../rdsEntity.h"
#include "../rdsScene.h"

#include "rds_render_api_layer/thread/rdsRenderThreadQueue.h"

namespace rds
{

#if 0
#pragma mark --- rdsCRenderableSystem-Impl ---
#endif // 0
#if 1

//CRenderableSystem* CRenderableSystem::s_instance = nullptr;


CRenderableSystem::CRenderableSystem()
{

}

CRenderableSystem::~CRenderableSystem()
{
	destroy();
}

void 
CRenderableSystem::create(EngineContext* egCtx)
{
	Base::create(egCtx);
	_framedRdReq.resize(s_kFrameInFlightCount);

	_objTransformBuf.setDebugName(	"rds_objTransforms");
	_drawPramBuf.setDebugName(		"rds_drawPrams");

	RenderUtil::createMaterial(&_mtlScreenQuad, "asset/shader/pass_feature/utility/image/rdsScreenQuad.shader");
}

void 
CRenderableSystem::destroy()
{
	_framedRdReq.clear();
	Base::destroy();

	_drawDataAlloc.destructAndClear<DrawData>(LinearAllocator::s_kDefaultAlign);
}

void 
CRenderableSystem::commit(const Scene& scene)
{
	// transform system update
	{
		RDS_PROFILE_SECTION("transform buffer update");

		RDS_TODO("only model and model_inv is ok, calculate mvp in gpu save lots of memory");

		auto& drawData		= *this->drawData()[0];
		auto& renderables_	= renderables();

		// should resize as scene ent size, * 10 is a temp dirty fix
		_objTransformBuf.resize(scene.entities().size() + 1);		// id start at 1, not using 2 table mapping currently

		auto mat_vp = drawData.camera->viewProjMatrix();
		auto mat_v	= drawData.camera->viewMatrix();

		// or loop dirty transform only
		for (auto* rdable : renderables_)
		{
			auto& ent		= rdable->entity();
			auto& transform = ent.transform();

			auto idx = ent.id();
			auto& objTransform = _objTransformBuf.at(idx);
			objTransform.matrix_model		= transform.localMatrix();
			objTransform.matrix_mvp			= mat_vp * objTransform.matrix_model;
			objTransform.matrix_mv			= mat_v  * objTransform.matrix_model;
			objTransform.matrix_m_inv_t		= objTransform.matrix_model.inverse().transpose();
			//objTransform.matrix_mv_inv_t	= mat_v * objTransform.matrix_m_inv_t;
			objTransform.matrix_mv_inv_t	= (mat_v * objTransform.matrix_model).inverse().transpose();

		}
		// for all materials and setParam
		_objTransformBuf.uploadToGpu();
	}
	
	auto& rdGraph = renderGraph();
	{
		RDS_PROFILE_SECTION("draw param update");

		auto drawDataCount = drawData().size();
		_drawPramBuf.resize(drawDataCount);
		for (auto& e : this->drawData())
		{
			auto& drawData	= *e;
			auto& drawParam = _drawPramBuf.at(drawData.drawParamIdx);

			drawData.setupDrawParam(&drawParam);
			_drawPramBuf.uploadToGpu();

			// lights
			/*
			* use gpu to update light transform maybe faster, but need to place barrier before use those buffer
			*/
			{
				RDS_PROFILE_SECTION("light update");

				RDS_CORE_ASSERT(drawDataCount == 1, "only support 1 set lights, otherwise, there should be multi ParamBuffer<T>");
				auto& lightSys = engineContext().lightSystem();
				lightSys.update(drawData);
			}

			transitPresentTexture(rdGraph, &drawData);

			drawData.setupMaterial(drawData._mtlLine);
		}
	}

	// record command
	{
		RDS_PROFILE_SECTION("record render command");

		rdGraph.compile();
		rdGraph.execute();
		//rdGraph.dumpGraphviz();
	}
}


void 
CRenderableSystem::drawUi(RenderContext* renderContext, bool isDrawUi, bool isDrawToScreen)
{
	// present pass
	auto* rdCtx = renderContext;

	// record present
	{
		auto& rdReq = renderRequest(engineContext().engineFrameParam().frameIndex());
		//RDS_CORE_LOG_ERROR("drawUi - {}", engineContext().engineFrameParam().frameIndex());
		//RDS_CORE_ASSERT(rdCtx == rdCtx_, "");

		rdReq.reset(rdCtx);
		auto* clearValue = rdReq.clearFramebuffers();
		clearValue->setClearColor();
		clearValue->setClearDepth();

		if (isDrawUi)
			rdCtx->drawUI(rdReq);
		else
		{
			if (isDrawToScreen)
			{
				RDS_TODO("temporary fix");
				RenderRequest temp;
				rdCtx->drawUI(temp);

				rdReq.drawSceneQuad(RDS_SRCLOC, _mtlScreenQuad);
			}
			//rdReq.swapBuffers();
		}
	}
}

void
CRenderableSystem::render()
{
	RDS_PROFILE_SCOPED();

	auto&	rdGraph		= renderGraph();
	auto*	rdCtx		= rdGraph.renderContext();
	auto	frameIndex	= sCast<u32>(rdCtx->renderFrameParam().frameIndex());

	rdGraph.commit(rdGraph.frameIndex());

	auto& rdReq = renderRequest(frameIndex);
	rdCtx->commit(rdReq);
}

void 
CRenderableSystem::setupRenderJob(RenderData_RenderJob& out)
{
	auto& rdJob = out;

	auto&	rdGraph		= renderGraph();
	auto	frameIndex	= engineContext().engineFrameParam().frameIndex();
	auto&	rdReq		= renderRequest(frameIndex);

	rdJob.create(&rdGraph, &rdReq);
}

void 
CRenderableSystem::addCamera(math::Camera3f* camera)
{
	u32 drawParamIdx = sCast<u32>(_drawData.size());
	
	auto& drawData = *_drawData.emplace_back(_drawDataAlloc.newT<DrawData>());
	drawData.renderableSystem	= this;
	drawData.camera				= camera;
	drawData.drawParamIdx		= drawParamIdx;

	RenderUtil::createMaterial(&drawData._mtlLine, "asset/shader/line.shader");
}

void 
CRenderableSystem::removeCamera(math::Camera3f*	camera)
{
	_notYetSupported(RDS_SRCLOC);
}

void 
CRenderableSystem::transitPresentTexture(RenderGraph& rdGraph, DrawData* drawData)
{
	//auto* rdCtx = rdGraph.renderContext();

	auto texPresent = drawData->oTexPresent;
	if (!texPresent)
		return;
	//auto backBufferRt	= rdGraph.importTexture("back_buffer", rdCtx.backBuffer()); RDS_UNUSED(backBufferRt);

	Material* mtl = _mtlScreenQuad;

	auto& finalComposePass = rdGraph.addPass("present_transition", RdgPassTypeFlags::Graphics);
	finalComposePass.readTexture(texPresent);
	//finalComposePass.setRenderTarget(backBufferRt, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
	finalComposePass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			rdReq.reset(nullptr);
			mtl->setParam("tex_color",			texPresent.texture2D());

			//rdReq.reset(rdCtx);
			/*auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor();
			clearValue->setClearDepth();*/

			//mtlPresent->setParam("texColor",			texPresent.texture2D());
			//rdReq.drawMesh(RDS_SRCLOC, _fullScreenTriangle, mtlPresent, Mat4f::s_identity());
			//rdGraph->renderContext()->drawUI(rdReq);
			//rdReq.swapBuffers();
		}
	);
}

Material* 
CRenderableSystem::getOverrideMaterial(EntityId id, const Shader* shader)
{
	SPtr<Shader> s = constCast(shader);
	auto it = _overrideMtls.find(s);
	if (it == _overrideMtls.end())
		return nullptr;

	auto& mtlTable = it->second;

	auto itMtl = mtlTable.find(id);
	if (itMtl == mtlTable.end())
		return nullptr;

	return itMtl->second;
}

DrawData* 
CRenderableSystem::mainDrawData()
{
	return _drawData[0];
}

RenderDevice* CRenderableSystem::renderDevice() { return renderGraph().renderContext()->renderDevice(); }


#endif

}