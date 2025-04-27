#include "rds_render_api_layer-pch.h"
#include "rdsGpuProfiler.h"

namespace rds
{

template<const char* TFile, const char* TFunc, size_t TLine>
struct SrcLocT
{
	template<class... TArgs>
	SrcLocT(const char* fmt, TArgs&&... args)
	{
		RDS_FMT_STATIC_VAR(str, TempString, fmt, rds::forward<TArgs>(args)...);
		static TempString str = fmtAs_T<TempString>(fmt, rds::forward<TArgs>(args)...);

		RDS_LOG("file: {}, func: {}, line: {}, msg: {}", TFile, TFunc, TLine, str);
		RDS_LOG("file: {}, func: {}, line: {}", TFile, TFunc, TLine);

		/*
		* TODO: pass a TFunc in template args
		*/

		/*
		* example
		* static const char cstrFileArr[] = RDS_FILE;
		* static const char cstrFuncArr[] = RDS_FUNC_NAME_SZ;
		* static SrcLocT<cstrFileArr, cstrFuncArr, RDS_LINE> srcLocT{"hello {}", 123};
		*/
	}
};

const SrcLocData* 
GpuProfiler::addUniqueProfileSection(const SrcLocData& srcLocData)
{
	RDS_CORE_ASSERT(StrUtil::len(srcLocData.name) <= RDS_GPU_PROFILER_MAX_NAME_COUNT, "gpu profile name count > RDS_GPU_PROFILER_MAX_NAME_COUNT");

	if (auto* p = findScLocData(srcLocData))
	{
		return p;
	}

	auto* newSrcLocData = newScLocData(srcLocData);
	return newSrcLocData;
}

const SrcLocData* 
GpuProfiler::findScLocData(const SrcLocData& srcLocData)
{
	auto lock = _srcLocList.scopedSLock();
	auto it = lock->_data.find(&srcLocData);
	return it != lock->_data.end() ? *it : nullptr;
}

const char* 
GpuProfiler::newUnqiueCStr(StrView v)
{
	if (v.is_empty())
		return "";

	auto lock = _cStrList.scopedULock();
	auto* buf = reinCast<char*>(lock->_alloc.alloc((v.size() + 1) * sizeof(char)));
	memory_copy(buf, v.data(), v.size());
	buf[v.size()] = '\0';
	return buf;
}

const SrcLocData* 
GpuProfiler::newScLocData(const SrcLocData& v)
{
	const auto* name = newUnqiueCStr(v.name);

	{
		auto lock = _srcLocList.scopedULock();
		auto* buf = lock->_alloc.alloc(sizeof(SrcLocData));
		auto* obj = new(buf) SrcLocData(v);
		obj->name = name;
		lock->_data.emplace(obj);
		return obj;
	}
}

}