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
	REGISTER_VERTEX_4(Vertex_PosUv);
	REGISTER_VERTEX_4(Vertex_PosUvNormal);
	REGISTER_VERTEX_4(Vertex_PosUvNT);
	REGISTER_VERTEX_4(Vertex_PosUvNTB);

	//REGISTER_VERTEX_4(Vectex_Pos2fColor4fUv);		// this will collide with Vectex_Pos2fUvColor4f, later need account with member offset/index too

	#undef REGISTER_VERTEX_4

	registerLayout<Vertex_ImGui>();
	registerLayout<Vectex_Pos2fUvColor4f<1> >();
	registerLayout<Vertex_Pos>();
	registerLayout<VertexT_Pos<Tuple4f> >();
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