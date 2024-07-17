#ifndef __rdsVct_ConeTracing_Common_HLSL__
#define __rdsVct_ConeTracing_Common_HLSL__

/*
references:
~ https://github.com/compix/VoxelConeTracingGI/tree/master
~ https://github.com/turanszkij/WickedEngine
*/

#include "built-in/shader/rds_shader.hlsl"

static const float Vct_kStepSize = 1.0;

#define RDS_VCT_DIFFUSE_CONE_COUNT 6

#if RDS_VCT_DIFFUSE_CONE_COUNT == 6

static const uint 	Vct_kDiffuseConeCount 		= 6;        
static const float 	Vct_kDiffuseConeAperture 	= 0.577f; // 6 cones, 60 deg, tan (cone angle / 2)

const static float3 Vct_kDiffuseConeDirections[] = {
    float3( 0.0,       1.0,  0.0),
    float3( 0.0,       0.5,  0.866025),
    float3( 0.823639,  0.5,  0.267617),
    float3( 0.509037,  0.5, -0.7006629),
    float3(-0.50937,   0.5, -0.7006629),
    float3(-0.823639,  0.5,  0.267617)
};

#elif RDS_VCT_DIFFUSE_CONE_COUNT == 16

static const uint 	Vct_kDiffuseConeCount 		= 16;       // 8 cone per hemisphere
static const float 	Vct_kDiffuseConeAperture 	= 0.872665; // tan (cone angle / 2)

const static float3 Vct_kDiffuseConeDirections[] = {
    float3( 0.57735,     0.57735,  	 0.57735),
    float3( 0.57735,    -0.57735, 	-0.57735),
    float3(-0.57735,     0.57735, 	-0.57735),
    float3(-0.57735,    -0.57735,  	 0.57735),
    float3(-0.903007,   -0.182696, 	-0.388844),
    float3(-0.903007,    0.182696, 	 0.388844),
    float3( 0.903007,   -0.182696, 	 0.388844),
    float3( 0.903007,    0.182696, 	-0.388844),
    float3(-0.388844,   -0.903007, 	-0.182696),
    float3(	0.388844,   -0.903007, 	 0.182696),
    float3(	0.388844,    0.903007, 	-0.182696),
    float3(-0.388844,    0.903007, 	 0.182696),
    float3(-0.182696,   -0.388844, 	-0.903007),
    float3( 0.182696,    0.388844, 	-0.903007),
    float3(-0.182696,    0.388844, 	 0.903007),
    float3( 0.182696,   -0.388844, 	 0.903007),
};

#elif RDS_VCT_DIFFUSE_CONE_COUNT == 32

static const uint 	Vct_kDiffuseConeCount 		= RDS_VCT_DIFFUSE_CONE_COUNT;   // 16 cone per hemisphere
static const float 	Vct_kDiffuseConeAperture 	= 0.628319;

static const float3 Vct_kDiffuseConeDirections[] = {
    float3( 0.898904, 	0.435512, 	 0.0479745),
    float3( 0.898904,  -0.435512, 	-0.0479745),
    float3( 0.898904, 	0.0479745, 	-0.435512),
    float3( 0.898904,  -0.0479745, 	 0.435512),
    float3(-0.898904, 	0.435512, 	-0.0479745),
    float3(-0.898904,  -0.435512, 	 0.0479745),
    float3(-0.898904, 	0.0479745, 	 0.435512),
    float3(-0.898904,  -0.0479745, 	-0.435512),
    float3( 0.0479745, 	0.898904, 	 0.435512),
    float3(-0.0479745,  0.898904, 	-0.435512),
    float3(-0.435512, 	0.898904, 	 0.0479745),
    float3( 0.435512, 	0.898904, 	-0.0479745),
    float3(-0.0479745, -0.898904, 	 0.435512),
    float3( 0.0479745, -0.898904, 	-0.435512),
    float3( 0.435512,  -0.898904, 	 0.0479745),
    float3(-0.435512,  -0.898904, 	-0.0479745),
    float3( 0.435512, 	0.0479745, 	 0.898904),
    float3(-0.435512,  -0.0479745, 	 0.898904),
    float3( 0.0479745, -0.435512, 	 0.898904),
    float3(-0.0479745,  0.435512, 	 0.898904),
    float3( 0.435512,  -0.0479745, 	-0.898904),
    float3(-0.435512, 	0.0479745, 	-0.898904),
    float3( 0.0479745,  0.435512, 	-0.898904),
    float3(-0.0479745, -0.435512, 	-0.898904),
    float3( 0.57735, 	0.57735, 	 0.57735),
    float3( 0.57735, 	0.57735, 	-0.57735),
    float3( 0.57735,   -0.57735, 	 0.57735),
    float3( 0.57735,   -0.57735, 	-0.57735),
    float3(-0.57735, 	0.57735, 	 0.57735),
    float3(-0.57735, 	0.57735, 	-0.57735),
    float3(-0.57735,   -0.57735, 	 0.57735),
    float3(-0.57735,   -0.57735, 	-0.57735),
};

#endif


#endif