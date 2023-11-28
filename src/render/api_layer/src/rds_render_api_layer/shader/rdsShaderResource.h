#pragma once

#include "rdsShaderInfo.h"

#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"

namespace rds
{

struct ShaderStage;

class Shader;
class ShaderPass;

struct	ShaderResources;
using	FramedShaderResources = Vector<ShaderResources, RenderApiLayerTraits::s_kFrameInFlightCount>;

struct SamplerParam;

#if 0
#pragma mark --- rdsShaderResource-Decl ---
#endif // 0
#if 1

template<class INFO>
struct ShaderResource
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Info = typename INFO;

public:
	void create(const Info* info);

	bool isValid() const;

	const Info&			info		() const;

	const String&		name		() const;
	DataType			dataType	() const;
	u16					bindPoint	() const;
	u16					bindCount	() const;

public:
	const Info*	_info = nullptr;
};

template<class INFO> inline 
void 
ShaderResource<INFO>::create(const Info* info)
{
	_info = info;
}

template<class INFO> inline bool ShaderResource<INFO>::isValid() const { return _info; }

template<class INFO> inline const typename ShaderResource<INFO>::Info&		ShaderResource<INFO>::info		() const { return *_info; }
template<class INFO> inline const String&									ShaderResource<INFO>::name		() const { return info().name; }
template<class INFO> inline typename ShaderResource<INFO>::DataType			ShaderResource<INFO>::dataType	() const { return info().dataType; }
template<class INFO> inline u16												ShaderResource<INFO>::bindPoint	() const { return info().bindPoint; }
template<class INFO> inline u16												ShaderResource<INFO>::bindCount	() const { return info().bindCount; }

#endif

#if 0
#pragma mark --- rdsShaderResources-Decl ---
#endif // 0
#if 1

struct ShaderResources
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static constexpr SizeType s_kLocalConstBufSize	= 2;
	static constexpr SizeType s_kLocalTextureSize	= 4;

	static constexpr const char* s_kAutoSamplerNamePrefix = "_rds_";
	static constexpr const char* s_kAutoSamplerNameSuffix = "_sampler";

public:
	template<class T>	void setParam		(StrView name, const T& v);
	template<class TEX>	void setTexParam	(StrView name, TEX* v, bool isAutoSetSampler);
						void setSamplerParam(StrView name, const SamplerState& v);

	void create	(ShaderStage* shaderStage, ShaderPass* pass);
	void destroy();

	void bind();

	void uploadToGpu();

	void clear();

	bool isDirty() const { return _isDirty; }

	struct ConstBuffer : public ShaderResource<ShaderStageInfo::ConstBuffer>
	{
	public:
		using Base		= ShaderResource<ShaderStageInfo::ConstBuffer>;
		using VarInfo	= ShaderVariableInfo;

	public:
		static constexpr SizeType s_kLocalDataSize = 2;

	public:
		void create	(const Info* info, ShaderPass* pass);
		void destroy();

		template<class T> 
		void setParam(StrView name, const T& v)
		{
			const ShaderVariableInfo* var = info().findVariable(name);
			if (!var)
				return;
			_setParam(*var, v);
		}

		void uploadToGpu();

				u8* data();
		const	u8* data() const;

	protected:
		void _setParam(const VarInfo& varInfo, const i32&		v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const f32&		v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const Color4b&	v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const Tuple2f&	v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const Tuple3f&	v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const Tuple4f&	v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const Mat4f&		v)	{ return _setParamCheckType(varInfo, v); }

		template<class T> void _setParamCheckType	(const VarInfo& varInfo, const T& v);
		template<class T> bool _checkType			(const VarInfo& varInfo, const T& v);
		template<class T> void _setValue			(const VarInfo& varInfo, const T& v);

	protected:

	public:
		Vector<u8>				_cpuBuf;
		SPtr<RenderGpuBuffer>	_gpuBuffer;

		bool _isDirty = false;
	};

	struct TexParam : public ShaderResource<ShaderStageInfo::Texture>
	{
	public:
		using Base = ShaderResource<ShaderStageInfo::Texture>;

	public:
		void create(const Info* info, ShaderPass* pass);

		/* if set success, return true, means it is dirty */
		template<class TEX> bool setTexure(TEX* v);

								Texture*	getUpdatedTexture (RenderDevice* rdDev);
		template<class TEX_T>	TEX_T*		getUpdatedTextureT(RenderDevice* rdDev);
		
								Texture*	getUpdatedTexture (RenderDevice* rdDev)	const;
		template<class TEX_T>	TEX_T*		getUpdatedTextureT(RenderDevice* rdDev)	const;

	public:
		SPtr<Texture>		_tex;
	};

	struct SamplerParam : public ShaderResource<ShaderStageInfo::Sampler>
	{
	public:
		using Base = ShaderResource<ShaderStageInfo::Sampler>;

	public:
		void create(const Info* info, ShaderPass* pass);

		void setSamplerParam(const SamplerState& v);
		const SamplerState& samplerState() const;

	public:
		SamplerState _samplerState;
	};

	const SamplerParam* findSamplerParam(StrView name) const;

	const ShaderStageInfo& info() const;

	ConstBuffer&				constBufs(SizeType i);
	Span<ConstBuffer>			constBufs();
	Span<const ConstBuffer>		constBufs() const;

	TexParam&					texParams(SizeType i);
	Span<TexParam>				texParams();
	Span<const TexParam>		texParams() const;

	SamplerParam&				samplerParams(SizeType i);
	Span<SamplerParam>			samplerParams();
	Span<const SamplerParam>	samplerParams() const;

