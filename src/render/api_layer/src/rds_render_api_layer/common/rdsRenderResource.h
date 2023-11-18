#pragma once

#include "rdsRenderDataType.h"

namespace rds
{

class RenderResource;

class Renderer;
class RenderDevice;

class RenderFrame;
class RenderRequest;

class TransferRequest;
class TransferFrame;
class TransferContext;


#if 0
#pragma mark --- rdsRenderResource-Decl ---
#endif // 0
#if 1

struct RenderResource_CreateDesc
{
	friend class RenderResource;
	friend class RenderDevice;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	void _internal_create(RenderDevice* rdDev, bool isBypassChecking = false) const
	{
		_rdDev = rdDev;
		_isBypassChecking = isBypassChecking;
	}

protected:
	
protected:
	mutable RenderDevice*	_rdDev				= nullptr;
	mutable bool			_isBypassChecking	= false;
};

class RenderResource : public RefCount_Base
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
	friend class Renderer;
public:
	using CreateDesc = RenderResource_CreateDesc;

public:
	static constexpr bool s_kIsRenderResource = true;

public:
	RenderResource();
	virtual ~RenderResource();

public:
	void create(const RenderResource_CreateDesc& cDesc);
	void create(RenderDevice* rdDev);
	void destroy();

public:
	bool hasCreated() const;

public:
	Renderer*		renderer();
	RenderDevice*	renderDevice();

	RenderFrame&		renderFrame();

	TransferContext&	transferContext();
	TransferRequest&	transferRequest();

protected:
	RenderDevice* _rdDev = nullptr;
};


#endif

}