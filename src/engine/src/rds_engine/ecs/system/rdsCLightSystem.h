#pragma once

#include "rds_engine/common/rds_engine_common.h"

#include "rdsCSystem.h"
#include "../component/rdsCLight.h"

#include <rds_render_api_layer/buffer/rdsParamBuffer.h>

namespace rds
{

class Scene;
//class CLight;
class CTransform;
class CLight;

#if 0
#pragma mark --- rdsCLightSystem-Decl ---
#endif // 0
#if 1

class CLightSystem : public CSystemT<CLight>
{
	friend class CLight;
public:
	using Light	= CLight::Light;

public:
	CLightSystem();
	~CLightSystem();

	void update(const DrawData& drawData);

public:
	u32					lightCount() const;

	ParamBuffer<Light>& lightParamBuf();

protected:
	template<class U> void onNewComponent(	 Entity*	entity, U* component);
					  void onDeleteComponent(EntityId	id);

protected:
	bool updateLight(CLight* light, CTransform& transform, const Mat4f& viewMatrix, bool isTransformDirty);

protected:
	ParamBuffer<Light> _lightParamBuf;

};

inline u32									CLightSystem::lightCount() const	{ return sCast<u32>(components().size()); }

inline ParamBuffer<CLightSystem::Light>&	CLightSystem::lightParamBuf()		{ return _lightParamBuf; }


#endif

}