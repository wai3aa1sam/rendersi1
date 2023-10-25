#include "rds_render_api_layer-pch.h"
#include "rdsMaterial.h"
#include "rdsMaterialPass.h"
#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderParams-Impl ---
#endif // 0
#if 1

const ShaderStageInfo& ShaderParams::info() const { return *this->_info; }

void 
ShaderParams::create(ShaderStage* shaderStage)
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
}

void 
ShaderParams::destroy()
{
	clear();
}

void 
ShaderParams::ConstBuffer::create(const Info* info)
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
ShaderParams::ConstBuffer::destroy()
{
	_info = nullptr;
	_gpuBuffer.reset(nullptr);
}

void 
ShaderParams::ConstBuffer::uploadToGpu()
{
	if (!_isDirty)
		return;

	_gpuBuffer->uploadToGpu(_cpuBuf);
	_isDirty = false;
}


#endif

#if 0
#pragma mark --- rdsMaterialPassStage-Impl ---
#endif // 0
#if 1

void 
MaterialPass_Stage::create(MaterialPass* pass, ShaderStage* shaderStage)
{
	if (!shaderStage)
		return;

	destroy();
	_shaderStage = shaderStage;
	_framedShaderParams.resize(s_kFrameInFlightCount);
	for (auto& e : _framedShaderParams)
	{
		e.create(shaderStage);
	}
}

void 
MaterialPass_Stage::destroy()
{
	for (auto& e : _framedShaderParams)
	{
		e.destroy();
	}
	_framedShaderParams.clear();
	this->_shaderStage = nullptr;
}

ShaderParams& MaterialPass_Stage::shaderParams(Material* mtl) { return _framedShaderParams[mtl->frameIdx()]; }

#endif

#if 0
#pragma mark --- rdsMaterialPass-Impl ---
#endif // 0
#if 1

void 
MaterialPass::destroy()
{
	
}



#endif

}