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
		_objTransformBuf.resize(renderables().size() + 1);		// id start at 1, not using 2 table mapping currently

		auto mat_vp = drawData.camera->viewProjMatrix();

		// or loop dirty transform only
		for (auto* rdable : renderables())
		{
			auto& ent		= rdable->entity();
			auto& transform = ent.transform();

			auto idx = ent.id();
			auto& objTransform = _objTransformBuf.at(idx);
			objTransform.matrix_model	= transform.localMatrix();
			objTransform.matrix_mvp		= mat_vp * objTransform.matrix_model;
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
CRenderableSystem::render(RenderContext* rdCtx_, RenderMesh& fullScreenTriangle, Material* mtlPresent)
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
			auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor();
			clearValue->setClearDepth();

			//mtlPresent->setParam("texColor",			texPresent.texture2D());
			//rdReq.drawMesh(RDS_SRCLOC, _fullScreenTriangle, mtlPresent, Mat4f::s_identity());
			//rdGraph->renderContext()->drawUI(rdReq);
			//rdReq.swapBuffers();
		}
	);
}

#endif

#if 0
#pragma mark --- rdsDrawData-Impl ---
#endif // 0
#if 1

void 
DrawData::setupDrawParam(DrawParam* oDrawParam)
{
	auto& drawParam = *oDrawParam;
	drawParam.camera_pos		= camera->pos();
	drawParam.camera_near		= camera->nearClip();
	drawParam.camera_far		= camera->farClip();

	drawParam.matrix_proj		= camera->projMatrix();
	drawParam.matrix_view		= camera->viewMatrix();
	drawParam.matrix_vp			= camera->viewProjMatrix();
	drawParam.matrix_proj_inv	= drawParam.matrix_proj.inverse();
	drawParam.matrix_view_inv	= drawParam.matrix_view.inverse();

	drawParam.resolution		= camera->viewport().size;
	drawParam.delta_time		= Tuple2f(deltaTime, deltaTime);
}

void 
DrawData::setupMaterial(Material* oMtl)
{
	auto& mtl = oMtl;
	auto& sysRdable = sceneView->renderableSystem();
	auto& sysLight	= sysRdable.engineContext().lightSystem();

	mtl->setParam("rds_objTransforms",		&sysRdable._objTransformBuf.gpuBuffer());
	mtl->setParam("rds_drawParams",			&sysRdable._drawPramBuf.gpuBuffer());
	mtl->setParam("rds_lights",				&sysLight.lightParamBuf().gpuBuffer());
	mtl->setParam("rds_nLights",			sysLight.lightCount());
}

const Tuple2f& DrawData::resolution() const { return camera->viewport().size; }

Material* 
DrawData::mtlLine()
{
	/*if (!_mtlLine)
	{
		auto* rdDev = Renderer::rdDev();
		_mtlLine	= Renderer::rdDev()->createMaterial(rdDev->shaderStock().shaderLine);
	}*/
	RDS_CORE_ASSERT(_mtlLine, "no line material");
	return _mtlLine;
}

#endif
}