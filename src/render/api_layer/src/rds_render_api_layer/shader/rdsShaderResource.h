#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsShaderInfo.h"

#include "rds_render_api_layer/buffer/rdsRenderMultiGpuBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rds_render_api_layer/texture/rdsTexture3D.h"
#include "rds_render_api_layer/texture/rdsTextureCube.h"
#include "rds_render_api_layer/texture/rdsTexture2DArray.h"

#define RDS_NO_BINDLESS 0

namespace rds
{

struct ShaderStage;

class Shader;
class ShaderPass;

struct	ShaderResources;
//using	MultiShaderResources = Vector<ShaderResources, RenderApiLayerTraits::s_kMaxFrameAheadCountHardLimit>;

struct SamplerParam;

/*
3 design: 

1. MultiShaderResources (Vector<ShaderResources, s_kMaxFrameAheadCountHardLimit> as data member at MaterialPass)
- but the all resources need to copy when rotateFrame

2. Vector<MultiShaderResource<T>, N> _xxParams on ShaderResources
- but need vkUpdateDescriptorSets when rotateFrame on ConstBuffer

3. hybrid - Vector<Vector<ConstBuffer, s_kLocalConstBufSize>, s_kMaxFrameAheadCountHardLimit> +  Vector<MultiShaderResource<T>, N> _xxParams on ShaderResources
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
struct ShaderResource : public RenderApiLayerCommon_Base
{
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
//template<class INFO> inline typename ShaderResource<INFO>::DataType			ShaderResource<INFO>::dataType	() const { return info().dataType; }
template<class INFO> inline u16												ShaderResource<INFO>::bindPoint	() const { return info().bindPoint; }
template<class INFO> inline u16												ShaderResource<INFO>::bindCount	() const { return info().bindCount; }

#endif

#if 0
#pragma mark --- rdsShaderResources-Decl ---
#endif // 0
#if 1

struct ShaderResources : public RenderApiLayerCommon_Base
{
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

	using ConstBufferT		= ConstBuffer;
	using TexParamT			= TexParam;
	using SamplerParamT		= SamplerParam;
	using BufferParamT		= BufferParam;
	using ImageParamT		= ImageParam;

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
	static constexpr SizeType s_kConstBufferLocalSize	= 1;
	static constexpr SizeType s_kTexParamLocalSize		= 4;
	static constexpr SizeType s_kBufferParamLocalSize	= 4;
	static constexpr SizeType s_kImageParamLocalSize	= 4;

	//static constexpr const char* s_kAutoSamplerNamePrefix = "_rds_";
	static constexpr const char* s_kAutoSamplerNameSuffix	= "_sampler";
	static constexpr const char* s_kAutoTextureStNameSuffix = "_ST_";

public:
	using ConstBuffers	= Vector<ConstBuffer,	s_kConstBufferLocalSize>;

	using TexParams		= Vector<TexParam,		s_kTexParamLocalSize>;
	using SamplerParams	= Vector<SamplerParam,	s_kTexParamLocalSize>;
	using BufferParams	= Vector<BufferParam,	s_kBufferParamLocalSize>;
	using ImageParams	= Vector<ImageParam,	s_kImageParamLocalSize>	;

public:
	static void getSamplerNameTo(TempString& out, StrView name);
	static void getTextureNameTo(TempString& out, StrView name);
	static void getTextureStNameTo(TempString& out, StrView name);

public:
	void create	(const ShaderStageInfo& info_, ShaderPass* pass, u32 frameIdx);
	void destroy();

public:
	template<class T>	bool setParam(			StrView name, const T&				v);
	template<class T>	bool setArray(			StrView name, const Span<T>&		v);
	template<class TEX>	bool setTexParam(		StrView name, TEX*					v);
						bool setSamplerParam(	StrView name, const SamplerState&	v);
						bool setBufferParam(	StrView name, RenderGpuBuffer*		v);
						bool setImageParam(		StrView name, Texture*				v, u32 mipLevel);

public:
	bool uploadToGpu(ShaderPass* pass);		// return true (isDirty) if upload is executed

	void clear();
	void copy(const ShaderResources& rsc);
	void resetTexBufImgDirty();
	
						void*	findParam( StrView name);
						void*	findParam( StrView name) const;
	template<class T>	T*		findParamT(StrView name);
	template<class T>	T*		findParamT(StrView name) const;

	TexParamT*				findTexParam(	 StrView name);
	SamplerParamT*			findSamplerParam(StrView name);
	BufferParamT*			findBufferParam( StrView name);
	ImageParamT*			findImageParam(	 StrView name);

	const TexParamT*		findTexParam(	 StrView name) const;
	const SamplerParamT*	findSamplerParam(StrView name) const;
	const BufferParamT*		findBufferParam( StrView name) const;
	const ImageParamT*		findImageParam(	 StrView name) const;

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
	bool isTexBufImgDirty() const;

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

		template<class T> 
		bool setArray(StrView name, const Span<T>& v)
		{
			const ShaderVariableInfo* var = info().findVariable(name);
			if (!var)
				return false;
			return _setArray(*var, v);
		}

		template<class T>	T*		findParamT(StrView name);
							void*	findParam( StrView name);

		bool uploadToGpu(); // return true (isDirty) if upload is executed

		void copy(const ConstBuffer& rhs)
		{
			_cpuBuf = rhs._cpuBuf;
		}

	public:
				u8* data();
		const	u8* data() const;

	protected:
		bool _setParam(const VarInfo& varInfo, const bool&			v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const i32&			v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const u32&			v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const f32&			v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Color4b&		v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Color4f&		v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Tuple2u&		v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Tuple3u&		v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Tuple4u&		v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Tuple2f&		v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Tuple3f&		v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Tuple4f&		v)	{ return _setParamCheckType(varInfo, v); }
		bool _setParam(const VarInfo& varInfo, const Mat4f&			v)	{ return _setParamCheckType(varInfo, v); }

		bool _setArray(const VarInfo& varInfo, const Span<float>&	v)	{ return _setArrayCheckType(varInfo, v); }
		bool _setArray(const VarInfo& varInfo, const Span<Vec4f>&	v)	{ return _setArrayCheckType(varInfo, v); }
		bool _setArray(const VarInfo& varInfo, const Span<Mat4f>&	v)	{ return _setArrayCheckType(varInfo, v); }

		template<class T> bool _setParamCheckType(	const VarInfo& varInfo, const T& v);
		template<class T> bool _checkType(			const VarInfo& varInfo);
		template<class T> bool _setValue(			const VarInfo& varInfo, const T& v);
		template<class T> T*   _getValueT(			const VarInfo& varInfo);

		template<class T> bool _setArrayCheckType(	const VarInfo& varInfo, const Span<T>& v);
		template<class T> bool _setArrayValue(		const VarInfo& varInfo, const Span<T>& v);
		
		void* _getValue(const VarInfo& varInfo);

	public:
		Vector<u8>					_cpuBuf;
		SPtr<RenderMultiGpuBuffer>	_gpuBuffer;		// ignore for non-bindless side, just keep it simple

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

		bool operator==(const ImageParam& v) const { return _image == v._image && _mipLevel == v._mipLevel; }
		bool operator!=(const ImageParam& v) const { return !operator==(v); }

	public:
		bool setImageParam(Texture* v, u32 mipLevel);

		void copy(const ImageParam& rhs)
		{
			if (this != &rhs)
			{
				_image		= rhs._image;
				_mipLevel	= rhs._mipLevel;
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
		SPtr<Texture>	_image;
		u32				_mipLevel = 0;
	};

	#endif

protected:
	const ShaderStageInfo*							_info = nullptr;

	ConstBuffers	_constBufs;

	TexParams		_texParams;
	SamplerParams	_samplerParams;
	BufferParams	_bufferParams;
	ImageParams		_imageParams;
	bool _isTexBufImgDirty : 1;
};

template<class T> inline
bool 
ShaderResources::setParam(StrView name, const T& v)
{
	bool isDirty = false;
	for (auto& e : constBufs())
	{
		auto& rsc = e/*.shaderResources()*/;
		isDirty |= rsc.setParam(name, v);
	}
	return isDirty;
}

