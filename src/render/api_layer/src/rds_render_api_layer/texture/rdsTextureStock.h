#pragma once
#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/shader/rdsBindlessResourceHandle.h"

namespace rds
{

class RenderDevice;
class Texture;
class Texture2D;
class Texture2DArray;
class Texture3D;
class TextureCube;

#if 0
#pragma mark --- rdsTextureStock-Decl ---
#endif // 0
#if 1

class TextureStock
{
public:
	SPtr<Texture2D>			white;
	SPtr<Texture2D>			black;
	SPtr<Texture2D>			red;
	SPtr<Texture2D>			green;
	SPtr<Texture2D>			blue;
	SPtr<Texture2D>			magenta;
	SPtr<Texture2D>			error;
	SPtr<Texture2DArray>	errorArray;
	SPtr<Texture3D>			error3D;
	SPtr<TextureCube>		errorCube;

public:
	~TextureStock();

	void create(RenderDevice* rdDev);
	void destroy();

	SPtr<Texture2D>			createSolidColorTexture2D(			RDS_DebugLabel_PARAM, const Color4b& color);
	SPtr<Texture2D>			createCheckerboardTexture2D(		RDS_DebugLabel_PARAM, const Color4b& color);
	SPtr<Texture2DArray>	createCheckerboardTexture2DArray(	RDS_DebugLabel_PARAM, const Color4b& color);
	SPtr<Texture3D>			createCheckerboardTexture3D(		RDS_DebugLabel_PARAM, const Color4b& color);
	SPtr<TextureCube>		createCheckerboardTextureCube(		RDS_DebugLabel_PARAM, const Color4b& color);

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

private:
	RenderDevice* _rdDev = nullptr;
};

#endif

}