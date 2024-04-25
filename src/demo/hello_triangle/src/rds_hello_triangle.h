#pragma once

#include "rds_hello_triangle/common/rds_hello_triangle_common.h"


namespace rds
{

#if 0
#pragma mark --- rdsHelloTriangle-Decl ---
#endif // 0
#if 1
class HelloTriangle : public GraphicsDemo
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
	SPtr<Shader>	_shaderHelloTriangle;
	SPtr<Material>	_mtlHelloTriangle;
	SPtr<Texture2D>	_texUvChecker;
};
RDS_DEMO(HelloTriangle);
#endif

}