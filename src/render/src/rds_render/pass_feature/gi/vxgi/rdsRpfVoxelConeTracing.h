#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

#if 0

struct RpfGeometryBuffer_Result;

struct RpfVoxelConeTracing_Result
{
	RdgTextureHnd voxelTexColor;

	RdgTextureHnd texDiffuse;
	RdgTextureHnd texSpecular;

	/*
	* for mipmap impl only
	*/
	RdgTextureHnd voxelTexColorMip_PosX;
	RdgTextureHnd voxelTexColorMip_NegX;
	RdgTextureHnd voxelTexColorMip_PosY;
	RdgTextureHnd voxelTexColorMip_NegY;
	RdgTextureHnd voxelTexColorMip_PosZ;
	RdgTextureHnd voxelTexColorMip_NegZ;
};

#if 0
#pragma mark --- rdsRpfVoxelConeTracing-Decl ---
#endif // 0
#if 1

class RpfVoxelConeTracing : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfVoxelConeTracing);
public:
	static constexpr u32	s_kBorderWidth	= 0;
	static constexpr bool	s_isUse6Faces	= 1;

public:
	Result result;

	u32		voxelResolution				= 128;
	u32		clipmapMaxLevel				= 6;
	float	clipmapRegionWorldExtentL0	= sCast<float>(voxelResolution);
	u32		curLevel		= 0;
	u32		visualizeLevel	= 0;

	SPtr<Shader>				shaderVoxelization;


public:
	RdgPass* addVoxelizationPass(const DrawSettings& drawSettings, RpfGeometryBuffer_Result& gBuf);

	RdgPass* addVoxelizationDebugPass(RdgTextureHnd rtColor, RdgTextureHnd dsBuf);
	RdgPass* addVoxelVisualizationPass(RdgTextureHnd rtColor, RdgTextureHnd dsBuf);

	RdgPass* addAnisotropicMipmappingPass();
	RdgPass* addVoxelConeTracingPass(RpfGeometryBuffer_Result& gBuf, RdgTextureHnd depth);

protected:
	RpfVoxelConeTracing();
	~RpfVoxelConeTracing();

	void create();
	void destroy();

protected:
	void setupCommonParam(Material* mtl, u32 level);
	void setupMipmapParam(Material* mtl);
	void setupPassReadMipmap(RdgPass& pass, ShaderStageFlag stage);

private:
	SPtr<Texture3D>				_voxelTexRadiance;

	SPtr<Shader>				_shaderVoxelConeTracing;
	SPtr<Material>				_mtlVoxelConeTracing;

	SPtr<Shader>				_shaderVoxelizationDebug;
	SPtr<Material>				_mtlVoxelizationDebug;

	SPtr<Shader>				_shaderVoxelVisualization;
	SPtr<Material>				_mtlVoxelVisualization;

	SPtr<Shader>				_shaderClearVoxelClipmap;
	SPtr<Material>				_mtlClearVoxelClipmap;

	SPtr<Shader>				_shaderClearImage3D;
	SPtr<Material>				_mtlClearImage3D;

	SPtr<Shader>				_shaderClearImage2D;
	Vector<SPtr<Material>, 2>	_mtlClearImage2D;

	SPtr<Shader>				_shaderAnisotropicMipmapping;
	Vector<SPtr<Material>, 8>	_mtlAnisotropicMipmappings;
};

#endif
#endif // 0


}