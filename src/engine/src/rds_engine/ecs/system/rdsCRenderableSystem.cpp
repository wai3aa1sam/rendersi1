#include "rds_engine-pch.h"
#include "rdsCRenderableSystem.h"
#include "rds_engine/ecs/component/rdsCRenderable.h"

#include "rds_engine/rdsEngineContext.h"
#include "rdsCLightSystem.h"


#include "../rdsEntity.h"
#include "../rdsScene.h"

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
}

void 
CRenderableSystem::destroy()
{
	_framedRdReq.clear();
	Base::destroy();
}

void 
CRenderableSystem::update(DrawData& drawData)
{
	// transform system update
	{
		auto& renderables_ = renderables();

		// should resize as scene ent size, * 10 is a temp dirty fix
		_objTransformBuf.resize(renderables_.size() * 10 + 1);		// id start at 1, not using 2 table mapping currently

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
			objTransform.matrix_mv_inv_t	= mat_v * objTransform.matrix_m_inv_t;
		}
		// for all materials and setParam
		_objTransformBuf.uploadToGpu();
	}

	{
		auto n = 1;
		_drawPramBuf.resize(n);
		for (size_t i = 0; i < n; i++)
		{
			auto& drawParam = _drawPramBuf.at(i);
			drawData.setupDrawParam(&drawParam);
			_drawPramBuf.uploadToGpu();

			// lights
			{
				RDS_CORE_ASSERT(n == 1, "only support 1 set lights, otherwise, there should be multi ParamBuffer<T>");
				auto& sysLight = engineContext().lightSystem();
				sysLight.update(drawData);
			}
		}

		drawData.setupMaterial(drawData._mtlLine); // TODO: temporary
	}


	// record command
	{
		auto& rdGraph = renderGraph();

		//rdGraph.reset();
		// update
		// present

		rdGraph.compile();
		rdGraph.execute();
	}
}

void
CRenderableSystem::render(RenderContext* rdCtx_, RenderMesh& fullScreenTriangle, Material* mtlPresent, bool isDrawUi)
{
	auto& rdGraph = renderGraph();
	
	// rdGraph.execute(); // execute in render thread, then later need to copy struct renderableObject list to render thread, keep simple first
	rdGraph.commit();

	// record present
	{
		auto* rdCtx = rdGraph.renderContext();
		auto& rdReq = renderRequest();
		RDS_CORE_ASSERT(rdCtx == rdCtx_, "");

		rdReq.reset(rdCtx);
		auto* clearValue = rdReq.clearFramebuffers();
		clearValue->setClearColor();
		clearValue->setClearDepth();

		if (mtlPresent)
		{
			rdReq.drawMesh(RDS_SRCLOC, fullScreenTriangle, mtlPresent);
			rdReq.swapBuffers();
		}
		if (isDrawUi)
			rdCtx->drawUI(rdReq);		
		rdCtx->commit(rdReq);
	}
}

void 
CRenderableSystem::present(RenderGraph& rdGraph, DrawData& drawData, RenderMesh& fullScreenTriangle, Material* mtlPresent)
{
	// present pass
	auto* rdCtx = rdGraph.renderContext();

	auto texPresent		= drawData.oTexPresent;
	if (!texPresent)
		return;
	//auto backBufferRt	= rdGraph.importTexture("back_buffer", rdCtx.backBuffer()); RDS_UNUSED(backBufferRt);

	auto& finalComposePass = rdGraph.addPass("final_composite", RdgPassTypeFlags::Graphics);
	finalComposePass.readTexture(texPresent);
	//finalComposePass.setRenderTarget(backBufferRt, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
	finalComposePass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			rdReq.reset(rdCtx);
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

#endif

}