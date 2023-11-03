#include "rds_render_api_layer-pch.h"
#include "rdsShaderResource.h"
#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{


#if 0
#pragma mark --- rdsShaderResources-Impl ---
#endif // 0
#if 1

const ShaderStageInfo& ShaderResources::info() const { return *this->_info; }

void 
ShaderResources::create(ShaderStage* shaderStage)
{
	using ConstBufInfo = ShaderStageInfo::ConstBuffer;

	_info = &shaderStage->info();

	const auto& constBufInfos = info().constBufs;
	_constBufs.reserve(constBufInfos.size());
	for (const ConstBufInfo& e : constBufInfos)
	{
		ConstBuffer& cb = _constBufs.emplace_back();
		cb.create(&e);
	}

	const auto& texInfos = info().textures;
	_texParams.reserve(texInfos.size());
	for (const auto& e : texInfos)
	{
		auto& texParam = _texParams.emplace_back();
		texParam.create(&e);
	}

	const auto& samplerInfos = info().samplers;
	_samplerParams.reserve(samplerInfos.size());
	for (const auto& e : samplerInfos)
	{
		auto& samplerParam = _samplerParams.emplace_back();
		samplerParam.create(&e);
	}
}

void 
ShaderResources::destroy()
{
	clear();
}

void 
ShaderResources::ConstBuffer::create(const Info* info)
{
	destroy();

	_info = info;
	_cpuBuf.resize(info->size);

	auto bufCDesc = RenderGpuBuffer::makeCDesc();
	bufCDesc.typeFlags	= RenderGpuBufferTypeFlags::Const;
	bufCDesc.bufSize	= info->size;
	_gpuBuffer = Renderer::instance()->createRenderGpuBuffer(bufCDesc);
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

	_gpuBuffer->uploadToGpu(_cpuBuf);
	_isDirty = false;
}


#endif

#if 0
#pragma mark --- rdsShaderResources::TexParam-Impl ---
#endif // 0
#if 1

Texture* 
ShaderResources::TexParam::getUpdatedTexture()
{
	if (!_tex) 
	{
		auto* rdr = Renderer::instance();
		switch (_info->dataType) 
		{
			case DataType::Texture2D: { return rdr->textureStock().error; } break;
		}
		RDS_THROW("unsupported texture type");
	}

	RDS_TODO("update texture");
	return _tex;
}

#endif

}