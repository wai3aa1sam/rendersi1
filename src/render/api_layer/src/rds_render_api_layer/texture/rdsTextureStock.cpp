#include "rds_render_api_layer-pch.h"
#include "rdsTextureStock.h"
#include "rdsTexture.h"

#include "rds_render_api_layer/rdsRenderDevice.h"

namespace rds
{


#if 0
#pragma mark --- rdsTextureStock-Impl ---
#endif // 0
#if 1

TextureStock::~TextureStock()
{
	destroy();
}

void 
TextureStock::create(RenderDevice* rdDev)
{
	_rdDev = rdDev;
	{
		error		= createCheckerboardTexture2D(	Color4b(255, 0,   220, 0));
		white		= createSolidColorTexture2D(	Color4b(255, 255, 255, 255));
		black		= createSolidColorTexture2D(	Color4b(0,   0,   0,   255));
		red			= createSolidColorTexture2D(	Color4b(255, 0,   0,   255));
		green		= createSolidColorTexture2D(	Color4b(0,   255, 0,   255));
		blue		= createSolidColorTexture2D(	Color4b(0,   0,   255, 255));
		magenta		= createSolidColorTexture2D(	Color4b(255, 0,   255, 255));
	}
}

void 
TextureStock::destroy()
{
	error.reset(nullptr);
	white.reset(nullptr);
	black.reset(nullptr);
	red.reset(nullptr);
	green.reset(nullptr);
	blue.reset(nullptr);
	magenta.reset(nullptr);
}

SPtr<Texture2D>	
TextureStock::createSolidColorTexture2D(const Color4b& color)
{
	#if 1
	int w = 4;
	int h = 4;
	auto texDesc = Texture2D::makeCDesc(RDS_SRCLOC);
	texDesc.format		= ColorType::RGBAb;
	texDesc.mipCount	= 1;
	texDesc.size.set(w, h, 1);

	auto& image = texDesc.uploadImage;
	image.create(Color4b::s_kColorType, w, h);

	for (int y = 0; y < h; y++) 
	{
		auto span = image.row<Color4b>(y);
		for (int x = 0; x < w; x++) 
		{
			span[x] = color;
		}
	}
	return _rdDev->createTexture2D(texDesc);
	#else
	return nullptr;
	#endif // 0
}

SPtr<Texture2D>	
TextureStock::createCheckerboardTexture2D(const Color4b& color)
{
	int w = 4;
	int h = 4;
	auto texDesc = Texture2D::makeCDesc(RDS_SRCLOC);
	texDesc.format		= ColorType::RGBAb;
	texDesc.mipCount	= 1;
	texDesc.size.set(w, h, 1);

	auto& image = texDesc.uploadImage;
	image.create(Color4b::s_kColorType, w, h);

	auto colorA_factor = sCast<u8>(1);
	auto colorB_factor = sCast<u8>(0);

	Color4b colorA = Color4b{ sCast<u8>(colorA_factor * color.r), sCast<u8>(colorA_factor * color.g), sCast<u8>(colorA_factor * color.b), color.a};
	Color4b colorB = Color4b{ sCast<u8>(colorB_factor * color.r), sCast<u8>(colorB_factor * color.g), sCast<u8>(colorB_factor * color.b), color.a};

	for (int y = 0; y < h; y++) 
	{
		auto span = image.row<Color4b>(y);
		for (int x = 0; x < w; x++) 
		{
			span[x] = ((x + y) % 2 == 0) ? colorA : colorB;
		}
	}
	return _rdDev->createTexture2D(texDesc);
}

#endif


#if 0
#pragma mark --- rdsTextureStock::Textures-Impl ---
#endif // 0
#if 1

TextureStock::Textures::~Textures()
{
	clear();
}

Texture* 
TextureStock::Textures::add(Texture* v)
{
	RDS_CORE_ASSERT(v, "invalid texture");
	auto& table = _table;
	auto lock	= table.scopedULock();
	auto& data	= *lock;
	auto rscIdx = v->bindlessHandle().getResourceIndex();
	data[rscIdx] = v;
	return v;
}

void 
TextureStock::Textures::remove(Texture* v)
{
	if (!v || !find(v->bindlessHandle().getResourceIndex()))
		return;

	RDS_CORE_ASSERT(v, "invalid texture");
	auto& table = _table;
	auto lock	= table.scopedULock();
	auto& data	= *lock;
	auto rscIdx = v->bindlessHandle().getResourceIndex();
	data.erase(rscIdx);
}

void 
TextureStock::Textures::clear()
{
	auto& table = _table;
	auto lock	= table.scopedULock();
	auto& data	= *lock;
	data.clear();
}

Texture* 
TextureStock::Textures::find(u32 rscIdx)
{
	auto& table = _table;
	auto lock	= table.scopedULock();
	auto& data	= *lock;
	auto it = data.find(rscIdx);
	if (it == data.end())
	{
		return nullptr;
	}
	return it->second;
}

#endif

}