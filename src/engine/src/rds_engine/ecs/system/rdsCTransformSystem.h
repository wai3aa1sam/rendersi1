#pragma once

#include "rds_engine/common/rds_engine_common.h"

#include "rdsCSystem.h"
#include "../component/rdsCTransform.h"


namespace rds
{

class Scene;
class CTransform;

#if 0
#pragma mark --- rdsCTransformSystem-Decl ---
#endif // 0
#if 1

class CTransformSystem : public CSystemT<CTransform>
{
	friend class CTransform;
public:
	
public:
	CTransformSystem();
	~CTransformSystem();


protected:;
};


#endif

}