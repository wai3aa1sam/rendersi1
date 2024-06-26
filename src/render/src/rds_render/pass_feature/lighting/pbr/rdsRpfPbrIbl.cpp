#include "rds_render-pch.h"
#include "rdsRpfPbrIbl.h"

namespace rds
{

#if 0
#pragma mark --- rdsRpfPbrIbl-Impl ---
#endif // 0
#if 1

RpfPbrIbl::RpfPbrIbl()
{

}

RpfPbrIbl::~RpfPbrIbl()
{
	destroy();
}

void 
RpfPbrIbl::create()
{
	RenderUtil::createShader(&_shaderHdrToCube,				"asset/shader/pass_feature/lighting/pbr/rdsPbr_HdrToCube.shader");
	RenderUtil::createShader(&_shaderIrradianceEnvCube,		"asset/shader/pass_feature/lighting/pbr/rdsPbr_IrradianceEnvCube.shader");
	RenderUtil::createShader(&_shaderPrefilteredEnvCube,	"asset/shader/pass_feature/lighting/pbr/rdsPbr_PrefilteredEnvCube.shader");

	RenderUtil::createMaterial(&_shaderBrdfLut, &_mtlBrdfLut, "asset/shader/pass_feature/lighting/pbr/rdsPbr_BrdfLut.shader");
}

void 
RpfPbrIbl::destroy()
{
	RenderUtil::destroyMaterials(_mtlsHdrToCube);
	RenderUtil::destroyMaterials(_mtlsIrradianceEnvCube);
	RenderUtil::destroyMaterials(_mtlsPrefilteredEnvCube);


	RenderUtil::destroyShader(_shaderHdrToCube);
	RenderUtil::destroyShader(_shaderIrradianceEnvCube);
	RenderUtil::destroyShader(_shaderPrefilteredEnvCube);
	RenderUtil::destroyShader(_shaderBrdfLut);
}

RdgPass* 
RpfPbrIbl::addPreparePass(Result* oResult, Texture2D* texHdrEnvMap, const RenderMesh& box, u32 cubeSize, u32 irradianceCubeSize)
{
	//auto*		rdGraph		= renderGraph();
	//auto*		drawData	= drawDataBase();
	RdgPass*	pass		= nullptr;

	auto* result = oResult;

	bool isFirst = createTexture(*result, texHdrEnvMap->debugName(), cubeSize, irradianceCubeSize);

	#if 1
	RdgPass* passHdrToCube			= addRenderToCubePass("hdrToCube",				result->cubeEnvMap, _shaderHdrToCube, box, nullptr
		, _mtlsHdrToCube
		, [=](Material* mtl, u32 mip, u32 face)
		{
			mtl->setParam("tex_equirectangular", texHdrEnvMap);
		}
	); RDS_UNUSED(passHdrToCube);
	#if 1
	/*
	* this pass will trigger crash
	*/
	RdgPass* passIrradianceEnvCube = addRenderToCubePass("irradianceEnvCube",		result->cubeIrradinceMap, _shaderIrradianceEnvCube, box, passHdrToCube
		, _mtlsIrradianceEnvCube
		, [=](Material* mtl, u32 mip, u32 face)
		{
			mtl->setParam("delta_phi",		0.025f);
			mtl->setParam("delta_theta",	0.025f);
			mtl->setParam("cube_env",		result->cubeEnvMap);
		}
	); RDS_UNUSED(passIrradianceEnvCube);
	#endif // 0
	#if 1
	RdgPass* passPrefilteredEnvCube = addRenderToCubePass("PrefilteredEnvCube",		result->cubePrefilteredMap, _shaderPrefilteredEnvCube, box, passIrradianceEnvCube
		, _mtlsPrefilteredEnvCube
		, [=](Material* mtl, u32 mip, u32 face)
		{
			mtl->setParam("roughness",	sCast<float>(mip) / result->cubePrefilteredMap->mipCount());
			mtl->setParam("cube_env", result->cubeEnvMap);
		}
	); RDS_UNUSED(passPrefilteredEnvCube);
	pass = passPrefilteredEnvCube;
	#endif // 0

	#endif // 0

	if (isFirst)
	{
		pass = addBrdfLutPass(result->brdfLut, passPrefilteredEnvCube);
	}

	return pass;
}

RdgPass* 
RpfPbrIbl::addLightingPass(const DrawSettings& drawSettings, RdgTextureHnd rtColor, RdgTextureHnd dsBuf)
{
	auto*		rdGraph			= renderGraph();
	auto*		drawData		= drawDataBase();
	RdgPass*	passLighting	= nullptr;

	auto screenSize	= drawData->resolution2u();

	Material* mtl = nullptr;
	{
		auto& pass = rdGraph->addPass("pbr_lighting", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
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
		passLighting = &pass;
	}

	return passLighting;
}

RdgPass* 
RpfPbrIbl::addBrdfLutPass(Texture2D* outBrdfLut, RdgPass* dependency)
{
	auto*		rdGraph		= renderGraph();
	auto*		drawData	= drawDataBase();
	RdgPass*	passBrdfLut	= nullptr;

	auto texDst		= rdGraph->importTexture("tex_brdf_lut", outBrdfLut);
	auto viewport	= Rect2f{ Tuple2f::s_zero(), Tuple2f::s_cast(outBrdfLut->size())};

	Material* mtl = _mtlBrdfLut;
	{
		auto& pass = rdGraph->addPass("pbr_brdf_lut", RdgPassTypeFlags::Graphics);
		pass.runAfter(dependency);
		pass.setRenderTarget(texDst,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq) 
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				rdReq.setViewport(viewport);
				rdReq.setScissorRect(viewport);

				drawData->setupMaterial(mtl);
				auto drawCall = rdReq.addDrawCall();
				drawCall->setDebugSrcLoc(RDS_SRCLOC);
				drawCall->vertexCount = 3;
				drawCall->setMaterial(mtl);
			});

		auto& passEnvMapTransit = rdGraph->addPass("brdfLut_transit", RdgPassTypeFlags::Graphics);
		passEnvMapTransit.readTexture(texDst);
		passEnvMapTransit.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
			}
		);

		passBrdfLut = &passEnvMapTransit;
	}
	return passBrdfLut;
}


