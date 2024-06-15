#include "rds_engine-pch.h"
#include "rdsCRenderableMesh.h"
#include "rds_engine/ecs/rdsEntity.h"

#include "rds_engine/draw/rdsDrawData.h"
#include "rds_engine/draw/rdsDrawSettings.h"

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

template<class T>
bool isOverlapped(const T& a, const T& b)
{
	return a.isOverlapped(b);
}

template<class T, class FUNC>
void executeWhenOverlapped(FUNC&& fn, const T& a, const T& b)
{
	if (isOverlapped(a, b))
	{
		fn();
	}
}

void 
CRenderableMesh::onRender(RenderRequest& rdReq, Material* mtl, DrawData* drawData, const DrawSettings& drawSettings)
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
	perObjParam.id				= sCast<u32>(id);
	//perObjParam.drawParamIdx	= drawData ? sCast<u32>(drawData->drawParamIdx) : 0;

	if (auto& fn = drawSettings.setMaterialFn)
	{
		fn(mtl);
	}

	auto&		transform	= entity().transform();
	const auto& matrix		= transform.worldMatrix();
	auto&		rdMesh		= meshAsset->renderMesh;

	auto& cullingSetting = drawSettings.cullingSetting;
	switch (cullingSetting.mode)
	{
		case CullingMode::None:
		{
			if (subMeshIndex == s_kInvalidSubMeshIndex)
			{
				rdReq.drawMesh(RDS_RD_CMD_DEBUG_ARG, rdMesh, mtl, perObjParam);
			}
			else
			{
				auto& subMesh = rdMesh.subMeshes()[subMeshIndex];
				rdReq.drawSubMeshT(RDS_RD_CMD_DEBUG_ARG, subMesh, mtl, perObjParam);
			}
		} break;

		case CullingMode::CameraFustrum:
		{
			const auto& cullingFrustum = cullingSetting.cameraFustrum;
			if (subMeshIndex == s_kInvalidSubMeshIndex)
			{
				for (auto& subMesh : rdMesh.subMeshes())
				{
					bool isOverlapped = cullingFrustum.isOverlapped(subMesh.aabbox(), matrix);
					if (isOverlapped)
					{
						rdReq.drawSubMeshT(RDS_RD_CMD_DEBUG_ARG, subMesh, mtl, perObjParam);
					}
				}
			}
			else
			{
				auto& subMesh = rdMesh.subMeshes()[subMeshIndex];
				bool isOverlapped = cullingFrustum.isOverlapped(subMesh.aabbox(), matrix);
				if (isOverlapped)
				{
					rdReq.drawSubMeshT(RDS_RD_CMD_DEBUG_ARG, subMesh, mtl, perObjParam);
				}
				else
				{
					//RDS_LOG("culled: {}", entity().name());
				}
			}
		} break;
		case CullingMode::AABBox:
		{
			const auto& aabbox = cullingSetting.aabbox;
			if (subMeshIndex == s_kInvalidSubMeshIndex)
			{
				for (auto& subMesh : rdMesh.subMeshes())
				{
					bool isOverlapped = aabbox.isOverlapped(subMesh.aabbox().makeExpanded(matrix));
					if (isOverlapped)
					{
						rdReq.drawSubMeshT(RDS_RD_CMD_DEBUG_ARG, subMesh, mtl, perObjParam);
					}
				}
			}
			else
			{
				auto& subMesh = rdMesh.subMeshes()[subMeshIndex];
				bool isOverlapped = aabbox.isOverlapped(subMesh.aabbox().makeExpanded(matrix));
				if (isOverlapped)
				{
					rdReq.drawSubMeshT(RDS_RD_CMD_DEBUG_ARG, subMesh, mtl, perObjParam);
					//RDS_LOG("isOverlapped: {}", entity().name());
				}
			}
		} break;

		default: { RDS_THROW("invalid CullingMode"); } break;
	}
}


#endif

}