#pragma once

/*
references:
~ https://github.com/compix/VoxelConeTracingGI/tree/master
~ https://github.com/steaklive/DXR-Sandbox-GI
*/

#include "rds_voxel_cone_tracing/common/rds_voxel_cone_tracing_common.h"


namespace rds
{

struct RpfVoxelConeTracing;

#if 0
#pragma mark --- rdsVoxelConeTracing-Decl ---
#endif // 0
#if 1

class VoxelConeTracing : public GraphicsDemo
{
public:
	using Base = GraphicsDemo;

public:
	virtual void onCreate()								override;
	virtual void onCreateScene(Scene* oScene)			override;
	virtual void onPrepareRender(RenderGraph* oRdGraph, DrawData* drawData) override;
	virtual void onExecuteRender(RenderGraph* oRdGraph, DrawData* drawData) override;

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) override;

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) override;

protected:
	SPtr<Shader>			_shaderPostProcess;
	SPtr<Material>			_mtlPostProcess;

	UPtr<RpfVoxelConeTracing>	_rpfVct;
};
RDS_DEMO(VoxelConeTracing);
#endif

#if 0
#pragma mark --- rdsRpfVoxelConeTracing-Decl ---
#endif // 0
#if 1

struct RpfVoxelConeTracing
{
public:
	u32		voxelResolution				= 64;
	u32		clipmapMaxLevel				= 6;
	float	clipmapRegionWorldExtentL0	= sCast<float>(voxelResolution);
	u32		curLevel = 0;

	SPtr<Shader>			shaderVct;
	SPtr<Material>			mtlVct;

	SPtr<Shader>			shaderVoxelization;
	SPtr<Material>			mtlVoxelization;

	SPtr<Shader>			shaderVoxelizationDebug;
	SPtr<Material>			mtlVoxelizationDebug;

	SPtr<Shader>			shaderVoxelVisualization;
	SPtr<Material>			mtlVoxelVisualization;

	SPtr<Shader>			shaderClearImage3D;
	SPtr<Material>			mtlClearImage3D;

	SPtr<Shader>				shaderAnisotropicMipmapping;
	Vector<SPtr<Material>, 8>	mtlAnisotropicMipmappings;

	struct Result
	{
		RdgTextureHnd voxelTexColor;

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
	Result result;

public:
	//static void getMakeFrustumsThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 tileCount);
	//static void getLightCulllingThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 tileCount);

public:
	RpfVoxelConeTracing();
	~RpfVoxelConeTracing();

	void create();
	void destroy();

	RdgPass* addVoxelizationPass(RenderGraph* oRdGraph, DrawData* drawData);
	RdgPass* addLightingPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, RdgTextureHnd dsBuf);

	RdgPass* addVoxelizationDebugPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, RdgTextureHnd dsBuf);
	RdgPass* addVoxelVisualizationPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, RdgTextureHnd dsBuf);

	RdgPass* addAnisotropicMipmappingPass(RenderGraph* oRdGraph, DrawData* drawData);

	//RDS_NODISCARD RdgBufferHnd			addMakeFrustumsPass(RenderGraph* oRdGraph, DrawData* drawData);
	
	RenderMesh rdMeshVisualizeVoxel;

public:
	void setCommonParam(Material* mtl, u32 level, DrawData* drawData);
};


struct VoxelClipmapRegion
{
public:
	using T			= float;
	using Tuple3	= Tuple3<T>;
	using Vec3		= Vec3<T>;

public:
	Tuple3i	minPosLocal	= Vec3i::s_zero();
	Tuple3i	extentLocal	= Vec3i::s_zero();
	T		voxelSize	= T(0);
};

struct VctVoxelClipmapUtil
{
public:
	using SizeType	= DemoTraits::SizeType;

	using T			= float;
	using Tuple3	= Tuple3<T>;
	using Vec3		= Vec3<T>;

public:
	static T computeVoxelSize(int level, T extentWsL0, SizeType voxelResoultion) { return (extentWsL0 * math::pow2(sCast<T>(level))) / sCast<T>(voxelResoultion); }

	static Vec3 getMinPosWs(VoxelClipmapRegion v)					{ return Vec3::s_cast(Vec3i{v.minPosLocal}) * v.voxelSize; }
	static Vec3 getMaxPosWs(VoxelClipmapRegion v)					{ return Vec3::s_cast(Vec3i{v.minPosLocal} + Vec3i{v.extentLocal}) * v.voxelSize; }
	static Vec3 getExtentWs(VoxelClipmapRegion v)					{ return Vec3::s_cast(Vec3i{v.extentLocal}) * v.voxelSize; }
	static Vec3 getCenter(	VoxelClipmapRegion v, Vec3 cameraPos)	{ auto texelSize = v.voxelSize * T(2.0); return Vec3::s_cast(Vec3{math::floor(cameraPos.x / texelSize), math::floor(cameraPos.y / texelSize), math::floor(cameraPos.z / texelSize)} * texelSize); }

	static VoxelClipmapRegion makeClipmapRegion(int level, T extentWsL0, SizeType voxelResoultion)
	{
		int resolution = sCast<int>(voxelResoultion);
		VoxelClipmapRegion o;
		o.minPosLocal = -Vec3i{resolution, resolution, resolution} / 2;
		o.extentLocal =  Vec3i{resolution, resolution, resolution};
		o.voxelSize	  = computeVoxelSize(level, extentWsL0, voxelResoultion);
		return o;
	}

};

#endif

}