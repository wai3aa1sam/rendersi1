#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderGraphResource.h"
#include "rds_render_api_layer/command/rdsRenderCommand.h"

namespace rds
{

class RenderGraph;
class RdgDrawer;

#define RdgPassTypeFlag_ENUM_LIST(E) \
	E(None, = 0) \
	E(Graphics,		= BitUtil::bit<EnumInt<RdgPassTypeFlag> >(0)) \
	E(Compute,		= BitUtil::bit<EnumInt<RdgPassTypeFlag> >(1)) \
	E(AsyncCompute,	= BitUtil::bit<EnumInt<RdgPassTypeFlag> >(2)) \
	E(Transfer,		= BitUtil::bit<EnumInt<RdgPassTypeFlag> >(3)) \
	E(_kMax,		= BitUtil::bit<EnumInt<RdgPassTypeFlag> >(4)) \
//---
RDS_ENUM_CLASS(RdgPassTypeFlag, u8);
RDS_ENUM_ALL_OPERATOR(RdgPassTypeFlag);

#define RdgPassFlag_ENUM_LIST(E) \
	E(None, = 0) \
	E(NeverCull,	= BitUtil::bit<EnumInt<RdgPassFlag> >(0)) \
	E(_kMax,		= BitUtil::bit<EnumInt<RdgPassFlag> >(1)) \
//---
RDS_ENUM_CLASS(RdgPassFlag, u8);
RDS_ENUM_ALL_OPERATOR(RdgPassFlag);

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
	using Access	= RdgResourceAccess;
	using TypeFlag	= RdgPassTypeFlag;
	using PassFlag	= RdgPassFlag;

	using ExecuteFunc		= Function_T<void(), s_kFuncLocalSize>;

	// const for std::initlist
	using RdgTextureHndSpan							= Span<const RdgTextureHnd>;
	using RdgBufferHndSpan							= Span<const RdgBufferHnd>;
	using RdgResourceSpan							= Span<const RdgResource*>;
	using RdgResourceHndSpan						= Span<const RdgResourceHnd>;
	template<class T> using RdgResourceHndSpan_T	= Span<const RdgResourceHndT<T> >;

public:
	static constexpr SizeType s_kLocalSize = 6;

public:
	RdgPass(RenderGraph* rdGraph, StrView name, int id, RdgPassTypeFlag typeFlag, RdgPassFlag flag);
	~RdgPass();

	void setExecuteFunc(ExecuteFunc&& func);

	void setRenderTarget(RdgTextureHnd		hnd);
	void setRenderTarget(RdgTextureHndSpan	hnds);
	void setDepthStencil(RdgTextureHnd		hnd, Access access);

	void readTexture	(RdgTextureHnd		hnd);
	void readTextures	(RdgTextureHndSpan	hnds);

	void writeTexture	(RdgTextureHnd		hnd);
	void writeTextures	(RdgTextureHndSpan	hnds);

	void readBuffer		(RdgBufferHnd		hnd);
	void readBuffers	(RdgBufferHndSpan	hnds);

	void writeBuffer	(RdgBufferHnd		hnd);
	void writeBuffers	(RdgBufferHndSpan	hnds);

	RdgId	id()		const;
	StrView name()		const;
	bool	isCulled()	const;

protected:
	void execute();

	void accessResources(RdgResourceHndSpan hnds,	Access access);
	void accessResource (RdgResourceHnd hnd,		Access access);

	void cull();

	bool isDuplicatedHnd	(Span<RdgResource*> dst,	RdgResourceHnd src);
	bool isDuplicatedPass	(Span<Pass*> dst,			Pass* src);
	bool isDuplicatedPasses	(Span<Pass*> dst,			Span<Pass*> src);

	template<class T>
	RdgResourceHndSpan toHndSpan(RdgResourceHndSpan_T<T> hndTs);

protected:
	Vector<RdgResource*,	s_kLocalSize> _reads;
	Vector<RdgResource*,	s_kLocalSize> _writes;
	Vector<Pass*,			s_kLocalSize> _runBefore;
	Vector<Pass*,			s_kLocalSize> _runAfter;

	RenderGraph* _rdGraph = nullptr;

	String	_name;
	u32		_id			= 0 ;
	int		_refCount	= 0;

	bool _isCulled : 1;
	RdgPassTypeFlag		_typeFlag;
	RdgPassFlag			_flag;

	RenderCommandBuffer _cmdBuf;
	ExecuteFunc			_executeFunc;
};
//
//template<class T> inline
//const RdgResourceHndT<T>*
//RdgPass::findDuplicateHnd(Span<RdgResource*> from, RdgResourceHndSpan_T<T> hnds)
//{
//	return findDuplicateHnd(from, toHndSpan(hnds));
//}

template<class T> inline
RdgPass::RdgResourceHndSpan 
RdgPass::toHndSpan(RdgResourceHndSpan_T<T> hndTs)
{
	using ElementT = typename RdgResourceHndSpan::ElementType;
	return RdgResourceHndSpan{reinCast<ElementT*>(hndTs.data()), hndTs.size()};
}

inline RdgId	RdgPass::id()		const	{ return _id; }
inline StrView	RdgPass::name()		const	{ return _name; }
inline bool		RdgPass::isCulled() const	{ return _isCulled; }


#endif


}