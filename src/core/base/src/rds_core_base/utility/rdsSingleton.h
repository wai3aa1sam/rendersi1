#pragma once

#include "rds_core_base/common/rdsClass_Common.h"

namespace rds
{
    
#if 0
#pragma mark --- rdsSingleton-Decl ---
#endif // 0
#if 1
template<class T, class BASE = NonCopyable>
class Singleton : public BASE
{
public:
	using Base = BASE;

public:
	static T* instance();

public:
	static void init();
	static void terminate();

protected:
	Singleton();
	~Singleton();

	static T* s_instance;
};

#endif // 1


#if 0
#pragma mark --- rdsStackSingleton-Decl ---
#endif // 0
#if 1

template<class T, class BASE = NonCopyable>
class StackSingleton : public NonCopyable
{
public:
	static T* instance();

public:
	StackSingleton();
	~StackSingleton();

protected:
	static T* s_instance;
};

#endif

#if 0
#pragma mark --- rdsSingleton-Impl ---
#endif // 0
#if 1

template<class T, class BASE> inline
T* 
Singleton<T, BASE>::instance()
{
	RDS_CORE_ASSERT(s_instance);
	return s_instance;
}

template<class T, class BASE> inline
Singleton<T, BASE>::Singleton()
{
	RDS_CORE_ASSERT(!s_instance, "!s_instance");
	s_instance = sCast<T*>(this);
}

template<class T, class BASE> inline
Singleton<T, BASE>::~Singleton()
{
	RDS_CORE_ASSERT(s_instance == this, "s_instance == this");
	s_instance = nullptr;
}

#endif // 1

#if 0
#pragma mark --- rdsStackSingleton-Impl ---
#endif // 0
#if 1

template<class T, class BASE> inline
T* 
StackSingleton<T, BASE>::instance()
{
	return s_instance;
}

template<class T, class BASE> inline
StackSingleton<T, BASE>::StackSingleton()
{
	RDS_CORE_ASSERT(!s_instance);
	s_instance = sCast<T*>(this);
}

template<class T, class BASE> inline
StackSingleton<T, BASE>::~StackSingleton()
{
	RDS_CORE_ASSERT(s_instance == this);
	s_instance = nullptr;
}
#endif


}