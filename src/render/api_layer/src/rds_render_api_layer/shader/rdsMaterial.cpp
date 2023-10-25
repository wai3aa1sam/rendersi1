#include "rds_render_api_layer-pch.h"
#include "rdsMaterial.h"
#include "rds_render_api_layer/rdsRenderer.h"
#include "rdsShader.h"

namespace rds
{

SPtr<Material> 
Renderer::createMaterial(Shader* shader)
{
	auto p = onCreateMaterial();
	p->setShader(shader);
	return p;
}

SPtr<Material> 
Renderer::createMaterial()
{
	auto p = onCreateMaterial();
	return p;
}

#if 0
#pragma mark --- rdsMaterial-Impl ---
#endif // 0
#if 1

SPtr<Material>
Material::make(Shader* shader)
{
	return Renderer::instance()->createMaterial(shader);
}

Material::Material()
{

}

Material::~Material()
{
}

void 
Material::create()
{
	onCreate();
}

void 
Material::destroy()
{
	_passes.clear();
	_shader.reset(nullptr);
}

void 
Material::setShader(Shader* shader)
{
	destroy();

	if (!shader)
	{
		return;
	}

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
Material::onCreate()
{
}

void
Material::onPostCreate()
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