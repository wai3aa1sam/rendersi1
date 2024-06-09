#pragma once

#include "rdsShaderInfo.h"

#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rds_render_api_layer/texture/rdsTexture3D.h"
#include "rds_render_api_layer/texture/rdsTextureCube.h"

#define RDS_NO_BINDLESS 0

namespace rds
{

struct ShaderStage;

class Shader;
class ShaderPass;

struct	ShaderResources;
//using	FramedShaderResources = Vector<ShaderResources, RenderApiLayerTraits::s_kFrameInFlightCount>;

struct SamplerParam;

/*
3 design: 

1. FramedShaderResources (Vector<ShaderResources, s_kFrameInFlightCount> as data member at MaterialPass)
- but the all resources need to copy when rotateFrame

2. Vector<FramedShaderResource<T>, N> _xxParams on ShaderResources
- but need vkUpdateDescriptorSets when rotateFrame on ConstBuffer

3. hybrid - Vector<Vector<ConstBuffer, s_kLocalConstBufSize>, s_kFrameInFlightCount> +  Vector<FramedShaderResource<T>, N> _xxParams on ShaderResources
- no need copy for other Params
- only need to copy cpu data after rotateFrame, only need to uploadToGpu() and copy cpu data, as each descr set bind on each ConstBuffer

2024.04.08
when making inspector window, I realized that draw command should save the framed index for material, separate framed index is hard to handle
, material resource should be also copy on write (like all other framed resource)

*/

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
	ShaderResource() = default;

	void create(const Info* info);
	void destroy();

	bool find(StrView name) const;

	bool isValid() const;

public:
	const Info&			info		() const;

	const String&		name		() const;
	DataType			dataType	() const;
	u16					bindPoint	() const;
	u16					bindCount	() const;

public:
	template<class FRAMED_SHADER_RSC>
	void _internal_onPostRotateFrame(FRAMED_SHADER_RSC* rsc)
	{

	}

public:
	const Info*	_info = nullptr;
};

template<class INFO> inline 
void 
ShaderResource<INFO>::create(const Info* info)
{
	_info = info;
}

template<class INFO> inline 
void 
ShaderResource<INFO>::destroy()
{
	
}

template<class INFO> inline 
bool 
ShaderResource<INFO>::find(StrView name) const
{
	bool hasFound = name.compare(info().name) == 0;
	return hasFound;

}

template<class INFO> inline bool ShaderResource<INFO>::isValid() const { return _info; }

template<class INFO> inline const typename ShaderResource<INFO>::Info&		ShaderResource<INFO>::info		() const { return *_info; }
template<class INFO> inline const String&									ShaderResource<INFO>::name		() const { return info().name; }
template<class INFO> inline typename ShaderResource<INFO>::DataType			ShaderResource<INFO>::dataType	() const { return info().dataType; }
template<class INFO> inline u16												ShaderResource<INFO>::bindPoint	() const { return info().bindPoint; }
template<class INFO> inline u16												ShaderResource<INFO>::bindCount	() const { return info().bindCount; }

#endif

/*
	useless 
*/
#if 0
#pragma mark --- rdsFramedShaderResource-Decl ---
#endif // 0
#if 0

template<class SHADER_RSC >
struct FramedShaderResource : public ShaderResource<typename SHADER_RSC::Info>
{
	//friend struct SHADER_RSC;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Base				= ShaderResource<typename SHADER_RSC::Info>;
	using Info				= typename SHADER_RSC::Info;
	using ShaderResource	= SHADER_RSC;
	using ShaderResources	= Vector<SHADER_RSC, s_kFrameInFlightCount>;

public:
	void create(const Info* info, ShaderPass* pass)
	{
		destroy();

		Base::create(info);

		_shaderRscs.reserve(s_kFrameInFlightCount);
		auto& dst = _shaderRscs.emplace_back();
		dst.create(info, nullptr);

	}

	void destroy()
	{
		for (auto& rsc : _shaderRscs)
		{
			rsc.destroy();
			Base::destroy();
		}
	}

	void roatateFrame()
	{
		_iFrame = (_iFrame + 1) % s_kFrameInFlightCount;
		if (_iFrame + 1 > _shaderRscs.size())
		{
			auto& dst = _shaderRscs.emplace_back();
			dst.create(&Base::info(), nullptr);
		}
		shaderResource()._internal_onPostRotateFrame(this);
	}

public:
			ShaderResource& shaderResource()				{ return _shaderRscs[iFrame()]; }
	const	ShaderResource& shaderResource() const			{ return _shaderRscs[iFrame()]; }

