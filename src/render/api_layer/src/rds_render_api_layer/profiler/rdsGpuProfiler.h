#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#define RDS_USE_GPU_PROFILER 1
#define RDS_GPU_PROFILER_MAX_NAME_COUNT 255

namespace rds
{

using ProfileColor = tracy::Color::ColorType;

struct SrcLocData : public tracy::SourceLocationData
{
public:
	using Base = tracy::SourceLocationData;

public:
	SrcLocData()
	{
		this->color		= ProfileColor::Black;
		this->name		= nullptr;
		this->file		= nullptr;
		this->function	= nullptr;
		this->line		= 0;
	}

	SrcLocData(const SrcLoc& srcLoc, const char* name, ProfileColor color = ProfileColor::Black)
	{
		this->color		= color;
		this->name		= name;
		this->file		= srcLoc.file;
		this->function	= srcLoc.func;
		this->line		= srcLoc.line;
	}

	SrcLocData(const SrcLocData& srcLocData)
	{
		this->color		= srcLocData.color;
		this->name		= srcLocData.name;
		this->file		= srcLocData.file;
		this->function	= srcLocData.function;
		this->line		= srcLocData.line;
	}
};

#if 0
#pragma mark --- rdsGpuProfiler-Decl ---
#endif // 0
#if 1

class GpuProfiler : public NonCopyable
{
public:
	struct SrcLocData_Hash
	{
		using SizeType = CoreTraits::SizeType;
		bool operator()(const SrcLocData* const& lhs, const SrcLocData* const& rhs) const 
		{
			RDS_CORE_ASSERT(lhs && rhs, "");
			return (lhs->color == rhs->color)
				&& (lhs->line  == rhs->line)
				&& StrUtil::isSame(lhs->name,		rhs->name)
				&& StrUtil::isSame(lhs->function,	rhs->function)
				&& StrUtil::isSame(lhs->file,		rhs->file)
				;
		}

		SizeType operator()(const SrcLocData* const& lhs) const
		{
			SizeType o = 0;
			math::hashCombine(o, lhs->color);
			math::hashCombine(o, lhs->line);
			math::hashCombine(o, Hash<const char*>()(lhs->name));
			math::hashCombine(o, Hash<const char*>()(lhs->file));
			math::hashCombine(o, Hash<const char*>()(lhs->function));
			return o;
		}

	};
	struct SrcLocData_EqualTo
	{
		using SizeType = CoreTraits::SizeType;
		bool operator()(const SrcLocData* const& lhs, const SrcLocData* const& rhs) const 
		{
			RDS_CORE_ASSERT(lhs && rhs, "");
			return (lhs->color == rhs->color)
				&& (lhs->line  == rhs->line)
				&& StrUtil::isSame(lhs->name,		rhs->name)
				&& StrUtil::isSame(lhs->function,	rhs->function)
				&& StrUtil::isSame(lhs->file,		rhs->file)
				;
		}

	};

public:
	const SrcLocData* addUniqueProfileSection(const SrcLocData& srcLocData);

public:
	const SrcLocData* findScLocData(const SrcLocData& srcLocData);

private:
	const char*			newUnqiueCStr(StrView v);
	const SrcLocData*	newScLocData(const SrcLocData& v);

private:
	struct SrcLocList
	{
		using Data = UnordSet<const SrcLocData*, SrcLocData_Hash, SrcLocData_EqualTo>;
		LinearAllocator	_alloc;
		Data			_data;
	};
	struct CStrList
	{
		using Data = const char*;
		LinearAllocator	_alloc;
		//Set<Data>		_data;
	};
	SMutexProtected<SrcLocList>					_srcLocList;
	SMutexProtected<CStrList>					_cStrList;
	//SMutexProtected<Vector<const SrcLocData*> >	_staticSrcLocList;
};

#endif

}