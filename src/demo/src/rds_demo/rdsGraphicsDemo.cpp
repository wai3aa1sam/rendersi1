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
GraphicsDemo::onCreate()
{
	{
		auto texCDesc = Texture2D::makeCDesc();

		texCDesc.create("asset/texture/uvChecker.png");
		_texUvChecker = Renderer::rdDev()->createTexture2D(texCDesc);
		_texUvChecker->setDebugName("uvChecker");
	}

	{
		auto texCDesc = TextureCube::makeCDesc();
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
}

void 
GraphicsDemo::onCreateScene(Scene* oScene)
{
	
}

void 
GraphicsDemo::onPrepareRender(RenderGraph* oRdGraph, DrawData* drawData)
{

}

void 
GraphicsDemo::onExecuteRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	//auto& rdGraph = *oRdGraph;
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
GraphicsDemo::createDefaultScene(Scene* oScene, Material* mtl, MeshAsset* meshAsset, int n, Tuple2f startPos, Tuple2f step)
{
	auto& scene = *oScene;

	//auto n = 25;
	auto row = math::sqrt(sCast<int>(n));
	auto col = row;
	//auto step		= Tuple2f{ 3.0f, 3.0f};
	//auto startPos	= Tuple2f{-0.0f, 0.0f};

	for (size_t r = 0; r < row; r++)
	{
		for (size_t c = 0; c < col; c++)
		{
			auto* ent = scene.addEntity("");

			auto* rdableMesh = ent->addComponent<CRenderableMesh>();
			rdableMesh->material = mtl;
			rdableMesh->meshAsset = meshAsset;

			auto* transform	= ent->getComponent<CTransform>();
			transform->setLocalPosition(startPos.x + step.x * c, 0.0f, startPos.y + step.y * r);

			TempString buf;
			fmtTo(buf, "Entity-{}", sCast<u64>(ent->id()));
			ent->setName(buf);
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
	auto createMeshAsset = [&](SPtr<MeshAsset>* oMeshAsset, StrView filename)
		{
			auto& meshAsset = *oMeshAsset;

			EditMesh mesh;
			WavefrontObjLoader::loadFile(mesh, filename);

			meshAsset = makeSPtr<MeshAsset>();
			meshAsset->rdMesh.create(mesh);
		};

	createMeshAsset(&box,		"asset/mesh/box.obj");
	createMeshAsset(&sphere,	"asset/mesh/sphere.obj");
	createMeshAsset(&suzanne,	"asset/mesh/suzanne.obj");
}

void 
MeshAssets::destroy()
{
	box.reset(nullptr);
	sphere.reset(nullptr);
	suzanne.reset(nullptr);
}

#endif // 1

}