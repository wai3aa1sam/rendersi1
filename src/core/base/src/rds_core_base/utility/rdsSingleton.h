#pragma once

#include "rdsClass_Common.h"

namespace rds
{
    
#if 0
#pragma mark --- rdsSingleton-Decl ---
#endif // 0
#if 1
template<class T>
class Singleton : public NonCopyable
{
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

template<class T>
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

template<class T> inline
T* 
Singleton<T>::instance()
{
	RDS_CORE_ASSERT(s_instance);
	return s_instance;
}

template<class T> inline
Singleton<T>::Singleton()
{
	RDS_CORE_ASSERT(!s_instance);
	s_instance = sCast<T*>(this);
}

template<class T> inline
Singleton<T>::~Singleton()
{
	RDS_CORE_ASSERT(s_instance == this);
	s_instance = nullptr;
}

#endif // 1

#if 0
#pragma mark --- rdsStackSingleton-Impl ---
#endif // 0
#if 1

template<class T> inline
T* StackSingleton<T>::instance()
{
	return s_instance;
}

template<class T> inline
StackSingleton<T>::StackSingleton()
{
	RDS_CORE_ASSERT(!s_instance);
	s_instance = sCast<T*>(this);
}

template<class T> inline
StackSingleton<T>::~StackSingleton()
{
	RDS_CORE_ASSERT(s_instance == this);
	s_instance = nullptr;
}
#endif


}