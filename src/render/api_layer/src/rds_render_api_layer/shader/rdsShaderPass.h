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
struct ShaderStage
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Info = ShaderStageInfo;
	
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

struct PixelShaderStage : public ShaderStage
{
public:
	static constexpr ShaderStageFlag stageFlag() { return ShaderStageFlag::Pixel; }

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
	using Stage			= ShaderStage;
	using VertexStage	= VertexShaderStage;
	using PixelStage	= PixelShaderStage;

	using Info = ShaderInfo::Pass;

public:
	ShaderPass() {}
	virtual ~ShaderPass() {}

	void create(Shader* shader, const Info* info, StrView passPath);

	VertexStage*	vertexStage();
	PixelStage*		pixelStage();

	const Info& info() const;

protected:
	virtual void onCreate(Shader* shader, const Info* info, StrView passPath) {};

protected:
	Shader*			_shader			= nullptr;
	const Info*		_info			= nullptr;
	VertexStage*	_vertexStage	= nullptr;
	PixelStage*		_pixelStage		= nullptr;
};

inline void ShaderPass::create(Shader* shader, const Info* info, StrView passPath) { onCreate(shader, info, passPath); }

inline ShaderPass::VertexStage*	ShaderPass::vertexStage()	{ return _vertexStage; }
inline ShaderPass::PixelStage*	ShaderPass::pixelStage()	{ return _pixelStage; }


inline const ShaderPass::Info& ShaderPass::info() const { return *_info; }

#endif

}