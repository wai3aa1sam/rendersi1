#include "rds_render_api_layer-pch.h"
#include "rdsTextureStock.h"
#include "rdsTexture.h"

namespace rds
{


#if 0
#pragma mark --- rdsTextureStock-Impl ---
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