#pragma once

#include "rdsRenderDataType.h"
#include "rdsRenderResourceState.h"
#include "rdsRenderResourceType.h"
#include "rdsRenderApiLayerCommon_Base.h"

namespace rds
{

class RenderResource;

class Renderer;
class RenderDevice;

class RenderFrameParam;

class RenderFrame;
class RenderRequest;
class RenderGraph;

class TransferRequest;
class TransferFrame;
class TransferContext;

class RenderContext;
class Texture;
class RenderGpuBuffer;

class TransferCommand_SetDebugLabel;

#define RDS_RenderResource_CreateDesc_COMMON_BODY(T) \
	T() {} \
// ---

struct Empty {};

#if 0
#pragma mark --- rdsRenderResource_CreateDesc-Decl ---
#endif // 0
#if 1

template<class BASE>
struct RenderResource_CreateDescT : public BASE, public RenderApiLayerCommon_Base
{
	RDS_DebugLabel_COMMON_BODY();
	friend class RenderResource;
	friend class RenderDevice;
public:
	void _internal_create(RenderDevice* rdDev) const
	{
		_rdDev = rdDev;
	}

	void _internal_create(RDS_DebugLabel_PARAM, RenderDevice* rdDev) const
	{
		_internal_create(rdDev);
		RDS_DebugLabel_ASSIGN();
	}

	void _internal_create(RenderDevice* rdDev, bool isBypassChecking) const
	{
		_internal_create(rdDev);
		_isBypassChecking = isBypassChecking;
	}

protected:
	mutable RenderDevice*	_rdDev				= nullptr;
	mutable bool			_isBypassChecking	= false;
};
using RenderResource_CreateDesc = RenderResource_CreateDescT<Empty>;

#endif

#if 0
#pragma mark --- rdsRenderResource-Decl ---
#endif // 0
#if 1

class RenderResource : public RefCount_Base, public RenderApiLayerCommon_Base
{
	RDS_DebugLabel_COMMON_BODY();
	friend class Renderer;
	friend class RenderGraph;
	template<class T, class ENABLE> friend struct RdsDeleter;
public:
	using CreateDesc	= RenderResource_CreateDesc;
	using StateUtil		= RenderResourceStateFlagsUtil;

public:
	static constexpr bool s_kIsRenderResource = true;

public:
	RenderResource();
	virtual ~RenderResource();

public:
	template<class T> void create(const RenderResource_CreateDescT<T>& cDesc);
	void create(RenderDevice* rdDev);
	void create(RenderDevice* rdDev, bool isBypassChecking);
	void destroy();

	virtual void setDebugLabel(RDS_DebugLabel_PARAM);

public:
	bool				hasCreated()			const;
	RenderApiType		apiType()				const;
	RenderResourceType	renderResourceType()	const;

public:
	Renderer*			renderer();
	RenderDevice*		renderDevice();
	RenderDevice*		renderDevice() const;

	TransferContext&	transferContext();
	TransferContext*	transferContextPtr();
	TransferRequest&	transferRequest();

	RenderResourceStateFlags renderResourceStateFlags(u32 subResource = RenderResourceState::s_kAllSubResource) const;

	ProjectSetting& projectSetting();

protected:
	void setSubResourceCount(SizeType n);
	void setRenderResourceState(RenderResourceStateFlags state, u32 subResource = RenderResourceState::s_kAllSubResource);

public:
	virtual void onRenderResouce_SetDebugLabel(TransferCommand_SetDebugLabel* cmd);

protected:
	virtual void onDestroy();
	void RenderResource_CreateEnd();
	template<class T> static void destroyObject(T* p, typename T::CmdDestroy* cmd);

protected:
	RenderDevice*		_rdDev = nullptr;
	RenderResourceState _rdState;
	RenderResourceType	_rdRscType = RenderResourceType::None;
};

template<class T> inline
void 
RenderResource::create(const RenderResource_CreateDescT<T>& cDesc)
{
	RDS_CORE_ASSERT(!hasCreated() || cDesc._isBypassChecking, "!hasCreated() || cDesc._isBypassChecking");
	RDS_CORE_ASSERT(cDesc._rdDev, "cDesc._rdDev");
	RDS_CORE_ASSERT(!hasCreated() || (cDesc._rdDev && _rdDev && cDesc._rdDev == _rdDev), "RenderDevice is not the same as the original one");

	_rdDev = cDesc._rdDev;

	RenderResource_CreateEnd();

	RDS_DebugLabel_ASSIGN_IMPL(RDS_DebugLabel_VAR_NAME, cDesc.DebugLabel_get());
	setDebugLabel(cDesc.DebugLabel_get());
}

inline RenderDevice*			RenderResource::renderDevice()			{ return _rdDev; }
inline RenderDevice*			RenderResource::renderDevice() const	{ return _rdDev; }

inline ProjectSetting& RenderResource::projectSetting() { return *ProjectSetting::instance(); }

#endif

template<class T> 
struct RdsDeleter<T, EnableIf<IsBaseOf<RenderResource, T> > >
{
	static void rds_delete(T* p) RDS_NOEXCEPT;
};

template<class T> inline
void 
RenderResource::destroyObject(T* p, typename T::CmdDestroy* cmd)
{
	checkRenderThreadExclusive(RDS_SRCLOC);
	p->onTransferCommand_Destroy();
	rds_delete_impl(p);
}

}