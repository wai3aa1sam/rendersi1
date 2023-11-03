#include "rds_render_api_layer-pch.h"
#include "rdsTexture.h"
#include "rds_render_api_layer/rdsRenderer.h"


namespace rds
{

SPtr<Texture2D> 
Renderer::createTexture2D(const Texture2D_CreateDesc& cDesc)
{
	auto p = onCreateTexture2D(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsTexture2D-Impl ---
#endif // 0
#if 1

Texture::Texture(RenderDataType type)
	: _type(type)
{

}

#endif

#if 0
#pragma mark --- rdsTexture2D-Impl ---
#endif // 0
#if 1

Texture2D::CreateDesc	
Texture2D::makeCDesc()
{
	return CreateDesc{};
}

SPtr<Texture2D>
Texture2D::make(const CreateDesc& cDesc)
{
	return Renderer::instance()->createTexture2D(cDesc);
}



Texture2D::Texture2D()
	: Base(RenderDataType::Texture2D)
{
}

Texture2D::~Texture2D()
{
}

void 
Texture2D::create(const CreateDesc& cDesc)
{
	destroy();

	onCreate(cDesc);

	onPostCreate(cDesc);
}

void 
Texture2D::destroy()
{
	onDestroy();
}

void 
Texture2D::onCreate(const CreateDesc& cDesc)
{
	_desc = cDesc;
	_size = cDesc.size;
}

void 
Texture2D::onPostCreate(const CreateDesc& cDesc)
{

}

void 
Texture2D::onDestroy()
{

}

#endif
}