public:
	void _getAutoSetSamplerNameTo(TempString& out, StrView name) const;

protected:

protected:
	const ShaderStageInfo*							_info = nullptr;
	Vector<ConstBuffer,		s_kLocalConstBufSize>	_constBufs;
	Vector<TexParam,		s_kLocalTextureSize>	_texParams;
	Vector<SamplerParam,	s_kLocalTextureSize>	_samplerParams;

	bool _isDirty = false;
};

template<class T> inline
void 
ShaderResources::setParam(StrView name, const T& v)
{
	for (auto& e : constBufs())
	{
		e.setParam(name, v);
	}
}

template<class TEX> inline
void 
ShaderResources::setTexParam(StrView name, TEX* v, bool isAutoSetSampler)
{
	for (auto& e : texParams())
	{
		bool isSame = name.compare(e.info().name) == 0;
		if (!isSame)
			continue;
		_isDirty |= e.setTexure(v);
		if (isAutoSetSampler)
		{
			TempString temp;
			_getAutoSetSamplerNameTo(temp, name);
			setSamplerParam(temp, v->samplerState());
		}

	}
}

inline				ShaderResources::ConstBuffer&	ShaderResources::constBufs(SizeType i)		{ return _constBufs[i]; }
inline Span<		ShaderResources::ConstBuffer>	ShaderResources::constBufs()				{ return _constBufs; }
inline Span<const	ShaderResources::ConstBuffer>	ShaderResources::constBufs() const			{ return spanCast<const ConstBuffer>(_constBufs.span()); }

inline				ShaderResources::TexParam&		ShaderResources::texParams(SizeType i)		{ return _texParams[i]; }
inline Span<		ShaderResources::TexParam>		ShaderResources::texParams()				{ return _texParams; }
inline Span<const	ShaderResources::TexParam>		ShaderResources::texParams() const			{ return spanCast<const TexParam>(_texParams.span()); }

inline				ShaderResources::SamplerParam&	ShaderResources::samplerParams(SizeType i)	{ return _samplerParams[i]; }
inline Span<		ShaderResources::SamplerParam>	ShaderResources::samplerParams()			{ return _samplerParams; }
inline Span<const	ShaderResources::SamplerParam>	ShaderResources::samplerParams() const		{ return spanCast<const SamplerParam>(_samplerParams.span()); }

