#include "rds_render-pch.h"
#include "rdsRpfGeometryBuffer.h"

namespace rds
{

#if 0
#pragma mark --- rdsRpfGeometryBuffer-Impl ---
#endif // 0
#if 1

RpfGeometryBuffer::RpfGeometryBuffer()
{

}

RpfGeometryBuffer::~RpfGeometryBuffer()
{
	destroy();
}

void 
RpfGeometryBuffer::create()
{
	createMaterial(&_shaderGeometryBuffer, &_mtlGeometryBuffer, "asset/shader/pass_feature/geometry/rdsGeometryBuffer.shader");
}

void 
RpfGeometryBuffer::destroy()
{
	_mtlGeometryBuffer.reset(nullptr);
	_shaderGeometryBuffer.reset(nullptr);
}

RdgPass* 
RpfGeometryBuffer::addGeometryPass(const DrawSettings& drawSettings, RdgTextureHnd dsBuf)
{
	auto*		rdGraph		= renderGraph();
	auto*		drawData	= drawDataBase();
	RdgPass*	passGeom	= nullptr;

	auto screenSize	= drawData->resolution2u();

	RdgTextureHnd texNormal		= rdGraph->createTexture("gBuf_normal",			Texture2D_CreateDesc{ screenSize, ColorType::RGh,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });
	RdgTextureHnd texAlbedo		= rdGraph->createTexture("gBuf_albedo",			Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });
	result.debugPosition		= rdGraph->createTexture("gBuf_debugPosition",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAf,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });

	Material* mtl = _mtlGeometryBuffer;
	{
		auto& pass = rdGraph->addPass("geometry_pass", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(texNormal,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setRenderTarget(texAlbedo,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setRenderTarget(result.debugPosition,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf, RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				drawData->drawScene(rdReq, mtl, drawSettings);
			}
		);
		passGeom = &pass;
	}

	result.normal	= texNormal;
	result.albedo	= texAlbedo;

	return passGeom;
}


#endif

}