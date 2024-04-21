#include "rds_render_api_layer-pch.h"
#include "rdsShaderPermutations.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderPermutationInfo-Decl ---
#endif // 0
#if 1

bool 
ShaderPermutationInfo::isValueExist(StrView val) const
{
	return findValueIdx(val) != s_kInvalid;
}

int 
ShaderPermutationInfo::findValueIdx(StrView val) const
{
	for (int i = 0; i < values.size(); ++i)
	{
		if (StrUtil::isSame(val, values[i]))
			return i;
	}
	return s_kInvalid;
}

#endif

#if 0
#pragma mark --- rdsShaderPermutations-Impl ---
#endif // 0
#if 1

void 
ShaderPermutations::create(const Info& info)
{
	_info = &info;
	_permutParams.resize(info.size());
	for (size_t i = 0; i < _permutParams.size(); i++)
	{
		_permutParams[i].create(info[i]);
	}
}

void 
ShaderPermutations::set(StrView name, StrView value)
{
	int nameIdx		= -1;
	int valueIdx	= -1;

	if (!findIndex(nameIdx, valueIdx, name, value))
	{
		RDS_THROW("unexpected permutation name/value");
	}

	RDS_ASSERT(nameIdx < _permutParams.size());
	_permutParams[nameIdx].set(valueIdx);
}

bool 
ShaderPermutations::operator==(const ShaderPermutations& permuts) const
{
	//RDS_CORE_ASSERT(_permutParams.size() == permuts.size(), "wrong ShaderPermutations size");
	if (_permutParams.is_empty() || permuts.size() != permuts.size())
		return false;
	for (size_t i = 0; i < _permutParams.size(); i++)
	{
		if (_permutParams[i].valueIdx() != permuts._permutParams[i].valueIdx())
			return false;
	}
	return true;
}
bool 
ShaderPermutations::operator!=(const ShaderPermutations& permuts) const 
{ 
	return !(operator==(permuts)); 
}

void 
ShaderPermutations::clear() 
{ 
	_permutParams.clear(); 
}

bool 
ShaderPermutations::findIndex(int& nameIdx, int& valueIdx, StrView name, StrView value) const
{
	RDS_CORE_ASSERT(_info, "permutation info");
	nameIdx		= ShaderPermutationInfo::s_kInvalid;
	valueIdx	= ShaderPermutationInfo::s_kInvalid;

	for (int i = 0; i < _info->size(); i++)
	{
		if ((*_info)[i].name == name)
			nameIdx = i;
	}
	if (nameIdx == -1)
		return false;

	auto& values = (*_info)[nameIdx].values;
	for (int i = 0; i < values.size(); i++)
	{
		if (values[i] == value)
			valueIdx = i;
	}
	if (valueIdx == ShaderPermutationInfo::s_kInvalid)
		return false;

	return true;
}



#endif

}