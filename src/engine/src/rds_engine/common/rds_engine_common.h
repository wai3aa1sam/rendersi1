#pragma once

#include <rds_render.h>

#include "rds_engine-config.h"
#include "rds_engine_traits.h"

namespace rds
{

class EngineContext;
class Entity;

#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1



#endif



#if 0
#pragma mark --- rdsEntityId-Decl ---
#endif // 0
#if 1

struct EntityId
{
	RDS_ENGINE_COMMON_BODY();
public:
	static constexpr SizeType s_kInvalid = 0;

public:
	EntityId() = default;
	EntityId(u32 id) : _id(id) {}
	~EntityId()
	{
		_id = s_kInvalid;
	}

	operator SizeType() const { return _id; }

	EntityId&	operator++()		{ _id++; return *this; }
	EntityId	operator++(int)		{ EntityId id; id._id = _id; ++_id;; return id; }

	bool operator==(EntityId id) const { return _id == id._id; }
	bool operator!=(EntityId id) const { return !operator==(id); }
	bool operator>=(EntityId id) const { return _id >= id._id; }
	bool operator<=(EntityId id) const { return _id <= id._id; }
	bool operator> (EntityId id) const { return _id >  id._id; }
	bool operator< (EntityId id) const { return _id <  id._id; }

private:
	SizeType _id = s_kInvalid;
};

#endif

#if 1

#if 0
#pragma mark --- rdsEcsAllocator-Decl ---
#endif // 0
#if 1

/*
	currently want to assume all allocation is the same size,
	however, for the use of Renderable / RenderableMesh / ..., they may have different size
	the impl maybe different, 
*/
// TODO
class EcsAllocator : public Mallocator
{
public:
	EcsAllocator()
	{
		RDS_TODO("");
	}
};

#endif

#if 0
#pragma mark --- rdsEcsVectorTable-Decl ---
#endif // 0
#if 1

template<class T, class VALUE>
class EcsVectorTable
{
public:
	using SizeType		= EngineTraits::SizeType;

	//template<class U> using SPtr = SPtr<U, DefaultDestructor<U> >;
	//template<class U> using SPtr = SPtr<U, DefaultDeleter<U> >;

	using Key	= EntityId;
	using Value = VALUE;

	using Table			= VectorMap<Key, Value >;
	using Elements		= Vector<T*>;
	using IndexTable	= VectorMap<Key, SizeType>;

public:
	EcsVectorTable();
	~EcsVectorTable();

	void create(EcsAllocator* ecsAlloc);
	void destroy();

	//template<class U, class... ARGS>	U*		newElement(Entity* entity,	ARGS&&... args);
	template<class U, class... ARGS>	U*		newElement(const Key& key,		ARGS&&... args);
	void	removeElement(const Key& key);


	T* findElement(const Key& key);

public:
			Elements&	elements();
	const	Elements&	elements() const;

	SizeType	size() const;

private:
	EcsAllocator*	_ecsAlloc = nullptr;
	// ALLOC* _alloc;
	Table			_table;
	Elements		_elements;
	IndexTable		_indexTable;
};

template<class T, class VALUE> inline
EcsVectorTable<T, VALUE>::EcsVectorTable()
{

}

template<class T, class VALUE> inline
EcsVectorTable<T, VALUE>::~EcsVectorTable()
{

}

template<class T, class VALUE> inline
void 
EcsVectorTable<T, VALUE>::create(EcsAllocator* ecsAlloc)
{
	_ecsAlloc = ecsAlloc;
}

template<class T, class VALUE> inline
void 
EcsVectorTable<T, VALUE>::destroy()
{
	_ecsAlloc = nullptr;

	// free by SPtr now, if use DefaultDestructor, then need to loop and _ecsAlloc to free, still thinking

	_elements.clear();
	_indexTable.clear();
	_table.clear();
}

//template<class T, class VALUE> 
//template<class U, class... ARGS> inline
//U*		
//EcsVectorTable<T, VALUE>::newElement(Entity* entity, ARGS&&... args)
//{
//	auto* obj = newElement<U>(entity->id(), rds::forward<ARGS>(args)...);
//	obj->create(entity);
//	return obj;
//}

template<class T, class VALUE> 
template<class U, class... ARGS> inline
U*		
EcsVectorTable<T, VALUE>::newElement(const Key& key, ARGS&&... args)
{
	auto* buf = _ecsAlloc->alloc(sizeof(U));
	auto* obj = new(buf) U(rds::forward<ARGS>(args)...);

	_table[key]			= Value(sCast<U*>(obj));
	_indexTable[key]	= _elements.size();
	_elements.emplace_back(obj);

	return obj;
}

template<class T, class VALUE> inline
void	
EcsVectorTable<T, VALUE>::removeElement(const Key& key)
{
	T* p = findElement(key);
	if (!p)
		return;

	RDS_CORE_ASSERT(!_elements.is_empty(), "");

	//auto size		= _elements.size();
	auto removeIdx	= _indexTable[key];
	auto lastIdx	= size() - 1;

	auto lastKey = _elements[lastIdx]->id();		// must overload a id() for type T
	_indexTable[lastKey] = removeIdx;

	// erase unsort, if the remove is not last, otherwise has bug
	if (removeIdx != lastIdx)
	{
		rds::swap(_elements[removeIdx], _elements[lastIdx]);
	}
	_elements.pop_back();
	_indexTable.erase(key);

	_table.erase(key);
	//_ecsAlloc->free(p);		// free by SPtr
}

template<class T, class VALUE> inline
T* 
EcsVectorTable<T, VALUE>::findElement(const Key& key)
{
	#if 1
	auto it = _table.find(key);
	if (it == _table.end())
		return nullptr;
	auto trg = it->second;
	return trg;
	#else
	// overhead as there is a second memory access
	auto it = _indexTable.find(key);
	if (it == _indexTable.end())
		return nullptr;
	return _elements[it->second];
	#endif // 0
}

template<class T, class VALUE> inline typename			EcsVectorTable<T, VALUE>::Elements&	EcsVectorTable<T, VALUE>::elements()		{ return _elements; }
template<class T, class VALUE> inline typename const	EcsVectorTable<T, VALUE>::Elements&	EcsVectorTable<T, VALUE>::elements() const	{ return _elements; }

template<class T, class VALUE> inline typename EcsVectorTable<T, VALUE>::SizeType	EcsVectorTable<T, VALUE>::size()		const	{ return _elements.size(); }

#endif

#endif // 1


}
