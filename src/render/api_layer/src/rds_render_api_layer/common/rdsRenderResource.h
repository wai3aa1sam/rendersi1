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

#define RDS_RenderResource_COMMON_BODY(T) \
	T() {} \
	T(const SrcLoc& RDS_DEBUG_SRCLOC_ARG) { RDS_DEBUG_SRCLOC_ASSIGN(); }

struct RenderResource_CreateDesc
{
	friend class RenderResource;
	friend class RenderDevice;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	void _internal_create(RenderDevice* rdDev) const
	{
		_rdDev = rdDev;
	}

	void _internal_create(RenderDevice* rdDev, bool isBypassChecking) const
	{
		_rdDev = rdDev;
		_isBypassChecking = isBypassChecking;
	}

	void _internal_create(RenderDevice* rdDev, bool isBypassChecking, const SrcLoc& debugSrcLoc_) const
	{
		_rdDev = rdDev;
		_isBypassChecking = isBypassChecking;
		RDS_DEBUG_SRCLOC_ASSIGN();
	}

protected:
	
protected:
	mutable RenderDevice*	_rdDev				= nullptr;
	mutable bool			_isBypassChecking	= false;
	mutable RDS_DEBUG_SRCLOC_DECL;
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
	void create(RenderDevice* rdDev, bool isBypassChecking);
	void create(RenderDevice* rdDev, bool isBypassChecking, const SrcLoc& debugSrcLoc_);
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
	RDS_DEBUG_SRCLOC_DECL;
	RenderDevice* _rdDev = nullptr;
};


#endif

}