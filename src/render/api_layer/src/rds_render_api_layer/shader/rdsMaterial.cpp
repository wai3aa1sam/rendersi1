#include "rds_render_api_layer-pch.h"
#include "rdsMaterial.h"
#include "rdsShader.h"
#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

SPtr<Material> 
RenderDevice::createMaterial(const Material_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateMaterial(cDesc);
	p->setShader(cDesc.shader);
	return p;
}

SPtr<Material> 
RenderDevice::createMaterial(Shader* shader)
{
	auto cDesc = Material::makeCDesc();
	cDesc.shader = shader;

	return createMaterial(cDesc);
}

SPtr<Material> 
RenderDevice::createMaterial()
{
	auto cDesc = Material::makeCDesc();
	auto p = createMaterial(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsMaterial-Impl ---
#endif // 0
#if 1

Material::CreateDesc 
Material::makeCDesc()
{
	return CreateDesc{};
}

SPtr<Material> 
Material::make(const Material_CreateDesc& cDesc)
{
	return Renderer::rdDev()->createMaterial(cDesc);
}

SPtr<Material>
Material::make(Shader* shader)
{
	return Renderer::rdDev()->createMaterial(shader);
}

Material::Material()
{

}

Material::~Material()
{
	RDS_PROFILE_SCOPED();
}

void 
Material::create(const CreateDesc& cDesc)
{
	destroy();

	Base::create(cDesc);
	onCreate(cDesc);
}

void 
Material::destroy()
{
	if (!_shader || _passes.is_empty())
		return;

	checkMainThreadExclusive(RDS_SRCLOC);

	renderDevice()->shaderStock().removeMaterial(this);

	_passes.clear();
	_shader.reset(nullptr);

	onDestroy();
	Base::destroy();
}

void 
Material::setShader(Shader* shader)
{
	checkMainThreadExclusive(RDS_SRCLOC);

	Vector<ShaderResources, s_kLocalPassSize> prevShaderRscs;
	if (shader && _shader)
	{
		/*
		* materialPasses maybe reference to a destroyed shader, so need to get the non permutated shader
		* copy for restore the previous value
		*/
		Shader* nonPermutatedShader = renderDevice()->shaderStock().findShader(filename());
		if (nonPermutatedShader)
		{
			prevShaderRscs.reserve(_passes.size());

			auto nCopyPass = math::min(nonPermutatedShader->passes().size(), _passes.size());
			for (size_t i = 0; i < nCopyPass; i++)
			{
				auto* shaderPass = nonPermutatedShader->passes()[i].ptr();
				auto& dst = prevShaderRscs.emplace_back();
				dst.create(shaderPass->info().allStageUnionInfo, shaderPass, 0);
				dst.copy(_passes[i]->shaderResources());
			}
		}
	}

	destroy();

	if (!shader)
	{
		return;
	}

	RDS_CORE_ASSERT(!renderDevice() || renderDevice() == shader->renderDevice(), "renderDevice() == shader->renderDevice()");
	Base::create(shader->renderDevice(), true);
	_shader = shader;

	SizeType passCount = info().passes.size();
	_passes.reserve(passCount);
	for (SizeType i = 0; i < passCount; i++)
	{
		UPtr<Pass>& e = _passes.emplace_back(onMakePass());
		e->create(this, shader->passes()[i]);
	}

	resetPermutation(shader->info().permuts);
	renderDevice()->shaderStock().appendUniqueMaterial(this, shader);
	
	auto nCopyPass = math::min(prevShaderRscs.size(), passCount);
	for (size_t i = 0; i < nCopyPass; i++)
	{
		auto& prevShaderRsc = prevShaderRscs[i];
		_passes[i]->shaderResources().copy(prevShaderRsc);
	}
}

void
Material::onCreate(const CreateDesc& cDesc)
{
}

void
Material::onPostCreate(const CreateDesc& cDesc)
{
}

void
Material::onDestroy()
{
}

ShaderParamId 
Material::getParamId(StrView name) const
{
	RDS_NOT_YET_SUPPORT();
	return 0;
}

void 
Material::_setSamplerParam(StrView name, const SamplerState& v)
{
	/*for (auto& pass : _passes)
	{
	pass->setSamplerParam(name, v);
	}*/

	setParam(name, renderDevice()->bindlessResource().findSamplerIndex(v));
}

void 
Material::setPermutation(StrView name, StrView value)						
{
	checkMainThreadExclusive(RDS_SRCLOC);

	auto& info = shader()->info();
	if (_permuts.isEmpty())
	{
		resetPermutation(info.permuts);
		return;
	}
	_setPermutation(name, value); 

	auto& shaderStock = renderDevice()->shaderStock();
	shaderStock.sendPermutationRequest(this); 
}

void 
Material::resetPermutation(const ShaderInfo::Permuts& permutations)	
{ 
	checkMainThreadExclusive(RDS_SRCLOC);

	_permuts.create(permutations); 
}

void Material::clearPermutation()
{
	checkMainThreadExclusive(RDS_SRCLOC);

	_permuts.clear();

	const auto& filename = shader()->filename();
	auto shader = renderDevice()->createShader(filename);
	setShader(shader);
}

void 
Material::_setPermutation(StrView name, StrView value)
{
	auto& info = shader()->info();
	bool isExist = info.isPermutationValid(name, value);
	throwIf(!isExist, "unknow Permutation name");
	_permuts.set(name, value);
}

const ShaderInfo&		Material::info()		const { return _shader->info(); }
const String&			Material::filename()	const { return _shader->filename(); }

#endif

}