template<class T> bool 
ShaderResources::setArray(StrView name, const Span<T>& v)
{
	bool isDirty = false;
	for (auto& e : constBufs())
	{
		auto& rsc = e/*.shaderResources()*/;
		isDirty |= rsc.setArray(name, v);
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
		auto& rsc = *it/*->shaderResources()*/;
		isDirty = rsc.setTexure(v);
	}
	_isTexBufImgDirty |= isDirty;
	return isDirty;
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

inline bool									ShaderResources::isTexBufImgDirty() const	{ return _isTexBufImgDirty; }

inline
void 
ShaderResources::getSamplerNameTo(TempString& out, StrView name)
{
	out.clear();
	fmtTo(out, "{}{}", name, s_kAutoSamplerNameSuffix);
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
	throwIf(!_checkType<T>(varInfo), "material set param [{} {}] failed, set wrong type as {}", varInfo.dataType, varInfo.name, RenderDataTypeUtil::get<T>());
	return _setValue(varInfo, v);
}

template<class T> inline
bool 
ShaderResources::ConstBuffer::_setArrayCheckType(const VarInfo& varInfo, const Span<T>& v)
{
	throwIf(!_checkType<T>(varInfo), "material set param [{} {}] failed, set wrong type as {}", varInfo.dataType, varInfo.name, RenderDataTypeUtil::get<T>());
	return _setArrayValue(varInfo, v);
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
	auto end = varInfo.offset + sizeof(T);
	throwIf(end > _cpuBuf.size() || !data(), "material set param failed, cpuBuffer overflow");

	auto* dst = sCast<T*>(_getValue(varInfo));
	RDS_TODO("impl operator==() for all Data");
	//if (*dst == v) return false;

	*dst = v;

	_isDirty = true;
	return _isDirty;
}

template<class T> inline
bool 
ShaderResources::ConstBuffer::_setArrayValue(const VarInfo& varInfo, const Span<T>& v)
{
	auto commitSize = sCast<u32>(sizeof(T) * v.size());
	auto end		= varInfo.offset + commitSize;
	throwIf(end > _cpuBuf.size() || !data() || commitSize > varInfo.size, "material set param failed, cpuBuffer overflow");

	auto* dst = sCast<T*>(_getValue(varInfo));
	RDS_TODO("impl operator==() for all Data");
	//if (*dst == v) return false;

	memory_copy(dst, v.data(), v.size());

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
#pragma mark --- rdsMultiShaderResource-Decl ---
#endif // 0
#if 1

class MultiShaderResources : public NC_RenderApiLayerCommon_Base
{
public:
	#if RDS_SHADER_USE_BINDLESS
	static constexpr int s_kMaxShaderResourcesCount = s_kFrameAheadCount; 
	#else
	// s_kMaxFrameAheadCountHardLimit + 1, ensure no race-condition when copy in EngineThread
	// since RenderThread may not be read-only for ShaderResources (currently is read-only, but if later change design may gg)
	static constexpr int s_kMaxShaderResourcesCount = s_kFrameAheadCount + 1; 
	#endif // RDS_SHADER_USE_BINDLESS
	using MultiT		= Vector<ShaderResources, s_kMaxShaderResourcesCount>;
	using ConstBuffer	= ShaderResources::ConstBuffer;

public:
	MultiShaderResources();
	~MultiShaderResources();

	void create(const ShaderStageInfo& info_, ShaderPass* pass);
	void destroy();

public:
	RDS_NODISCARD int uploadToGpu();	// return uploaded ShaderResourceIndex

	template<class T>	bool setParam(			StrView name, const T&				v);
	template<class T>	bool setArray(			StrView name, const Span<T>&		v);

public:
	template<class TEX>	bool setTexParam(		StrView name, TEX*					v);
	bool setSamplerParam(	StrView name, u32 samplerIndex, const SamplerState& v);
	bool setBufferParam(	StrView name, RenderGpuBuffer*		v);
	bool setImageParam(		StrView name, Texture*				v, u32 mipLevel);

#if !RDS_SHADER_USE_BINDLESS
public:
	ShaderResources&	shaderResources();
	ShaderResources&	shaderResources(int i);
private:
#endif // !RDS_SHADER_USE_BINDLESS

protected:
	ShaderPass*					_shaderPass				= nullptr;
	
	#if RDS_SHADER_USE_BINDLESS
	ConstBuffer _constBuf;
	#else
	MultiT	_shaderRscs;
	int		_i_shaderRscs = 0;
	#endif // RDS_SHADER_USE_BINDLESS
};

template<class T> inline
bool 
MultiShaderResources::setParam(StrView name, const T& v)
{
	#if RDS_SHADER_USE_BINDLESS
	return _constBuf.setParam(name, v);
	#else
	return shaderResources().setParam(name, v);
	#endif // RDS_SHADER_USE_BINDLESS		
}

template<class T> inline
bool 
MultiShaderResources::setArray(StrView name, const Span<T>& v)
{
	#if RDS_SHADER_USE_BINDLESS
	return _constBuf.setArray(name, v);
	#else
	return shaderResources().setArray(name, v);
	#endif // RDS_SHADER_USE_BINDLESS	
}

template<class TEX>	inline
bool 
MultiShaderResources::setTexParam(StrView name, TEX* v)
{
	#if RDS_SHADER_USE_BINDLESS
	auto bindlessIdx = v->bindlessHandle().getResourceIndex();
	return setParam(name, bindlessIdx);
	#else
	return shaderResources().setTexParam(name, v);
	#endif
}

inline
bool 
MultiShaderResources::setSamplerParam(StrView name, u32 samplerIndex, const SamplerState& v)
{
	bool isDirty = setParam(name, samplerIndex);
	#if !RDS_SHADER_USE_BINDLESS
	shaderResources().setSamplerParam(name, v);
	#endif
	return isDirty;
}

inline
bool 
MultiShaderResources::setBufferParam(StrView name, RenderGpuBuffer* v)
{
	#if RDS_SHADER_USE_BINDLESS
	auto bindlessIdx = v->bindlessHandle().getResourceIndex();
	return setParam(name, bindlessIdx);
	#else
	return shaderResources().setBufferParam(name, v);
	#endif
}

inline
bool 
MultiShaderResources::setImageParam(StrView name, Texture* v, u32 mipLevel)
{
	RDS_CORE_ASSERT(v->hasMipmapView(),					"{} cannot use as image, no TextureUsageFlags::UnorderedAccess usageFlags", v->DebugLabel_getName());
	RDS_CORE_ASSERT(mipLevel < v->mipmapViewCount(),	"mipLevel out of boundary");

	#if RDS_SHADER_USE_BINDLESS
	auto bindlessIdx = v->uavBindlessHandle().getResourceIndex(mipLevel);
	return setParam(name, bindlessIdx);
	#else
	return shaderResources().setImageParam(name, v, mipLevel);
	#endif
}

inline ShaderResources&		MultiShaderResources::shaderResources()					{ return _shaderRscs[s_bufferIndex(_i_shaderRscs, s_kMaxShaderResourcesCount)]; }
inline ShaderResources&		MultiShaderResources::shaderResources(int i)			{ return _shaderRscs[i]; }

#endif

}