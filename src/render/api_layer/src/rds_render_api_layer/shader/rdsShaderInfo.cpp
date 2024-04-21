#include "rds_render_api_layer-pch.h"
#include "rdsShaderInfo.h"

namespace rds
{


#if 0
#pragma mark --- rdsShaderInfo-Impl ---
#endif // 0
#if 1

void 
ShaderInfo::clear()
{
	props.clear();
	passes.clear();
	permuts.clear();
}

bool 
ShaderInfo::isPermutationValid(StrView name, StrView value) const
{
	for (auto& permut : permuts)
	{
		if (!(name == permut.name))
			continue;
		for (auto& v : permut.values)
		{
			if (!(value == v))
				continue;
			return true;
		}
	}
	return false;
}
int 
ShaderInfo::findPermutationNameIdx(StrView name) const
{
	for (int i = 0; i < permuts.size(); i++)
	{
		auto& permut = permuts[i];
		if (name == permut.name)
			return i;
	}
	return -1;
}

#endif

}