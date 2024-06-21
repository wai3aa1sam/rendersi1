#include "rds_render-pch.h"
#include "rdsRpfClearImage3D.h"

namespace rds
{

SPtr<Shader> RpfClearImage3D::_shaderClearImage3D;

#if 0
#pragma mark --- rdsRpfClearImage3D-Impl ---
#endif // 0
#if 1

RpfClearImage3D::RpfClearImage3D()
{

}

RpfClearImage3D::~RpfClearImage3D()
{
	destroy();
}

void 
RpfClearImage3D::create()
{
	createShader(&_shaderClearImage3D, "asset/shader/pass_feature/utility/image/rdsClearImage3D.shader");
}

void 
RpfClearImage3D::destroy()
{
	//_mtlClearImage3D.reset(nullptr);
	_shaderClearImage3D.reset(nullptr);
}

RdgPass* 
RpfClearImage3D::addClearImage3DPass(SPtr<Material>& mtl_, RdgTextureHnd image, const Tuple3u& image_extent, const Tuple3u& image_offset, const Tuple4f& clear_value)
{
	createMaterial(_shaderClearImage3D, &mtl_);

	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passClearImage3D	= nullptr;

	Material* mtl = mtl_;
	{
		auto passName = fmtAs_T<TempString>("clearImage3D-{}", image.name());
		auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
		pass.writeTexture(image, TextureUsageFlags::UnorderedAccess);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				mtl->setParam("image_extent",			image_extent);
				mtl->setParam("image_extent_offset",	image_offset);
				mtl->setParam("clear_value",			clear_value);
				mtl->setImage("image",					image.texture3D(), 0);
				drawData->setupMaterial(mtl);
				rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, image_extent);
			}
		);
		passClearImage3D = &pass;
	}

	return passClearImage3D;
}

RdgPass* 
RpfClearImage3D::addClearImage3DPass(SPtr<Material>& mtl, RdgTextureHnd image)
{
	return addClearImage3DPass(mtl, image, image.size(), Vec3u::s_zero(), Vec4f::s_zero());
}

#endif

}