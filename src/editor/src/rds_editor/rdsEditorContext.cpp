#include "rds_editor-pch.h"
#include "rdsEditorContext.h"
#include "ui/property/rdsEditorPropertyDrawer.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditorContext-Impl ---
#endif // 0
#if 1

EditorContext::EditorContext()
{

}

EditorContext::~EditorContext()
{
	destroy();
}

void 
EditorContext::create()
{
	#if 1
	#define RDS_registerEditorPropertyDrawerT(T) \
	{ \
		static RDS_CONCAT(EditorPropertyDrawer_, T) d; \
		Hash<const char*> hash;										\
		auto key = hash(RDS_STRINGIFY(T));							\
		registerPropertyDrawer(key, &d);							\
		registerPropertyDrawer(RenderDataTypeUtil::get<T>(), &d);	\
		registerPropertyDrawer(ShaderPropTypeUtil::get<T>(), &d);	\
	} \
 // ---
	RDS_registerEditorPropertyDrawerT(bool);
	RDS_registerEditorPropertyDrawerT(i32);
	RDS_registerEditorPropertyDrawerT(u64);
	RDS_registerEditorPropertyDrawerT(Vec2f);
	RDS_registerEditorPropertyDrawerT(Vec3f);
	RDS_registerEditorPropertyDrawerT(Vec4f);
	RDS_registerEditorPropertyDrawerT(Color4f);
	RDS_registerEditorPropertyDrawerT(Texture2D);
	#undef RDS_registerEditorPropertyDrawerT
	#endif // 1
}

void 
EditorContext::destroy()
{
	/*for (auto& e : _propDrawerTable)
	{
		_propDrawerTableAlloc.deleteT(e.second);
	}
	_propDrawerTableAlloc.clear();*/
	_propDrawerTable.clear();

}

EditorUiDrawRequest 
EditorContext::makeUiDrawRequest(RenderRequest* rdReq)
{
	EditorUiDrawRequest o;
	o.create(*this, rdReq);
	return o;
}

struct EditorContext_Helper
{
	template<class T, class U>
	static 
	EditorPropertyDrawer* 
	findPropertyDrawer(EditorContext* edtCtx, EditorContext::PropertyDrawerTableT<U>& table, const T& key)
	{
		RDS_CORE_ASSERT(table.size() > 0, "not yet create()");

		auto& propDrawerTable = table;
		auto it = propDrawerTable.find(key);
		if (it == propDrawerTable.end())
		{
			static EditorPropertyDrawer_struct s;
			auto* p = &s;
			edtCtx->registerPropertyDrawer(key, p);
			return p;
		}

		return it->second;
	}
};

EditorPropertyDrawer*	
EditorContext::findPropertyDrawer_cStr(const char* type)
{
	Hash<const char*> hash;
	auto key = hash(type);
	return EditorContext_Helper::findPropertyDrawer(this, _propDrawerTable, key);
}

EditorPropertyDrawer* 
EditorContext::findPropertyDrawer(RenderDataType	v)
{
	return EditorContext_Helper::findPropertyDrawer(this, _propDrawerTable_rdDataType, v);
}

EditorPropertyDrawer* 
EditorContext::findPropertyDrawer(ShaderPropType	v)
{
	return EditorContext_Helper::findPropertyDrawer(this, _propDrawerTable_shaderPropType, v);
}

void 
EditorContext::registerPropertyDrawer(u64 nameHash, EditorPropertyDrawer* drawer)
{
	auto& propDrawerTable = _propDrawerTable;

	auto key = nameHash;
	propDrawerTable[key] = drawer;
}

void 
EditorContext::registerPropertyDrawer(RenderDataType key, EditorPropertyDrawer* drawer)
{
	auto& propDrawerTable = _propDrawerTable_rdDataType;
	propDrawerTable[key] = drawer;
}

void 
EditorContext::registerPropertyDrawer(ShaderPropType key, EditorPropertyDrawer* drawer)
{
	auto& propDrawerTable = _propDrawerTable_shaderPropType;
	propDrawerTable[key] = drawer;
}

#endif

}