			ShaderResource& previousShaderResource()		{ auto prevFrameIdx = sCast<int>(iFrame()) - 1; return _shaderRscs[prevFrameIdx]; }
	const	ShaderResource& previousShaderResource() const	{ auto prevFrameIdx = sCast<int>(iFrame()) - 1; return _shaderRscs[prevFrameIdx]; }

	u32 iFrame() const { return _iFrame; }

protected:
	ShaderResources _shaderRscs;
	u32				_iFrame = 0;
};

#endif

#if 0
#pragma mark --- rdsShaderResources-Decl ---
#endif // 0
#if 1

struct ShaderResources
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:

	#if 0
	#pragma mark --- rdsShaderResource-Decl ---
	#endif // 0
	#if 1

	struct ConstBuffer;
	struct TexParam;
	struct SamplerParam;
	struct BufferParam;
	struct ImageParam;

	#if 0
	using ConstBufferT		= ConstBuffer;
	using TexParamT			= FramedShaderResource<TexParam>;
	using SamplerParamT		= FramedShaderResource<SamplerParam>;
	using BufferParamT		= FramedShaderResource<BufferParam>;
	using ImageParamT		= FramedShaderResource<ImageParam>;
	#else
	using ConstBufferT		= ConstBuffer;
	using TexParamT			= TexParam;
	using SamplerParamT		= SamplerParam;
	using BufferParamT		= BufferParam;
	using ImageParamT		= ImageParam;
	#endif // 0

	using ConstBuffersView		= Span<ConstBufferT>;
	using CConstBuffersView		= Span<const ConstBufferT>;

	using TexParamsView			= Span<TexParamT>;
	using CTexParamsView		= Span<const TexParamT>;

	using SamplerParamsView		= Span<SamplerParamT>;
	using CSamplerParamsView	= Span<const SamplerParamT>;

	using BufferParamsView		= Span<BufferParamT>;
	using CBufferParamsView		= Span<const BufferParamT>;

	using ImageParamsView		= Span<ImageParamT>;
	using CImageParamsView		= Span<const ImageParamT>;

	#endif

public:
	static constexpr SizeType s_kLocalConstBufSize	= 2;
	static constexpr SizeType s_kLocalTextureSize	= 4;
	static constexpr SizeType s_kLocalBufferSize	= 4;
	static constexpr SizeType s_kLocalImageSize		= 4;

	static constexpr const char* s_kAutoSamplerNamePrefix = "_rds_";
	static constexpr const char* s_kAutoSamplerNameSuffix = "_sampler";

	static constexpr const char* s_kAutoTextureNamePrefix = "_rds_";
	static constexpr const char* s_kAutoTextureNameSuffix = "_texture";

	static constexpr const char* s_kAutoTextureStNameSuffix = "_ST_";

public:
	static void getSamplerNameTo(TempString& out, StrView name);
	static void getTextureNameTo(TempString& out, StrView name);
	static void getTextureStNameTo(TempString& out, StrView name);

public:
	template<class T>	bool setParam		(StrView name, const T& v);
	template<class TEX>	bool setTexParam	(StrView name, TEX* v);
						bool setSamplerParam(StrView name, const SamplerState&	v);
						bool setBufferParam	(StrView name, RenderGpuBuffer*		v);
						bool setImageParam	(StrView name, Texture*				v);

	void create	(const ShaderStageInfo& info_, ShaderPass* pass, u32 frameIdx);
	void destroy();

	void uploadToGpu(ShaderPass* pass);

	void clear();
	void copy(const ShaderResources& rsc);
	
						void*	findParam( StrView name);
						void*	findParam( StrView name) const;
	template<class T>	T*		findParamT(StrView name);
	template<class T>	T*		findParamT(StrView name) const;

	TexParamT*				findTexParam	(StrView name);
	SamplerParamT*			findSamplerParam(StrView name);
	BufferParamT*			findBufferParam	(StrView name);
	ImageParamT*			findImageParam	(StrView name);

	const TexParamT*		findTexParam	(StrView name) const;
	const SamplerParamT*	findSamplerParam(StrView name) const;
	const BufferParamT*		findBufferParam	(StrView name) const;
	const ImageParamT*		findImageParam	(StrView name) const;

