#include "rds_render_api_layer-pch.h"
#include "rdsShaderStock.h"

#include "rds_render_api_layer/rdsRenderDevice.h"
#include "rdsShader.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderStock-Impl ---
#endif // 0
#if 1

ShaderStock::ShaderStock()
{

}

ShaderStock::~ShaderStock()
{
	destroy();
}

void 
ShaderStock::create(RenderDevice* rdDev)
{
	_rdDev = rdDev;
}

void 
ShaderStock::destroy()
{
	clearPermutationRequest();
	_mtlTable.clear();
	_shaders.clear();
}

SPtr<Shader>
ShaderStock::createShader(const Shader_CreateDesc& cDesc)
{
	bool isPermut	= cDesc.permuts;
	auto filename	= cDesc.filename;

	// find shader
	auto it = _shaders.find(filename);
	bool hasFound = it != _shaders.end();
	if (hasFound && !isPermut) 
	{
		return it->second.front();
	}

	auto& shaders = hasFound ? it->second : _shaders[filename];
	if (isPermut)
	{
		auto& permuts = *cDesc.permuts;
		for (auto& s : shaders)
		{
			if (s->permutations() == permuts)
			{
				return s;
			}
		}
	}

	/*
	*	find failed
	*/
	auto& rdDev = renderDevice();
	cDesc._internal_create(&rdDev);
	auto shader = rdDev.onCreateShader(cDesc);

	shaders.emplace_back(shader);
	if (!isPermut)
	{
		_mtlTable[shader];
	}
	return shader;
}

#if 0
SPtr<Shader> 
ShaderStock::createShader(StrView filename_)
{
	TempString tmpName = filename_;

	auto it = _shaders.find(tmpName.c_str());
	if (it != _shaders.end()) 
	{
		return it->second.front();
	}

	auto& shaders = _shaders[tmpName.c_str()];
	RDS_CORE_ASSERT(shaders.size() == 0, "");

	auto& rdDev = renderDevice();
	auto s = rdDev.createShader(tmpName);
	shaders.emplace_back(s.ptr());
	_mtlTable[s.ptr()];
	return s;
}

SPtr<Shader> 
ShaderStock::createShader(Shader* shader, const ShaderPermutations& permuts)
{
	const auto& filename = shader->filename();

	auto it = _shaders.find(filename.c_str());
	if (it == _shaders.end()) 
	{
		RDS_CORE_ASSERT(false, "no shader for permutation");
	}

	auto& shaders = _shaders[filename.c_str()];
	for (auto& s : shaders)
	{
		if (s->permutations() == permuts)
			return s;
	}

	auto& rdDev = renderDevice();
	auto s = rdDev.createShader(shader->filename(), permuts);
	return shaders.emplace_back(s.ptr());
}

SPtr<Material> 
ShaderStock::createMaterial()
{
	auto& rdDev = renderDevice();
	auto mtl = rdDev.createMaterial();
	return mtl;
}

#endif // 0

void 
ShaderStock::removeShader(Shader* shader)
{
	if (!shader || shader->isPermutatedShader())
		return;

	const auto& filename = shader->filename();

	auto it = _mtlTable.find(shader);
	if (it == _mtlTable.end())
		return;

	auto& mtls = it->second;
	for (auto& mtl : mtls)
	{
		mtl->setShader(nullptr);
	}

	_mtlTable.erase(shader);
	_shaders.erase(filename);
}

void 
ShaderStock::appendUniqueMaterial(Material* mtl, Shader* shader)
{
	if (!mtl || !shader)
	{
		return;
	}

	auto& mtlTable = _mtlTable[shader];
	for (auto& m : mtlTable)
	{
		if (m == mtl)
			return;
	}
	mtlTable.emplace_back(mtl);
}

void 
ShaderStock::removeMaterial(Material* mtl)
{
	auto* mtls = getMaterials(mtl->shader());
	if (!mtls)
		return;

	RDS_TODO("temporary solution, need to think the life cycle between threads"
		", although material will not use on other thread directly, but GraphicsJob in other thread may use and die at other thread, then gg");

	auto it = mtls->find(mtl);
	if (it)
	{
		mtls->erase_unsorted(it);
	}
}

void 
ShaderStock::sendPermutationRequest(Material* mtl)
{
	auto& permutReq = _permutReq;

	auto it = permutReq.find(mtl);
	if (it != permutReq.end())
		return;
	permutReq[mtl];
}

void 
ShaderStock::clearPermutationRequest()
{
	auto& permutReq = _permutReq;
	permutReq.clear();
}

Shader* 
ShaderStock::findShader(StrView filename) 
{
	auto it = getShaders(filename);
	if (it == nullptr)
		return nullptr;
	return it->front(); 
}

ShaderStock::Shaders*
ShaderStock::getShaders(StrView filename)
{
	auto it = _shaders.find(filename);
	if (it == _shaders.end())
		return nullptr;
	return &it->second; 
}

ShaderStock::Materials* 
ShaderStock::getMaterials(Shader* shader)
{
	//const auto& filename = shader->filename();
	auto it = _mtlTable.find(shader);
	if (it == _mtlTable.end())
		return nullptr;
	return &it->second; 
}

#endif

}