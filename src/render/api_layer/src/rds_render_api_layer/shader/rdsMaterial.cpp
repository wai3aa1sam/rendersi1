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
	_passes.clear();
	_shader.reset(nullptr);

	onDestroy();
	Base::destroy();
}

void 
Material::setShader(Shader* shader)
{
	if (!shader)
	{
		destroy();
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

const ShaderInfo&	Material::info() const	{ return _shader->info(); }

#endif

}