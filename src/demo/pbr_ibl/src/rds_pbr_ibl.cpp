#include "rds_pbr_ibl-pch.h"

#include "rds_pbr_ibl.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"


namespace rds
{

static Material*		s_mtl			= nullptr;
static CRenderableMesh* s_rdableMesh	= nullptr;

#if 0
#pragma mark --- rdsPbrIbl-Impl ---
#endif // 0
#if 1

void 
PbrIbl::onCreate()
{
	Base::onCreate();
	// temporary
	auto& rdPassPipeline = *_rdPassPipelines[0];
	_rpfPbrIbl	= rdPassPipeline.addRenderPassFeature<RpfPbrIbl>();
}

void 
PbrIbl::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);

	u32 objCount = 6;

	auto shader = Renderer::rdDev()->createShader("asset/shader/lighting/rdsDefaultLighting.shader");
	createDefaultScene(oScene, shader, meshAssets().sphere, Vec3u{objCount, objCount, 1});
	createLights(oScene, Vec3u{ objCount, objCount, 1 });

	{
		auto totalCount = (objCount * objCount - 1);
		totalCount = math::clamp<u32>(totalCount, 1, (objCount * objCount - 1));
		auto metalnessStep = 1.0f / totalCount;
		auto roughnessStep = 1.0f / totalCount;

		auto i = 0;
		for (auto& e : scene().entities())
		{
			auto* transf = e->getComponent<CTransform>();

			{
				auto* light = e->getComponent<CLight>();
				if (light)
				{
					transf->setLocalRotation(Vec3f(math::radians(134.398f), math::radians(20.0f), math::radians(-4.4f)));
				}
			}
			auto* rdable = e->getComponent<CRenderableMesh>();
			if (!rdable)
				continue;

			auto& mtl = rdable->material;
			mtl->setParam("metalness",			metalnessStep * math::clamp((totalCount - i), sCast<u32>(0), sCast<u32>(totalCount)));
			mtl->setParam("roughness",			roughnessStep * i);
			mtl->setParam("ambientOcclusion",	0.2f);
			++i;

			s_mtl			= mtl;
			s_rdableMesh	= rdable;
		}
	}

	{
		auto& camera = app().mainWindow().camera();
		camera.setPos(12.987f,	14.578f,	20.333f);
		camera.setAim(8.021f,	7.989f,		-0.492f);
		camera.setFov(45.0f);
		camera.setNearClip(	0.1f	* 10.0f);
		camera.setFarClip(	1000.0f * 10.0f);
	}
}

void 
PbrIbl::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);

	//auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	auto* rpfPbrIbl	= _rpfPbrIbl;
	rpfPbrIbl->addPreparePass(&_rpfPbrIblResult, defaultHdrEnv(), meshAssets().box->renderMesh);
}

void 
PbrIbl::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	//auto* rpfPbrIbl	= _rpfPbrIbl;

	RdgTextureHnd rtColor	= rdGraph->createTexture("pbr_ibl_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf		= rdGraph->createTexture("pbr_ibl_depth",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

	auto& passPbrIbl = rdGraph->addPass("pbr_ibl", RdgPassTypeFlags::Graphics);
	passPbrIbl.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
	passPbrIbl.setDepthStencil(dsBuf,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);
	passPbrIbl.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			rdReq.reset(rdGraph->renderContext(), drawData);

			auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor(Color4f{ 0.1f, 0.2f, 0.3f, 1.0f });
			clearValue->setClearDepth(1.0f);

			DrawSettings drawSettings;
			drawSettings.cullingSetting.setCameraFrustum(drawData->camera->frustum());
			drawSettings.setMaterialFn = 
				[=](Material* mtl_)
				{
					mtl_->setParam("tex_brdfLut",			_rpfPbrIblResult.brdfLut);
					mtl_->setParam("cube_irradianceEnv",	_rpfPbrIblResult.cubeIrradinceMap);
					mtl_->setParam("cube_prefilteredEnv",	_rpfPbrIblResult.cubePrefilteredMap);
				};
			drawData->drawScene(rdReq, drawSettings);

			#if 0
			{
				auto* mtl = s_mtl;
				drawData->setupMaterial(mtl);
				{
					auto&	ent				= s_rdableMesh->entity();
					auto	id				= ent.id();
					auto&	meshAsset		= s_rdableMesh->meshAsset;
					auto	subMeshIndex	= s_rdableMesh->subMeshIndex;

					PerObjectParam perObjParam;
					perObjParam.id				= sCast<u32>(id);
					//perObjParam.drawParamIdx	= drawData ? sCast<u32>(drawData->drawParamIdx) : 0;

					if (auto& fn = drawSettings.setMaterialFn)
					{
						fn(mtl);
					}

					//auto&		transform	= ent.transform();
					//const auto& matrix		= transform.worldMatrix();
					auto&		rdMesh		= meshAsset->renderMesh;

					if (subMeshIndex == NumLimit<u32>::max())
					{
						rdReq.drawMesh(RDS_RD_CMD_DEBUG_ARG, rdMesh, mtl, perObjParam);
					}
					else
					{
						auto& subMesh = rdMesh.subMeshes()[subMeshIndex];
						rdReq.drawSubMeshT(RDS_RD_CMD_DEBUG_ARG, subMesh, mtl, perObjParam);
					}
				}
				/*auto drawCall = rdReq.addDrawCall();
				drawCall->setDebugSrcLoc(RDS_SRCLOC);
				drawCall->vertexCount = 3;
				drawCall->setMaterial(mtl);*/
			}
			#endif // 0

		}
	);

	addDrawLightOutlinePass(rdGraph, drawData, rtColor, drawData->mtlLine());
	addSkyboxPass(rdGraph, drawData, _rpfPbrIblResult.cubeEnvMap, rtColor, dsBuf);

	drawData->oTexPresent = rtColor;
}

void 
PbrIbl::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	
}

void 
PbrIbl::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);
}

void 
PbrIbl::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);
}
#endif

}