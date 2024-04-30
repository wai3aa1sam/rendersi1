#ifndef __rdsConstant_Common_HLSL__
#define __rdsConstant_Common_HLSL__

#include "built-in/shader/rdsShaderInterop.hlsl"

static const int rds_LightType_None         = RDS_LIGHT_TYPE_NONE;
static const int rds_LightType_Point        = RDS_LIGHT_TYPE_POINT;
static const int rds_LightType_Spot         = RDS_LIGHT_TYPE_SPOT;
static const int rds_LightType_Directional  = RDS_LIGHT_TYPE_DIRECTIONAL;
static const int rds_LightType_Area         = RDS_LIGHT_TYPE_AREA;

static const float rds_pi           = 3.14159265359;
static const float rds_epsilon      = 0.00001;


#endif