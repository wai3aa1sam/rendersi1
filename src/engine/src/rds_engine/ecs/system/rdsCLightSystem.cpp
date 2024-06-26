#include "rds_engine-pch.h"
#include "rdsCLightSystem.h"
#include "rds_engine/ecs/component/rdsCLight.h"
#include "rds_engine/ecs/component/rdsCTransform.h"

#include "../rdsEntity.h"

namespace rds
{

#if 0
#pragma mark --- rdsCLightSystem-Impl ---
#endif // 0
#if 1


CLightSystem::CLightSystem()
{
	_lightParamBuf.setDebugName("rds_lights");
}

CLightSystem::~CLightSystem()
{
	destroy();
}

void 
CLightSystem::update(const DrawData& drawData)
{
	//_lights
	{
		Mat4f matrixView = drawData.camera->viewMatrix();

		auto& sysLight		= *this;
		for (auto* light : sysLight.components())
		{
			auto& ent		= light->entity();
			auto& transform = ent.transform(); RDS_UNUSED(transform);
			updateLight(light, transform, matrixView, true/*transform.isDirty()*/);
		}

		#if 1
		auto& lightParamBuf = sysLight.lightParamBuf();
		lightParamBuf.uploadToGpu();
		#endif // 0
	}
}

bool 
CLightSystem::updateLight(CLight* light, CTransform& transform, const Mat4f& viewMatrix, bool isTransformDirty)
{
	RDS_CORE_ASSERT(light, "light");

	bool isDirty = light->isDirty();
	light->resetDirty();

	if (!isTransformDirty)
	{
		return isDirty;
	}

	auto posWs		= transform.localPosition();
	auto dirWs		= transform.forward().normalize();

	auto posVs		= viewMatrix.mulPoint4x3(posWs);
	auto dirVs		= viewMatrix.mulVector(dirWs).normalize();

	light->setPositionWs( posWs);
	light->setDirectionWs(dirWs);

	light->setPositionVs( posVs);
	light->setDirectionVs(dirVs);

	return isDirty;
}

//void
//CLightSystem::render(RenderRequest& rdReq)
//{
//	
//}


#endif

}