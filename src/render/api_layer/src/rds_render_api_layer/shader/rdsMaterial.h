#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
//#include "rds_render_api_layer/rdsRenderDevice.h"
#include "rdsShaderInfo.h"
#include "rdsMaterialPass.h"

namespace rds
{

class RenderDevice;

class Shader;
class RenderContext;

class RenderGpuBuffer;

class Texture;
class RenderGpuBuffer;

#if 0
#pragma mark --- rdsMaterial-Decl ---
#endif // 0
#if 1

struct Material_CreateDesc : public RenderResource_CreateDesc
{
	Shader* shader = nullptr;
};

class Material : public RenderResource
{
	friend class RenderDevice;
public:
	using Base			= RenderResource;
	using This			= Material;

	using CreateDesc	= Material_CreateDesc;
	using CmdCreate		= TransferCommand_CreateMaterial;
	using CmdDestroy	= TransferCommand_DestroyMaterial;

	using Pass			= MaterialPass;
	using Stage			= MaterialPass_Stage;
	using VertexStage	= MaterialPass_VertexStage;
	using PixelStage	= MaterialPass_PixelStage;

public:
	//static constexpr SizeType s_kShaderStageCount	= enumInt(ShaderStageFlag::_kCount);
	static constexpr SizeType s_kLocalPassSize		= 2;

public:
	using Passes = Vector<UPtr<MaterialPass>, s_kLocalPassSize>;

public:
	static CreateDesc		makeCDesc();
	static SPtr<Material>	make(RDS_DebugLabel_PARAM, const CreateDesc& cDesc);
	static SPtr<Material>	make(RDS_DebugLabel_PARAM, Shader* shader);

public:
	Material();
	virtual ~Material();

protected:
	void create	(const CreateDesc& cDesc);
	virtual void onDestroy() override;

public:
	void setShader(Shader* shader);

	void setParam(StrView name,			Texture*			v);
	void setParam(StrView name,			Texture2D*			v);
	void setParam(StrView name,			Texture3D*			v);
	void setParam(StrView name,			TextureCube*		v);
	void setParam(StrView name,			Texture2DArray*		v);
	void setParam(StrView name, const	SamplerState&		v);

	void setParam(StrView name, const bool&					v);
	void setParam(StrView name, const i32&					v);
	void setParam(StrView name, const u32&					v);
	void setParam(StrView name, const f32&					v);
	void setParam(StrView name, const Color4b&				v);
	void setParam(StrView name, const Color4f&				v);
	void setParam(StrView name, const Tuple2u&				v);
	void setParam(StrView name, const Tuple3u&				v);
	void setParam(StrView name, const Tuple4u&				v);
	void setParam(StrView name, const Tuple2f&				v);
	void setParam(StrView name, const Tuple3f&				v);
	void setParam(StrView name, const Tuple4f&				v);
	void setParam(StrView name, const Mat4f&				v);
	void setParam(StrView name, RenderGpuBuffer*			v);

	void setArray(StrView name, Span<const float>			v);		// hlsl will still pack it as float x[4] == total size is 16 * 4, minimum is 16 bytes
	void setArray(StrView name, Span<const Vec4f>			v);
	void setArray(StrView name, Span<const Mat4f>			v);

	void setImage(StrView name, Texture*		v, u32 mipLevel);
	void setImage(StrView name, Texture2D*		v, u32 mipLevel);
	void setImage(StrView name, Texture3D*		v, u32 mipLevel);

	void setParamsToDefault();

public:
	void setPermutation(StrView name, StrView value)					;
	void resetPermutation(const ShaderInfo::Permuts& permuts) ;
	void clearPermutation()												;

public:
	Pass*			getPass(SizeType i);
	ShaderPropId	makePropNameId(StrView name) const;

	bool			isValidNameIdForProp(const ShaderPropId& nameId) const;
	bool			isValidNameIdForPass(const ShaderPropId& nameId) const;

	Span<UPtr<Pass> >	passes();
			Shader*		shader();
	const	Shader*		shader() const;

	const ShaderInfo&	info() const;
	const String&		filename() const;

	const ShaderPermutations& permutations() const;

protected:
	virtual void onCreate		(const CreateDesc& cDesc);
	virtual void onPostCreate	(const CreateDesc& cDesc);

	virtual UPtr<MaterialPass> onMakePass() = 0;

protected:
	template<class T>	void _setParam			(StrView name, const	T&					v);
	template<class T>	void _setArray			(StrView name, const	Span<const T>&		v);
	template<class TEX> void _setTexParam		(StrView name,			TEX*				v);
						void _setSamplerParam	(StrView name, const	SamplerState&		v);
						void _setBufferParam	(StrView name,			RenderGpuBuffer*	v);
	template<class TEX> void _setImageParam		(StrView name,			TEX*				v, u32 mipLevel);


						void _setPermutation(StrView name, StrView value);

protected:
	SPtr<Shader>		_shader;
	Passes				_passes;
	ShaderPermutations	_permuts;
};

inline void Material::setParam(StrView name,		Texture2D*			v)										{ return _setTexParam(name, v); }
inline void Material::setParam(StrView name,		Texture3D*			v)										{ return _setTexParam(name, v); }
inline void Material::setParam(StrView name,		TextureCube*		v)										{ return _setTexParam(name, v); }
inline void Material::setParam(StrView name,		Texture2DArray*		v)										{ return _setTexParam(name, v); }
inline void Material::setParam(StrView name, const	SamplerState&		v)										{ return _setSamplerParam(name, v); }

inline void Material::setParam(StrView name, const bool&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const i32&			v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const u32&			v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const f32&			v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Color4b&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Color4f&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Tuple2u&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Tuple3u&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Tuple4u&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Tuple2f&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Tuple3f&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Tuple4f&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, const Mat4f&		v)		{ return _setParam(name, v); }
inline void Material::setParam(StrView name, RenderGpuBuffer*	v)		{ return _setBufferParam(name, v); }

inline void Material::setArray(StrView name, Span<const Vec4f>	v)		{ return _setArray(name, v); }
inline void Material::setArray(StrView name, Span<const Mat4f>	v)		{ return _setArray(name, v); }

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

inline			Shader*		Material::shader()			{ return _shader; }
inline const	Shader*		Material::shader() const	{ return _shader; }

inline Span<UPtr<Material::Pass>>	Material::passes() { return _passes; }

template<class T> inline
void 
Material::_setParam(StrView name, const T& v)
{
	for (auto& pass : _passes)
	{
		pass->setParam(name, v);
	}
}

template<class T> inline
void 
Material::_setArray(StrView name, const	Span<const T>& v)
{
	if (v.is_empty())
		return;

	for (auto& pass : _passes)
	{
		pass->setArray(name, spanConstCast<T>(v));
	}
}

template<class TEX> inline 
void 
Material::_setTexParam(StrView name, TEX* v)
{
	for (auto& pass : _passes)
	{
		pass->setTexParam(name, v);
	}
}

inline
void 
Material::_setBufferParam(StrView name, RenderGpuBuffer* v)
{
	for (auto& pass : _passes)
	{
		pass->setBufferParam(name, v);
	}
}

template<class TEX> inline 
void 
Material::_setImageParam(StrView name, TEX* v, u32 mipLevel)
{
	for (auto& pass : _passes)
	{
		pass->setImageParam(name, v, mipLevel);
	}
}

inline const ShaderPermutations&	Material::permutations()	const { return _permuts; }

#endif

}