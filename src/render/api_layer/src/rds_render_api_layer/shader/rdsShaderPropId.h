#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"


namespace rds
{

#define RDS_NameId_STATIC(name) ([] { static auto _name = ShaderPropId::make(name); return _name; }())

#if 0
#pragma mark --- rdsShaderPropId-Decl ---
#endif // 0
#if 1

// TODO: NameId and PersistentString, instead of this "ShaderPropId"

struct ShaderPropId
{
public:
	using Id = i64;

public:
	static constexpr Id s_kInvalid = -1;

public:
	static ShaderPropId makeInvalid();
	static ShaderPropId make(Id id);
	static ShaderPropId make(Id id, StrView name);
	static ShaderPropId make(StrView name);

public:
	Id	getId() const;

	bool operator==(const ShaderPropId& other) const;
	bool operator!=(const ShaderPropId& other) const;
	bool operator<( const ShaderPropId& other) const;

public:
	const char* Debug_getName() const;

protected:
	ShaderPropId(Id id);
	ShaderPropId(Id id, StrView name);

protected:
	Id _id = s_kInvalid;

	struct Debug
	{
		TempString name;
	};
	#if RDS_DEBUG
	Debug _debug;
	#endif
};

#endif

inline bool						ShaderPropId::operator==(const ShaderPropId& other) const { return _id == other._id; }
inline bool						ShaderPropId::operator!=(const ShaderPropId& other) const { return !operator==(other); }
inline bool						ShaderPropId::operator<(const ShaderPropId& other)	const { return _id < other._id; }

inline ShaderPropId::Id	ShaderPropId::getId()								const { return _id; }

}