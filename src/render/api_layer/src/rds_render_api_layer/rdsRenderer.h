#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderer-Decl ---
#endif // 0
#if 1

#define RenderApiType_ENUM_LIST(E) \
	E(None, = 0) \
	E(OpenGL,) \
	E(Dx11,) \
	E(Metal,) \
	E(Vulkan,) \
	E(Dx12,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderApiType, u8);

struct Renderer_CreateDesc
{
public:
	Renderer_CreateDesc();

public:
	RenderApiType apiType;

	bool isDebug	: 1;
	bool isPresent	: 1;
};


class RenderContext;
struct RenderContext_CreateDesc;

class Renderer : public VirtualSingleton<Renderer>
{
public:
    using Base = VirtualSingleton<Renderer>;

	using CreateDesc = Renderer_CreateDesc;

public:
	static CreateDesc makeCDesc();

public:
	Renderer();
	virtual ~Renderer();

	void create(const CreateDesc& cDesc);
	void destroy();

	SPtr<RenderContext> createContext(const RenderContext_CreateDesc& cDesc);

protected:
	Renderer* _init(const CreateDesc& cDesc);
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

protected:
	virtual SPtr<RenderContext> onCreateContext(const RenderContext_CreateDesc& cDesc) = 0;

protected:
	RenderAdapterInfo	_adapterInfo;

};


#endif


}