RdgPass* 
RpfPbrIbl::addRenderToCubePass(StrView name, TextureCube* outCube, Shader* shader, const RenderMesh& box, RdgPass* dependency, CubeMaterial& mtls, Function<void(Material*, u32, u32)> setMtlFunc)
{
	auto*	rdGraph		= renderGraph();
	auto*	drawData	= drawDataBase();

	RdgTextureHnd	texDst;
	RdgPass*		lastPass = nullptr;

	ColorType	outFormat	= outCube->format();
	auto		cubeSize	= sCast<float>(outCube->size());
	auto		center		= Vec3f{ 0.0f, 0.0f, 0.0f};
	auto		mipCount	= Texture_mipCount(sCast<u32>(cubeSize));

	if (mtls.size() < TextureCube::s_kFaceCount * mipCount)
	{
		auto prevSize = mtls.size();
		mtls.resize(TextureCube::s_kFaceCount * mipCount);
		for (size_t i = prevSize; i < TextureCube::s_kFaceCount * mipCount; i++)
		{
			auto& mtl = mtls[i];
			RenderUtil::createMaterial(shader,	&mtl);
		}
	}

	{
		TempString renderToCubeName;
		fmtToNew(renderToCubeName, "{}_tex_temp_rdToCube", name);

		RdgPass* lastCopyPass = nullptr;
		RdgTextureHnd texTempToCubeColor = rdGraph->createTexture(renderToCubeName
			, Texture2D_CreateDesc{ sCast<u32>(cubeSize), sCast<u32>(cubeSize), outFormat, TextureUsageFlags::RenderTarget | TextureUsageFlags::TransferSrc});

		for (u32 mip = 0; mip < mipCount; mip++)
		{
			auto viewportFactor = math::pow(0.5f, sCast<float>(mip));
			auto viewport = Rect2f{ Tuple2f::s_zero(), Tuple2f{cubeSize * viewportFactor, cubeSize * viewportFactor} };

			for (u32 face = 0; face < TextureCube::s_kFaceCount; face++)
			{
				auto* mtl = mtls[mip * TextureCube::s_kFaceCount + face].ptr(); RDS_UNUSED(mtl);

				setMtlFunc(mtl, mip, face);

				fmtToNew(renderToCubeName, "{}-mip{}-face{}", name, mip, face);
				auto& passRdToCube = rdGraph->addPass(renderToCubeName, RdgPassTypeFlags::Graphics);
				passRdToCube.runAfter(lastCopyPass);
				passRdToCube.setRenderTarget(texTempToCubeColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
				if (face == 0 && mip == 0)
				{
					passRdToCube.runAfter(dependency);
				}
				passRdToCube.setExecuteFunc(
					[=](RenderRequest& rdReq) 
					{
						rdReq.reset(rdGraph->renderContext(), drawData);

						Vector<Mat4f, TextureCube::s_kFaceCount> matViews = 
						{
							Mat4f::s_lookAt(center, Vec3f::s_right(),	Vec3f{0.0f, -1.0f,   0.0f}),
							Mat4f::s_lookAt(center, Vec3f::s_left(),	Vec3f{0.0f, -1.0f,   0.0f}),
							Mat4f::s_lookAt(center, Vec3f::s_down(),	Vec3f{0.0f,  0.0f,  -1.0f}),
							Mat4f::s_lookAt(center, Vec3f::s_up(),		Vec3f{0.0f,  0.0f,   1.0f}),
							Mat4f::s_lookAt(center, Vec3f::s_forward(), Vec3f{0.0f, -1.0f,   0.0f}),
							Mat4f::s_lookAt(center, Vec3f::s_back(),	Vec3f{0.0f, -1.0f,   0.0f}),
						};

						auto	matProj		= Mat4f::s_perspective(math::radians(90.0f), 1.0, 0.1f, viewport.size.x);
						auto&	matView		= matViews[face];
						auto	matrixVp	= matProj * matView;

						rdReq.setViewport(viewport);
						rdReq.setScissorRect(viewport);

						drawData->setupMaterial(mtl);
						RDS_TODO("use a buffer to store matrix_vp and PerObjectParam to get the index");
						mtl->setParam("matrix_vp", matrixVp);
						rdReq.drawMesh(RDS_SRCLOC, box, mtl);
					});

				fmtToNew(renderToCubeName, "{}_tex-mip{}-face{}", name, mip, face);
				texDst = rdGraph->importTexture(renderToCubeName, outCube);

				fmtToNew(renderToCubeName, "{}_copyToDst-mip{}-face{}", name, mip, face);
				auto& passCopyPbrEnvToCube = rdGraph->addPass(renderToCubeName, RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Transfer);
				passCopyPbrEnvToCube.readTexture(texTempToCubeColor);
				passCopyPbrEnvToCube.writeTexture(texDst);
				passCopyPbrEnvToCube.setExecuteFunc(
					[=](RenderRequest& rdReq)
					{
						rdReq.copyTexture(RDS_SRCLOC, outCube, texTempToCubeColor.renderResource(), sCast<u32>(viewport.w), sCast<u32>(viewport.h), 0, face, 0, mip);
					}
				);

				lastCopyPass = &passCopyPbrEnvToCube;
			}
		}

		#if 1
		fmtToNew(renderToCubeName, "{}_transit", name);
		auto& passEnvMapTransit = rdGraph->addPass(renderToCubeName, RdgPassTypeFlags::Graphics);
		passEnvMapTransit.readTexture(texDst);
		passEnvMapTransit.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
			}
		);
		#else
		rdGraph->exportTexture(texDstIrradiance, TextureUsageFlags::ShaderResource);		// this is not work, idk why, but export only transit in the end
		#endif // 0

		lastPass = &passEnvMapTransit;
	}

	return lastPass;
}

