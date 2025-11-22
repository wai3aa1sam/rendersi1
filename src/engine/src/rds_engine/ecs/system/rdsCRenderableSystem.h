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
struct	RenderData_RenderJob;

#if 0
#pragma mark --- rdsCRenderableSystem-Decl ---
#endif // 0
#if 1

class CRenderableSystem : public CSystemT<CRenderable> // Singleton<CRenderableSystem, CSystem>
{
	friend class	CRenderable;
	friend class	SceneView;
	friend class	DrawData;		// temporary
public:
	using RenderableTable		= VectorMap<EntityId, SPtr<CRenderable> >;

public:
	CRenderableSystem();
	~CRenderableSystem();

	void create(EngineContext* egCtx);
	void destroy();

public:
	void commit(RenderJob* rdJob, const Scene& scene);
	void drawUi(RenderContext* renderContext, bool isDrawUi, bool isDrawToScreen);
	
	void render();

public:
	void addCamera(		math::Camera3f*	camera);
	void removeCamera(	math::Camera3f*	camera);

public:
	Material* getOverrideMaterial(EntityId id, const Shader* shader);

	Span<DrawData*>	drawData();
	DrawData*		mainDrawData();

protected:
	void transitPresentTexture(RenderGraph& rdGraph, DrawData* drawData);

public:
	Vector<CRenderable*>& renderables();

public:
	//RenderGraph& renderGraph();

protected:
	/*
		all of this should have a Vector<Data>, for multi cameras
	*/
	//RenderGraph						_rdGraph;
	/*
		maybe use a two level index table to prevent empty space, since not all ent has Renderable
		, also, maybe separate mvp and model, since when have two camera, the model since redundant
	*/
	ParamBuffer<ObjectTransform>	_objTransformBuf;		
	ParamBuffer<DrawParam>			_drawPramBuf;

	Vector<DrawData*, 4>	_drawData;
	LinearAllocator			_drawDataAlloc;
	
	VectorMap<SPtr<Shader>, VectorMap<EntityId, SPtr<Material> > > _overrideMtls;		// TODO: temporary solution
};

//inline RenderGraph&				CRenderableSystem::renderGraph()						{ return _rdGraph; }
inline Vector<CRenderable*>&	CRenderableSystem::renderables()						{ return components(); }

inline Span<DrawData*>			CRenderableSystem::drawData()							{ return _drawData; }

#endif


}