#pragma once

#include "rds_core_base/common/rds_core_base_common.h"

namespace rds
{
    

#if 0
#pragma mark --- Singleton-Decl ---
#endif // 0
#if 1
template<class T>
class Singleton : public NonCopyable
{
public:
	static T* instance();

public:
	void create(T* p);
	void destroy();

protected:
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
#pragma mark --- Singleton-Impl ---
#endif // 0
#if 1

template<class T> inline
T* 
Singleton<T>::instance()
{
	NMSP_ASSERT(s_instance);
	return s_instance;
}

template<class T> inline
void
Singleton<T>::create(T* p)
{
	NMSP_ASSERT(!s_instance);
	s_instance = p;
}

template<class T> inline
void
Singleton<T>::destroy()
{
	//NMSP_ASSERT(s_instance);
	if (!s_instance)
	{
		return;
	}
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
	NMSP_ASSERT(!s_instance);
	s_instance = sCast<T*>(this);
}

template<class T> inline
StackSingleton<T>::~StackSingleton()
{
	NMSP_ASSERT(s_instance);
	s_instance = nullptr;
}
#endif


}