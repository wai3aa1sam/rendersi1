#pragma once

#include <rds_demo/common/rds_demo_traits.h>
#include "rds_hello_triangle-config.h"

namespace rds {

#if 0
#pragma mark --- rdsHelloTriangleTraits-Impl ---
#endif // 0
#if 1
struct HelloTriangleDefaultTraits_T : public DemoTraits
{
public:
	using Base = DemoTraits;

public:


public:

};


#if !RDS_HELLO_TRIANGLE_CUSTOM_TRAITS

using HelloTriangleTraits = HelloTriangleDefaultTraits_T;

#else

#endif // 

#endif

}
