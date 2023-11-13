#pragma once

#include "rdsRenderDataType.h"

namespace rds
{

class Renderer;
class RenderFrame;

class RenderRequest;
class TransferRequest;

class TransferContext;

using RenderDevice = Renderer;

#if 0
#pragma mark --- rdsRenderResource-Decl ---
#endif // 0
#if 1

class RenderResource : public RefCount_Base
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
	friend class Renderer;
public:
	static constexpr bool s_kIsRenderResource = true;

public:
	RenderResource()			= default;
	virtual ~RenderResource()	= default;

	Renderer*		renderer();
	RenderDevice*	device();

	RenderFrame&		renderFrame();

	TransferContext&	transferContext();
	TransferRequest&	transferRequest();

protected:
	RenderDevice* _rdDev = nullptr;
};


#endif

}