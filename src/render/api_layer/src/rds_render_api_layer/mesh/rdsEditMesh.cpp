#include "rds_render_api_layer-pch.h"
#include "rdsEditMesh.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditMesh-Impl ---
#endif // 0
#if 1
void 
EditMesh::addColors(const Color4b& c) 
{
	for (size_t i = color.size(); i < pos.size(); i++) 
	{
		color.emplace_back(c);
	}
}

void 
EditMesh::clear() 
{
	indices.clear();

	pos.clear();
	for (auto& t : uvs) 
	{
		t.clear();
	}
	color.clear();
	normal.clear();
	tangent.clear();
	binormal.clear();
}
#endif


#if 0
#pragma mark --- rdsEditMeshUtil-Decl ---
#endif // 0
#if 1

EditMesh 
EditMeshUtil::getFullScreenTriangle()
{
	static size_t s_kVtxCount = 3;
	EditMesh editMesh;
	{
		auto& e = editMesh.pos;
		e.reserve(s_kVtxCount);
		e.emplace_back(-1.0f, -1.0f, 0.0f);
		e.emplace_back( 3.0f, -1.0f, 0.0f);
		e.emplace_back(-1.0f,  3.0f, 0.0f);
	}
	{
		auto& e = editMesh.uvs[0];
		e.reserve(s_kVtxCount);
		e.emplace_back(0.0f, 0.0f);
		e.emplace_back(2.0f, 0.0f);
		e.emplace_back(0.0f, 2.0f);
	}

	{
		auto& e = editMesh.normal;
		e.reserve(s_kVtxCount);
		e.emplace_back(0.0f, 0.0f, 1.0f);
		e.emplace_back(0.0f, 0.0f, 1.0f);
		e.emplace_back(0.0f, 0.0f, 1.0f);
	}

	editMesh.indices = { 0, 1, 2 };

	RDS_ASSERT(editMesh.getVertexLayout() == Vertex_PosUvNormal<1>::vertexLayout(), "");
	return editMesh;
}

#endif

}