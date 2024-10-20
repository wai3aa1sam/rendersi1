#pragma once

#include "rds_demo/common/rds_demo_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

class	DemoEditorApp;
class	DemoEditorLayer;
class	GraphicsDemo;

struct 	MeshAssets;

extern UPtr<GraphicsDemo> makeDemo();

#if 0
#pragma mark --- rdsGraphicsDemo-Decl ---
#endif // 0
#if 1

class GraphicsDemo : public NonCopyable
{
	friend class DemoEditorLayer;
public:
	static void createMaterial(SPtr<Shader>* oShader, SPtr<Material>* oMtl, StrView filename, const Function<void(Material*)>& fnSetParam = {});

public:
	SPtr<Shader>	_shaderScreenQuad;
	SPtr<Material>	_mtlScreenQuad;

public:
	virtual ~GraphicsDemo();

	void		createDefaultScene(Scene* oScene, Shader* shader, MeshAsset* meshAsset, Vec3u objectCount, Vec3f startPos = Vec3f::s_zero(), Vec3f step = Vec3f::s_one() * 3.0f, Vec3f scale = Vec3f::s_one());
	CTransform* createLights(Scene* oScene, Vec3u objectCount, Vec3f startPos = Vec3f::s_zero(), Vec3f step = Vec3f::s_one() * 3.0f
							, Quat4f direction = Quat4f::s_eulerDegX(141.4f), float range = 4.0f, float intensity = 1.0f, const AABBox3f& rndBounding = {});

	void prepareRender(RenderGraph* oRdGraph, DrawData* drawData);
	void executeRender(RenderGraph* oRdGraph, DrawData* drawData);

public:
	virtual void onCreate();
	virtual void onCreateScene(Scene* oScene);
	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline);
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline);

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq);

	virtual void onUiMouseEvent(	UiMouseEvent&		ev);
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev);

public:
	/*
	* TODO: change to RenderPassFeature
	*/
	RdgPass* addSkyboxPass(				RenderGraph* oRdGraph, DrawData* drawData, TextureCube* texSkybox, RdgTextureHnd texColor, RdgTextureHnd texDepth);
	RdgPass* addPostProcessingPass(		RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd* outColor, RdgTextureHnd texColor);
	RdgPass* addPostProcessingPass(		RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd* outRtColor);
	RdgPass* addDrawLightOutlinePass(	RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, Material* material);
	RdgPass* addDisplayNormalPass(		RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor);
	RdgPass* addDisplayAABBoxPass(		RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, const DrawSettings& drawSettings);

public:
	Entity* addEntity(Material* mtl);

public:
	DemoEditorApp& app();
	EngineContext& engineContext();

	Scene&		scene();

	MeshAssets& meshAssets();

	Texture2D*		texUvChecker();
	Texture2D*		defaultHdrEnv();
	TextureCube*	skyboxDefault();

	CTransform* directionalLightTransform();

private:
	DemoEditorLayer*	_demoLayer	= nullptr;
	Scene*				_scene		= nullptr;

	SPtr<Texture2D>		_texUvChecker;
	SPtr<Texture2D>		_texDefaultHdrEnv;
	SPtr<TextureCube>	_texDefaultSkybox;


protected:
	SPtr<Shader>	_shaderSkybox;
	SPtr<Material>	_mtlSkybox;

	SPtr<Shader>	_shaderDisplayNormals;
	SPtr<Material>	_mtlDisplayNormals;

	SPtr<Material>	_mtlPostProcessing;

	Vector<UPtr<RenderPassPipeline>, 2>	_rdPassPipelines;

	CTransform*		_directionalLightTransf	= nullptr;
};

inline Scene&		GraphicsDemo::scene()			{ return *_scene; }

inline Texture2D*	GraphicsDemo::texUvChecker()	{ return _texUvChecker; }
inline Texture2D*	GraphicsDemo::defaultHdrEnv()	{ return _texDefaultHdrEnv; }
inline TextureCube* GraphicsDemo::skyboxDefault()	{ return _texDefaultSkybox; }

inline CTransform*	GraphicsDemo::directionalLightTransform() { return _directionalLightTransf; }

#endif

#if 0
#pragma mark --- rdsMeshAssets-Decl ---
#endif // 0
#if 1

struct MeshAssets
{
public:
	SPtr<MeshAsset> box;
	SPtr<MeshAsset> sphere;
	SPtr<MeshAsset> plane;
	SPtr<MeshAsset> cone;
	SPtr<MeshAsset> suzanne;

	SPtr<MeshAsset> sponza;

	SPtr<MeshAsset> fullScreenTriangle;

public:
	MeshAssets();
	~MeshAssets();

	void create();
	void destroy();

	void loadSponza(Shader* shader);
};

#endif // 1

}