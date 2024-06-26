#include "rds_render-pch.h"
#include "rdsRpfClearImage2D.h"

namespace rds
{
SPtr<Shader> RpfClearImage2D::_shaderClearImage2D;

#if 0
#pragma mark --- rdsRpfClearImage2D-Impl ---
#endif // 0
#if 1

RpfClearImage2D::RpfClearImage2D()
{

}

RpfClearImage2D::~RpfClearImage2D()
{
	destroy();
}

void 
RpfClearImage2D::create()
{
	RenderUtil::createShader(&_shaderClearImage2D, "asset/shader/pass_feature/utility/image/rdsClearImage2D.shader");
}

void 
RpfClearImage2D::destroy()
{
	RenderUtil::destroyShader(_shaderClearImage2D);
}

RdgPass* 
RpfClearImage2D::addClearImage2DPass(SPtr<Material>& mtl_, RdgTextureHnd image, const Tuple2u& image_extent, const Tuple2u& image_offset, const Tuple4f& clear_value)
{
	RenderUtil::createMaterial(_shaderClearImage2D, &mtl_);

	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passClearImage2D	= nullptr;

	Material* mtl = mtl_;
	{
		auto passName = fmtAs_T<TempString>("clearImage2D-{}", image.name());
		auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
		pass.writeTexture(image, TextureUsageFlags::UnorderedAccess);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				mtl->setParam("image_extent",			Tuple3u{image_extent.x, image_extent.y, 1});
				mtl->setParam("image_extent_offset",	Tuple3u{image_offset.x, image_offset.y, 0});
				mtl->setParam("clear_value",			clear_value);
				mtl->setImage("image",					image.texture2D(), 0);
				drawData->setupMaterial(mtl);
				rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, Vec3u{image_extent, 1});
			}
		);
		passClearImage2D = &pass;
	}

	return passClearImage2D;
}

RdgPass* 
RpfClearImage2D::addClearImage2DPass(SPtr<Material>& mtl, RdgTextureHnd image)
{
	auto extent = image.size();
	return addClearImage2DPass(mtl, image, Tuple2u{extent.x, extent.y}, Vec2u::s_zero(), Vec4f::s_zero());
}

#endif

}