public:
	const ShaderStageInfo&	info()		const;

	ConstBuffer&				constBufs(SizeType i);
	ConstBuffersView			constBufs();
	CConstBuffersView			constBufs() const;

	TexParam&					texParams(SizeType i);
	TexParamsView				texParams();
	CTexParamsView				texParams() const;

	SamplerParam&				samplerParams(SizeType i);
	SamplerParamsView			samplerParams();
	CSamplerParamsView			samplerParams() const;

	BufferParam&				bufferParams(SizeType i);
	BufferParamsView			bufferParams();
	CBufferParamsView			bufferParams() const;

	ImageParam&					imageParams(SizeType i);
	ImageParamsView				imageParams();
	CImageParamsView			imageParams() const;

public:

	#if 0
	#pragma mark --- rdsShaderResource-Decl ---
	#endif // 0
	#if 1

	struct ConstBuffer : public ShaderResource<ShaderStageInfo::ConstBuffer>
	{
	public:
		using Base		= ShaderResource<ShaderStageInfo::ConstBuffer>;
		using Info		= ShaderStageInfo::ConstBuffer;
		using VarInfo	= ShaderVariableInfo;

	public:
		ConstBuffer();
		~ConstBuffer();

		//ConstBuffer(	const ConstBuffer& rhs) { copy(rhs); }
		//void operator=(	const ConstBuffer& rhs) { copy(rhs); }

		void create	(const Info* info, ShaderPass* pass, u32 idx);
		void destroy();

		template<class T> 
		bool setParam(StrView name, const T& v)
		{
			const ShaderVariableInfo* var = info().findVariable(name);
			if (!var)
				return false;
			return _setParam(*var, v);
		}

		template<class T>	T*		findParamT(StrView name);
							void*	findParam( StrView name);

		void uploadToGpu();

		void copy(const ConstBuffer& rhs)
		{
			_cpuBuf = rhs._cpuBuf;
		}

	public:
				u8* data();
		const	u8* data() const;

	public:
		//template<class FRAMED_SHADER_RSC>
		//void _internal_onPostRotateFrame(FRAMED_SHADER_RSC* rsc)
		//{
		//	//FramedShaderResource<ShaderResources::ConstBuffer>* rsc;
		//	_cpuBuf = rsc->previousShaderResource()._cpuBuf;
		//}

	protected:
		bool _setParam(const VarInfo& varInfo, const bool&		v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const i32&		v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const u32&		v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const f32&		v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Color4b&	v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Color4f&	v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Tuple2u&	v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Tuple3u&	v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Tuple4u&	v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Tuple2f&	v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Tuple3f&	v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Tuple4f&	v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Mat4f&		v)	{ return _setParamCheckType(varInfo, v); }

		template<class T> bool _setParamCheckType(	const VarInfo& varInfo, const T& v);
		template<class T> bool _checkType(			const VarInfo& varInfo);
		template<class T> bool _setValue(			const VarInfo& varInfo, const T& v);
		template<class T> T*   _getValueT(			const VarInfo& varInfo);

		void* _getValue(const VarInfo& varInfo);

	protected:

	public:
		Vector<u8>				_cpuBuf;
		SPtr<RenderGpuBuffer>	_gpuBuffer;

		bool _isDirty = false;
	};

	struct TexParam : public ShaderResource<ShaderStageInfo::Texture>
	{
	public:
		using Base		= ShaderResource<ShaderStageInfo::Texture>;
		using Resource	= Texture;

	public:
		TexParam() = default;
		~TexParam() { destroy(); }

		//TexParam(		const TexParam& rhs) { copy(rhs); }
		//void operator=(	const TexParam& rhs) { copy(rhs); }

		void create(const Info* info, ShaderPass* pass);

		/* if set success, return true, means it is dirty */
		template<class TEX> bool setTexure(TEX* v);

		void copy(const TexParam& rhs)
		{
			if (_tex.ptr() != rhs._tex.ptr())
			{
				_tex = rhs._tex;
			}
		}

	public:
								Texture*	getUpdatedTexture (RenderDevice* rdDev);
		template<class TEX_T>	TEX_T*		getUpdatedTextureT(RenderDevice* rdDev);
		
								Texture*	getUpdatedTexture (RenderDevice* rdDev)	const;
		template<class TEX_T>	TEX_T*		getUpdatedTextureT(RenderDevice* rdDev)	const;

	public:
				Resource* texture()			{ return resource(); }
		const	Resource* texture() const	{ return resource(); }

				Resource* resource()		{ return _tex; }
		const	Resource* resource() const	{ return _tex; }

	public:
		SPtr<Texture>		_tex;
	};

	struct SamplerParam : public ShaderResource<ShaderStageInfo::Sampler>
	{
	public:
		using Base		= ShaderResource<ShaderStageInfo::Sampler>;
		using Resource	= SamplerState;

	public:
		SamplerParam() = default;
		~SamplerParam() { destroy(); }

		//SamplerParam(	const SamplerParam& rhs) { copy(rhs); }
		//void operator=(	const SamplerParam& rhs) { copy(rhs); }

		void create(const Info* info, ShaderPass* pass);

		bool setSamplerParam(const SamplerState& v);

		void copy(const SamplerParam& rhs)
		{
			_samplerState = rhs._samplerState;
		}
		
	public:
				Resource& samplerState()		{ return resource(); }
		const	Resource& samplerState() const	{ return resource(); }

				Resource& resource()			{ return _samplerState; }
		const	Resource& resource() const		{ return _samplerState; }

	public:
		SamplerState _samplerState;
	};

	struct BufferParam : public ShaderResource<ShaderStageInfo::StorageBuffer>
	{
	public:
		using Base		= ShaderResource<ShaderStageInfo::StorageBuffer>;
		using Resource	= RenderGpuBuffer;

	public:
		BufferParam() = default;
		~BufferParam() { destroy(); }

		//BufferParam(	const BufferParam& rhs) { copy(rhs); }
		//void operator=(	const BufferParam& rhs) { copy(rhs); }

		void create(const Info* info, ShaderPass* pass);

		bool setBufferParam(RenderGpuBuffer* v);

		void copy(const BufferParam& rhs)
		{
			if (_buffer.ptr() != rhs._buffer.ptr())
			{
				_buffer = rhs._buffer;
			}
		}

	public:
				Resource* buffer()			{ return resource(); }
		const	Resource* buffer() const	{ return resource(); }

				Resource* resource()		{ return _buffer; }
		const	Resource* resource() const	{ return _buffer; }

	public:
		SPtr<RenderGpuBuffer> _buffer;
	};

	struct ImageParam : public ShaderResource<ShaderStageInfo::StorageImage>
	{
	public:
		using Base		= ShaderResource<ShaderStageInfo::StorageImage>;
		using Resource	= Texture;

	public:
		ImageParam() = default;
		~ImageParam() { destroy(); }

		//ImageParam(		const ImageParam& rhs) { copy(rhs); }
		//void operator=(	const ImageParam& rhs) { copy(rhs); }

		void create(const Info* info, ShaderPass* pass);

		bool setImageParam(Texture* v);

		void copy(const ImageParam& rhs)
		{
			if (_image.ptr() != rhs._image.ptr())
			{
				_image = rhs._image;
			}
		}
	
	public:
				Resource* buffer()			{ return resource(); }
		const	Resource* buffer() const	{ return resource(); }

				Resource* resource()		{ return _image; }
		const	Resource* resource() const	{ return _image; }


	public:
		Texture* image();

	public:
		SPtr<Texture> _image;
	};

	#endif

