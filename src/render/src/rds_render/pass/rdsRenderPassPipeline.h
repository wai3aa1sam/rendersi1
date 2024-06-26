#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render_api_layer/graph/rdsRenderGraph.h"

#include "rds_render/pass_feature/utility/image/rdsRpfClearImage2D.h"
#include "rds_render/pass_feature/utility/image/rdsRpfClearImage3D.h"

namespace rds
{

class DrawData_Base;
class RenderPassFeature;

/*
* create RenderPassFeature maybe waste for each DrawData (primary for different camera)
* however, some parts can not be reused, like RpfXXX_Result and some of the materials, therefore create a new RenderPassPipeline each time is better to manage
* if some materials / shaders can be reused, then set it to static variable to reuse it
* 
* still thinking how to handle changing material for renderable in runtime
*/

#if 0
#pragma mark --- rdsRenderPassPipeline-Decl ---
#endif // 0
#if 1

class RenderPassPipeline : public NonCopyable
{
public:
	RenderPassPipeline();
	~RenderPassPipeline();

	void reset(RenderGraph* renderGraph, DrawData_Base* drawData);

	template<class T, class... ARGS> T* addRenderPassFeature(ARGS&&... args);

public:
	RenderGraph*		renderGraph();
	DrawData_Base*		drawDataBase();
	template<class T> T	drawDataT();


protected:
	RenderGraph*	_rdGraph	= nullptr;
	DrawData_Base*	_drawData	= nullptr;

	LinearAllocator					_alloc;
	Vector<RenderPassFeature*, 4>	_rpfs;

	/*
	* TODO: temporary, later have a hash to get the rpf instead of caching
	*/
	friend class RenderPassFeature;
	RpfClearImage2D* _rpfClearImage2D = nullptr;
	RpfClearImage3D* _rpfClearImage3D = nullptr;
};

template<class T, class... ARGS> inline
T* 
RenderPassPipeline::addRenderPassFeature(ARGS&&... args)
{
	auto*	buf = _alloc.alloc(sizeof(T));
	T*		obj = new (buf) T();
	
	_rpfs.emplace_back(obj);
	obj->setRenderPassPipeline(this);
	obj->create(rds::forward<ARGS>(args)...);

	return obj;
}

inline RenderGraph*			RenderPassPipeline::renderGraph()	{ return _rdGraph; }
inline DrawData_Base*		RenderPassPipeline::drawDataBase()	{ return _drawData; }
template<class T> inline T	RenderPassPipeline::drawDataT()		{ return sCast<T>(_drawData); }




#endif

}