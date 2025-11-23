#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"


namespace rds
{

#if 0
#pragma mark --- rdsShaderPropId-Decl ---
#endif // 0
#if 1

// TODO: NameId and PersistentString, instead of this "ShaderPropId"

struct ShaderPropId
{
public:
	using SizeType = CoreTraits::SizeType;

public:
	static constexpr SizeType s_kInvalid = NumLimit<SizeType>::max();

public:
	static ShaderPropId makeInvalid();
	static ShaderPropId make(SizeType id);
	static ShaderPropId make(SizeType id, StrView name);
	static ShaderPropId make(StrView name);

public:
	SizeType	getId() const;

	bool operator==(const ShaderPropId& other) const;
	bool operator!=(const ShaderPropId& other) const;
	bool operator<( const ShaderPropId& other) const;

public:
	const char* Debug_getName() const;

protected:
	ShaderPropId(SizeType id);
	ShaderPropId(SizeType id, StrView name);

protected:
	SizeType _id = s_kInvalid;

	struct Debug
	{
		TempString name;
	};
	#if RDS_DEBUG
	Debug _debug;
	#endif
};

struct ShaderPassId : public ShaderPropId
{
public:
	using Base = ShaderPropId;

public:
	ShaderPassId(const ShaderPropId& propId);
};

#endif

inline bool						ShaderPropId::operator==(const ShaderPropId& other) const { return _id == other._id; }
inline bool						ShaderPropId::operator!=(const ShaderPropId& other) const { return !operator==(other); }
inline bool						ShaderPropId::operator<(const ShaderPropId& other)	const { return _id < other._id; }

inline ShaderPropId::SizeType	ShaderPropId::getId()								const { return _id; }

}