protected:
	const ShaderStageInfo*							_info = nullptr;

	/*Vector<ConstBuffer,	s_kLocalConstBufSize>	_constBufs;
	Vector<TexParam,		s_kLocalTextureSize>	_texParams;
	Vector<SamplerParam,	s_kLocalTextureSize>	_samplerParams;
	Vector<BufferParam,		s_kLocalBufferSize>		_bufferParams;*/

	#if 0
	u32	_iFrame	= 0;
	Vector<Vector<ConstBuffer, s_kLocalConstBufSize>, s_kFrameInFlightCount> _framedConstBufs;

	// each has framed then
	//Vector<FramedShaderResource<ConstBuffer>	, s_kLocalConstBufSize>	_constBufs;
	Vector<FramedShaderResource<TexParam>		, s_kLocalTextureSize>	_texParams;
	Vector<FramedShaderResource<SamplerParam>	, s_kLocalTextureSize>	_samplerParams;
	Vector<FramedShaderResource<BufferParam>	, s_kLocalBufferSize>	_bufferParams;
	Vector<FramedShaderResource<ImageParam>		, s_kLocalImageSize>	_imageParams;
	#endif // 0

	Vector<ConstBuffer,		s_kLocalConstBufSize>	_constBufs;
	Vector<TexParam,		s_kLocalTextureSize>	_texParams;
	Vector<SamplerParam,	s_kLocalTextureSize>	_samplerParams;
	Vector<BufferParam,		s_kLocalBufferSize>		_bufferParams;
	Vector<ImageParam,		s_kLocalImageSize>		_imageParams;
};

