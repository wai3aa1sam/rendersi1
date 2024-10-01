#include "rds_render-pch.h"
#include "rdsRpfGeometryBuffer.h"

namespace rds
{

void 
RpfGeometryBuffer_Result::setupRdgPassForRead(RdgPass& pass, ShaderStageFlag shaderStageFlag)
{
	pass.readTexture(normal,				TextureUsageFlags::ShaderResource, shaderStageFlag);
	pass.readTexture(baseColor,				TextureUsageFlags::ShaderResource, shaderStageFlag);
	pass.readTexture(roughnessMetalness,	TextureUsageFlags::ShaderResource, shaderStageFlag);
	pass.readTexture(emission,				TextureUsageFlags::ShaderResource, shaderStageFlag);
}

void 
RpfGeometryBuffer_Result::setupRdgPassForRead(RdgPass& pass, RdgTextureHnd depth, ShaderStageFlag shaderStageFlag)
{
	pass.readTexture(depth, TextureUsageFlags::ShaderResource, shaderStageFlag);
	setupRdgPassForRead(pass, shaderStageFlag);
}

void 
RpfGeometryBuffer_Result::setupMaterial(Material* mtl)
{
	mtl->setParam("gBuf_normal",				normal.texture2D());
	mtl->setParam("gBuf_baseColor",				baseColor.texture2D());
	mtl->setParam("gBuf_roughnessMetalness",	roughnessMetalness.texture2D());
	mtl->setParam("gBuf_emission",				emission.texture2D());
	mtl->setParam("gBuf_position",				debugPosition.texture2D());

	auto smprPointClamp = SamplerState::makeNearestClampToEdge();
	mtl->setParam("gBuf_normal",				smprPointClamp);
	mtl->setParam("gBuf_baseColor",				smprPointClamp);
	mtl->setParam("gBuf_roughnessMetalness",	smprPointClamp);
	mtl->setParam("gBuf_emission",				smprPointClamp);
	mtl->setParam("gBuf_position",				smprPointClamp);
}

void 
RpfGeometryBuffer_Result::setupMaterial(Material* mtl, RdgTextureHnd depth)
{
	auto smprPointClamp = SamplerState::makeNearestClampToEdge();

	mtl->setParam("tex_depth",					depth.texture2D());
	mtl->setParam("tex_depth",					smprPointClamp);
	setupMaterial(mtl);
}


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
	RenderUtil::createMaterial(&_shaderGeometryBuffer, &_mtlGeometryBuffer, "asset/shader/pass_feature/geometry/rdsGeometryBuffer.shader");
}

void 
RpfGeometryBuffer::destroy()
{
	RenderUtil::destroyShaderMaterial(_shaderGeometryBuffer, _mtlGeometryBuffer);
}

RdgPass* 
RpfGeometryBuffer::addGeometryPass(Result& oResult, const DrawSettings& drawSettings, RdgTextureHnd dsBuf)
{
	auto*		rdGraph		= renderGraph();
	auto*		drawData	= drawDataBase();
	RdgPass*	passGeom	= nullptr;

	auto screenSize	= drawData->resolution2u();
	
	RDS_TODO("pack the normal as RG16s");
	RdgTextureHnd texNormal				= rdGraph->createTexture("gBuf_normal",				Texture2D_CreateDesc{ screenSize, ColorType::RGBAh,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });
	RdgTextureHnd texBaseColor			= rdGraph->createTexture("gBuf_baseColor",			Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });
	RdgTextureHnd texRoughnessMetalness = rdGraph->createTexture("gBuf_roughnessMetalness",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });
	RdgTextureHnd texEmission			= rdGraph->createTexture("gBuf_emission",			Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });
	RdgTextureHnd texDebugPosition		= rdGraph->createTexture("gBuf_debugPosition",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAf,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });

	//Material* mtl = _mtlGeometryBuffer;
	{
		auto& pass = rdGraph->addPass("geometry_pass", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(texNormal,					RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setRenderTarget(texBaseColor,				RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setRenderTarget(texRoughnessMetalness,		RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setRenderTarget(texEmission,				RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setRenderTarget(texDebugPosition,			RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf, RdgAccess::Write,	RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);
		pass.setExecuteFunc(
			[=, drawSettings = drawSettings](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();
				
				constCast(drawSettings).overrideShader = _shaderGeometryBuffer;
				drawData->drawScene(rdReq, drawSettings);
			}
		);
		passGeom = &pass;
	}

	oResult.normal				= texNormal;
	oResult.baseColor			= texBaseColor;
	oResult.roughnessMetalness	= texRoughnessMetalness;
	oResult.emission			= texEmission;
	oResult.debugPosition		= texDebugPosition;

	return passGeom;
}

RdgPass* 
RpfGeometryBuffer::addGeometryPass(const DrawSettings& drawSettings, RdgTextureHnd dsBuf)
{
	RdgPass*	passGeom	= nullptr;
	passGeom = addGeometryPass(result, drawSettings, dsBuf);
	return passGeom;
}

#endif

}