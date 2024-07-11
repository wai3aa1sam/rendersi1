#pragma once

#include "rds_engine/common/rds_engine_common.h"
#include "rdsCRenderable.h"
#include "rds_engine/asset/mesh/rdsMeshAsset.h"

namespace rds
{


#if 0
#pragma mark --- rdsCRenderableMesh-Decl ---
#endif // 0
#if 1

class CRenderableMesh : public CRenderable
{
public:
	static constexpr u32 s_kInvalidSubMeshIndex = NumLimit<u32>::max();

public:
	SPtr<MeshAsset> meshAsset;
	SPtr<Material>	material;
	u32				subMeshIndex	= s_kInvalidSubMeshIndex;
	u32				materialIndex	= s_kInvalidSubMeshIndex;

public:
	CRenderableMesh();
	virtual ~CRenderableMesh();

protected:
	virtual void onRender(RenderRequest& rdReq, Material* mtl, DrawData* drawData, const DrawSettings& drawSettings) override;
};



#endif

}