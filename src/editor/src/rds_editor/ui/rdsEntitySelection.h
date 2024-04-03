#pragma once

#include "rds_editor/common/rds_editor_common.h"

/*
references:
~ EntitySelection.h in https://github.com/SimpleTalkCpp/SimpleGameEngine/tree/main
*/

namespace rds
{
#if 0
#pragma mark --- rdsEntitySelection-Decl ---
#endif // 0
#if 1

class EntitySelection : public NonCopyable
{
public:
	void add(const Entity* ent);
	void select(const Entity* ent);
	void deselectAll();

public:
	bool has(const Entity* ent) const;

			Vector<EntityId>& entities();
	const	Vector<EntityId>& entities() const;


private:
	Vector<EntityId>	_entities;
	Set<EntityId>		_entityTable;
};

inline 			Vector<EntityId>& EntitySelection::entities()			{ return _entities; }
inline const	Vector<EntityId>& EntitySelection::entities() const		{ return _entities; }



#endif
}