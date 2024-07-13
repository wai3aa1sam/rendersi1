#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render_api_layer/graph/rdsRenderGraphPass.h"
#include "rds_render_api_layer/graph/rdsRenderGraph.h"

#include "rdsDrawData_Base.h"
#include "rds_render/setting/rdsDrawSettings.h"

namespace rds
{

/*
* TODO, typeInfo / use T as a hash
*/
class RenderPassPipeline;

#define RDS_RPF_COMMON_BODY(T)					\
	friend class RenderPassPipeline;			\
protected:										\
	using Base		= RenderPassFeature;		\
public:											\
	using Result	= RDS_CONCAT(T, _Result);	\
	using Param		= RDS_CONCAT(T, _Param);	\
private:										\
// ---

#if 0
#pragma mark --- rdsRenderPassFeature-Decl ---
#endif // 0
#if 1

struct RenderPassFeature_Result
{

};

struct RenderPassFeature_Param
{

};

class RenderPassFeature : public NonCopyable
{
	friend class RenderPassPipeline;
public:
	RdgPass* _addClearImage2DPass(	SPtr<Material>& material, RdgTextureHnd image);
	RdgPass* _addClearImage3DPass(	SPtr<Material>& material, RdgTextureHnd image);
	RdgPass* _addDrawScreenQuadPass(SPtr<Material>& material, RdgTextureHnd rtColor, RdgTextureHnd texColor);

public:
	RenderGraph*			renderGraph();
	DrawData_Base*			drawDataBase();
	template<class T> T*	drawDataT();

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
T*
RenderPassFeature::drawDataT()
{
	return _rdPassPipeline->drawDataT<T>();
}

#endif

#if 0
#pragma mark --- rdsRenderUtil-Decl ---
#endif // 0
#if 1

struct RenderUtil
{
	RenderUtil() = delete;
public:
	static void createShader(	SPtr<Shader>* oShader, StrView filename);
	static void createMaterial(	SPtr<Shader>* oShader, SPtr<Material>* oMtl, StrView filename, const Function<void(Material*)>& fnSetParam = {});
	static void createMaterial(	Shader*		  shader,  SPtr<Material>* oMtl);
	static void createMaterials(SPtr<Shader>* oShader,  Span<SPtr<Material> > oMtls, StrView filename);

	static void destroyShader(			SPtr<Shader>&			shader);
	static void destroyShaderMaterial(	SPtr<Shader>&			shader, SPtr<Material>&			material);
	static void destroyShaderMaterials(	SPtr<Shader>&			shader, Span<SPtr<Material> >	materials);
	static void destroyMaterial(		SPtr<Material>&			material);
	static void destroyMaterials(		Span<SPtr<Material> >	materials);

};


#endif

}