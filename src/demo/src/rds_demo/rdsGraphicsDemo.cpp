#include "rds_demo-pch.h"
#include "rdsGraphicsDemo.h"
#include "editor/rdsDemoEditorLayer.h"

namespace rds
{

#if 0
#pragma mark --- rdsGraphicsDemo-Impl ---
#endif // 0
#if 1

GraphicsDemo::~GraphicsDemo()
{
}

void 
GraphicsDemo::prepareRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	auto& rdGraph	= oRdGraph;
	auto* rdCtx		= rdGraph->renderContext();
	RDS_CORE_ASSERT(math::equals(rdCtx->framebufferSize(), Vec2f{drawData->resolution()}), "RenderContext resoultion is different from DrawData resoultion");

	auto& rdPassPipeline = *_rdPassPipelines[drawData->drawParamIdx];
	rdPassPipeline.reset(oRdGraph, drawData);
	onPrepareRender(&rdPassPipeline);
}

void 
GraphicsDemo::executeRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	auto& rdGraph	= oRdGraph;
	auto* rdCtx		= rdGraph->renderContext();
	RDS_CORE_ASSERT(math::equals(rdCtx->framebufferSize(), Vec2f{drawData->resolution()}), "RenderContext resoultion is different from DrawData resoultion");

	auto& rdPassPipeline = *_rdPassPipelines[drawData->drawParamIdx];
	rdPassPipeline.reset(oRdGraph, drawData);
	onExecuteRender(&rdPassPipeline);
}

void 
GraphicsDemo::onCreate()
{
	_rdPassPipelines.emplace_back(makeUPtr<RenderPassPipeline>());
	{
		auto texCDesc = Texture2D::makeCDesc(RDS_SRCLOC);

		texCDesc.create("asset/texture/uvChecker.png");
		_texUvChecker = Renderer::rdDev()->createTexture2D(texCDesc);
		_texUvChecker->setDebugName("uvChecker");
	}

	{
		auto texCDesc = TextureCube::makeCDesc(RDS_SRCLOC);
		Vector<StrView, TextureCube::s_kFaceCount> filenames;
		filenames.emplace_back("asset/texture/skybox/default/right.png");
		filenames.emplace_back("asset/texture/skybox/default/left.png");
		filenames.emplace_back("asset/texture/skybox/default/top.png");
		filenames.emplace_back("asset/texture/skybox/default/bottom.png");
		filenames.emplace_back("asset/texture/skybox/default/front.png");
		filenames.emplace_back("asset/texture/skybox/default/back.png");

		texCDesc.create(filenames);
		_texDefaultSkybox = Renderer::rdDev()->createTextureCube(texCDesc);
		_texDefaultSkybox->setDebugName("skybox_default");
	}

	{
		auto texCDesc = Texture2D::makeCDesc();
		texCDesc.create("asset/texture/hdr/newport_loft.hdr");
		_texDefaultHdrEnv = Renderer::rdDev()->createTexture2D(texCDesc);
		_texDefaultHdrEnv->setDebugName("texHdrEnvMap");
	}

	createMaterial(&_shaderSkybox, &_mtlSkybox, "asset/shader/skybox.shader"
		, [&](Material* mtl) {mtl->setParam("skybox", skyboxDefault()); });

	RenderUtil::createMaterial(&_shaderScreenQuad,		&_mtlScreenQuad,		"asset/shader/util/rdsScreenQuad.shader");
	RenderUtil::createMaterial(&_shaderDisplayNormals,	&_mtlDisplayNormals,	"asset/shader/util/rdsDisplayNormals.shader");
}

void 
GraphicsDemo::onCreateScene(Scene* oScene)
{
	_scene = oScene;
}

void 
GraphicsDemo::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{

}

void 
GraphicsDemo::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	
}

void 
GraphicsDemo::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{

}

void 
GraphicsDemo::onUiMouseEvent(UiMouseEvent& ev)
{

}

void 
GraphicsDemo::onUiKeyboardEvent(UiKeyboardEvent& ev)
{

}

