#include "rds_editor-pch.h"
#include "rdsEntitySelection.h"

namespace rds
{

#if 0
#pragma mark --- rdsEntitySelection-Impl ---
#endif // 0
#if 1

void 
EntitySelection::add(const Entity* ent)
{
	if (has(ent))
	{
		return;
	}

	auto id = ent->id();
	_entities.emplace_back(id);
	_entityTable.emplace(id);
}

void 
EntitySelection::select(const Entity* ent)
{
	deselectAll();
	add(ent);
}

void
EntitySelection::deselectAll()
{
	_entities.clear();
	_entityTable.clear();
}

bool 
EntitySelection::has(const Entity* ent) const
{
	auto it = _entityTable.find(ent->id());
	if (it == _entityTable.end())
	{
		return false;
	}
	return true;
}


#endif

}