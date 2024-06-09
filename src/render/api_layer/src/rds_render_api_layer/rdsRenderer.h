#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderDevice.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderer-Decl ---
#endif // 0
#if 1

struct Renderer_CreateDesc : public RenderDevice_CreateDesc
{
	SizeType		gpuCount			= 1;
	bool			isCompilerShader	= false;		// TODO: remove
};

class Renderer : public Singleton<Renderer>
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
    using Base			= Singleton<Renderer>;
	using CreateDesc	= Renderer_CreateDesc;

public:
	static CreateDesc		makeCDesc();
	static RenderDevice*	rdDev();
	static RenderDevice*	rdDev(u32 i);

public:
	Renderer();
	~Renderer();

	void create(const CreateDesc& cDesc);
	void destroy();

protected:
	Vector<SPtr<RenderDevice>, 8> _rdDevs;
};

#endif


}