#pragma once

#include "rds_demo/common/rds_demo_common.h"


namespace rds
{

class	DemoEditorApp;
class	DemoEditorLayer;
class	GraphicsDemo;

struct	MeshAssets;

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
	virtual ~GraphicsDemo();

	void createDefaultScene(Scene* oScene, Material* mtl, MeshAsset* meshAsset, int n, Tuple2f startPos = Tuple2f{0.0f, 0.0f}, Tuple2f step = Tuple2f{3.0f, 3.0f});

public:
	virtual void onCreate();
	virtual void onCreateScene(Scene* oScene);
	virtual void onPrepareRender(RenderGraph* oRdGraph, DrawData* drawData);
	virtual void onExecuteRender(RenderGraph* oRdGraph, DrawData* drawData);

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq);

	virtual void onUiMouseEvent(	UiMouseEvent&		ev);
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev);

public:
	RdgPass* addSkyboxPass(		RenderGraph* oRdGraph, DrawData* drawData, TextureCube* texSkybox, RdgTextureHnd texColor, RdgTextureHnd texDepth);
	RdgPass* addPreDepthPass(	RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd* oDsBuf, RdgTextureHnd* oTexDepthHnd, Color4f clearColor);
	RdgPass* addPostProcessPass(RenderGraph* oRdGraph, DrawData* drawData, StrView passName, RdgTextureHnd rtColor, RdgTextureHnd texColor, Material* material);
	RdgPass* addDrawLightOutlinePass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor, Material* material);
	RdgPass* addDisplayNormalPass(RenderGraph* oRdGraph, DrawData* drawData, RdgTextureHnd rtColor);

public:
	DemoEditorApp& app();
	EngineContext& engineContext();

	Scene&		scene();

	MeshAssets& meshAssets();

	Texture2D*		texUvChecker();
	TextureCube*	skyboxDefault();

private:
	DemoEditorLayer*	_demoLayer	= nullptr;
	Scene*				_scene		= nullptr;

	SPtr<Texture2D>		_texUvChecker;
	SPtr<TextureCube>	_texDefaultSkybox;

protected:
	SPtr<Shader>	_shaderSkybox;
	SPtr<Material>	_mtlSkybox;

	SPtr<Shader>	_shaderPreDepth;
	SPtr<Material>	_mtlPreDepth;

	SPtr<Shader>	_shaderDisplayNormals;
	SPtr<Material>	_mtlDisplayNormals;
};

inline Scene&		GraphicsDemo::scene()			{ return *_scene; }

inline Texture2D*	GraphicsDemo::texUvChecker()	{ return _texUvChecker; }
inline TextureCube* GraphicsDemo::skyboxDefault()	{ return _texDefaultSkybox; }

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

	SPtr<MeshAsset> fullScreenTriangle;

public:
	MeshAssets();
	~MeshAssets();

	void create();
	void destroy();
};

#endif // 1

}