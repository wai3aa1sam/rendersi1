#include "rds_render_api_layer-pch.h"
#include "rdsVertexLayoutManager.h"


namespace rds
{

#if 0
#pragma mark --- rdsVertexLayoutManager-Impl ---
#endif // 0
#if 1

VertexLayoutManager* VertexLayoutManager::s_instance = nullptr;

//VertexLayoutManager* VertexLayoutManager::instance()
//{
//	static VertexLayoutManager vlm;
//	return &vlm;
//}

VertexLayoutManager::VertexLayoutManager()
{
	#define REGISTER_VERTEX_4(VERTEX) \
		registerLayout<VERTEX<1> >(); \
		registerLayout<VERTEX<2> >(); \
		registerLayout<VERTEX<3> >(); \
		registerLayout<VERTEX<4> >()

	REGISTER_VERTEX_4(Vertex_PosColorUvNTB);
	REGISTER_VERTEX_4(Vertex_PosColorUvNT);
	REGISTER_VERTEX_4(Vertex_PosColorUvNormal);
	REGISTER_VERTEX_4(Vertex_PosColorUv);
	REGISTER_VERTEX_4(Vertex_PosColor);

	#undef REGISTER_VERTEX_4
}

VertexLayoutManager::~VertexLayoutManager()
{

}

const VertexLayout*
VertexLayoutManager::get(const VertexType& t)
{
	auto* p = find(t);
	RDS_CORE_ASSERT(p, "not yet register");
	return p;
}

const VertexLayout*
VertexLayoutManager::find(const VertexType& t) const
{
	auto it = _layoutTable.find(t);
	if (it == _layoutTable.end())
	{
		return nullptr;
	}
	return &it->second;
}

VertexLayout*
VertexLayoutManager::_createLayout(const VertexType& t)
{
	RDS_CORE_ASSERT(OsTraits::isMainThread(), "only create layout in main thread");
	RDS_CORE_ASSERT(!find(t), "already registered");
	return &_layoutTable[t];
}

#endif
}