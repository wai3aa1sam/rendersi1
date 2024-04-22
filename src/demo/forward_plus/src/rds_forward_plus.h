#pragma once

#include "rds_forward_plus/common/rds_forward_plus_common.h"


namespace rds
{

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

	SPtr<Shader>	_shaderSkybox;
	SPtr<Material>	_mtlSkybox;
	
};
RDS_DEMO(ForwardPlus);

}