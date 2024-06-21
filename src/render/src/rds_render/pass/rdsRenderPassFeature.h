#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render_api_layer/graph/rdsRenderGraphPass.h"
#include "rds_render_api_layer/graph/rdsRenderGraph.h"

#include "rdsDrawData_Base.h"
#include "rds_render/setting/rdsDrawSettings.h"

namespace rds
{

class RenderPassPipeline;

#define RDS_RPF_COMMON_BODY(T)					\
	friend class RenderPassPipeline;			\
protected:										\
	using Base		= RenderPassFeature;		\
	using Result	= RDS_CONCAT(T, _Result);	\
private:										\
// ---

#if 0
#pragma mark --- rdsRenderPassFeature-Decl ---
#endif // 0
#if 1

struct RenderPassFeature_Result
{

};

class RenderPassFeature : public NonCopyable
{
	friend class RenderPassPipeline;
public:
	static void createShader(  SPtr<Shader>* oShader, StrView filename);
	static void createMaterial(SPtr<Shader>* oShader, SPtr<Material>* oMtl, StrView filename, const Function<void(Material*)>& fnSetParam = {});
	static void createMaterial(Shader*		 shader,  SPtr<Material>* oMtl);

public:
	RenderGraph*		renderGraph();
	DrawData_Base*		drawDataBase();
	template<class T> T	getDrawDataT();

	RenderDevice*		renderDevice();

protected:
	RenderPassFeature();
	virtual ~RenderPassFeature();

protected:
	void setRenderPassPipeline(RenderPassPipeline* v);

private:
	RenderPassPipeline* _rdPassPipeline = nullptr;
};

template<class T> 
T	
RenderPassFeature::getDrawDataT()
{
	return sCast<T>(_rdPassPipeline->drawData());
}

#endif

}