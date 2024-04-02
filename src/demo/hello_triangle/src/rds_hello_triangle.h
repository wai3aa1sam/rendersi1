#pragma once

#include "rds_hello_triangle/common/rds_hello_triangle_common.h"


namespace rds
{

class HelloTriangle : public GraphicsDemo
{
public:
	using Base = GraphicsDemo;

public:
	virtual void onCreate()								override;
	virtual void onCreateScene(Scene* oScene)			override;
	virtual void onPrepareRender(RenderGraph* oRdGraph, RenderData& rdData) override;
	virtual void onExecuteRender(RenderGraph* oRdGraph, RenderData& rdData) override;

protected:
	SPtr<Shader>	_shaderHelloTriangle;
	SPtr<Material>	_mtlHelloTriangle;
	SPtr<Texture2D>	_texUvChecker;
};
RDS_DEMO(HelloTriangle);

}