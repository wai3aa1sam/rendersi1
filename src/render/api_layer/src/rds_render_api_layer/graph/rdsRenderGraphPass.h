#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderGraphResource.h"
#include "rds_render_api_layer/command/rdsRenderRequest.h"
#include "rds_render_api_layer/pass/rdsRenderTarget.h"

namespace rds
{

class RenderGraph;
class RdgDrawer;

#define RdgPassTypeFlags_ENUM_LIST(E) \
	E(None, = 0) \
	E(Graphics,		= BitUtil::bit<EnumInt<RdgPassTypeFlags> >(0)) \
	E(Compute,		= BitUtil::bit<EnumInt<RdgPassTypeFlags> >(1)) \
	E(AsyncCompute,	= BitUtil::bit<EnumInt<RdgPassTypeFlags> >(2)) \
	E(Transfer,		= BitUtil::bit<EnumInt<RdgPassTypeFlags> >(3)) \
	E(_kMax,		= BitUtil::bit<EnumInt<RdgPassTypeFlags> >(4)) \
//---
RDS_ENUM_CLASS(RdgPassTypeFlags, u8);
RDS_ENUM_ALL_OPERATOR(RdgPassTypeFlags);

#define RdgPassFlag_ENUM_LIST(E) \
	E(None, = 0) \
	E(NeverCull,	= BitUtil::bit<EnumInt<RdgPassFlag> >(0)) \
	E(_kMax,		= BitUtil::bit<EnumInt<RdgPassFlag> >(1)) \
//---
RDS_ENUM_CLASS(RdgPassFlag, u8);
RDS_ENUM_ALL_OPERATOR(RdgPassFlag);

struct RdgRenderTarget
{
	ColorType format() const { return targetHnd.format(); }
	explicit operator bool() const { return targetHnd.resource(); }

	RdgTextureHnd		targetHnd;
	RenderTargetLoadOp	loadOp;
	RenderTargetStoreOp	storeOp;
	int					_localId = -1;
};

struct RdgDepthStencil
{
	ColorType format() const { return targetHnd.format(); }
	explicit operator bool() const { return targetHnd.resource(); }

	RdgTextureHnd		targetHnd;
	RenderTargetLoadOp	loadOp;
	RenderTargetLoadOp	stencilLoadOp;
	RdgAccess			access = RdgAccess::None;
	int					_localId = -1;
};

struct RdgResourceAccess
{
	using Resource	= RdgResource;
	using State		= RdgResourceState;

public:
	Resource*	rsc		= nullptr;
	State		state	= {};
	//State			srcState	= {};
	//State			dstState	= {};
};

#if 0
#pragma mark --- rdsRdgPass-Decl ---
#endif // 0
#if 1

class RdgPass : public NonCopyable
{
	friend class RenderGraph;
	friend class RdgDrawer;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static constexpr SizeType s_kFuncLocalSize = 256;

public:
	using Pass = RdgPass;
	using TypeFlag	= RdgPassTypeFlags;
	using PassFlag	= RdgPassFlag;

	using Access	= RenderAccess;
	using Usage		= RdgResourceUsage;

	using ExecuteFunc		= Function_T<void(RenderRequest& rdReq), s_kFuncLocalSize>;

	// const for std::initlist
	using RdgTextureHndSpan							= Span<const RdgTextureHnd>;
	using RdgBufferHndSpan							= Span<const RdgBufferHnd>;
	using RdgResourceSpan							= Span<const RdgResource*>;
	using RdgResourceHndSpan						= Span<const RdgResourceHnd>;
	template<class T> using RdgResourceHndSpan_T	= Span<const RdgResourceHndT<T> >;

public:
	static constexpr SizeType s_kLocalSize = 6;

public:
	using ResourceAccesses = Vector<RdgResourceAccess, s_kLocalSize>;

public:
	RdgPass(RenderGraph* rdGraph, StrView name, int id, RdgPassTypeFlags typeFlag, RdgPassFlag flag);
	~RdgPass();

