#pragma once

#include "rdsShaderInfo.h"

#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rds_render_api_layer/texture/rdsTextureCube.h"

#define RDS_NO_BINDLESS 0

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

#if 0
#pragma mark --- rdsFramedShaderResource-Decl ---
#endif // 0
#if 1

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

	using ConstBufferT		= ConstBuffer;
	using TexParamT			= FramedShaderResource<TexParam>;
	using SamplerParamT		= FramedShaderResource<SamplerParam>;
	using BufferParamT		= FramedShaderResource<BufferParam>;
	using ImageParamT		= FramedShaderResource<ImageParam>;

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

public:
	static void getSamplerNameTo(TempString& out, StrView name);

public:
	template<class T>	void setParam		(StrView name, const T& v);
	template<class TEX>	void setTexParam	(StrView name, TEX* v);
						void setSamplerParam(StrView name, const SamplerState&	v);
						void setBufferParam	(StrView name, RenderGpuBuffer*		v);
						void setImageParam	(StrView name, Texture*				v);

	void create	(const ShaderStageInfo& info_, ShaderPass* pass);
	void destroy();

	void uploadToGpu(ShaderPass* pass);

	void clear();

	const SamplerParamT* findSamplerParam(StrView name) const;

	TexParamT*		findTexParam	(StrView name);
	SamplerParamT*	findSamplerParam(StrView name);
	BufferParamT*	findBufferParam	(StrView name);
	ImageParamT*	findImageParam	(StrView name);

public:
	const ShaderStageInfo&	info()		const;
	u32						iFrame()	const;

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

		void create	(const Info* info, ShaderPass* pass, u32 idx);
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
		void _setParam(const VarInfo& varInfo, const i32&		v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const u32&		v)	{ return _setParamCheckType(varInfo, v); }
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

		bool setSamplerParam(const SamplerState& v);
		const SamplerState& samplerState() const;

	public:
		SamplerState _samplerState;
	};

	struct BufferParam : public ShaderResource<ShaderStageInfo::StorageBuffer>
	{
	public:
		using Base = ShaderResource<ShaderStageInfo::StorageBuffer>;

	public:
		void create(const Info* info, ShaderPass* pass);

		bool setBufferParam(RenderGpuBuffer* v);
		RenderGpuBuffer* buffer();

	public:
		SPtr<RenderGpuBuffer> _buffer;
	};

	struct ImageParam : public ShaderResource<ShaderStageInfo::StorageImage>
	{
	public:
		using Base = ShaderResource<ShaderStageInfo::StorageImage>;

	public:
		void create(const Info* info, ShaderPass* pass);

		bool setImageParam(Texture* v);
		Texture* image();

	public:
		SPtr<Texture> _image;
	};

	/*template<>
	struct FramedShaderResource : public ShaderResource<ConstBuffer::Info>
	{

	};*/

	#endif

protected:
	const ShaderStageInfo*							_info = nullptr;

	/*
	3 design: 
	
	1. FramedShaderResources (Vector<ShaderResources, s_kFrameInFlightCount> as data member at MaterialPass)
	- but the all resources need to copy when rotateFrame

	2. Vector<FramedShaderResource<T>, N> _xxParams on ShaderResources
	- but need vkUpdateDescriptorSets when rotateFrame on ConstBuffer

	3. hybrid - Vector<Vector<ConstBuffer, s_kLocalConstBufSize>, s_kFrameInFlightCount> +  Vector<FramedShaderResource<T>, N> _xxParams on ShaderResources
	- no need copy for other Params
	- only need to copy cpu data after rotateFrame, only need to uploadToGpu() and copy cpu data, as each descr set bind on each ConstBuffer
	*/

	/*Vector<ConstBuffer,	s_kLocalConstBufSize>	_constBufs;
	Vector<TexParam,		s_kLocalTextureSize>	_texParams;
	Vector<SamplerParam,	s_kLocalTextureSize>	_samplerParams;
	Vector<BufferParam,		s_kLocalBufferSize>		_bufferParams;*/

	u32	_iFrame	= 0;
	Vector<Vector<ConstBuffer, s_kLocalConstBufSize>, s_kFrameInFlightCount> _framedConstBufs;

	// each has framed then
	//Vector<FramedShaderResource<ConstBuffer>	, s_kLocalConstBufSize>	_constBufs;
	Vector<FramedShaderResource<TexParam>		, s_kLocalTextureSize>	_texParams;
	Vector<FramedShaderResource<SamplerParam>	, s_kLocalTextureSize>	_samplerParams;
	Vector<FramedShaderResource<BufferParam>	, s_kLocalBufferSize>	_bufferParams;
	Vector<FramedShaderResource<ImageParam>		, s_kLocalImageSize>	_imageParams;
};

