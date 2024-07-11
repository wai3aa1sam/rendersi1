#include "rds_engine-pch.h"
#include "rdsSceneView.h"

#include "rds_engine/rdsEngineContext.h"

#include "../ecs/rdsEntity.h"
#include "../ecs/rdsScene.h"
#include "../ecs/component/rdsCRenderableMesh.h"
#include "../ecs/system/rdsCRenderableSystem.h"

namespace rds
{

#if 0
#pragma mark --- rdsSceneView-Impl ---
#endif // 0
#if 1

void 
SceneView::SceneView::create(Scene* scene, CRenderableSystem* sys) 
{
	_scene		= scene;
	_rdableSys	= sys; 
}

void 
SceneView::drawScene(RenderRequest& rdReq, Material* mtl, DrawData* drawData)
{
	RDS_CORE_ASSERT(drawData, "draw scene fail");
	auto& rdableSys = renderableSystem();

	if (mtl)
		drawData->setupMaterial(mtl);
	for (auto* e : rdableSys.renderables())
	{
		e->render(rdReq, mtl, drawData, DrawSettings{});
	}
}

void 
SceneView::drawScene(RenderRequest& rdReq, Material* mtl, DrawData* drawData, const DrawSettings& drawSettings)
{
	RDS_CORE_ASSERT(drawData, "draw scene fail");
	RDS_TODO("temporary solution");

	auto& rdableSys = renderableSystem();

	for (auto* e : rdableSys.renderables())
	{
		RDS_CORE_ASSERT(e->isRenderableMesh, "isRenderableMesh");
		e->render(rdReq, mtl, drawData, drawSettings);
	}
}

void 
SceneView::drawScene(RenderRequest& rdReq, DrawData* drawData, const DrawSettings& drawSettings)
{
	if (auto* shader = drawSettings.overrideShader)
	{
		auto* rdDev = renderableSystem().renderGraph().renderContext()->renderDevice();

		auto&	rdableSys		= renderableSystem();
		auto	renderableCount = rdableSys.renderables().size();

		auto& mtls = rdableSys._overrideMtls[shader];
		if (mtls.size() == 0)
		{
			mtls.resize(renderableCount);

			RDS_TODO("when multi thread, drawScene may call from other thread, then we need to modify vulkan material to fit this");
			for (auto* e : rdableSys.renderables())
			{
				auto& mtl = mtls[e->entity().id()];
				mtl = rdDev->createMaterial(shader);

				RDS_CORE_ASSERT(e->isRenderableMesh, "isRenderableMesh");
				auto* rdableMesh = sCast<CRenderableMesh*>(e);
				if (MeshAsset * meshAsset = rdableMesh->meshAsset)
				{
					meshAsset->materialList.setupMaterial(mtl, rdableMesh->materialIndex);
				}
			}
		}
		
		for (auto* e : rdableSys.renderables())
		{
			auto& mtl = mtls[e->entity().id()];

			if (mtl)
				drawData->setupMaterial(mtl);

			e->render(rdReq, mtl, drawData, drawSettings);
		}
	}
	else
	{
		drawScene(rdReq, nullptr, drawData, drawSettings);
	}
}

void 
SceneView::drawSceneAABBox(RenderRequest& rdReq, DrawData* drawData, const DrawSettings& drawSettings)
{
	RDS_TODO("temporary solution");

	auto& rdableSys = renderableSystem();

	auto& mtl = rdReq.lineMaterial;
	drawData->setupMaterial(mtl);
	for (auto* e : rdableSys.renderables())
	{
		if (e->isRenderableMesh)
		{
			#if 1

			auto&		rdableMesh	= *sCast<CRenderableMesh*>(e);
			const auto& matrix		= rdableSys._objTransformBuf.at(e->id()).matrix_model;
			if (!rdableMesh.meshAsset)
				continue;
			
			auto&	rdMesh	= rdableMesh.meshAsset->renderMesh;
			auto	color	= Color4f(0.0f, 1.0f, 0.0f, 1.0f).toColorRGBAb();

			auto subMeshIndex = rdableMesh.subMeshIndex;

			auto& cullingSetting = drawSettings.cullingSetting;
			switch (cullingSetting.mode)
			{
				case CullingMode::None:
				{
					if (subMeshIndex == CRenderableMesh::s_kInvalidSubMeshIndex)
					{
						rdReq.drawAABBox(rdMesh, color, matrix);
					}
					else
					{
						rdReq.drawAABBox(rdMesh.subMeshes()[subMeshIndex].aabbox(), color, matrix);
					}
				} break;

				case CullingMode::CameraFustrum:
				{
					const auto& cullingFrustum = cullingSetting.cameraFustrum;
					if (subMeshIndex == CRenderableMesh::s_kInvalidSubMeshIndex)
					{
						for (auto& subMesh : rdMesh.subMeshes())
						{
							bool isOverlapped = cullingFrustum.isOverlapped(subMesh.aabbox(), matrix);
							if (isOverlapped)
							{
								rdReq.drawAABBox(rdMesh, color, matrix);
							}
						}
					}
					else
					{
						auto& subMesh = rdMesh.subMeshes()[subMeshIndex];
						//Frustum3f temp;
						//RDS_DUMP_VAR(subMesh.aabbox().min, subMesh.aabbox().max);
						//RDS_DUMP_VAR(subMesh.aabbox().makeExpanded(matrix).min, subMesh.aabbox().makeExpanded(matrix).max);

						//temp.setByBBox(subMesh.aabbox().makeExpanded(matrix), Mat4f::s_identity());
						//rdReq.drawFrustum(temp, Color4b{255, 0, 0, 255});
						bool isOverlapped = cullingFrustum.isOverlapped(subMesh.aabbox(), matrix);
						if (isOverlapped)
						{
							rdReq.drawAABBox(rdMesh.subMeshes()[subMeshIndex].aabbox(), color, matrix);
						}
					}
				} break;
				case CullingMode::AABBox:
				{
					const auto& aabbox = cullingSetting.aabbox;
					if (subMeshIndex == CRenderableMesh::s_kInvalidSubMeshIndex)
					{
						for (auto& subMesh : rdMesh.subMeshes())
						{
							bool isOverlapped = aabbox.isOverlapped(subMesh.aabbox().makeExpanded(matrix));
							if (isOverlapped)
							{
								rdReq.drawAABBox(rdMesh, color, matrix);
							}
						}
					}
					else
					{
						auto& subMesh = rdMesh.subMeshes()[subMeshIndex];
						bool isOverlapped = aabbox.isOverlapped(subMesh.aabbox().makeExpanded(matrix));
						if (isOverlapped)
						{
							rdReq.drawAABBox(rdMesh.subMeshes()[subMeshIndex].aabbox(), color, matrix);
							//RDS_LOG("isOverlapped: {}", e->entity().name());
						}
					}
				} break;

				default: { RDS_THROW("invalid CullingMode"); } break;
			}
			#endif // 0

		}
		else
		{
			RDS_CORE_ASSERT("");
		}
	}
}


Entity* 
SceneView::findEntity(EntityId id)
{
	return _scene->findEntity(id);
}


#endif

}