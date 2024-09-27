#include "rds_render_api_layer-pch.h"
#include "rdsShaderResource.h"
#include "rds_render_api_layer/rdsRenderer.h"

#include "rdsShader.h"

namespace rds
{


#if 0
#pragma mark --- rdsShaderResources-Impl ---
#endif // 0
#if 1

const ShaderStageInfo& ShaderResources::info() const { return *this->_info; }

void 
ShaderResources::create(const ShaderStageInfo& info_, ShaderPass* pass, u32 frameIdx)
{
	using ConstBufInfo = ShaderStageInfo::ConstBuffer;
	destroy();

	_info = &info_;

	/*const auto& constBufInfos = info().constBufs;
	_constBufs.reserve(constBufInfos.size());
	for (const auto& e : constBufInfos)
	{
		auto& cb = _constBufs.emplace_back();
		cb.create(&e, pass);
	}*/

	const auto& constBufInfos = info().constBufs;
	_constBufs.reserve(constBufInfos.size());
	//auto& constBufs = _constBufs.emplace_back();
	for (const auto& e : constBufInfos)
	{
		auto& cb = _constBufs.emplace_back();
		cb.create(&e, pass, frameIdx);
	}

	const auto& texInfos = info().textures;
	_texParams.reserve(texInfos.size());
	for (const auto& e : texInfos)
	{
		auto& texParam = _texParams.emplace_back();
		texParam.create(&e, pass);
	}

	const auto& samplerInfos = info().samplers;
	_samplerParams.reserve(samplerInfos.size());
	for (const auto& e : samplerInfos)
	{
		auto& samplerParam = _samplerParams.emplace_back();
		samplerParam.create(&e, pass);
	}

	const auto& bufferInfos = info().storageBufs;
	_bufferParams.reserve(bufferInfos.size());
	for (const auto& e : bufferInfos)
	{
		auto& bufferParam = _bufferParams.emplace_back();
		bufferParam.create(&e, pass);
	}

	const auto& imageInfos = info().storageImages;
	_imageParams.reserve(imageInfos.size());
	for (const auto& e : imageInfos)
	{
		auto& imageParam = _imageParams.emplace_back();
		imageParam.create(&e, pass);
	}
}

void 
ShaderResources::destroy()
{
	clear();
	_info = nullptr;
}

void 
ShaderResources::uploadToGpu(ShaderPass* pass)
{
	const auto& constBufInfos = info().constBufs;
	auto src = constBufs();
	bool isDirty = !constBufInfos.is_empty() && !src.is_empty() && src[0]._isDirty;
	if (!isDirty)
		return;

	#if 0
	auto copyCpuBufToNextFrame =
		[&](auto* oConstBufs)
		{
			auto& dst = *oConstBufs;
			for (size_t i = 0; i < constBufInfos.size(); i++)
			{
				dst[i]._cpuBuf = src[i]._cpuBuf;
				// gpu buffer will create on demand in backend
			}
		};
	#endif // 0

	#if 0
	for (auto& e : src)
	{
		e.uploadToGpu();
	}

	if (_constBufs.size() < s_kFrameInFlightCount && isDirty)
	{
		auto& dst = _constBufs.emplace_back();

		dst.reserve(constBufInfos.size());
		for (const auto& e : constBufInfos)
		{
			auto& cb = dst.emplace_back();
			cb.create(&e, pass, sCast<u32>(_constBufs.size() - 1));
		}
	}

	if (isDirty)
	{
		_iFrame = (_iFrame + 1) % s_kFrameInFlightCount;
		auto dst = constBufs();
		for (size_t i = 0; i < constBufInfos.size(); i++)
		{
			dst[i]._cpuBuf = src[i]._cpuBuf;
			// gpu buffer will create on demand in backend
		}
	}
	#endif // 0

	for (auto& e : _constBufs)
	{
		e.uploadToGpu();
	}
}

void 
ShaderResources::clear()
{
	/*for (auto& e : _constBufs)
	{
		e.destroy();
	}*/

	for (auto& e : _constBufs)
	{
		e.destroy();
	}

	for (auto& e : _texParams)
	{
		e.destroy();
	}

	for (auto& e : _samplerParams)
	{
		e.destroy();
	}

	for (auto& e : _bufferParams)
	{
		e.destroy();
	}

	for (auto& e : _imageParams)
	{
		e.destroy();
	}

	//_constBufs.clear();
	_constBufs.clear();
	_texParams.clear();
	_samplerParams.clear();
	_bufferParams.clear();
	_imageParams.clear();
}

void 
ShaderResources::copy(const ShaderResources& rsc)
{
	//RDS_CORE_ASSERT(&rsc.info() == &info(), "all ShaderResources must be created");
	
	// simple assign will call ctor instead of operator=(), _info will be invalid
	#if 0
	_constBufs		= rsc._constBufs;
	_texParams		= rsc._texParams;
	_samplerParams	= rsc._samplerParams;
	_bufferParams	= rsc._bufferParams;
	_imageParams	= rsc._imageParams;
	#endif // 0

	auto nCopyConstBuf		= math::min(constBufs().size(),		rsc.constBufs().size());
	auto nCopyTexParam		= math::min(texParams().size(),		rsc.texParams().size());
	auto nCopySamplerParam	= math::min(samplerParams().size(), rsc.samplerParams().size());
	auto nCopyBufferParam	= math::min(bufferParams().size(),	rsc.bufferParams().size());
	auto nCopyImageParam	= math::min(imageParams().size(),	rsc.imageParams().size());

	for (size_t i = 0; i < nCopyConstBuf;		++i) { auto& src = rsc._constBufs[i];		auto& dst = _constBufs[i];		dst.copy(src); }
	for (size_t i = 0; i < nCopyTexParam;		++i) { auto& src = rsc._texParams[i];		auto& dst = _texParams[i];		dst.copy(src); }
	for (size_t i = 0; i < nCopySamplerParam;	++i) { auto& src = rsc._samplerParams[i];	auto& dst = _samplerParams[i];	dst.copy(src); }
	for (size_t i = 0; i < nCopyBufferParam;	++i) { auto& src = rsc._bufferParams[i];	auto& dst = _bufferParams[i];	dst.copy(src); }
	for (size_t i = 0; i < nCopyImageParam;		++i) { auto& src = rsc._imageParams[i];		auto& dst = _imageParams[i];	dst.copy(src); }

	#if 0
	for (size_t i = 0; i < rsc.constBufs().size(); ++i)
	{
		const auto& src = constBufs(i);
		auto& dst		= rsc.constBufs(i);
		dst._cpuBuf = src._cpuBuf;
	}

	for (size_t i = 0; i < rsc.texParams().size(); ++i)
	{
		const auto& src = texParams(i);
		auto& dst		= rsc.texParams(i);
		if (src._tex.ptr() != dst._tex.ptr())
		{
			//dst._tex.reset(src._tex.ptr());
			dst._tex = src._tex;
		}
	}
	#endif // 0
}

void*	
ShaderResources::findParam(StrView name)
{
	void* param = nullptr;
	for (auto& e : constBufs())
	{
		param = e.findParam(name);
		if (param)
		{
			return param;
		}
	}
	return param;
}

void*	
ShaderResources::findParam(StrView name) const
{
	return constCast(*this).findParam(name);
}

ShaderResources::TexParamT*		
ShaderResources::findTexParam(StrView name)
{
	for (auto& e : texParams())
	{
		auto& rsc = e;
		bool isSame = StrUtil::ignoreCaseCompare(rsc.name(), name) == 0;
		if (isSame)
		{
			return &e;
		}
	}
	return nullptr;
}

ShaderResources::SamplerParamT*	
ShaderResources::findSamplerParam(StrView name)
{
	for (auto& e : samplerParams())
	{
		auto& rsc = e;
		bool isSame = StrUtil::ignoreCaseCompare(rsc.name(), name) == 0;
		if (isSame)
		{
			return &e;
		}
	}
	return nullptr;
}

ShaderResources::ImageParamT*	
ShaderResources::findImageParam(StrView name)
{
	for (auto& e : imageParams())
	{
		auto& rsc = e;
		bool isSame = StrUtil::ignoreCaseCompare(rsc.name(), name) == 0;
		if (isSame)
		{
			return &e;
		}
	}
	return nullptr;
}

ShaderResources::BufferParamT*	
ShaderResources::findBufferParam(StrView name)
{
	for (auto& e : bufferParams())
	{
		auto& rsc = e;
		bool isSame = StrUtil::ignoreCaseCompare(rsc.name(), name) == 0;
		if (isSame)
		{
			return &e;
		}
	}
	return nullptr;
}

const ShaderResources::TexParamT*		
ShaderResources::findTexParam(StrView name) const
{
	return constCast(*this).findTexParam(name);
}

const ShaderResources::SamplerParamT* 
ShaderResources::findSamplerParam(StrView name) const
{
	return constCast(*this).findSamplerParam(name);
}

const ShaderResources::BufferParamT*	
ShaderResources::findBufferParam(StrView name) const
{
	return constCast(*this).findBufferParam(name);
}

const ShaderResources::ImageParamT*	
ShaderResources::findImageParam(StrView name) const
{
	return constCast(*this).findImageParam(name);
}

bool
ShaderResources::setSamplerParam(StrView name, const SamplerState& v)
{
	auto it			= findSamplerParam(name);
	bool isDirty	= false;
	if (it)
	{
		auto& rsc	= *it;
		isDirty		= rsc.setSamplerParam(v);
	}
	return isDirty;
}

bool 
ShaderResources::setBufferParam(StrView name, RenderGpuBuffer* v)
{
	auto it			= findBufferParam(name);
	bool isDirty	= false;
	if (it)
	{
		auto& rsc	= *it;
		isDirty		= rsc.setBufferParam(v);
	}
	return isDirty;
}

bool 
ShaderResources::setImageParam(StrView name, Texture* v)
{
	auto it			= findImageParam(name);
	bool isDirty	= false;
	if (it)
	{
		auto& rsc	= *it;
		isDirty		= rsc.setImageParam(v);
	}
	return isDirty;
}

#endif


#if 0
#pragma mark --- rdsShaderResources::ConstBuffer-Impl ---
#endif // 0
#if 1

ShaderResources::ConstBuffer::ConstBuffer()
{

}

ShaderResources::ConstBuffer::~ConstBuffer()
{
	destroy();
}

void 
ShaderResources::ConstBuffer::create(const Info* info, ShaderPass* pass, u32 idx)
{
	auto* rdDev	= pass->shader()->renderDevice();

	destroy();
	throwIf(info->size == 0, "constbuffer size is 0");

	//auto* rdDev		= pass->shader()->renderDevice();
	auto bufSize	= info->size;

	_info = info;
	_cpuBuf.resize(bufSize);

	auto bufCDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
	bufCDesc.typeFlags	= RenderGpuBufferTypeFlags::Constant;
	bufCDesc.bufSize	= bufSize;
	_gpuBuffer = rdDev->createRenderGpuBuffer(bufCDesc);
	_gpuBuffer->setDebugName(fmtAs_T<TempString>("{}-cb-{}-{}", pass->shader()->filename(), info->name, idx));
}

void 
passTest(Vector<u8>& data, const ShaderResources::ConstBuffer::VarInfo& varInfo, const ShaderResources::ConstBuffer::Info* info, const void* value)
{
	//for (size_t i = 0; i < data.size(); i++)
	//{
	//	//RDS_DUMP_VAR(i, data[i]);
	//	//data[i] = 0;
	//}
	RDS_CALL_ONCE(RDS_LOG("sizeof(Mat4f) == {}, *reinCast<const Mat4f*>(value) = {}", sizeof(Mat4f), *reinCast<const Mat4f*>(value)));
	RDS_CALL_ONCE(RDS_DUMP_VAR(reinCast<void*>(data.data()), *reinCast<Mat4f*>(&data[varInfo.offset])));
	//memcpy(data.data() + varInfo.offset, value, sizeof(Mat4f));										// this will success
	//memory_copy(reinCast<Mat4f*>(data.data() + varInfo.offset), sCast<const Mat4f*>(value), 1);		// this will fail

	RDS_CALL_ONCE(RDS_LOG("passTest end"));

	RDS_TODO("ShaderResources::ConstBuffer::_setValue(const VarInfo& varInfo, const T& v) will crash in release mode, further debug is needed");
	RDS_TODO("2023.11.22, the offset is not align as 16, it is align as 8");
	//reinterpret_cast<Mat4f&>(data[varInfo.offset - 8]) = *reinCast<const Mat4f*>(value);				// this will success
	//reinterpret_cast<Mat4f&>(data[varInfo.offset]) = *reinCast<const Mat4f*>(value);					// this will fail
}

void 
ShaderResources::ConstBuffer::destroy()
{
	_info = nullptr;
	_gpuBuffer.reset(nullptr);
	_cpuBuf.clear();
	_isDirty = false;
}

void 
ShaderResources::ConstBuffer::uploadToGpu()
{
	if (!_isDirty || info().size > _gpuBuffer->bufSize())
		return;

	// ByteSpan{_cpuBuf.data(), _cpuBuf.size()}
	_gpuBuffer->uploadToGpu(_cpuBuf);
	_isDirty = false;
}


#endif

#if 0
#pragma mark --- rdsShaderResources::TexParam-Impl ---
#endif // 0
#if 1

void 
ShaderResources::TexParam::create(const Info* info, ShaderPass* pass)
{
	Base::create(info);
}

Texture* 
ShaderResources::TexParam::getUpdatedTexture(RenderDevice* rdDev)
{
	if (!_tex) 
	{
		switch (_info->dataType) 
		{
			case DataType::Texture2D: { return rdDev->textureStock().error; } break;
		}
		RDS_THROW("unsupported texture type");
	}

	RDS_TODO("update texture");
	return _tex;
}

#endif

#if 0
#pragma mark --- rdsShaderResources::SamplerParam-Impl ---
#endif // 0
#if 1

void 
ShaderResources::SamplerParam::create(const Info* info, ShaderPass* pass)
{
	Base::create(info);
}

bool 
ShaderResources::SamplerParam::setSamplerParam(const SamplerState& v)
{
	bool isDirty = _samplerState.hash() != v.hash();
	if (isDirty)
	{
		_samplerState = v;
	}
	return isDirty;
}

#endif

#if 0
#pragma mark --- rdsShaderResources::BufferParam-Impl ---
#endif // 0
#if 1

void 
ShaderResources::BufferParam::create(const Info* info, ShaderPass* pass)
{
	Base::create(info);
}

bool 
ShaderResources::BufferParam::setBufferParam(RenderGpuBuffer* v)
{
	if (!v) RDS_CORE_ASSERT(false, "buffer == nullptr");

	bool isSame = _buffer == v;
	if (!isSame)
	{
		_buffer.reset(v);
	}

	bool isDirty = !isSame;
	return isDirty;
}

#endif

#if 0
#pragma mark --- rdsShaderResources::ImageParam-Impl ---
#endif // 0
#if 1

void 
ShaderResources::ImageParam::create(const Info* info, ShaderPass* pass)
{
	Base::create(info);
}

bool 
ShaderResources::ImageParam::setImageParam(Texture* v)
{
	if (!v) RDS_CORE_ASSERT(false, "Texture == nullptr");

	bool isSame = _image == v;
	if (!isSame)
	{
		_image.reset(v);
	}

	bool isDirty = !isSame;
	return isDirty;
}


#endif


#if 0
#pragma mark --- rdsFramedShaderResource-Impl ---
#endif // 0
#if 1

FramedShaderResources::FramedShaderResources()
{

}

FramedShaderResources::~FramedShaderResources()
{
	destroy();
}

void 
FramedShaderResources::create(const ShaderStageInfo& info_, ShaderPass* pass)
{
	_shaderPass = pass;
	_shaderRscs.resize(s_kFrameInFlightCount);
	u32 i = 0;
	for (auto& e : _shaderRscs)
	{
		e.create(info_, pass, i);
		++i;
	}
}

void 
FramedShaderResources::destroy()
{
	_shaderRscs.clear();
}

void 
FramedShaderResources::rotateFrame()
{
	if (!shouldRotateFrame())
	{
		return;
	}

	auto frameCount = _shaderPass->shader()->engineFrameCount();
	auto srcFrame	= Traits::rotateFrame(lastEngineFrameCount());
	auto dstFrame	= Traits::rotateFrame(frameCount);

	if (srcFrame == dstFrame)
	{
		_lastEngineFrameCount = frameCount;
		return;
	}

	auto& src = _shaderRscs[srcFrame];
	auto& dst = _shaderRscs[dstFrame];

	dst.copy(src);

	_lastEngineFrameCount = frameCount;
}

bool 
FramedShaderResources::shouldRotateFrame() const
{
	RDS_TODO("add a last thread id to debug whether two / more thread want to access the same material");

	auto frameCount = _shaderPass->shader()->engineFrameCount();
	return _lastEngineFrameCount != frameCount;
}

void 
FramedShaderResources::uploadToGpu()
{
	shaderResource().uploadToGpu(_shaderPass);
}


#endif

}