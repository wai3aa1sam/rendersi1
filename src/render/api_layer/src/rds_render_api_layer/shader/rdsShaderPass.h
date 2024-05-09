#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsShaderInfo.h"

namespace rds
{

class Shader;

#if 0
#pragma mark --- rdsShaderStage-Decl ---
#endif // 0
#if 1

// no virtual, virtual in upper layer (ShaderPass) as a whole
// one virtual for all
// TODO: add a template
struct ShaderStage
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Info = ShaderStageInfo;
	
public:
	void createInfo(StrView binFilepath);

public:
	const Info& info() const;

protected:
	Info _info;
};

inline const ShaderStage::Info& ShaderStage::info() const { return _info; }

struct VertexShaderStage : public ShaderStage
{
public:
	static constexpr ShaderStageFlag stageFlag() { return ShaderStageFlag::Vertex; }

protected:

};

struct TessellationControlShaderStage : public ShaderStage
{
public:
	static constexpr ShaderStageFlag stageFlag() { return ShaderStageFlag::TessellationControl; }

protected:

};

struct TessellationEvaluationShaderStage : public ShaderStage
{
public:
	static constexpr ShaderStageFlag stageFlag() { return ShaderStageFlag::TessellationEvaluation; }

protected:

};

struct GeometryShaderStage : public ShaderStage
{
public:
	static constexpr ShaderStageFlag stageFlag() { return ShaderStageFlag::Geometry; }

protected:

};

struct PixelShaderStage : public ShaderStage
{
public:
	static constexpr ShaderStageFlag stageFlag() { return ShaderStageFlag::Pixel; }

protected:
};

struct ComputeShaderStage : public ShaderStage
{
public:
	static constexpr ShaderStageFlag stageFlag() { return ShaderStageFlag::Compute; }

protected:
};

#endif

#if 0
#pragma mark --- rdsShaderPass-Decl ---
#endif // 0
#if 1

class ShaderPass : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Stage							= ShaderStage;
	using VertexStage					= VertexShaderStage;
	using TessellationControlStage		= TessellationControlShaderStage;
	using TessellationEvaluationStage	= TessellationEvaluationShaderStage;
	using GeometryStage					= GeometryShaderStage;
	using PixelStage					= PixelShaderStage;
	using ComputeStage					= ComputeShaderStage;

	using Info = ShaderInfo::Pass;

public:
	ShaderPass();
	virtual ~ShaderPass();

	void create(Shader* shader, const Info* info, StrView passPath);
	void destroy();

	VertexStage*					vertexStage();
	TessellationControlStage*		tessellationControlStage();
	TessellationEvaluationStage*	tessellationEvaluationStage();
	GeometryStage*					geometryStage();
	PixelStage*						pixelStage();
	ComputeStage*					computeStage();

	const Info& info() const;

	Shader* shader();

protected:
	virtual void onCreate(Shader* shader, const Info* info, StrView passPath);
	virtual void onDestroy();

protected:
	Shader*							_shader			= nullptr;
	const Info*						_info			= nullptr;
	VertexStage*					_vertexStage	= nullptr;
	TessellationControlStage*		_tescStage		= nullptr;
	TessellationEvaluationStage*	_teseStage		= nullptr;
	GeometryStage*					_geometryStage	= nullptr;
	PixelStage*						_pixelStage		= nullptr;
	ComputeStage*					_computeStage	= nullptr;
};

inline ShaderPass::VertexStage*					ShaderPass::vertexStage()					{ return _vertexStage; }
inline ShaderPass::TessellationControlStage*	ShaderPass::tessellationControlStage()		{ return _tescStage; }
inline ShaderPass::TessellationEvaluationStage* ShaderPass::tessellationEvaluationStage()	{ return _teseStage; }
inline ShaderPass::GeometryStage*				ShaderPass::geometryStage()					{ return _geometryStage; }
inline ShaderPass::PixelStage*					ShaderPass::pixelStage()					{ return _pixelStage; }
inline ShaderPass::ComputeStage*				ShaderPass::computeStage()					{ return _computeStage; }

inline const	ShaderPass::Info&	ShaderPass::info() const	{ return *_info; }

inline			Shader*				ShaderPass::shader()		{ return _shader; }

#endif

}