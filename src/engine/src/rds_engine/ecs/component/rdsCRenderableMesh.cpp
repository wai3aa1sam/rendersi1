#include "rds_engine-pch.h"
#include "rdsCRenderableMesh.h"
#include "rds_engine/ecs/rdsEntity.h"

namespace rds
{

#if 0
#pragma mark --- rdsCRenderableMesh-Impl ---
#endif // 0
#if 1

CRenderableMesh::CRenderableMesh()
{
	isRenderableMesh = true;
}

CRenderableMesh::~CRenderableMesh()
{
	
}

void 
CRenderableMesh::onRender(RenderRequest& rdReq, Material* mtl, DrawData* drawData)
{
	if (!meshAsset) return;
	if (!mtl)
	{
		if (!material)
			return;
		mtl = material;
	}

	if (drawData)
		setMaterialCommonParam(mtl, *drawData);

	auto id = entity().id();
	PerObjectParam perObjParam;
	perObjParam.id = sCast<u32>(id);

	rdReq.drawMesh(RDS_RD_CMD_DEBUG_ARG, meshAsset->rdMesh, mtl, perObjParam);
}


#endif

}