void 
GraphicsDemo::createDefaultScene(Scene* oScene, Shader* shader, MeshAsset* meshAsset, Vec3u size, Vec3f startPos, Vec3f step, bool isCreateLights)
{
	auto& scene = *oScene;

	//auto n = 25;
	auto row	= size.x;
	auto col	= size.y;
	auto depth	= size.z;

	#if 1
	if (isCreateLights)
	{
		row = 5;
		col = 5;

		for (size_t r = 0; r < row; r++)
		{
			for (size_t c = 0; c < col; c++)
			{
				auto* ent = scene.addEntity();

				auto* transform	= ent->getComponent<CTransform>();
				transform->setLocalPosition(startPos.x + step.x * c, 1.0f, startPos.y + step.y * r);
				transform->setLocalRotation(Vec3f{math::radians(90.0f), 0.0f, 0.0f});

				//auto* rdableMesh = ent->addComponent<CRenderableMesh>();
				//rdableMesh->material = mtl;
				//rdableMesh->meshAsset = isDirectional ? meshAssets().box : isPoint ? meshAssets().sphere : meshAssets().cone;

				#if 1
				bool isPoint		= r % 2 == 0;
				bool isDirectional	= r == 0 && c == 0;

				auto* light = ent->addComponent<CLight>();
				light->setType(isPoint ? LightType::Point : LightType::Spot);
				//light->setType(LightType::Point);
				if (isDirectional)
				{
					light->setType(LightType::Directional);
				}

				if (light->lightType() == LightType::Spot)
				{
					light->setSpotAngle(math::radians(60.0f));
					light->setSpotInnerAngle(math::radians(30.0f));
				}

				light->setRange(4);

				TempString buf;
				fmtTo(buf, "{}-{}", enumStr(light->lightType()), sCast<u64>(ent->id()));
				ent->setName(buf);
				#endif // 0
			}
		}
	}
	#endif // 1

	for (u32 d = 0; d < depth; d++)
	{
		for (u32 c = 0; c < col; c++)
		{
			for (u32 r = 0; r < row; r++)
			{
				auto* ent = scene.addEntityWithDefaultName();

				auto* rdableMesh = ent->addComponent<CRenderableMesh>();
				rdableMesh->material = Renderer::rdDev()->createMaterial(shader);
				rdableMesh->meshAsset = meshAsset;

				auto* transform	= ent->getComponent<CTransform>();
				transform->setLocalPosition(startPos + step * Vec3f::s_cast(Vec3u{r, c, d}));
			}
		}
	}
}

void 
GraphicsDemo::createMaterial(SPtr<Shader>* oShader, SPtr<Material>* oMtl, StrView filename, const Function<void(Material*)>& fnSetParam)
{
	auto& shader	= *oShader;
	auto& mtl		= *oMtl;

	shader	= Renderer::rdDev()->createShader(filename);
	mtl		= Renderer::rdDev()->createMaterial();
	mtl->setShader(shader);

	if (fnSetParam)
		fnSetParam(mtl);
}

RdgPass*
GraphicsDemo::addSkyboxPass(RenderGraph* oRdGraph, DrawData* drawData, TextureCube* texSkybox, RdgTextureHnd texColor, RdgTextureHnd texDepth)
{
	if (!texColor)
		return nullptr;

	auto*	rdGraph		= oRdGraph;
	
	auto& skyboxPass = rdGraph->addPass("skybox", RdgPassTypeFlags::Graphics);
	skyboxPass.setRenderTarget(texColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
	if (texDepth)
		skyboxPass.setDepthStencil(texDepth, RenderAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
	skyboxPass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			rdReq.reset(rdGraph->renderContext(), drawData);
			auto mtl = _mtlSkybox;

			mtl->setParam("skybox", texSkybox);
			drawData->setupMaterial(mtl);
			rdReq.drawMesh(RDS_SRCLOC, meshAssets().box->renderMesh, mtl);
		});
	return &skyboxPass;
}

RdgPass* 
GraphicsDemo::addPreDepthPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd* oDsBufHnd, RdgTextureHnd* oTexDepthHnd, Color4f clearColor)
{
	auto* rdGraph		= oRdGraph;
	auto& dsBuf			= *oDsBufHnd;
	auto& texDepth		= *oTexDepthHnd;
	auto& passPreDepth	= rdGraph->addPass("pre_depth", RdgPassTypeFlags::Graphics);

	{
		auto& pass = passPreDepth;
		if (oTexDepthHnd)
			pass.setRenderTarget(texDepth, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf, RenderAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);
				auto mtl = _mtlPreDepth;

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(clearColor);
				clearValue->setClearDepth();

				drawData->setupMaterial(mtl);
				drawData->drawScene(rdReq, mtl);
			});
	}

	/*if (oTexDepthHnd)
	{
		auto& pass = rdGraph->addPass(fmtAs_T<TempString>("{}_transit", passPreDepth.name()), RdgPassTypeFlags::Graphics);
		pass.readTexture(texDepth);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
			}
		);
	}*/

	return &passPreDepth;
}