bool 
RpfPbrIbl::createTexture(Result& result_, StrView name, u32 cubeSize, u32 irradianceCubeSize)
{
	auto irrCubeSize	= irradianceCubeSize;
	auto cubeFormat		= ColorType::RGBAh;

	SamplerState samplerState;
	samplerState.wrapU = SamplerWrap::ClampToEdge;
	samplerState.wrapV = SamplerWrap::ClampToEdge;
	samplerState.wrapS = SamplerWrap::ClampToEdge;

	auto texCubeCDesc = TextureCube::makeCDesc(RDS_SRCLOC);

	TempString cubeName;

	auto& cubeEnvMap = result_.cubeEnvMap;
	fmtToNew(cubeName, "{}-cubeHdrEnvMap",			name);
	texCubeCDesc.create(cubeSize, cubeFormat, true, TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst, samplerState);
	cubeEnvMap = renderDevice()->createTextureCube(texCubeCDesc);
	cubeEnvMap->setDebugName(cubeName);

	auto& cubeIrradinceMap = result_.cubeIrradinceMap;
	fmtToNew(cubeName, "{}-cubeIrradianceEnvMap",	name);
	texCubeCDesc.create(irrCubeSize, cubeFormat, true, TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst, samplerState);
	cubeIrradinceMap = renderDevice()->createTextureCube(texCubeCDesc);
	cubeIrradinceMap->setDebugName(cubeName);

	auto& cubePrefilteredMap = result_.cubePrefilteredMap;
	fmtToNew(cubeName, "{}-cubePrefilteredMap",	name);
	texCubeCDesc.create(cubeSize, cubeFormat, true, TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferDst, samplerState);
	cubePrefilteredMap = renderDevice()->createTextureCube(texCubeCDesc);
	cubePrefilteredMap->setDebugName(cubeName);

	bool isFirst = !result_.brdfLut;
	if (isFirst)
	{
		auto texCDesc = Texture2D::makeCDesc(RDS_SRCLOC);
		texCDesc = Texture2D_CreateDesc{ Tuple2u{ cubeSize, cubeSize }, ColorType::RGh, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource, samplerState };

		auto& brdfLut = result_.brdfLut;
		brdfLut = renderDevice()->createTexture2D(texCDesc);
		brdfLut->setDebugName("brdfLut");
	}
	return isFirst;
}


#endif

}