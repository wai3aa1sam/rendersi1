#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"
#include <rds_render_api_layer/buffer/rdsParamBuffer.h>

namespace rds
{
namespace shaderInterop
{
#include "../../../example/Test000/asset/shader/demo/voxel_cone_tracing/rdsVct_ShaderInterop.hlsl"
}
}


namespace rds
{

struct RpfGeometryBuffer_Result;
struct RpfCascadedShadowMaps_Result;

struct RpfVoxelConeTracing_Result : public RenderPassFeature_Result
{
	RdgTextureHnd voxelTexRadiance;

	RdgTextureHnd texDiffuse;
	RdgTextureHnd texSpecular;

	/*
	* for mipmap impl only
	*/
	RdgTextureHnd voxelTexRadianceMip_PosX;
	RdgTextureHnd voxelTexRadianceMip_NegX;
	RdgTextureHnd voxelTexRadianceMip_PosY;
	RdgTextureHnd voxelTexRadianceMip_NegY;
	RdgTextureHnd voxelTexRadianceMip_PosZ;
	RdgTextureHnd voxelTexRadianceMip_NegZ;
};

struct RpfVoxelConeTracing_Param : public RenderPassFeature_Param
{

};

#if 0
#pragma mark --- rdsRpfVoxelConeTracing-Decl ---
#endif // 0
#if 1

class RpfVoxelConeTracing : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfVoxelConeTracing);
public:
	static constexpr u32	s_kBorderWidth		= 0;
	static constexpr bool	s_isUse6Faces		= 1;
	static constexpr u32	s_kMaxClipmapLevel	= VCT_MAX_CLIPMAP_LEVEL;

public:
	using VoxelClipmap  = shaderInterop::VoxelClipmap;
	using VoxelClipmaps = Vector<VoxelClipmap, 6>;

public:
	Result result;

	u32		voxelResolution				= 128;
	u32		clipmapMaxLevel				= 6;
	float	clipmapRegionWorldExtentL0	= sCast<float>(voxelResolution);
	u32		curLevel		= 0;
	u32		visualizeLevel	= 0;
	Vec3f	prevCameraPos	= Vec3f::s_zero();

	ParamBuffer<VoxelClipmap> voxelClipmaps;

public:
	RdgPass* addVoxelizationPass(const DrawSettings& drawSettings, const RpfCascadedShadowMaps_Result& csmResult);
	RdgPass* addOpacityAlphaPass(RpfVoxelConeTracing_Result* oResult);
	RdgPass* addCheckAlphaPass();

	RdgPass* addVoxelizationDebugPass(RdgTextureHnd rtColor, RdgTextureHnd dsBuf);
	RdgPass* addVoxelVisualizationPass(RdgTextureHnd rtColor, RdgTextureHnd dsBuf);

	RdgPass* addAnisotropicMipmappingPass();
	RdgPass* addVoxelConeTracingPass(RdgTextureHnd depth, RpfGeometryBuffer_Result& gBuf);
	RdgPass* addLightingPass(RdgTextureHnd rtColor, RdgTextureHnd depth, RpfGeometryBuffer_Result& gBuf);

public:
	Shader*		getVoxelizationShader();

	Material*	getVoxelVisualizationMaterial();
	Material*	getVoxelConeTracingMaterial();

protected:
	RpfVoxelConeTracing();
	virtual ~RpfVoxelConeTracing();

	void create();
	void destroy();

protected:
	void updateClipmap(u32 level, DrawData_Base* drawData);
	void setupCommonParam(Material* mtl, u32 level);
	void setupMipmapParam(Material* mtl);
	void setupPassReadMipmap(RdgPass& pass, ShaderStageFlag stage);

private:
	SPtr<Texture3D>				_voxelTexRadiance;

	SPtr<Shader>				_shaderVoxelization;
	SPtr<Material>				_mtlVoxelization;

	SPtr<Shader>				_shaderOpacityAlpha;
	SPtr<Material>				_mtlOpacityAlpha;

	SPtr<Shader>				_shaderVoxelConeTracing;
	SPtr<Material>				_mtlVoxelConeTracing;

	SPtr<Shader>				_shaderVoxelizationDebug;
	SPtr<Material>				_mtlVoxelizationDebug;

	SPtr<Shader>				_shaderVoxelVisualization;
	SPtr<Material>				_mtlVoxelVisualization;

	SPtr<Shader>				_shaderClearVoxelClipmap;
	SPtr<Material>				_mtlClearVoxelClipmap;

	Vector<SPtr<Material>, 2>	_mtlClearImage2Ds;
	SPtr<Material>				_mtlClearImage3D;

	SPtr<Shader>				_shaderAnisotropicMipmapping;
	Vector<SPtr<Material>, 8>	_mtlsAnisotropicMipmapping;

	SPtr<Shader>				_shaderCheckAlpha;
	SPtr<Material>				_mtlCheckAlpha;

	SPtr<Shader>				_shaderLighting;
	SPtr<Material>				_mtlLighting;
};

inline Shader*		RpfVoxelConeTracing::getVoxelizationShader()				{ return _shaderVoxelization; }

inline Material*	RpfVoxelConeTracing::getVoxelVisualizationMaterial()		{ return _mtlVoxelVisualization; }
inline Material*	RpfVoxelConeTracing::getVoxelConeTracingMaterial()			{ return _mtlVoxelConeTracing; }


#endif




}