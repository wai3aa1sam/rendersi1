#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

//		DepthTest	LessEqual

//		DepthTest	Always
//		DepthWrite	false

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

/*
reference:
~ https://learnopengl.com/PBR/IBL/Diffuse-irradiance
*/

#include "rdsPbrCommon.hlsl"

struct VertexIn
{
    float4 positionOS   : SV_POSITION;
};

struct PixelIn 
{
	float4 positionHCS  : SV_POSITION;
	float3 positionOS	: POSITION;
};

RDS_TEXTURE_CUBE(envCubeMap);
float roughness;

static const uint s_kSampleCount = 1024u;

PixelIn vs_main(VertexIn i)
{
    PixelIn o;

    o.positionHCS   = mul(RDS_MATRIX_MVP, i.positionOS);
    o.positionOS 	= i.positionOS.xyz;

    o.positionOS.x = roughness + i.positionOS.x;
    o.positionOS.x -= roughness;

    return o;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
    float3 o = float3(0.0, 0.0, 0.0);

    // tangent space
    float3 normal   = normalize(i.positionOS);
	float3 R        = normal;
	float3 viewDir  = R;

    int2 envCubeMapDims;
    RDS_TEXTURE_CUBE_GET_DIMENSIONS(envCubeMap, envCubeMapDims);
	float envMapDim = float(envCubeMapDims.x);

    float   totalWeight         = 0.0;   
    float3  prefilteredColor    = float3(0.0, 0.0, 0.0);     
    for(uint i = 0u; i < s_kSampleCount; ++i)
    {
        float2 sampleSeq    = SampleUtil_sequence_hammersley(i, s_kSampleCount);
        float3 halfDir      = Pbr_importanceSampleGGX(sampleSeq, normal, roughness);
        float3 L            = normalize(2.0 * dot(viewDir, halfDir) * halfDir - viewDir);

        float dotNL = max(dot(normal, L), 0.0);
        if(dotNL > 0.0)
        {
            float dotNH         = max(dot(normal,  halfDir), 0.0);
            float dotHV         = max(dot(halfDir, viewDir), 0.0);
            float normalDist    = Pbr_distributionGGX(dotNH, roughness);        // speculat highlight, distriGGX give us % of mircofacet align H 
            float pdf           = (normalDist * dotNH / (4.0 * dotHV)) + 0.0001;
            
            float saTexel       = 4.0 * rds_pi / (6.0 * envMapDim * envMapDim);     // Solid angle of current smple
            float saSample      = 1.0 / (float(s_kSampleCount) * pdf + 0.0001);     // Solid angle of 1 pixel across all cube faces

            float mipLevel      = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 

            prefilteredColor += RDS_TEXTURE_CUBE_SAMPLE_LOD(envCubeMap, L, mipLevel).rgb * dotNL;
            totalWeight      += dotNL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;
    o = prefilteredColor;
    
    return float4(o, 1.0);
}