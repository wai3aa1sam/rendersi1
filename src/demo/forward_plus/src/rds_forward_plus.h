#pragma once

#include "rds_forward_plus/common/rds_forward_plus_common.h"


namespace rds
{

struct RfpForwardPlus;

#if 0
#pragma mark --- rdsForwardPlus-Decl ---
#endif // 0
#if 1

class ForwardPlus : public GraphicsDemo
{
public:
	using Base = GraphicsDemo;

public:
	virtual void onCreate()								override;
	virtual void onCreateScene(Scene* oScene)			override;
	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline) override;
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline) override;

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) override;

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) override;

protected:
	SPtr<Shader>			_shaderForwardPlus;
	SPtr<Material>			_mtlForwardPlus;

	SPtr<Shader>			_shaderPostProcess;
	SPtr<Material>			_mtlPostProcess;

	UPtr<RfpForwardPlus>	_rfpForwardPlus;
};
RDS_DEMO(ForwardPlus);
#endif

#if 0
#pragma mark --- rdsRfpForwardPlus-Decl ---
#endif // 0
#if 1
/*
* view screen viewport as a grid, blocks in a grid, and each block has n tile
*/
struct RfpForwardPlus
{
public:
	using DebugIndexType = u32;

public:
	static constexpr u32			s_kTileCount		= 32;
	static constexpr u32			s_kBlockSize		= s_kTileCount;
	static constexpr u32			s_kEstLightPerGrid	= 256;

	static constexpr DebugIndexType s_debugFrustumIndices[] = {	0,1,
		0,2,
		3,1,
		3,2,
	};

public:
	struct LightCullingResult
	{
		RdgPass* lightCullingPass = nullptr;

		RdgBufferHnd  opaque_lightIndexList;
		RdgTextureHnd opaque_lightGrid;

		RdgBufferHnd  transparent_lightIndexList;
		RdgTextureHnd transparent_lightGrid;

	public:
		RdgBufferHnd	lightIndexList(	bool isOpaque) { return isOpaque ? opaque_lightIndexList	: transparent_lightIndexList; }
		RdgTextureHnd	lightGrid(		bool isOpaque) { return isOpaque ? opaque_lightGrid			: transparent_lightGrid; }

	};	

public:
	SPtr<Shader>			shaderFwdpMakeFrustum;
	SPtr<Material>			mtlFwdpMakeFrustum;
	Vec2f					resolution = Vec2f::s_zero();
	SPtr<RenderGpuBuffer>	gpuFrustums;

	SPtr<RenderGpuBuffer>	debugFrustumsPts;
	SPtr<RenderGpuBuffer>	debugFrustumsIdxBuf;

	SPtr<Shader>			shaderFwdp;
	SPtr<Material>			mtlFwdp;

	SPtr<Shader>			shaderFwdpLighting;
	SPtr<Material>			mtlFwdpLighting;

public:
	static void getMakeFrustumsThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 tileCount);
	static void getLightCulllingThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 tileCount);

public:
	RfpForwardPlus();
	~RfpForwardPlus();

	void create();
	void destroy();

	RDS_NODISCARD RdgBufferHnd			addMakeFrustumsPass(RenderGraph* oRdGraph, DrawData* drawData);
	RDS_NODISCARD LightCullingResult	addLightCullingPass(RenderGraph* oRdGraph, DrawData* drawData, RdgBufferHnd frustums, RdgTextureHnd texDepth);
	RdgPass*							addLightingPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd colorRt, RdgTextureHnd dsBuf, LightCullingResult& lightCulling, bool isOpaque, bool isClearColor);

	void renderDebugMakeFrustums(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd colorRt);

public:
	bool isInvalidate(const Vec2f& resoluton_) const;

public:

};
#endif

}