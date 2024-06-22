#pragma once

#include "rds_pbr_ibl/common/rds_pbr_ibl_common.h"


namespace rds
{

class RpfPbrIbl;

#if 0
#pragma mark --- rdsPbrIbl-Decl ---
#endif // 0
#if 1
class PbrIbl : public GraphicsDemo
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
	RpfPbrIbl* _rpfPbrIbl = nullptr;
};
RDS_DEMO(PbrIbl);
#endif

}