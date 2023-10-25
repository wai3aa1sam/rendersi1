#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsShaderInfo.h"
#include "rdsMaterialPass.h"

namespace rds
{

class Shader;
class RenderContext;

#if 0
#pragma mark --- rdsMaterial-Decl ---
#endif // 0
#if 1


class Material : public RenderResource
{
public:
	using Base			= RenderResource;
	using This			= Material;

	using Pass			= MaterialPass;
	using Stage			= MaterialPass_Stage;
	using VertexStage	= MaterialPass_VertexStage;
	using PixelStage	= MaterialPass_PixelStage;

public:
	static constexpr SizeType s_kShaderStageCount = enumInt(ShaderStageFlag::_kCount);
	static constexpr SizeType s_kLocalPassSize = 2;

public:
	static SPtr<Material>	make(Shader* shader);

public:
	Material();
	virtual ~Material();

	void create();
	void destroy();

	void setShader(Shader* shader);

	void setParam(StrView name, const i32&		v);
	void setParam(StrView name, const f32&		v);
	void setParam(StrView name, const Color4b&	v);
	void setParam(StrView name, const Tuple2f&	v);
	void setParam(StrView name, const Tuple3f&	v);
	void setParam(StrView name, const Tuple4f&	v);
	void setParam(StrView name, const Mat4f&	v);

	Pass*			getPass		(SizeType i);
	ShaderParamId	getParamId	(StrView name) const;

	Span<UPtr<Pass> >	passes();
	Shader*				shader();
	const ShaderInfo&	info() const;

	u32 frameIdx() const;

protected:
	virtual void onCreate		();
	virtual void onPostCreate	();
	virtual void onDestroy		();

	virtual UPtr<MaterialPass> onMakePass() = 0;

protected:
	template<class T> void _setParam(StrView name, const T& v);

protected:
	SPtr<Shader> _shader;
	Vector<UPtr<MaterialPass>, s_kLocalPassSize> _passes;

	u32 _frameIdx = 0;
};

inline void Material::setParam(StrView name, const i32&		v)	{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const f32&		v)	{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Color4b&	v)	{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Tuple2f&	v)	{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Tuple3f&	v)	{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Tuple4f&	v)	{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Mat4f&	v)	{ return _setParam(name, v); }

inline
Material::Pass* 
Material::getPass(SizeType i)
{
	if (i >= _passes.size())
	{
		RDS_CORE_ASSERT(false, "invalid pass index");
		return nullptr;
	}
	return _passes[i].ptr();
}

inline Shader*						Material::shader() { return _shader; }
inline Span<UPtr<Material::Pass>>	Material::passes() { return _passes; }

inline u32 Material::frameIdx() const { return _frameIdx; }

template<class T> inline
void 
Material::_setParam(StrView name, const T& v)
{
	for (auto& pass : _passes)
	{
		pass->setParam(name, v);
	}
}



#endif

}