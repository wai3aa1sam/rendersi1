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

	RenderUtil::createMaterial(&_mtlScreenQuad, "asset/shader/pass_feature/utility/image/rdsScreenQuad.shader");
}

void 
CRenderableSystem::destroy()
{
	_framedRdReq.clear();
	Base::destroy();
}

void 
CRenderableSystem::update(const Scene& scene, DrawData& drawData)
{
	// transform system update
	{
		RDS_PROFILE_SECTION("transform buffer update");

		auto& renderables_ = renderables();

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
	
	auto& rdGraph   = renderGraph();
	{
		RDS_PROFILE_SECTION("draw param update");

		auto n = 1;
		_drawPramBuf.resize(n);
		for (size_t i = 0; i < n; i++)
		{
			auto& drawParam = _drawPramBuf.at(i);

			drawData.setupDrawParam(&drawParam);
			_drawPramBuf.uploadToGpu();

			// lights
			{
				RDS_PROFILE_SECTION("light update");

				RDS_CORE_ASSERT(n == 1, "only support 1 set lights, otherwise, there should be multi ParamBuffer<T>");
				auto& sysLight = engineContext().lightSystem();
				sysLight.update(drawData);
			}

			transitPresentTexture(rdGraph, drawData);
		}

		drawData.setupMaterial(drawData._mtlLine); // TODO: temporary
	}


	// record command
	{
		RDS_PROFILE_SECTION("record render command");

		rdGraph.compile();
		rdGraph.execute();
	}
}

void
CRenderableSystem::render()
{
	RDS_PROFILE_SCOPED();

	auto& rdGraph = renderGraph();
	
	// rdGraph.execute(); // execute in render thread, then later need to copy struct renderableObject list to render thread, keep simple first
	rdGraph.commit();
}

void 
CRenderableSystem::present(RenderContext* renderContext, bool isDrawUi, bool isDrawToScreen)
{
	// present pass
	auto* rdCtx = renderContext;

	// record present
	{
		auto& rdReq = renderRequest();
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
				rdReq.drawSceneQuad(RDS_SRCLOC, _mtlScreenQuad);
			//rdReq.swapBuffers();
		}

		rdCtx->commit(rdReq);
	}
}

void 
CRenderableSystem::transitPresentTexture(RenderGraph& rdGraph, DrawData& drawData)
{
	//auto* rdCtx = rdGraph.renderContext();

	auto texPresent		= drawData.oTexPresent;
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

#endif

}