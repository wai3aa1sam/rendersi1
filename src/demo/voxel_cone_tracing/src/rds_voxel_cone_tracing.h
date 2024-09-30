#pragma once

/*
references:
~ https://github.com/compix/VoxelConeTracingGI/tree/master
~ https://github.com/steaklive/DXR-Sandbox-GI
*/

#include "rds_voxel_cone_tracing/common/rds_voxel_cone_tracing_common.h"

namespace rds
{

class	RpfGeometryBuffer;
struct	RpfGeometryBuffer_Result;

class	RpfCascadedShadowMaps;
struct  RpfCascadedShadowMaps_Result;

class	RpfVoxelConeTracing;

#if 0
#pragma mark --- rdsVoxelConeTracing-Decl ---
#endif // 0
#if 1

class VoxelConeTracing : public GraphicsDemo
{
public:
	using Base = GraphicsDemo;

public:
	int		voxelTextureIdx = 0;
	String	voxelTextureName;

	bool	isShowVoxelClipmap		= false;
	bool	isShowDebugVoxelClipmap = false;

public:
	RdgPass* addTestCascadedShadowMappingPass(RenderGraph* oRdGraph, DrawData* drawData, const DrawSettings& drawSettings, RdgTextureHnd rtColor, RdgTextureHnd dsBuf, RpfCascadedShadowMaps_Result& csmResult);

public:
	virtual void onCreate()								override;
	virtual void onCreateScene(Scene* oScene)			override;
	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline) override;
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline) override;

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) override;

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) override;

protected:
	RpfGeometryBuffer*			_rpfGeomBuf = nullptr;
	RpfCascadedShadowMaps*		_rpfCsm		= nullptr;
	RpfVoxelConeTracing*		_rpfVct		= nullptr;
};
RDS_DEMO(VoxelConeTracing);
#endif

}