	void setExecuteFunc(ExecuteFunc&& func);

	void setRenderTarget(RdgTextureHnd		hnd, RenderTargetLoadOp loadOp, RenderTargetStoreOp storeOp);
	//void setRenderTarget(RdgTextureHndSpan	hnds);
	void setDepthStencil(RdgTextureHnd		hnd, Access access, RenderTargetLoadOp depthLoadOp, RenderTargetLoadOp stencilLoadOp);

	void readTexture	(RdgTextureHnd		hnd);
	void readTextures	(RdgTextureHndSpan	hnds);

	void writeTexture	(RdgTextureHnd		hnd);
	void writeTextures	(RdgTextureHndSpan	hnds);

	void readBuffer		(RdgBufferHnd		hnd);
	void readBuffers	(RdgBufferHndSpan	hnds);

	void writeBuffer	(RdgBufferHnd		hnd);
	void writeBuffers	(RdgBufferHndSpan	hnds);

	RdgPassTypeFlags	typeFlags()			const;
	RdgId				id()				const;
	const String&		name()				const;
	bool				isCulled()			const;
	bool				isExecuted()		const;
	bool				isCommitted()		const;
	bool				isAsyncCompute()	const;

	Span<RdgRenderTarget>		renderTargets();
	RdgDepthStencil*			depthStencil();

	Span<const RdgResourceAccess> resourceAccesses() const;

	Span<RenderCommand*>		commnads();
	const RenderCommandBuffer&	commandBuffer() const;

	Span<RdgResource*>		reads();
	Span<RdgResource*>		writes();

	Span<Pass*>				runAfterThis();

	Opt<Rect2f>				renderTargetExtent() const;

	SizeType dependencyCount() const;

	bool hasRenderPass()				const;
	bool checkValidRenderTargetExtent() const;
	bool hasRenderTargetOrDepth()		const;
	bool hasDependency()				const;

public:
	void _internal_commit();

protected:
	void execute();

	void accessResources(RdgResourceHndSpan hnds,	Usage usage, Access access, bool isRenderTarget = false);
	void accessResource (RdgResourceHnd hnd,		Usage usage, Access access, bool isRenderTarget = false);

	void cull();

	bool isDuplicatedHnd	(Span<RdgResource*> dst,	RdgResourceHnd src);
	bool isDuplicatedPass	(Span<Pass*> dst,			Pass* src);
	bool isDuplicatedPasses	(Span<Pass*> dst,			Span<Pass*> src);

	template<class HND_SPAN>
	RdgResourceHndSpan toHndSpan(HND_SPAN hndTs);

protected:
	String			_name;
	RenderGraph*	_rdGraph = nullptr;

	Vector<RdgResource*,	s_kLocalSize> _reads;
	Vector<RdgResource*,	s_kLocalSize> _writes;
	Vector<Pass*,			s_kLocalSize> _runBefore;
	Vector<Pass*,			s_kLocalSize> _runAfter;

	ResourceAccesses _rscAccesses;

	Vector<RdgRenderTarget, s_kLocalSize>	_rdTargets;
	RdgDepthStencil							_depthStencil;

	u32	_id			= 0;
	int	_refCount	= 0;
	u32	_graphDepth = 0;

	bool _isCulled		: 1;
	bool _isExecuted	: 1;
	bool _isCommitted	: 1;

	RdgPassTypeFlags	_typeFlags;
	RdgPassFlag			_flag;

