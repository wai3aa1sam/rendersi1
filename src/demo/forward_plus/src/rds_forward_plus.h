#pragma once

#include "rds_forward_plus/common/rds_forward_plus_common.h"


namespace rds
{

class RpfPreDepth;
class RpfForwardPlusRendering;

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
	RpfPreDepth*				_rpfPreDepth		= nullptr;
	RpfForwardPlusRendering*	_rpfFwdpRendering	= nullptr;

	bool isShowHeatmap = false;

};
RDS_DEMO(ForwardPlus);
#endif


}