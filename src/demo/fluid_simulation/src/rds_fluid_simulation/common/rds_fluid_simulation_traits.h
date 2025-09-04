#pragma once

#include <rds_demo/common/rds_demo_traits.h>
#include "rds_fluid_simulation-config.h"

namespace rds {

#if 0
#pragma mark --- rdsFluidSimulationTraits-Impl ---
#endif // 0
#if 1
struct FluidSimulationDefaultTraits_T : public DemoTraits
{
public:
	using Base = DemoTraits;

public:


public:

};


#if !RDS_FLUID_SIMULATION_CUSTOM_TRAITS

using FluidSimulationTraits = FluidSimulationDefaultTraits_T;

#else

#endif // 

#endif

}
