#pragma once
#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/shader/rdsBindlessResourceHandle.h"

namespace rds
{

class Texture;
class Texture2D;

#if 0
#pragma mark --- rdsTextureStock-Decl ---
#endif // 0
#if 1

class TextureStock
{
public:
	SPtr<Texture2D>	white;
	SPtr<Texture2D>	black;
	SPtr<Texture2D>	red;
	SPtr<Texture2D>	green;
	SPtr<Texture2D>	blue;
	SPtr<Texture2D>	magenta;
	SPtr<Texture2D>	error;

public:
	struct Textures
	{
	public:
		using Table = VectorMap<BindlessResourceHandle::IndexT, Texture*>;

	public:
		~Textures();

		Texture*	add(Texture* v);
		void		remove(Texture* v);

		void		clear();

		Texture*	find(u32 rscIdx);

	protected:
		MutexProtected<Table> _table;
	};

public:
	Textures textures;
};

#endif

}