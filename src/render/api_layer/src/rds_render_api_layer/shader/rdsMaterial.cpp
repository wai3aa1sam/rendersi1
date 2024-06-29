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
	Base::destroy();
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

	bool isFirstShader = !_shader && shader;
	Vector<ShaderResources, s_kLocalPassSize> prevShaderRscs;
	if (shader && _shader)
	{
		/*
		* materialPasses maybe reference to a destroyed shader, so need to get the non permutated shader
		* copy for restore the previous value
		*/
		RDS_TODO("ShaderResources::create() will create gpu buffer, but we only need the cpu buffer, pass a bool to it?");
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

	if (isFirstShader)
	{
		setParamsToDefault();
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
Material::setParam(StrView name, Texture* v, const SamplerState& samplerState)
{
	using SRC = RenderDataType;
	switch (v->type())
	{
		case SRC::Texture2D: { setParam(name, sCast<Texture2D*>(v), samplerState); } break;
		case SRC::Texture3D: { setParam(name, sCast<Texture3D*>(v), samplerState); } break;
		default: { RDS_THROW("invalid texture type"); } break;
	}
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
Material::setImage(StrView name, Texture*	 v, u32 mipLevel)
{
	using SRC = RenderDataType;
	switch (v->type())
	{
		case SRC::Texture2D: { setImage(name, sCast<Texture2D*>(v), mipLevel); } break;
		case SRC::Texture3D: { setImage(name, sCast<Texture3D*>(v), mipLevel); } break;
		default: { RDS_THROW("invalid texture type"); } break;
	}
}

void Material::setImage(StrView name, Texture2D* v, u32 mipLevel)	{ return _setImageParam(name, v, mipLevel); }

void Material::setImage(StrView name, Texture3D* v, u32 mipLevel)	
{ 
	//RDS_LOG("{}: {}", name, v->uavBindlessHandle().getResourceIndex(mipLevel)); 
	return _setImageParam(name, v, mipLevel); 
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

void 
Material::setArray(StrView name, Span<const float>	v)		
{ 
	Vector<Vec4f, 128> temp; 
	temp.resize(v.size()); 
	for (size_t i = 0; i < v.size(); ++i)
	{
		temp[i].x = v[i];
	}
	return _setArray(name, v); 
}

/*
* temporary
*/
#if 1

template<class T>
struct TypeIo
{
	bool onParseStr(StrView str)
	{

	}
};

class TypeDeserializer
{
public:
	template<class T>
	bool tryParse(T& oValue, StrView str);
};

bool tryParse(bool& oValue, StrView str)
{
	RDS_TODO("TypeDeserializer");

	using TokenType = Lexer::TokenType;

	Lexer lexer;
	lexer.reset(str, "");

	const auto& token = lexer.token();
	if (token.type() == TokenType::Identifier)
	{
		if (StrUtil::isSame(token.value(), "false"))
		{
			oValue = false;
		}
		else if (StrUtil::isSame(token.value(), "true"))
		{
			oValue = true;
		}
		else return false;
		return true;
	}
	else if (token.type() == TokenType::Number)
	{
		u64 v = 0;
		StrUtil::tryParse(str, v);
		oValue = v;
		return true;
	}

	return false;
}


bool tryParse(Vec2f& oValue, StrView str)
{
	RDS_TODO("TypeDeserializer");

	using TokenType = Lexer::TokenType;

	Lexer lexer;
	lexer.reset(str, "");

	bool isValid = false;
	const auto& token = lexer.token();
	if (token.type() == TokenType::Operator)
	{
		isValid = StrUtil::hasAny(token.value(), "{(");
		if (!isValid) return false;

		lexer.nextToken();
		StrUtil::tryParse(token.value(), oValue.x);

		lexer.nextToken();
		isValid = StrUtil::hasAny(token.value(), ",");

		lexer.nextToken();
		StrUtil::tryParse(token.value(), oValue.y);

		lexer.nextToken();
		isValid = StrUtil::hasAny(token.value(), ")}");
	}
	return false;
}

bool tryParse(Vec3f& oValue, StrView str)
{
	RDS_TODO("TypeDeserializer");

	using TokenType = Lexer::TokenType;

	Lexer lexer;
	lexer.reset(str, "");

	bool isValid = false;
	const auto& token = lexer.token();
	if (token.type() == TokenType::Operator)
	{
		isValid = StrUtil::hasAny(token.value(), "{(");
		if (!isValid) return isValid;

		lexer.nextToken();
		isValid = StrUtil::tryParse(token.value(), oValue.x);

		lexer.nextToken();
		isValid = StrUtil::hasAny(token.value(), ",");

		lexer.nextToken();
		StrUtil::tryParse(token.value(), oValue.y);

		lexer.nextToken();
		isValid = StrUtil::hasAny(token.value(), ",");

		lexer.nextToken();
		StrUtil::tryParse(token.value(), oValue.z);

		lexer.nextToken();
		isValid = StrUtil::hasAny(token.value(), ")}");
	}
	return isValid;
}

bool tryParse(Vec4f& oValue, StrView str)
{
	RDS_TODO("TypeDeserializer");

	using TokenType = Lexer::TokenType;

	Lexer lexer;
	lexer.reset(str, "");

	bool isValid = false;
	const auto& token = lexer.token();
	if (token.type() == TokenType::Operator)
	{
		isValid = StrUtil::hasAny(token.value(), "{(");
		if (!isValid) return false;

		lexer.nextToken();
		StrUtil::tryParse(token.value(), oValue.x);

		lexer.nextToken();
		isValid = StrUtil::hasAny(token.value(), ",");

		lexer.nextToken();
		StrUtil::tryParse(token.value(), oValue.y);

		lexer.nextToken();
		isValid = StrUtil::hasAny(token.value(), ",");

		lexer.nextToken();
		StrUtil::tryParse(token.value(), oValue.z);

		lexer.nextToken();
		isValid = StrUtil::hasAny(token.value(), ",");

		lexer.nextToken();
		StrUtil::tryParse(token.value(), oValue.w);

		lexer.nextToken();
		isValid = StrUtil::hasAny(token.value(), ")}");
	}
	return isValid;
}

template<class T>
bool tryParse(T& v, StrView str)
{
	return StrUtil::tryParse(str, v);
}

#endif // 1

void 
Material::setParamsToDefault()
{
	RDS_TODO("modify parse and find texture st, maybe have a separate cache");
	if (!shader())
		return;

	for (const ShaderPropInfo& prop : info().props)
	{
		StrView defaultValue = prop.defaultValue;
		if (defaultValue.is_empty())
			continue;

		StrView			name = prop.name;
		ShaderPropType	type = prop.type;

		switch (type)
		{
			case ShaderPropType::Bool:		{ bool		v; if (tryParse(v, defaultValue)) { setParam(name, u32(v)); } } break;
			case ShaderPropType::Int:		{ int		v; if (tryParse(v, defaultValue)) { setParam(name, v); } } break;
			case ShaderPropType::Float:		{ float		v; if (tryParse(v, defaultValue)) { setParam(name, v); } } break;
			case ShaderPropType::Vec2f:		{ Vec2f		v; if (tryParse(v, defaultValue)) { setParam(name, v); } } break;
			case ShaderPropType::Vec3f:		{ Vec3f		v; if (tryParse(v, defaultValue)) { setParam(name, v); } } break;
			case ShaderPropType::Vec4f:		{ Vec4f		v; if (tryParse(v, defaultValue)) { setParam(name, v); } } break;
			case ShaderPropType::Color4f:	{ Vec4f		v; if (tryParse(v, defaultValue)) { setParam(name, v); } } break;
			default: { RDS_THROW("undefine type {}", type); }
		}
	}

	for (const auto& pass : passes())
	{
		for (auto& cbuf : pass->info().allStageUnionInfo.constBufs)
		{
			for (auto& var : cbuf.variables)
			{
				bool isTexStType = var.dataType == RenderDataType::Float32x4;
				if (!isTexStType)
					continue;

				StrView texStName = var.name;

				auto pos = texStName.rfind(ShaderResources::s_kAutoTextureStNameSuffix);
				if (pos == String::npos)
					continue;

				auto tiling		= Vec2f::s_one();
				auto offset		= Vec2f::s_zero();
				
				auto texStValue = Vec4f{ tiling, offset };
				setParam(texStName, texStValue);
			}
		}
	}
}

void
Material::_internal_resetFrame()
{
	RDS_TODO("putting on RenderCommand::setMaterial() may not be good (cache and loop to reset seems bad too)"
			", since user may setParam()->draw(record)->setParam() in the same frame, maybe add a checking for not allowing doing that is ok");


	for (auto& pass : _passes)
	{
		//pass->_framedShaderRscs.uploadToGpu(&pass->shaderPass());		// this will reset dirty
		pass->_framedShaderRscs._isRotated = false;
	}
}

const ShaderInfo&		Material::info()		const { return _shader->info(); }
const String&			Material::filename()	const { return _shader->filename(); }

#endif

}