inline
void 
ShaderResources::_getAutoSetSamplerNameTo(TempString& out, StrView name) const
{
	out.clear();
	fmtTo(out, "{}{}{}", s_kAutoSamplerNamePrefix, name, s_kAutoSamplerNameSuffix);
}


#if 0
#pragma mark --- rdsShaderResources::ConstBuffer-Impl ---
#endif // 0
#if 1

template<class T> inline
void 
ShaderResources::ConstBuffer::_setParamCheckType(const VarInfo& varInfo, const T& v)
{
	throwIf(!_checkType(varInfo, v), "material set param failed, in valid type");
	_setValue(varInfo, v);
}

template<class T> inline
bool 
ShaderResources::ConstBuffer::_checkType(const VarInfo& varInfo, const T& v)
{
	return varInfo.dataType == RenderDataTypeUtil::get<T>();
}

void passTest(Vector<u8>& data, const ShaderResources::ConstBuffer::VarInfo& varInfo, const ShaderResources::ConstBuffer::Info* info, const void* value);

template<class T> inline
void 
ShaderResources::ConstBuffer::_setValue(const VarInfo& varInfo, const T& v)
{
	auto end = varInfo.offset + sizeof(v);
	throwIf(end > _cpuBuf.size() || !data(), "material set param failed, cpuBuffer overflow");

	_cpuBuf.resize(_info->size);
	auto* dst = reinCast<T*>(data() + varInfo.offset);
	
	passTest(_cpuBuf, varInfo, _info, &v);
	RDS_CALL_ONCE(RDS_DUMP_VAR(varInfo.offset, _cpuBuf.size(), end, sizeof(v), sCast<void*>(data()), sCast<void*>(dst)));

	//*dst = v;
	//memory_copy(dst, &v, 1);
	::memcpy(dst, &v, sizeof(T));

	RDS_WARN_ONCE("*dst = v success");

	_isDirty = true;
}

inline 			u8*	ShaderResources::ConstBuffer::data()		{ return _cpuBuf.data(); }
inline const	u8*	ShaderResources::ConstBuffer::data() const	{ return _cpuBuf.data(); }


#endif

#if 0
#pragma mark --- rdsShaderResources::TexParam-Impl ---
#endif // 0
#if 1

template<class TEX> inline
bool 
ShaderResources::TexParam::setTexure(TEX* v)
{
	if (!v) RDS_CORE_ASSERT(false, "texure == nullptr");
	bool isValid = info().dataType != RenderDataTypeUtil::get<TEX>();
	// bool isValid = info().dataType() != v->type();
	if (!isValid)
	{
		_tex.reset(nullptr);
		RDS_THROW("shader texture type not match");
	}

	bool isSame = _tex == v;
	if (!isSame)
	{
		_tex.reset(v);
	}

	bool isDirty = !isSame || !isValid;
	return isDirty;
}

template<class TEX_T> inline
TEX_T* 
ShaderResources::TexParam::getUpdatedTextureT(RenderDevice* rdDev)
{
	return sCast<TEX_T*>(getUpdatedTexture(rdDev));
}

						inline Texture*	ShaderResources::TexParam::getUpdatedTexture (RenderDevice* rdDev)	const { return constCast(this)->getUpdatedTexture(rdDev); }
template<class TEX_T>	inline TEX_T*	ShaderResources::TexParam::getUpdatedTextureT(RenderDevice* rdDev)	const { return constCast(this)->getUpdatedTextureT<TEX_T>(rdDev); }

#endif


#if 0
#pragma mark --- rdsShaderResources::SamplerParam-Impl ---
#endif // 0
#if 1


inline const SamplerState& ShaderResources::SamplerParam::samplerState() const { return _samplerState; }



#endif

#endif

}