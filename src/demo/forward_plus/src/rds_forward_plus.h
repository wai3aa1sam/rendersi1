#pragma once

#include "rds_forward_plus/common/rds_forward_plus_common.h"


namespace rds
{

struct ForwardPlus_MakeFrustums;

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
	virtual void onPrepareRender(RenderGraph* oRdGraph, DrawData* drawData) override;
	virtual void onExecuteRender(RenderGraph* oRdGraph, DrawData* drawData) override;

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) override;

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) override;

protected:
	SPtr<Shader>	_shaderForwardPlus;
	SPtr<Material>	_mtlForwardPlus;
	SPtr<Texture2D>	_texUvChecker;

	SPtr<Shader>	_shaderHelloTriangle;
	SPtr<Material>	_mtlHelloTriangle;

	SPtr<Texture2D> _tex;

	UPtr<ForwardPlus_MakeFrustums> _fwpMakeFrustums;
	
};
RDS_DEMO(ForwardPlus);
#endif

#if 0
#pragma mark --- rdsForwardPlus-Decl ---
#endif // 0
#if 1
struct ForwardPlus_MakeFrustums
{
public:
	using DebugIndexType = u32;

public:
	static constexpr u32			s_kTileCount = 16;
	static constexpr DebugIndexType s_debugFrustumIndices[] = {	0,1,
																0,2,
																3,1,
																3,2,
	};

public:
	SPtr<Shader>			shaderFwpMakeFrustum;
	SPtr<Material>			mtlFwpMakeFrustum;
	Vec2f					resolution = Vec2f::s_zero();
	SPtr<RenderGpuBuffer>	frustums;

	SPtr<RenderGpuBuffer>	debugFrustumsPts;
	SPtr<RenderGpuBuffer>	debugFrustumsIdxBuf;

public:
	ForwardPlus_MakeFrustums();
	~ForwardPlus_MakeFrustums();

	void create();
	void destroy();

	RDS_NODISCARD RdgBufferHnd onExecuteRender(RenderGraph* oRdGraph, DrawData* drawData);
	void renderDebugMakeFrustums(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd texColor);

public:
	bool isInvalidate(const Vec2f& resoluton_) const;

public:

};
#endif

}