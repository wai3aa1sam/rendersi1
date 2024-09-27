#pragma once

#include "rdsRenderDataType.h"
#include "rdsRenderResourceState.h"
#include "rdsRenderResourceType.h"

namespace rds
{

class RenderResource;

class Renderer;
class RenderDevice;

class RenderFrameParam;

class RenderFrame;
class RenderRequest;

class TransferRequest;
class TransferFrame;
class TransferContext;

#define RDS_ENABLE_RenderResouce_DEBUG_NAME 1

#define RDS_RenderResouce_SET_DEBUG_NAME(RSC, NAME) (RSC)->setDebugName(NAME)

#if RDS_DEVELOPMENT

#define RDS_RenderResouce_DEBUG_PARAMS const SrcLoc& debugSrcLoc_, StrView debugName

#else

#define RDS_RenderResouce_DEBUG_PARAMS int, StrView

#endif // RDS_DEVELOPMENT


#if 0
#pragma mark --- rdsRenderResource-Decl ---
#endif // 0
#if 1

#define RDS_RenderResource_CreateDesc_COMMON_BODY(T) \
	T() {} \
	T(const SrcLoc& RDS_DEBUG_SRCLOC_ARG) { RDS_DEBUG_SRCLOC_ASSIGN(); }

#if 0
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

#endif // 0

#if 1

struct Empty {};

template<class BASE>
struct RenderResource_CreateDescT : public BASE
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

using RenderResource_CreateDesc = RenderResource_CreateDescT<Empty>;

#endif // 0

class RenderResource : public RefCount_Base
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
	friend class Renderer;
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
	void create(RenderDevice* rdDev, bool isBypassChecking, const SrcLoc& debugSrcLoc_);
	void destroy();

	virtual void	setDebugName(StrView name);

public:
	bool				hasCreated()			const;
	const char*			debugName()				const;
	RenderApiType		apiType()				const;
	RenderResourceType	renderResourceType()	const;

	u64					engineFrameCount()		const;
	u32					engineFrameIndex()		const;
	u64					frameCount()			const;
	u32					frameIndex()			const;

public:
	Renderer*			renderer();
	RenderDevice*		renderDevice();
	RenderDevice*		renderDevice() const;

	RenderFrameParam&	renderFrameParam();

	RenderFrame&		renderFrame(u64 frameIdx);

	TransferContext&	transferContext();
	TransferRequest&	transferRequest();
	TransferRequest&	transferRequest(u64 frameIdx);

	RenderResourceStateFlags renderResourceStateFlags(u32 subResource = RenderResourceState::s_kAllSubResource) const;

	ProjectSetting& projectSetting();

public:
	void _internal_setSubResourceCount(SizeType n);
	void _internal_setRenderResourceState(RenderResourceStateFlags state, u32 subResource = RenderResourceState::s_kAllSubResource);

protected:
	virtual void onCreateRenderResource();
	virtual void onDestroyRenderResource();

protected:
	RDS_DEBUG_SRCLOC_DECL;
	#if RDS_ENABLE_RenderResouce_DEBUG_NAME
	TempString _debugName;
	#endif // RDS_ENABLE_RenderResouce_DEBUG_NAME

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

	#if RDS_DEVELOPMENT
	RDS_DEBUG_SRCLOC = cDesc._debugSrcLoc;
	#endif // RDS_DEVELOPMENT
}

inline
const char* 
RenderResource::debugName() const
{
	#if RDS_ENABLE_RenderResouce_DEBUG_NAME
	return _debugName.c_str();
	#else
	return "";
	#endif // RDS_ENABLE_RenderResouce_DEBUG_NAME
}

inline RenderDevice*			RenderResource::renderDevice()			{ return _rdDev; }
inline RenderDevice*			RenderResource::renderDevice() const	{ return _rdDev; }

inline RenderResourceStateFlags RenderResource::renderResourceStateFlags(u32 subResource) const { return _rdState.state(subResource); }

inline ProjectSetting& RenderResource::projectSetting() { return *ProjectSetting::instance(); }

#endif

template<class T> 
struct RdsDeleter<T, EnableIf<IsBaseOf<RenderResource, T> > >
{
	static void rds_delete(RenderResource* p) RDS_NOEXCEPT
	{
		if (p)
		{
			p->onDestroyRenderResource();
			rds_delete_impl(p);
		}
	}
};


}