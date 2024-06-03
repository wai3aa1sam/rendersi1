#if 0
Shader {
	Properties {
		
	}
	
	Pass {
		CsFunc		Cs_preMipmap
	}

	Pass {
		CsFunc		Cs_mipmap
	}

	Permutation
	{
		
	}
}
#endif

#include "rdsVct_Common.hlsl"

struct ComputeIn 
{
	uint3 groupId           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 groupThreadId     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadId  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  groupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

static const uint3 anisotropicOffsets[8] =
{
    uint3(1, 1, 1),
	uint3(1, 1, 0),
	uint3(1, 0, 1),
	uint3(1, 0, 0),
	uint3(0, 1, 1),
	uint3(0, 1, 0),
	uint3(0, 0, 1),
	uint3(0, 0, 0)
};

uint3 	mip_dimemsions;
uint 	mip_level;

RDS_TEXTURE_3D_T(float4, src_image);

RDS_IMAGE_3D(float4, src_image_pos_x);
RDS_IMAGE_3D(float4, src_image_neg_x);
RDS_IMAGE_3D(float4, src_image_pos_y);
RDS_IMAGE_3D(float4, src_image_neg_y);
RDS_IMAGE_3D(float4, src_image_pos_z);
RDS_IMAGE_3D(float4, src_image_neg_z);

RDS_IMAGE_3D(float4, dst_image_pos_x);
RDS_IMAGE_3D(float4, dst_image_neg_x);
RDS_IMAGE_3D(float4, dst_image_pos_y);
RDS_IMAGE_3D(float4, dst_image_neg_y);
RDS_IMAGE_3D(float4, dst_image_pos_z);
RDS_IMAGE_3D(float4, dst_image_neg_z);

[numThreads(8, 8, 8)]
void Cs_preMipmap(ComputeIn input)
{
	bool isInBoundary = Image_isInBoundary(input.dispatchThreadId, mip_dimemsions);
	if (!isInBoundary)
		 return;
	
	uint3 dispatchThreadId 	= input.dispatchThreadId;
	uint3 srcImageCoords 	= dispatchThreadId * 2;
	uint3 dstImageCoords 	= dispatchThreadId;

	float4 values[8];
	for (uint i = 0; i < 8; ++i)
	{
		values[i] = RDS_TEXTURE_3D_T_LOAD(float4, src_image, srcImageCoords + anisotropicOffsets[i], 0);
	}

	RDS_IMAGE_3D_GET(float4, dst_image_pos_x)[dstImageCoords] = 
		(values[4] + values[0] * (1 - values[4].a) + values[5] + values[1] * (1 - values[5].a) +
         values[6] + values[2] * (1 - values[6].a) + values[7] + values[3] * (1 - values[7].a)) * 0.25f;

	RDS_IMAGE_3D_GET(float4, dst_image_neg_x)[dstImageCoords] = 
		(values[0] + values[4] * (1 - values[0].a) + values[1] + values[5] * (1 - values[1].a) +
		 values[2] + values[6] * (1 - values[2].a) + values[3] + values[7] * (1 - values[3].a)) * 0.25f;

	RDS_IMAGE_3D_GET(float4, dst_image_pos_y)[dstImageCoords] = 
		(values[2] + values[0] * (1 - values[2].a) + values[3] + values[1] * (1 - values[3].a) +
		 values[7] + values[5] * (1 - values[7].a) + values[6] + values[4] * (1 - values[6].a)) * 0.25f;

	RDS_IMAGE_3D_GET(float4, dst_image_neg_y)[dstImageCoords] = 
		(values[0] + values[2] * (1 - values[0].a) + values[1] + values[3] * (1 - values[1].a) +
		 values[5] + values[7] * (1 - values[5].a) + values[4] + values[6] * (1 - values[4].a)) * 0.25f;

	RDS_IMAGE_3D_GET(float4, dst_image_pos_z)[dstImageCoords] = 
		(values[1] + values[0] * (1 - values[1].a) + values[3] + values[2] * (1 - values[3].a) +
		 values[5] + values[4] * (1 - values[5].a) + values[7] + values[6] * (1 - values[7].a)) * 0.25f;

	RDS_IMAGE_3D_GET(float4, dst_image_neg_z)[dstImageCoords] = 
		(values[0] + values[1] * (1 - values[0].a) + values[2] + values[3] * (1 - values[2].a) +
		 values[4] + values[5] * (1 - values[4].a) + values[6] + values[7] * (1 - values[6].a)) * 0.25f;
}

[numThreads(8, 8, 8)]
void Cs_mipmap(ComputeIn input)
{
	bool isInBoundary = Image_isInBoundary(input.dispatchThreadId, mip_dimemsions);
	if (!isInBoundary)
		 return;

	uint3 dispatchThreadId 	= input.dispatchThreadId;

	#if 0
	if (mip_level == 0)
	{
		RDS_IMAGE_3D_GET(float4, dst_image_pos_x)[dispatchThreadId] = RDS_IMAGE_3D_GET(float4, src_image_pos_x)[dispatchThreadId];
		RDS_IMAGE_3D_GET(float4, dst_image_neg_x)[dispatchThreadId] = RDS_IMAGE_3D_GET(float4, src_image_neg_x)[dispatchThreadId];
		RDS_IMAGE_3D_GET(float4, dst_image_pos_y)[dispatchThreadId] = RDS_IMAGE_3D_GET(float4, src_image_pos_y)[dispatchThreadId];
		RDS_IMAGE_3D_GET(float4, dst_image_neg_y)[dispatchThreadId] = RDS_IMAGE_3D_GET(float4, src_image_neg_y)[dispatchThreadId];
		RDS_IMAGE_3D_GET(float4, dst_image_pos_z)[dispatchThreadId] = RDS_IMAGE_3D_GET(float4, src_image_pos_z)[dispatchThreadId];
		RDS_IMAGE_3D_GET(float4, dst_image_neg_z)[dispatchThreadId] = RDS_IMAGE_3D_GET(float4, src_image_neg_z)[dispatchThreadId];
		return;
	}
	#endif

	float4 	values[8];
	uint3 	srcImageCoords = dispatchThreadId * 2;
	uint3 	dstImageCoords = dispatchThreadId;

	for (uint i = 0; i < 8; ++i)
	{
		values[i] = RDS_IMAGE_3D_LOAD(float4, src_image_pos_x, srcImageCoords + anisotropicOffsets[i]);
	}
	RDS_IMAGE_3D_GET(float4, dst_image_pos_x)[dstImageCoords] = 
		(values[4] + values[0] * (1 - values[4].a) + values[5] + values[1] * (1 - values[5].a) +
         values[6] + values[2] * (1 - values[6].a) + values[7] + values[3] * (1 - values[7].a)) * 0.25f;

	for (uint i = 0; i < 8; ++i)
	{
		values[i] = RDS_IMAGE_3D_LOAD(float4, src_image_neg_x, srcImageCoords + anisotropicOffsets[i]);
	}
	RDS_IMAGE_3D_GET(float4, dst_image_neg_x)[dstImageCoords] = 
		(values[0] + values[4] * (1 - values[0].a) + values[1] + values[5] * (1 - values[1].a) +
		 values[2] + values[6] * (1 - values[2].a) + values[3] + values[7] * (1 - values[3].a)) * 0.25f;

	for (uint i = 0; i < 8; ++i)
	{
		values[i] = RDS_IMAGE_3D_LOAD(float4, src_image_pos_y, srcImageCoords + anisotropicOffsets[i]);
	}
	RDS_IMAGE_3D_GET(float4, dst_image_pos_y)[dstImageCoords] = 
		(values[2] + values[0] * (1 - values[2].a) + values[3] + values[1] * (1 - values[3].a) +
		 values[7] + values[5] * (1 - values[7].a) + values[6] + values[4] * (1 - values[6].a)) * 0.25f;

	for (uint i = 0; i < 8; ++i)
	{
		values[i] = RDS_IMAGE_3D_LOAD(float4, src_image_neg_y, srcImageCoords + anisotropicOffsets[i]);
	}
	RDS_IMAGE_3D_GET(float4, dst_image_neg_y)[dstImageCoords] = 
		(values[0] + values[2] * (1 - values[0].a) + values[1] + values[3] * (1 - values[1].a) +
		 values[5] + values[7] * (1 - values[5].a) + values[4] + values[6] * (1 - values[4].a)) * 0.25f;

	for (uint i = 0; i < 8; ++i)
	{
		values[i] = RDS_IMAGE_3D_LOAD(float4, src_image_pos_z, srcImageCoords + anisotropicOffsets[i]);
	}
	RDS_IMAGE_3D_GET(float4, dst_image_pos_z)[dstImageCoords] = 
		(values[1] + values[0] * (1 - values[1].a) + values[3] + values[2] * (1 - values[3].a) +
		 values[5] + values[4] * (1 - values[5].a) + values[7] + values[6] * (1 - values[7].a)) * 0.25f;

	for (uint i = 0; i < 8; ++i)
	{
		values[i] = RDS_IMAGE_3D_LOAD(float4, src_image_neg_z, srcImageCoords + anisotropicOffsets[i]);
	}
	RDS_IMAGE_3D_GET(float4, dst_image_neg_z)[dstImageCoords] = 
		(values[0] + values[1] * (1 - values[0].a) + values[2] + values[3] * (1 - values[2].a) +
		 values[4] + values[5] * (1 - values[4].a) + values[6] + values[7] * (1 - values[6].a)) * 0.25f;
}