RdgPass* 
GraphicsDemo::addPostProcessPass(RenderGraph* oRdGraph, DrawData* drawData, StrView passName, RdgTextureHnd rtColor, RdgTextureHnd texColor, Material* material)
{
	if (!texColor)
		return nullptr;

	auto*	rdGraph		= oRdGraph;

	auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Graphics);
	pass.readTexture(texColor);
	pass.setRenderTarget(rtColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			rdReq.reset(rdGraph->renderContext(), drawData);
			auto mtl = material;

			mtl->setParam("texColor", texColor.renderResource());
			drawData->setupMaterial(mtl);
			rdReq.drawMesh(RDS_SRCLOC, meshAssets().fullScreenTriangle->renderMesh, mtl);
		});
	return &pass;
}

RdgPass* 
GraphicsDemo::addDrawLightOutlinePass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, Material* material)
{
	auto*	rdGraph		= oRdGraph;

	RdgPass* passLightOutline = nullptr;

	auto& pass = rdGraph->addPass("draw_lightOutline", RdgPassTypeFlags::Graphics);
	passLightOutline = &pass;

	pass.setRenderTarget(rtColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			rdReq.reset(rdGraph->renderContext(), drawData, drawData->mtlLine());
			auto mtl = material ? material :drawData->mtlLine();	RDS_UNUSED(mtl);
			mtl->setParam("rds_matrix_vp", drawData->camera->viewProjMatrix());

			{
				auto viewMatrix = drawData->camera->viewMatrix();
				for (auto& light : engineContext().lightSystem().components())
				{
					auto ent = scene().findEntity(light->id());
					auto& transform = ent->transform();

					auto posWs		= transform.localPosition();
					auto dirWs		= transform.forward().normalize();

					RenderRequest::LineVtxType v0;
					v0.position		= posWs;
					v0.colors[0]	= Color4b{255, 0, 0, 255};

					RenderRequest::LineVtxType v1;
					v1.position		= posWs + dirWs * (light->lightType() == LightType::Directional ? 10 : light->range());
					v1.colors[0]	= Color4b{0, 255, 0, 255};

					rdReq.drawLine(v0, v1);
				}
			}

			{
				RenderRequest::LineVtxType v0;
				v0.position		= Tuple3f{0.0, 0.0, 0.0};
				v0.colors[0]	= Color4b{255, 0, 0, 255};

				RenderRequest::LineVtxType v1;
				v1.position		= Tuple3f{10.0, 0.0, 0.0};
				v1.colors[0]	= Color4b{255, 0, 0, 255};

				rdReq.drawLine(v0, v1);
}

			{
				RenderRequest::LineVtxType v0;
				v0.position		= Tuple3f{0.0, 0.0, 0.0};
				v0.colors[0]	= Color4b{255, 0, 0, 255};

				RenderRequest::LineVtxType v1;
				v1.position		= Tuple3f{0.0, 10.0, 0.0};
				v1.colors[0]	= Color4b{255, 0, 0, 255};

				rdReq.drawLine(v0, v1);
			}
			{
				RenderRequest::LineVtxType v0;
				v0.position		= Tuple3f{0.0, 0.0, 0.0};
				v0.colors[0]	= Color4b{255, 0, 0, 255};

				RenderRequest::LineVtxType v1;
				v1.position		= Tuple3f{0.0, 0.0, 10.0};
				v1.colors[0]	= Color4b{255, 0, 0, 255};

				rdReq.drawLine(v0, v1);
			}
		});

	return passLightOutline;
}

RdgPass* 
GraphicsDemo::addDisplayNormalPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor)
{
	auto*	rdGraph		= oRdGraph;

	auto& pass = rdGraph->addPass("displayNormals", RdgPassTypeFlags::Graphics);
	pass.setRenderTarget(rtColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			rdReq.reset(rdGraph->renderContext(), drawData);
			auto mtl = _mtlDisplayNormals;

			drawData->setupMaterial(mtl);
			drawData->drawScene(rdReq, mtl);
		});
	return &pass;
}

RdgPass* 
GraphicsDemo::addDisplayAABBoxPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, const DrawSettings& drawSettings)
{
	auto*	rdGraph		= oRdGraph;

	RdgPass* passDisplayAABBox = nullptr;
	{
		auto& pass = rdGraph->addPass("displayAABBox", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(rtColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData, drawData->mtlLine());

				#if 0
				if (drawSettings.cullingSetting.mode == CullingMode::CameraFustrum)
				{
					const auto& camera = drawData->camera;

					//auto fov = drawData->camera->fov();
					//auto camera = drawData->camera;
					//camera.setFov(fov / 2.0f);
					//rdReq.drawFrustum(camera.frustum(), Color4f{1.0f, 0.5f, 0.0f, 1.0f});
					//rdReq.drawFrustum(drawSettings.cullingSetting.cameraFustrum, Color4f{1.0f, 0.0f, 1.0f, 1.0f});
				}
				#endif // 0

				bool isDrawStaticDebugCamera = false;
				if (isDrawStaticDebugCamera)
				{
					static math::Camera3f camera;
					static bool isSet = false;
					if (!isSet)
					{
						camera = *drawData->camera;
						isSet = true;
					}
					rdReq.drawFrustum(camera.frustum(), Color4f{1.0f, 0.0f, 1.0f, 1.0f});
				}

				drawData->drawSceneAABBox(rdReq, drawSettings);
			});
		passDisplayAABBox = &pass;
	}
	return passDisplayAABBox;
}