	//RenderCommandBuffer _cmdBuf;
	RenderRequest	_rdReq;
	ExecuteFunc		_executeFunc;
};

template<class HND_SPAN> inline
RdgPass::RdgResourceHndSpan 
RdgPass::toHndSpan(HND_SPAN hndTs)
{
	using ElementT = typename HND_SPAN::ElementType;
	return RdgResourceHndSpan{reinCast<ElementT*>(hndTs.data()), hndTs.size()};
}

inline RdgPassTypeFlags	RdgPass::typeFlags()		const	{ return _typeFlags; }
inline RdgId			RdgPass::id()				const	{ return _id; }
inline const String&	RdgPass::name()				const	{ return _name; }
inline bool				RdgPass::isCulled()			const	{ return _isCulled; }
inline bool				RdgPass::isExecuted()		const	{ return _isExecuted; }
inline bool				RdgPass::isCommitted()		const	{ return _isCommitted; }
inline bool				RdgPass::isAsyncCompute()	const	{ return BitUtil::has(typeFlags(), RdgPassTypeFlags::AsyncCompute); }

inline Span<RdgRenderTarget>		RdgPass::renderTargets()			{ return _rdTargets; }
inline RdgDepthStencil*				RdgPass::depthStencil()				{ return _depthStencil ? &_depthStencil : nullptr; }

inline Span<const RdgResourceAccess>	RdgPass::resourceAccesses() const	{ return _rscAccesses; }
//inline RdgPass::ResourceAccesses&		RdgPass::resourceAccesses()			{ return _rscAccesses; }

inline Span<RenderCommand*>			RdgPass::commnads()					{ return _rdReq.commands(); }
inline const RenderCommandBuffer&	RdgPass::commandBuffer() const		{ return _rdReq.commandBuffer(); }

inline 
Opt<Rect2f> 
RdgPass::renderTargetExtent()  const
{ 
	Opt<Rect2f> o;
	if (!_rdTargets.is_empty())
	{
		const auto& size = _rdTargets[0].targetHnd.size();
		o = Rect2f { Tuple2f::s_zero(), Tuple2f::s_cast(size) };
	}
	else if (const RdgDepthStencil& depthStencil = _depthStencil)
	{
		const auto& size = depthStencil.targetHnd.size();
		o = Rect2f{ Tuple2f::s_zero(), Tuple2f::s_cast(size) };
	}
	return o; 
}

inline Span<RdgResource*>		RdgPass::reads()	{ return _reads; }
inline Span<RdgResource*>		RdgPass::writes()	{ return _writes; }

inline Span<RdgPass*>			RdgPass::runAfterThis() { return _runAfter; }


//inline Span<RdgResourceAccess>	RdgPass::resourceAccesses()	{ return _rscAccesses.span(); }
//inline RdgPass::ResourceAccesses&		RdgPass::resourceAccesses()			{ return _rscAccesses; }


inline RdgPass::SizeType	RdgPass::dependencyCount()	const	{ return _runBefore.size(); }
//inline bool					RdgPass::hasRenderPass()	const	{ return BitUtil::hasAny(typeFlags(), RdgPassTypeFlags::Graphics); }
inline bool					RdgPass::hasRenderPass()	const	{ return !_rdTargets.is_empty() || _depthStencil; }

inline 
bool 
RdgPass::checkValidRenderTargetExtent() const
{
	//!math::equals(renderTargetExtent().value().size, Tuple2f{0, 0}, Tuple2f{math::epsilon<f32>(), math::epsilon<f32>()});
	RDS_CORE_ASSERT(!hasRenderPass() 
					|| (hasRenderPass() && hasRenderTargetOrDepth() 
						&& renderTargetExtent() && !Vec2f{renderTargetExtent().value().size }.equals0())
		, "Graphics pass should have target");
	return !(!hasRenderPass() 
			|| (hasRenderPass() && hasRenderTargetOrDepth() 
				&& renderTargetExtent() && !Vec2f{renderTargetExtent().value().size }.equals0()));
}

inline bool RdgPass::hasRenderTargetOrDepth()	const { return (!_rdTargets.is_empty() || _depthStencil); }
inline bool RdgPass::hasDependency()			const { return dependencyCount() != 0; }

#endif


}