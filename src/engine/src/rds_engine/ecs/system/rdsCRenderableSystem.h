#pragma once

#include "rds_engine/common/rds_engine_common.h"

#include "rds_render_api_layer/graph/rdsRenderGraph.h"

#include "rdsCSystem.h"
#include "rds_engine/ecs/component/rdsCRenderable.h"
#include "rds_engine/draw/rdsDrawData.h"

#include <rds_render_api_layer/buffer/rdsParamBuffer.h>

namespace rds
{

class	Scene;
class	SceneView;
class	CRenderable;

struct	MeshAssets;
struct	DrawParam;

#if 0
#pragma mark --- rdsCRenderableSystem-Decl ---
#endif // 0
#if 1

class CRenderableSystem : public CSystemT<CRenderable> // Singleton<CRenderableSystem, CSystem>
{
	friend class	CRenderable;
	friend class	SceneView;
	friend class	DrawData;		// temporary
	RDS_ENGINE_COMMON_BODY();
public:
	using RenderableTable		= VectorMap<EntityId, SPtr<CRenderable> >;
	using FramedRenderRequest	= Vector<RenderRequest, RenderApiLayerTraits::s_kFrameInFlightCount>;

public:
	CRenderableSystem();
	~CRenderableSystem();

	void create(EngineContext* egCtx);
	void destroy();

	void update(const Scene& scene, DrawData& drawData);
	void render();
	void present(RenderContext* renderContext, bool isDrawUi, bool isDrawToScreen);

public:
	Material* getOverrideMaterial(EntityId id, const Shader* shader);

	RenderDevice* renderDevice();

protected:
	void transitPresentTexture(RenderGraph& rdGraph, DrawData& drawData);

public:
	Vector<CRenderable*>& renderables();

public:
	RenderGraph& renderGraph();

protected:
	RenderRequest& renderRequest();

protected:
	SPtr<Material>	_mtlScreenQuad;

	/*
		all of this should have a Vector<Data>, for multi cameras
	*/
	RenderGraph						_rdGraph;
	FramedT<RenderRequest>			_framedRdReq;
	/*
		maybe use a two level index table to prevent empty space, since not all ent has Renderable
		, also, maybe separate mvp and model, since when have two camera, the model since redundant
	*/
	ParamBuffer<ObjectTransform>	_objTransformBuf;		
	ParamBuffer<DrawParam>			_drawPramBuf;
	
	VectorMap<SPtr<Shader>, VectorMap<EntityId, SPtr<Material> > > _overrideMtls;		// TODO: temporary solution
};

inline RenderGraph&				CRenderableSystem::renderGraph()	{ return _rdGraph; }
inline RenderRequest&			CRenderableSystem::renderRequest()	{ return _framedRdReq[renderGraph().iFrame()]; }

inline Vector<CRenderable*>&	CRenderableSystem::renderables()	{ return components(); }


#endif


}