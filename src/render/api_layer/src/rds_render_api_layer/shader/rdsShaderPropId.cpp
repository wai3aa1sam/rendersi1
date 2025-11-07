#include "rds_render_api_layer-pch.h"
#include "rdsShaderPropId.h"

namespace rds
{

#if 0
#pragma mark --- rdsShaderPropId-Impl---
#endif // 0
#if 1

ShaderPropId 
ShaderPropId::makeInvalid()
{
	return ShaderPropId(s_kInvalid);
}

ShaderPropId 
ShaderPropId::make(SizeType id)
{
	return ShaderPropId(id);
}

ShaderPropId 
ShaderPropId::make(SizeType id, StrView name)
{
	return ShaderPropId(id, name);
}

ShaderPropId 
ShaderPropId::make(StrView name)
{
	auto h = Hash<const char*>()(name.data());
	return ShaderPropId(h, name);
}

ShaderPropId::ShaderPropId(SizeType id)
{
	_id = id;
}

ShaderPropId::ShaderPropId(SizeType id, StrView name)
{
	_id			= id;
	_debug.name	= name;
}

const char* 
ShaderPropId::Debug_getName() const
{
	#if RDS_DEBUG
	return _debug.name.data();
	#else
	return "";
	#endif // RDS_DEBUG

}

#endif

#if 1

ShaderPassId::ShaderPassId(const ShaderPropId& propId)
	: Base(propId)
{

}

#endif // 1


}