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
ShaderResources::create(ShaderStage* shaderStage, ShaderPass* pass)
{
	using ConstBufInfo = ShaderStageInfo::ConstBuffer;

	_info = &shaderStage->info();

	const auto& constBufInfos = info().constBufs;
	_constBufs.reserve(constBufInfos.size());
	for (const ConstBufInfo& e : constBufInfos)
	{
		ConstBuffer& cb = _constBufs.emplace_back();
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
}

void 
ShaderResources::destroy()
{
	clear();
}

void 
ShaderResources::uploadToGpu()
{
	for (auto& e : constBufs())
	{
		e.uploadToGpu();
	}
}

void 
ShaderResources::clear()
{
	_constBufs.clear();
	_texParams.clear();
	_samplerParams.clear();
}

const ShaderResources::SamplerParam* 
ShaderResources::findSamplerParam(StrView name) const
{
	for (const auto& e : samplerParams())
	{
		bool isSame = StrUtil::ignoreCaseCompare(e.name(), name) == 0;
		if (isSame)
			return &e;
	}
	return nullptr;
}


void 
ShaderResources::setSamplerParam(StrView name, const SamplerState& v)
{
	for (auto& e : samplerParams())
	{
		bool isSame = name.compare(e.info().name) == 0;
		if (!isSame)
			continue;
		e.setSamplerParam(v);
	}
}

#endif


#if 0
#pragma mark --- rdsShaderResources::ConstBuffer-Impl ---
#endif // 0
#if 1

void 
ShaderResources::ConstBuffer::create(const Info* info, ShaderPass* pass)
{
	destroy();
	throwIf(info->size == 0, "constbuffer size is 0");

	auto* rdDev		= pass->shader()->renderDevice();
	auto bufSize	= info->size;
	
	_info = info;
	_cpuBuf.resize(bufSize);

	auto bufCDesc = RenderGpuBuffer::makeCDesc();
	bufCDesc.typeFlags	= RenderGpuBufferTypeFlags::Const;
	bufCDesc.bufSize	= bufSize;
	_gpuBuffer = rdDev->createRenderGpuBuffer(bufCDesc);
}

void 
passTest(Vector<u8>& data, const ShaderResources::ConstBuffer::VarInfo& varInfo, const ShaderResources::ConstBuffer::Info* info, const void* value)
{
	for (size_t i = 0; i < data.size(); i++)
	{
		//RDS_DUMP_VAR(i, data[i]);
		data[i] = 0;
	}
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

void 
ShaderResources::SamplerParam::setSamplerParam(const SamplerState& v)
{
	_samplerState = v;
}

#endif


}