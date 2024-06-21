#include "rds_render-pch.h"
#include "rdsDrawData_Base.h"

namespace rds
{

#if 0
#pragma mark --- rdsDrawData_Base-Impl ---
#endif // 0
#if 1

Tuple2f DrawData_Base::resolution()		const { return camera->viewport().size; }
Tuple2u DrawData_Base::resolution2u()	const { return Vec2u::s_cast(Vec2f(resolution())).toTuple2(); }


#endif

}