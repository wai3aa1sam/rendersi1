#pragma once

#include "rds_core_base/common/rds_core_base_common.h"


#include "rds_core_base/profiler/rds_profiler.h"
#include "rds_core_base/stl/rds_stl.h"
#include "rds_core_base/os/rds_os.h"
#include "rds_core_base/log/rds_log.h"
#include "rds_core_base/math/rds_math.h"
#include "rds_core_base/file_io/rds_file_io.h"
#include "rds_core_base/job_system/rds_job_system.h"

namespace rds
{

#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1



#endif

/*
	using similar concept, can make PropertyDrawer using template for drawer like JsonIo
	, but when want to override the drawing, then it maybe worst
	, specialization can define in .cpp too
*/

/*
	temporary,
	should put in nmsp?
*/

#if 0
#pragma mark --- TypeInfo-Impl ---
#endif // 0
#if 1

class TypeInfo
{
public:
};

template<class T>
class TypeInfoT : public TypeInfo
{
public:
	static const char* getName() { RDS_CORE_ASSERT(false, "TypeInfo undefine type"); return ""; }
};


#define RDS_Define_TypeInfo(T)									\
template<>														\
class TypeInfoT<T> : public TypeInfo							\
{																\
public:															\
	static const char* getName() { return RDS_STRINGIFY(T); }	\
};																\
																\
class RDS_CONCAT(TypeInfoT_, T) : public TypeInfoT<T>			\
{																\
public:															\
};																\
// ---

//class TypeInfoT_int : public TypeInfoT<int>
//{
//public:
//	static const char* getName() { return "int"; }
//};

RDS_Define_TypeInfo(bool);
RDS_Define_TypeInfo(i32);
RDS_Define_TypeInfo(u32);
RDS_Define_TypeInfo(u64);
RDS_Define_TypeInfo(float);
RDS_Define_TypeInfo(Vec2f);
RDS_Define_TypeInfo(Vec3f);
RDS_Define_TypeInfo(Vec4f);
RDS_Define_TypeInfo(Color4f);

template<class T>
const char* getTypeName(const T& v)
{
	//static T
	RDS_LOG("getTypeName: {}", TypeInfoT<T>::getName());
	return TypeInfoT<T>::getName();
}

inline void test_TypeName()
{
	#define RDS_test_TypeName(T)   \
	{							   \
		T* j = new T();				   \
		getTypeName(*j);			   \
	}							   \
	// ---
	RDS_test_TypeName(i32);
	RDS_test_TypeName(u32);
	RDS_test_TypeName(u64);
	RDS_test_TypeName(Vec2f);
	RDS_test_TypeName(Vec3f);
	RDS_test_TypeName(Vec4f);
}

#endif // 1

}