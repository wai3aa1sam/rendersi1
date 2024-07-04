#include "rds_engine-pch.h"
#include "rdsDrawData.h"

#include "rds_engine/rdsEngineContext.h"

#include "rds_engine/draw/rdsSceneView.h"

#include "rds_engine/ecs/system/rdsCRenderableSystem.h"
#include "rds_engine/ecs/system/rdsCLightSystem.h"

namespace rds
{


#if 0
#pragma mark --- rdsDrawData-Impl ---
#endif // 0
#if 1

void 
DrawData::drawScene(RenderRequest& rdReq, Material* mtl)
{
	sceneView->drawScene(rdReq, mtl, this);
}

void 
DrawData::drawScene(RenderRequest& rdReq)
{
	sceneView->drawScene(rdReq, nullptr, this);
}

void 
DrawData::drawScene(RenderRequest& rdReq, Material* mtl, const DrawSettings& drawSettings)
{
	sceneView->drawScene(rdReq, mtl, this, drawSettings);
}

void 
DrawData::drawScene(RenderRequest& rdReq, const DrawSettings& drawSettings)
{
	sceneView->drawScene(rdReq, this, drawSettings);
}

void 
DrawData::drawSceneAABBox(RenderRequest& rdReq, const DrawSettings& drawSettings)
{
	sceneView->drawSceneAABBox(rdReq, this, drawSettings);
}

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
	drawParam.matrix_vp_inv		= drawParam.matrix_vp.inverse();

	drawParam.resolution		= camera->viewport().size;
	drawParam.delta_time		= Tuple2f(deltaTime, deltaTime);
}

void 
DrawData::setupMaterial(Material* oMtl)
{
	auto& mtl = oMtl;
	auto& sysRdable = sceneView->renderableSystem();
	auto& sysLight	= sysRdable.engineContext().lightSystem(); RDS_UNUSED(sysLight);

	mtl->setParam("rds_objTransforms",			sysRdable._objTransformBuf.gpuBuffer());
	mtl->setParam("rds_drawParams",				sysRdable._drawPramBuf.gpuBuffer());
	mtl->setParam("rds_lastFrame_drawParams",	sysRdable._drawPramBuf.prevGpuBuffer() ? sysRdable._drawPramBuf.prevGpuBuffer() : sysRdable._drawPramBuf.gpuBuffer());

	#if 1
	if (!sysLight.lightParamBuf().is_empty())
	{
		mtl->setParam("rds_lights",			sysLight.lightParamBuf().gpuBuffer());
		mtl->setParam("rds_nLights",		sysLight.lightCount());
	}
	else
	{
		mtl->setParam("rds_nLights",		u32(0));
	}
	#endif // 0
}

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