#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

/*
* view screen viewport as a grid, blocks in a grid, and each block has n tile
*/

struct RpfForwardPlusRendering_Result
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

#if 0
#pragma mark --- rdsRpfForwardPlusRendering-Decl ---
#endif // 0
#if 1

class RpfForwardPlusRendering : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfForwardPlusRendering);
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

	RdgPass* addFwdpDebugBufferPass();

protected:
	bool isInvalidate(const Vec2f& resoluton_) const;

protected:
	RpfForwardPlusRendering();
	~RpfForwardPlusRendering();

	void create();
	void destroy();

private:
	SPtr<Shader>			_shaderMakeFrustums;
	SPtr<Material>			_mtlMakeFrustums;

	SPtr<Shader>			_shaderLightsCulling;
	SPtr<Material>			_mtlLightsCulling;

	SPtr<Shader>			_shaderLighting;
	SPtr<Material>			_mtlLighting;

	SPtr<Shader>			_shaderClearBuffer;
	SPtr<Material>			_mtlClearOpaqueLightGrid;
	SPtr<Material>			_mtlClearOpaqueLightIndexCounter;
	SPtr<Material>			_mtlClearOpaqueLightIndexList;
	SPtr<Material>			_mtlClearTransparentLightGrid;
	SPtr<Material>			_mtlClearTransparentLightIndexCounter;
	SPtr<Material>			_mtlClearTransparentLightIndexList;

	SPtr<Shader>			_shaderDebugBuffer;
	SPtr<Material>			_mtlDebugBuffer;
	SPtr<Material>			_mtlClearDebugBuffer0;
	SPtr<Material>			_mtlClearDebugBuffer1;
};
#endif

}