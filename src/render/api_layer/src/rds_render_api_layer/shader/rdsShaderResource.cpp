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
ShaderResources::create(const ShaderStageInfo& info_, ShaderPass* pass)
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
	_framedConstBufs.reserve(s_kFrameInFlightCount);
	auto& constBufs = _framedConstBufs.emplace_back();
	constBufs.reserve(constBufInfos.size());
	for (const auto& e : constBufInfos)
	{
		auto& cb = constBufs.emplace_back();
		cb.create(&e, pass);
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
ShaderResources::uploadToGpu(RenderDevice* rdDev)
{
	const auto& constBufInfos = info().constBufs;
	auto src = constBufs();
	bool isDirty = !constBufInfos.is_empty() && !src.is_empty() && src[0]._isDirty;

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

	for (auto& e : src)
	{
		e.uploadToGpu();
	}

	if (_framedConstBufs.size() < s_kFrameInFlightCount && isDirty)
	{
		auto& dst = _framedConstBufs.emplace_back();
		
		dst.reserve(constBufInfos.size());
		for (const auto& e : constBufInfos)
		{
			auto& cb = dst.emplace_back();
			cb.create(&e, rdDev);
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
}

void 
ShaderResources::clear()
{
	/*for (auto& e : _constBufs)
	{
		e.destroy();
	}*/

	for (auto& _constBufs : _framedConstBufs)
	{
		for (auto& e : _constBufs)
		{
			e.destroy();
		}
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
	_framedConstBufs.clear();
	_texParams.clear();
	_samplerParams.clear();
	_bufferParams.clear();
	_imageParams.clear();
}

const ShaderResources::SamplerParamT* 
ShaderResources::findSamplerParam(StrView name) const
{
	for (const auto& e : samplerParams())
	{
		const auto& rsc = e.shaderResource();
		bool isSame = StrUtil::ignoreCaseCompare(rsc.name(), name) == 0;
		if (isSame)
			return &e;
	}
	return nullptr;
}

ShaderResources::TexParamT*		
ShaderResources::findTexParam(StrView name)
{
	for (auto& e : _texParams)
	{
		auto& rsc = e.shaderResource();
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
	for (auto& e : _samplerParams)
	{
		auto& rsc = e.shaderResource();
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
	for (auto& e : _bufferParams)
	{
		auto& rsc = e.shaderResource();
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
	for (auto& e : _imageParams)
	{
		auto& rsc = e.shaderResource();
		bool isSame = StrUtil::ignoreCaseCompare(rsc.name(), name) == 0;
		if (isSame)
		{
			return &e;
		}
	}
	return nullptr;
}

void 
ShaderResources::setSamplerParam(StrView name, const SamplerState& v)
{
	auto it = findSamplerParam(name);
	if (it)
	{
		auto& rsc = it->shaderResource();
		rsc.setSamplerParam(v);
		it->roatateFrame();
	}
}

void 
ShaderResources::setBufferParam(StrView name, RenderGpuBuffer* v)
{
	auto it = findBufferParam(name);
	if (it)
	{
		auto& rsc = it->shaderResource();
		rsc.setBufferParam(v);
		it->roatateFrame();
	}
}

void 
ShaderResources::setImageParam(StrView name, Texture* v)
{
	auto it = findImageParam(name);
	if (it)
	{
		auto& rsc = it->shaderResource();
		rsc.setImageParam(v);
		it->roatateFrame();
	}
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
ShaderResources::ConstBuffer::create(const Info* info, RenderDevice* rdDev)
{
	destroy();
	throwIf(info->size == 0, "constbuffer size is 0");

	//auto* rdDev		= pass->shader()->renderDevice();
	auto bufSize	= info->size;

	_info = info;
	_cpuBuf.resize(bufSize);

	auto bufCDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
	bufCDesc.typeFlags	= RenderGpuBufferTypeFlags::Const;
	bufCDesc.bufSize	= bufSize;
	_gpuBuffer = rdDev->createRenderGpuBuffer(bufCDesc);
}

void 
ShaderResources::ConstBuffer::create(const Info* info, ShaderPass* pass)
{
	auto* rdDev	= pass->shader()->renderDevice();
	create(info, rdDev);
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
	if (!_isDirty)
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

}