#if 0
void 
addDrawLineTest()
{
	#if 0
	{
		auto& passDrawLine = rdGraph->addPass("draw_line", RdgPassTypeFlags::Graphics);
		passDrawLine.setRenderTarget(texColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		passDrawLine.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);
				auto mtl = drawData->mtlLine();	RDS_UNUSED(mtl);
				mtl->setParam("rds_matrix_vp", drawData->camera->viewProjMatrix());
				{
					RenderRequest::LineVtxType v0;
					v0.position		= Tuple3f{0.0, 0.0, 0.0};
					v0.colors[0]	= Color4b{255, 0, 0, 255};

					RenderRequest::LineVtxType v1;
					v1.position		= Tuple3f{10.0, 0.0, 0.0};
					v1.colors[0]	= Color4b{255, 0, 0, 255};

					rdReq.drawLine(v0, v1, mtl);
				}

				{
					RenderRequest::LineVtxType v0;
					v0.position		= Tuple3f{0.0, 0.0, 0.0};
					v0.colors[0]	= Color4b{255, 0, 0, 255};

					RenderRequest::LineVtxType v1;
					v1.position		= Tuple3f{0.0, 10.0, 0.0};
					v1.colors[0]	= Color4b{255, 0, 0, 255};

					rdReq.drawLine(v0, v1, mtl);
				}
				{
					RenderRequest::LineVtxType v0;
					v0.position		= Tuple3f{0.0, 0.0, 0.0};
					v0.colors[0]	= Color4b{255, 0, 0, 255};

					RenderRequest::LineVtxType v1;
					v1.position		= Tuple3f{0.0, 0.0, 10.0};
					v1.colors[0]	= Color4b{255, 0, 0, 255};

					rdReq.drawLine(v0, v1, mtl);
				}
			});
	}
	#endif // 0
}
#endif // 0


DemoEditorApp&	GraphicsDemo::app()				{ return _demoLayer->app(); }
EngineContext&	GraphicsDemo::engineContext()	{ return _demoLayer->engineContext(); }
MeshAssets&		GraphicsDemo::meshAssets()		{ return _demoLayer->meshAssets(); }

#endif

#if 0
#pragma mark --- rdsMeshAssets-Impl ---
#endif // 0
#if 1

MeshAssets::MeshAssets()
{
	create();
}

MeshAssets::~MeshAssets()
{
	destroy();
}

void 
MeshAssets::create()
{
	#if 0
	if (true)
	{
		return;
	}
	#endif // 0

	#if 1
	auto createMeshAsset = [&](SPtr<MeshAsset>* oMeshAsset, StrView filename)
		{
			auto& meshAsset = *oMeshAsset;

			/*EditMesh mesh;
			WavefrontObjLoader::loadFile(mesh, filename);*/

			meshAsset = makeSPtr<MeshAsset>();
			//meshAsset->rdMesh.create(mesh);
			meshAsset->load(filename);
		};

	createMeshAsset(&box,		"asset/mesh/box.obj");
	createMeshAsset(&sphere,	"asset/mesh/sphere.obj");
	createMeshAsset(&plane,		"asset/mesh/plane.obj");
	createMeshAsset(&cone,		"asset/mesh/cone.obj");
	createMeshAsset(&suzanne,	"asset/mesh/suzanne.obj");

	#endif // 0

	auto& meshAsset = fullScreenTriangle;
	meshAsset = makeSPtr<MeshAsset>();
	meshAsset->renderMesh.create(EditMeshUtil::getFullScreenTriangle());
}

void 
MeshAssets::destroy()
{
	box.reset(nullptr);
	sphere.reset(nullptr);
	plane.reset(nullptr);
	cone.reset(nullptr);
	suzanne.reset(nullptr);
	sponza.reset(nullptr);
	fullScreenTriangle.reset(nullptr);
}

void 
MeshAssets::loadSponza(Shader* shader)
{
	if (!shader)
	{
		shader = Renderer::rdDev()->createShader("asset/shader/lighting/rdsDefaultLighting.shader");
	}

	sponza = makeSPtr<MeshAsset>();
	sponza->load("asset/mesh/scene/sponza/sponza.obj", shader);
}


#endif // 1

}