template<class T> inline
bool 
ShaderResources::setParam(StrView name, const T& v)
{
	bool isDirty = false;
	for (auto& e : constBufs())
	{
		auto& rsc = e/*.shaderResource()*/;
		isDirty |= rsc.setParam(name, v);
	}
	return isDirty;
}

template<class TEX> inline
bool 
ShaderResources::setTexParam(StrView name, TEX* v)
{
	auto it			= findTexParam(name);
	bool isDirty	= false;
	if (it)
	{
		auto& rsc = *it/*->shaderResource()*/;
		isDirty = rsc.setTexure(v);
	}
	return false;
}

inline ShaderResources::ConstBuffer&		ShaderResources::constBufs(SizeType i)		{ return _constBufs[i]; }
inline ShaderResources::ConstBuffersView	ShaderResources::constBufs()				{ return _constBufs; }
inline ShaderResources::CConstBuffersView	ShaderResources::constBufs() const			{ return spanCast<const ConstBufferT>(_constBufs.span()); }

inline ShaderResources::TexParam&			ShaderResources::texParams(SizeType i)		{ return _texParams[i]; }
inline ShaderResources::TexParamsView		ShaderResources::texParams()				{ return _texParams; }
inline ShaderResources::CTexParamsView		ShaderResources::texParams() const			{ return spanCast<const TexParamT>(_texParams.span()); }

inline ShaderResources::SamplerParam&		ShaderResources::samplerParams(SizeType i)	{ return _samplerParams[i]; }
inline ShaderResources::SamplerParamsView	ShaderResources::samplerParams()			{ return _samplerParams; }
inline ShaderResources::CSamplerParamsView	ShaderResources::samplerParams() const		{ return spanCast<const SamplerParamT>(_samplerParams.span()); }

inline ShaderResources::BufferParam&		ShaderResources::bufferParams(SizeType i)	{ return _bufferParams[i]; }
inline ShaderResources::BufferParamsView	ShaderResources::bufferParams()				{ return _bufferParams; }
inline ShaderResources::CBufferParamsView	ShaderResources::bufferParams() const		{ return spanCast<const BufferParamT>(_bufferParams.span()); }

inline ShaderResources::ImageParam&			ShaderResources::imageParams(SizeType i)	{ return _imageParams[i]; }
inline ShaderResources::ImageParamsView		ShaderResources::imageParams()				{ return _imageParams; }
inline ShaderResources::CImageParamsView	ShaderResources::imageParams() const		{ return spanCast<const ImageParamT>(_imageParams.span()); }

inline
void 
ShaderResources::getSamplerNameTo(TempString& out, StrView name)
{
	out.clear();
	fmtTo(out, "{}{}{}", s_kAutoSamplerNamePrefix, name, s_kAutoSamplerNameSuffix);
}

inline
void 
ShaderResources::getTextureNameTo(TempString& out, StrView name)
{
	out.clear();
	//fmtTo(out, "{}{}{}", s_kAutoTextureNamePrefix, name, s_kAutoTextureNameSuffix);		// hlsl side just use its name only
	out = name;
}

inline
void 
ShaderResources::getTextureStNameTo(TempString& out, StrView name)
{
	out.clear();
	fmtTo(out, "{}{}", name, s_kAutoTextureStNameSuffix);
}

template<class T> inline 
T* 
ShaderResources::findParamT(StrView name)
{
	return reinCast<T*>(findParam(name));
}

template<class T> inline 
T* 
ShaderResources::findParamT(StrView name) const
{
	return constCast(*this).findParam<T>(name);
}


#if 0
#pragma mark --- rdsShaderResources::ConstBuffer-Impl ---
#endif // 0
#if 1

template<class T> inline
T* 
ShaderResources::ConstBuffer::findParamT(StrView name)
{
	const ShaderVariableInfo* var = info().findVariable(name);
	if (!var)
		return nullptr;
	throwIf(!_checkType<T>(*var), "material find param failed, in valid type");
	return _getValueT<T>(*var);
}

inline
void* 
ShaderResources::ConstBuffer::findParam(StrView name)
{
	const ShaderVariableInfo* var = info().findVariable(name);
	if (!var)
		return nullptr;
	return _getValue(*var);
}

template<class T> inline
bool 
ShaderResources::ConstBuffer::_setParamCheckType(const VarInfo& varInfo, const T& v)
{
	throwIf(!_checkType<T>(varInfo), "material set param failed, in valid type {}", RenderDataTypeUtil::get<T>());
	return _setValue(varInfo, v);
}

