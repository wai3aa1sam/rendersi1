#pragma once

#include "rds_engine/common/rds_engine_common.h"
#include "rdsCRenderable.h"

namespace rds
{

class Asset : public Object
{

};

class MeshAsset : public Asset
{
public:
	RenderMesh rdMesh;
};

#if 0
#pragma mark --- rdsCRenderableMesh-Decl ---
#endif // 0
#if 1

class CRenderableMesh : public CRenderable
{
public:
	SPtr<MeshAsset> meshAsset;
	SPtr<Material>	material;

public:
	CRenderableMesh();
	virtual ~CRenderableMesh();

protected:
	virtual void onRender(RenderRequest& rdReq, Material* mtl) override;
};



#endif

}