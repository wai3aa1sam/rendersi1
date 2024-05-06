#pragma once

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

	UPtr<RpfVoxelConeTracing>	_rpfVxgi;
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
	Vec2f					resolution = Vec2f::s_zero();

	SPtr<Shader>			shaderVxgi;
	SPtr<Material>			mtlVxgi;

public:
	//static void getMakeFrustumsThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 tileCount);
	//static void getLightCulllingThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 tileCount);

public:
	RpfVoxelConeTracing();
	~RpfVoxelConeTracing();

	void create();
	void destroy();

	RdgPass* addLightingPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, RdgTextureHnd dsBuf);

	//RDS_NODISCARD RdgBufferHnd			addMakeFrustumsPass(RenderGraph* oRdGraph, DrawData* drawData);
	

public:
	bool isInvalidate(const Vec2f& resoluton_) const;

public:

};
#endif

}