template<class T> inline
bool 
ShaderResources::ConstBuffer::_checkType(const VarInfo& varInfo)
{
	return varInfo.dataType == RenderDataTypeUtil::get<T>();
}

void passTest(Vector<u8>& data, const ShaderResources::ConstBuffer::VarInfo& varInfo, const ShaderResources::ConstBuffer::Info* info, const void* value);

template<class T> inline
bool 
ShaderResources::ConstBuffer::_setValue(const VarInfo& varInfo, const T& v)
{
	auto end = varInfo.offset + sizeof(v);
	throwIf(end > _cpuBuf.size() || !data(), "material set param failed, cpuBuffer overflow");

	auto* dst = sCast<T*>(_getValue(varInfo));
	
	//passTest(_cpuBuf, varInfo, _info, &v);
	//RDS_CALL_ONCE(RDS_DUMP_VAR(varInfo.offset, _cpuBuf.size(), end, sizeof(v), sCast<void*>(data()), sCast<void*>(dst)));

	*dst = v;
	//memory_copy(dst, &v, 1);
	//auto sizeofT = sizeof(T); RDS_UNUSED(sizeofT);
	//::memcpy(dst, &v, sizeof(T));

	//RDS_WARN_ONCE("*dst = v success");

	_isDirty = true;
	return _isDirty;
}

template<class T> inline
T*   
ShaderResources::ConstBuffer::_getValueT(const VarInfo& varInfo)
{
	auto* dst = reinCast<T*>(_getValue(varInfo));
	return dst;
}

inline
void* 
ShaderResources::ConstBuffer::_getValue(const VarInfo& varInfo)
{
	auto* dst = data() + varInfo.offset;
	return dst;
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
	bool isValid = info().dataType == RenderDataTypeUtil::get<TEX>();
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


#endif

#if 0
#pragma mark --- rdsShaderResources::BufferParam-Impl ---
#endif // 0
#if 1

//inline RenderGpuBuffer* ShaderResources::BufferParam::buffer()  { return _buffer; }

#endif

#if 0
#pragma mark --- rdsShaderResources::ImageParam-Impl ---
#endif // 0
#if 1

inline Texture* ShaderResources::ImageParam::image()  { return _image; }

#endif

#endif

#if 0
#pragma mark --- rdsFramedShaderResource-Decl ---
#endif // 0
#if 1

class FramedShaderResources : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	FramedShaderResources();
	~FramedShaderResources();

	void create(const ShaderStageInfo& info_, ShaderPass* pass);
	void destroy();

	template<class T>	bool setParam		(StrView name, const T& v);
	template<class TEX>	bool setTexParam	(StrView name, TEX* v);
						bool setSamplerParam(StrView name, const SamplerState&	v);
						bool setBufferParam	(StrView name, RenderGpuBuffer*		v);
						bool setImageParam	(StrView name, Texture*				v);
	
	void uploadToGpu(ShaderPass* pass);

public:
	ShaderResources&	shaderResource();
	ShaderResources&	shaderResource(u32 iFrame);
	u32					iFrame() const;

public:
	bool _isRotated = false;

protected:
	void rotate();

protected:
	u32							_iFrame = 0;
	FramedT<ShaderResources>	_shaderRscs;
};

template<class T> inline
bool 
FramedShaderResources::setParam(StrView name, const T& v)
{
	rotate();
	return shaderResource().setParam(name, v);
}

template<class TEX>	inline
bool 
FramedShaderResources::setTexParam(StrView name, TEX* v)
{
	rotate();
	return shaderResource().setTexParam(name, v);
}

inline
bool 
FramedShaderResources::setSamplerParam(StrView name, const SamplerState& v)
{
	rotate();
	return shaderResource().setSamplerParam(name, v);
}

inline
bool 
FramedShaderResources::setBufferParam(StrView name, RenderGpuBuffer* v)
{
	rotate();
	return shaderResource().setBufferParam(name, v);
}

inline
bool 
FramedShaderResources::setImageParam(StrView name, Texture* v)
{
	rotate();
	return shaderResource().setImageParam(name, v);
}

inline ShaderResources& FramedShaderResources::shaderResource()				{ return _shaderRscs[iFrame()]; }
inline ShaderResources& FramedShaderResources::shaderResource(u32 iFrame)	{ return _shaderRscs[iFrame]; }

inline u32				FramedShaderResources::iFrame() const	{ return _iFrame; }


#endif

}