#pragma once

#include "rds_forward_plus/common/rds_forward_plus_common.h"


namespace rds
{

class RpfPreDepth;
class RpfForwardPlus;

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
	RpfPreDepth*			_rpfPreDepth	= nullptr;
	RpfForwardPlus*			_rpfForwardPlus	= nullptr;

};
RDS_DEMO(ForwardPlus);
#endif

#if 0
#pragma mark --- rdsRpfForwardPlus-Decl ---
#endif // 0
#if 1
/*
* view screen viewport as a grid, blocks in a grid, and each block has n tile
*/

struct RpfForwardPlus_Result
{
	RdgBufferHnd	cullingFrustum;

	RdgPass*		lightCullingPass = nullptr;

	RdgBufferHnd	opaque_lightIndexList;
	RdgTextureHnd	opaque_lightGrid;

	RdgBufferHnd	transparent_lightIndexList;
	RdgTextureHnd	transparent_lightGrid;

public:
	RdgBufferHnd	lightIndexList(	bool isOpaque) { return isOpaque ? opaque_lightIndexList	: transparent_lightIndexList; }
	RdgTextureHnd	lightGrid(		bool isOpaque) { return isOpaque ? opaque_lightGrid			: transparent_lightGrid; }

};

class RpfForwardPlus : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfForwardPlus);
public:
	using DebugIndexType = u32;

public:
	static constexpr u32			s_kTileCount		= 32;
	static constexpr u32			s_kBlockSize		= s_kTileCount;
	static constexpr u32			s_kEstLightPerGrid	= 128;

	static constexpr DebugIndexType s_debugFrustumIndices[] = 
	{	
		0, 1,
		0, 2,
		3, 1,
		3, 2,
	};

public:
	/*
	*	1 block for 1 furstum == 1 thread for 1 frustum
	*/
	static void getMakeFrustumsThreadParamTo( Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 tileCount);
	static void getLightCulllingThreadParamTo(Vec2u* oNThreads, Vec2u* oNThreadGrps, Vec2f resolution_, u32 tileCount);
	
public:

	Vec2f					resolution = Vec2f::s_zero();

	SPtr<RenderGpuBuffer>	gpuFrustums;
	SPtr<RenderGpuBuffer>	debugFrustumsPts;
	SPtr<RenderGpuBuffer>	debugFrustumsIdxBuf;

public:
	RdgPass* addMakeFrustumsPass(RdgBufferHnd& oBufFrustums);
	RdgPass* addLightCullingPass(Result& oResult, RdgBufferHnd frustums, RdgTextureHnd texDepth);
	RdgPass* addLightCullingPass(Result& oResult, RdgTextureHnd texDepth);
	RdgPass* addLightingPass(RdgTextureHnd colorRt, RdgTextureHnd dsBuf, Result& lightCulling, bool isOpaque, bool isClearColor);
	RdgPass* addRenderDebugFrustumsPass(RdgTextureHnd colorRt);

	RdgPass* addClearBufferPass(SPtr<Material>& material, RdgBufferHnd buffer);

	RdgPass* addFwdpDebugPass();


protected:
	bool isInvalidate(const Vec2f& resoluton_) const;

protected:
	RpfForwardPlus();
	~RpfForwardPlus();

	void create();
	void destroy();

public:
	SPtr<Shader>			_shaderFwdpMakeFrustum;
	SPtr<Material>			_mtlFwdpMakeFrustum;

	SPtr<Shader>			_shaderFwdp;
	SPtr<Material>			_mtlFwdp;

	SPtr<Shader>			_shaderFwdpLighting;
	SPtr<Material>			_mtlFwdpLighting;

	SPtr<Material>			_mtlClearOpaqueLightGrid;
	SPtr<Material>			_mtlClearTransparentLightGrid;

	SPtr<Material>			_mtlClearOpaqueLightIndexCounter;
	SPtr<Material>			_mtlClearOpaqueLightIndexList;
	SPtr<Material>			_mtlClearTransparentLightIndexCounter;
	SPtr<Material>			_mtlClearTransparentLightIndexList;

	SPtr<Shader>			_shaderClearBuffer;

	SPtr<Material>			_mtlFwdpDebug;
	SPtr<Shader>			_shaderFwdpDebug;
	SPtr<Material>			_mtlDebug0;
	SPtr<Material>			_mtlDebug1;

};
#endif

}