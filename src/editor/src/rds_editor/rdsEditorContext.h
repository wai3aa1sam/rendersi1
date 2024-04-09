#pragma once

#include "rds_editor/common/rds_editor_common.h"
#include "ui/rdsEditorUiDrawRequest.h"
#include "ui/rdsEntitySelection.h"

namespace rds
{

class EditorPropertyDrawer;
struct EditorContext_Helper;

#if 0
#pragma mark --- rdsEditorContext-Decl ---
#endif // 0
#if 1

class EditorContext : public NonCopyable
{
	friend struct EditorContext_Helper;
public:
	template<class T> using PropertyDrawerTableT	= VectorMap<T,			EditorPropertyDrawer*>;

public:
	EditorContext();
	~EditorContext();

	void create();
	void destroy();

	EditorUiDrawRequest		makeUiDrawRequest(RenderRequest* rdReq);
	EditorPropertyDrawer*	findPropertyDrawer_cStr(const char*		type);
	EditorPropertyDrawer*	findPropertyDrawer(		RenderDataType	v);
	EditorPropertyDrawer*	findPropertyDrawer(		ShaderPropType	v);

	template<class T> EditorPropertyDrawer*	findPropertyDrawer(const T& v);
public:
	EntitySelection& entitySelection();

protected:
	void registerPropertyDrawer(u64				nameHash,	EditorPropertyDrawer* drawer);
	void registerPropertyDrawer(RenderDataType	key,		EditorPropertyDrawer* drawer);
	void registerPropertyDrawer(ShaderPropType	key,		EditorPropertyDrawer* drawer);

private:
	EntitySelection		_entSel;

	//LinearAllocator		_propDrawerTableAlloc;
	PropertyDrawerTableT<u64>				_propDrawerTable;
	PropertyDrawerTableT<RenderDataType>	_propDrawerTable_rdDataType;
	PropertyDrawerTableT<ShaderPropType>	_propDrawerTable_shaderPropType;
};

inline EntitySelection& EditorContext::entitySelection() { return _entSel; }

template<class T> inline
EditorPropertyDrawer*	
EditorContext::findPropertyDrawer(const T& v)
{
	//Hash<const char*> hash;
	//auto key = hash(TypeInfoT<T>::getName());
	const char* name = TypeInfoT<T>::getName(); RDS_UNUSED(name);
	return this->findPropertyDrawer_cStr(name);
}


#endif
}