template<class T> inline
void 
ShaderResources::setParam(StrView name, const T& v)
{
	for (auto& e : constBufs())
	{
		auto& rsc = e/*.shaderResource()*/;
		rsc.setParam(name, v);
	}
}

template<class TEX> inline
void 
ShaderResources::setTexParam(StrView name, TEX* v)
{
	auto it = findTexParam(name);
	if (it)
	{
		auto& rsc = it->shaderResource();
		bool isDirty = rsc.setTexure(v);
		if (isDirty)
		{
			it->roatateFrame();
		}
	}
}

inline u32	ShaderResources::iFrame()	const { return _iFrame; }

inline ShaderResources::ConstBuffer&		ShaderResources::constBufs(SizeType i)		{ return _framedConstBufs[iFrame()][i]; }
inline ShaderResources::ConstBuffersView	ShaderResources::constBufs()				{ return _framedConstBufs[iFrame()]; }
inline ShaderResources::CConstBuffersView	ShaderResources::constBufs() const			{ return spanCast<const ConstBufferT>(_framedConstBufs[iFrame()].span()); }

inline ShaderResources::TexParam&			ShaderResources::texParams(SizeType i)		{ return _texParams[i].shaderResource(); }
inline ShaderResources::TexParamsView		ShaderResources::texParams()				{ return _texParams; }
inline ShaderResources::CTexParamsView		ShaderResources::texParams() const			{ return spanCast<const TexParamT>(_texParams.span()); }

inline ShaderResources::SamplerParam&		ShaderResources::samplerParams(SizeType i)	{ return _samplerParams[i].shaderResource(); }
inline ShaderResources::SamplerParamsView	ShaderResources::samplerParams()			{ return _samplerParams; }
inline ShaderResources::CSamplerParamsView	ShaderResources::samplerParams() const		{ return spanCast<const SamplerParamT>(_samplerParams.span()); }

inline ShaderResources::BufferParam&		ShaderResources::bufferParams(SizeType i)	{ return _bufferParams[i].shaderResource(); }
inline ShaderResources::BufferParamsView	ShaderResources::bufferParams()				{ return _bufferParams; }
inline ShaderResources::CBufferParamsView	ShaderResources::bufferParams() const		{ return spanCast<const BufferParamT>(_bufferParams.span()); }

inline ShaderResources::ImageParam&			ShaderResources::imageParams(SizeType i)	{ return _imageParams[i].shaderResource(); }
inline ShaderResources::ImageParamsView		ShaderResources::imageParams()				{ return _imageParams; }
inline ShaderResources::CImageParamsView	ShaderResources::imageParams() const		{ return spanCast<const ImageParamT>(_imageParams.span()); }

inline
void 
ShaderResources::getSamplerNameTo(TempString& out, StrView name)
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

	auto* dst = reinCast<T*>(data() + varInfo.offset);
	
	passTest(_cpuBuf, varInfo, _info, &v);
	RDS_CALL_ONCE(RDS_DUMP_VAR(varInfo.offset, _cpuBuf.size(), end, sizeof(v), sCast<void*>(data()), sCast<void*>(dst)));

	//*dst = v;
	//memory_copy(dst, &v, 1);
	auto sizeofT = sizeof(T); RDS_UNUSED(sizeofT);
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


inline const SamplerState& ShaderResources::SamplerParam::samplerState() const { return _samplerState; }



#endif

#if 0
#pragma mark --- rdsShaderResources::BufferParam-Impl ---
#endif // 0
#if 1

inline RenderGpuBuffer* ShaderResources::BufferParam::buffer()  { return _buffer; }

#endif

#if 0
#pragma mark --- rdsShaderResources::ImageParam-Impl ---
#endif // 0
#if 1

inline Texture* ShaderResources::ImageParam::image()  { return _image; }

#endif

#endif

}