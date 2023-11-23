#if 0
Shader {
	Properties {
		Float	test  = 0.5
		Vec4f	test2 = {0,0,0,1}
		
		[DisplayName="Color Test"]
		Color4f	color = {1,1,1,1}
		
		Texture2D mainTex
		Texture2D heightTex
	}
	
	Pass {
		// Queue	"Transparent"
		Cull		None

		DepthTest	LessEqual

//		DepthTest	Always
//		DepthWrite	false

		Wireframe true

		//BlendRGB 	Add One OneMinusSrcAlpha
		//BlendAlpha	Add One OneMinusSrcAlpha
		
		VsFunc		vs_main
		PsFunc		ps_main
	}
}
#endif

struct VertexIn {
	float4 positionOS : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelIn {
	float4 positionHCS : SV_POSITION;
	float4 positionWS  : TEXCOORD10;
	float2 uv		: TEXCOORD0;
	float4 color  	: COLOR;
	float3 normal 	: NORMAL;
};

cbuffer rds_params
{
	float3		sge_camera_pos;
	float3		sge_light_pos;
	float3		sge_light_dir;
	float		sge_light_power;
	float3		sge_light_color;

	float		sge_test_array[10];
	float4		sge_test_array2[10];

	float4x4	sge_matrix_view;
	float4x4	sge_matrix_proj;
	float4x4	sge_matrix_mvp;
}

cbuffer rds_params2
{
	float test0[10];
}

cbuffer rds_params3
{
	float test2[10];
}

cbuffer rds_params4
{
	float test3[10];
}

float2	_patch_id;
float2	_patch_pos;
float2	_patch_uv;

struct TestStruct
{
	float rds_struct0;
	float rds_structArray[10];
};
//TestStruct rdsTestStruct;

float3  _terrainPos;

Texture2D mainTex;
SamplerState mainTex_Sampler;

Texture2D heightTex;
SamplerState heightTex_Sampler;

PixelIn vs_main(VertexIn i) {
	PixelIn o;

	o.uv   = i.uv + _patch_uv;
	o.uv.x = 1.0 - o.uv.x;

	float height = heightTex.SampleLevel(heightTex_Sampler, o.uv, 0).r * 200.0f;
	//height = height * 60.0f - 20.0f;
	i.positionOS.y = height;

	float4 positionOS = float4(	i.positionOS.x, i.positionOS.y, i.positionOS.z, 1);
	positionOS.y = 0;
	
	positionOS.xz  += _patch_pos;
	positionOS.xyz += _terrainPos;

	o.positionHCS = mul(sge_matrix_mvp,   positionOS);
	
	//o.uv.x += rdsTestStruct.rds_struct0 + rdsTestStruct.rds_structArray[2];
	o.uv.x += sge_test_array[2];
	o.uv.x += mainTex.SampleLevel(mainTex_Sampler, o.uv, 0).r * 200.0f;;

	o.uv.x += test0[2];
	o.uv.x += test2[2];
	//o.uv.x += test3[2];

	o.uv     = i.uv;
	o.color	 = i.color;
	o.normal = i.normal;
	return o;
}

struct Surface {
	float3 positionWS;
	float3 normal;	
	float3 color;
	float3 ambient;
	float3 diffuse;
	float3 specular;
	float  shininess;
};

float3 Color_Linear_to_sRGB(float3 x) 
{ 
	//return select(x < 0.0031308, 12.92 * x, 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719); 
	return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; 
}
float3 Color_sRGB_to_Linear(float3 x) 
{ 
	//return select(x < 0.04045, x / 12.92, -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864); 
	return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; 
}

float3 lighting_blinn_phong(Surface s) {
	float3 normal   = normalize(s.normal);
	float3 lightDir = s.positionWS - sge_light_pos;
	float  lightDistance = length(sge_light_dir);
	lightDir = normalize(lightDir);

	float lambertian = max(dot(normal, -sge_light_dir), 0);
	float specularPower = 0;

	if (lambertian > 0) {
		float3 viewDir = normalize(sge_camera_pos - s.positionWS);

		if (1) { // blinn-phong
			float3 halfDir = normalize(viewDir - lightDir);
			float specAngle = saturate(dot(halfDir, normal));
			specularPower = pow(specAngle, s.shininess);

		} else { // phong
			float3 reflectDir = reflect(-lightDir, normal);
			float specAngle = max(dot(reflectDir, viewDir), 0);
			specularPower = pow(specAngle, s.shininess / 4.0);
		}
	}

	float3 outLightColor = sge_light_color * sge_light_power / (lightDistance * lightDistance);

	float3 outAmbient  = s.ambient;
	float3 outDiffuse  = s.diffuse  * lambertian    * outLightColor;
	float3 outSpecular = s.specular * specularPower * outLightColor;

	float3 outColor = (outAmbient + outDiffuse + outSpecular) * s.color;
	return outColor;
}

float4 ps_main(PixelIn i) : SV_TARGET
{
	Surface s;
	s.positionWS = i.positionWS.xyz;
	s.normal     = i.normal;
	s.color	     = float3(1, 1, 1);
	s.specular   = float3(0.8, 0.8, 0.8);
	s.ambient    = float3(0.2, 0.2, 0.2);
	s.diffuse	 = float3(1, 1, 1);
	s.shininess	 = 1;

	s.shininess += test0[2];
	s.shininess += test2[2];
	//s.shininess += test3[2];

	float4 texCol = mainTex.Sample(mainTex_Sampler, i.uv);
	float3 color = lighting_blinn_phong(s) * texCol.rgb;
	return float4(Color_Linear_to_sRGB(color), 1);
}
