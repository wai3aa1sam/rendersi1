#include "rds_render_api_layer-pch.h"
#include "rdsRenderGraphPass.h"
#include "rdsRenderGraph.h"

namespace rds
{

#if 0
#pragma mark --- rdsRdgPass-Impl ---
#endif // 0
#if 1

RdgPass::RdgPass()
{

}

//RdgPass::RdgPass(RenderGraph* rdGraph, StrView name, int id, RdgPassTypeFlags typeFlag, RdgPassFlags flags)
//{
//	_rdGraph	= rdGraph;
//	_name		= name;
//	_id			= id;
//	_typeFlags	= typeFlag;
//	_flags		= flags;
//
//	_isCulled		= false;
//	_isExecuted		= false;
//	_isCommitted	= false;
//}

RdgPass::~RdgPass()
{
	destroy();
}

void 
RdgPass::create(RenderGraph* rdGraph, StrView name, int id, RdgPassTypeFlags typeFlag, RdgPassFlags flags)
{
	destroy();

	_rdGraph	= rdGraph;
	_name		= name;
	_id			= id;
	_typeFlags	= typeFlag;
	_flags		= flags;
}

void 
RdgPass::destroy()
{
	_rdGraph	= nullptr;
	
	_id			= 0;
	_typeFlags	= {};
	_flags		= {};

	_isCulled		= false;
	_isExecuted		= false;
	_isCommitted	= false;

	_reads.clear();
	_writes.clear();
	_runBefore.clear();
	_runAfter.clear();
	_rscAccesses.clear();
	_rdTargets.clear();
	_depthStencil = {};

	_rdReq.reset(nullptr);
	_executeFunc = {};

	_name.clear();
}

void 
RdgPass::execute()
{
	//RDS_ASSERT(_executeFunc, "setExecuteFunc");
	if (!_executeFunc)
	{
		RDS_THROW("RenderGraphPass has not call setExecuteFunc()");
	}
	if (_isExecuted)
		return;
	_executeFunc(_rdReq);
	_isExecuted = true;
}

void 
RdgPass::setExecuteFunc(ExecuteFunc&& func)
{
	_executeFunc = rds::move(func);
}

void 
RdgPass::setRenderTarget(RdgTextureHnd hnd, RenderTargetLoadOp loadOp, RenderTargetStoreOp storeOp)
{
	RDS_CORE_ASSERT(hnd, "invalid RenderTarget hnd");
	RDS_CORE_ASSERT(BitUtil::has(_typeFlags, TypeFlag::Graphics));
	//accessResource(hnd, Access::Write);
	//RDS_CORE_ASSERT(!isDuplicatedHnd(_rdTargets[0].span(), hnd), "RenderPass: {} has repeated read resource {}", _name, hnd.name());
	
	RDS_TODO("check is unqiue");

	auto& dst = _rdTargets.emplace_back();
	dst.loadOp		= loadOp;
	dst.storeOp		= storeOp;
	dst.targetHnd	= hnd;
	dst._localId	= sCast<int>(_rscAccesses.size());

	auto usage = TextureUsageFlags::RenderTarget;
	auto state = StateUtil::make(usage, Access::Write);
	accessResource(hnd, state, true);
}

void 
RdgPass::setDepthStencil(RdgTextureHnd hnd, Access access, RenderTargetLoadOp depthLoadOp, RenderTargetLoadOp stencilLoadOp)
{
	RDS_CORE_ASSERT(hnd, "invalid DepthStencil hnd");
	RDS_CORE_ASSERT(!_depthStencil, "depthStencil already set");
	RDS_CORE_ASSERT(BitUtil::has(_typeFlags, TypeFlag::Graphics));
	RDS_CORE_ASSERT(BitUtil::has(hnd.usageFlags(), TextureUsageFlags::DepthStencil));

	_depthStencil.loadOp		= depthLoadOp;
	_depthStencil.stencilLoadOp = stencilLoadOp;
	_depthStencil.targetHnd		= hnd;
	_depthStencil.access		= access;
	_depthStencil._localId		= sCast<int>(_rscAccesses.size());

	auto usage = TextureUsageFlags::DepthStencil;
	auto state = StateUtil::make(usage, access);
	accessResource(hnd, state, true);
}

void 
RdgPass::readTexture(RdgTextureHnd	hnd, TextureUsageFlags usage, ShaderStageFlag stage)
{
	RDS_TODO("add validation for stage");
	RDS_CORE_ASSERT(BitUtil::has(typeFlags(), RdgPassTypeFlags::Transfer) || BitUtil::has(hnd.usageFlags(), TextureUsageFlags::ShaderResource), "must have ShaderResource flag");
	if (BitUtil::has(typeFlags(), RdgPassTypeFlags::Transfer))
	{
		RDS_CORE_ASSERT(BitUtil::has(hnd.desc().usageFlags, TextureUsageFlags::TransferSrc));
		usage = TextureUsageFlags::TransferSrc;
		stage = ShaderStageFlag::None;
	}
	auto state = StateUtil::make(usage, Access::Read, stage);
	accessResource(hnd, state);
}
void 
RdgPass::readTextures(RdgTextureHndSpan hnds)
{
	RDS_CORE_ASSERT(BitUtil::has(typeFlags(), RdgPassTypeFlags::Transfer) || BitUtil::has(hnds[0].usageFlags(), TextureUsageFlags::ShaderResource), "must have ShaderResource flag");
	auto usage = TextureUsageFlags::ShaderResource;
	auto state = StateUtil::make(usage, Access::Read);
	accessResources(toHndSpan(hnds), state);
}

void
RdgPass::writeTexture(RdgTextureHnd hnd, TextureUsageFlags usage, ShaderStageFlag stage)
{
	RDS_TODO("add validation for stage");
	if (BitUtil::has(typeFlags(), RdgPassTypeFlags::Transfer))
	{
		RDS_CORE_ASSERT(BitUtil::has(hnd.desc().usageFlags, TextureUsageFlags::TransferDst));
		usage = TextureUsageFlags::TransferDst;
		stage = ShaderStageFlag::None;
	}
	auto state = StateUtil::make(usage, Access::Write, stage);
	accessResource(hnd, state);
}

void 
RdgPass::writeTextures(RdgTextureHndSpan hnds)
{
	auto usage = TextureUsageFlags::UnorderedAccess;
	auto state = StateUtil::make(usage, Access::Write);
	accessResources(toHndSpan(hnds), state);
}

void 
RdgPass::readBuffer(RdgBufferHnd hnd, ShaderStageFlag useStages)
{
	auto usage = RenderGpuBufferTypeFlags::Compute;
	if		(BitUtil::has(useStages, ShaderStageFlag::Vertex))	usage = RenderGpuBufferTypeFlags::Vertex;
	else if (BitUtil::has(useStages, ShaderStageFlag::Pixel))	usage = RenderGpuBufferTypeFlags::Const;
	auto state = StateUtil::make(usage, Access::Read);
	accessResource(hnd, state);
}

void 
RdgPass::readBuffers(RdgBufferHndSpan hnds, ShaderStageFlag useStages)
{
	auto usage = RenderGpuBufferTypeFlags::Compute;
	if		(BitUtil::has(useStages, ShaderStageFlag::Vertex))	usage = RenderGpuBufferTypeFlags::Vertex;
	else if (BitUtil::has(useStages, ShaderStageFlag::Pixel))	usage = RenderGpuBufferTypeFlags::Const;
	auto state = StateUtil::make(usage, Access::Read);
	accessResources(toHndSpan(hnds), state);
}

void 
RdgPass::writeBuffer(RdgBufferHnd hnd)
{
	auto state = StateUtil::make(RenderGpuBufferTypeFlags::Compute, Access::Write);
	accessResource(hnd, state);
}

void 
RdgPass::writeBuffers(RdgBufferHndSpan hnds)
{
	auto state = StateUtil::make(RenderGpuBufferTypeFlags::Compute, Access::Write);
	accessResources(toHndSpan(hnds), state);
}

void 
RdgPass::runAfter(RdgPass* pass)
{
	if (!pass)
		return;
	if (!isDuplicatedPass(pass->runAfterThis(), this))
	{
		pass->_runAfter.emplace_back(this);
	}
}

void 
RdgPass::accessResources(RdgResourceHndSpan hnds, RenderResourceStateFlags state, bool isRenderTarget)
{
	for (auto& e : hnds)
	{
		accessResource(e, state, isRenderTarget);
	}
}

void 
RdgPass::accessResource(RdgResourceHnd hnd, RenderResourceStateFlags state, bool isRenderTarget)
{
	RDS_TODO("check unique resource");
	using SRC		= RenderAccess;

	RDS_CORE_ASSERT(hnd._rdgRsc, "invalid hnd");

	auto* rdgRsc = hnd._rdgRsc;

	auto& rscAccess = _rscAccesses.emplace_back();
	rscAccess.rsc				= rdgRsc;
	rscAccess.dstState			= state;

	auto access = StateUtil::getRenderAccess(rscAccess.dstState);

	auto appendUniqueProducerTo = [this](auto& passes, RdgResource* rdgRsc)
		{
			for (auto& e : rdgRsc->producers())
			{
				if (!isDuplicatedPass(passes, e) && e != this)
				{
					passes.emplace_back(e);
				}
			}
		};

	switch (access)
	{
		case SRC::Read:
		{
			RDS_CORE_ASSERT(!isDuplicatedHnd(_reads.span(), hnd), "RenderPass: {} has repeated read resource {}", _name, hnd.name());

			appendUniqueProducerTo(_runBefore, rdgRsc);
			for (auto& e : rdgRsc->producers())
			{
				if (!isDuplicatedPass(e->runAfterThis(), this))
				{
					e->_runAfter.emplace_back(this);
				}
			}

			_reads.emplace_back(rdgRsc);
		} break;

		case SRC::Write:
		{
			RDS_CORE_ASSERT(!isDuplicatedHnd(_writes.span(), hnd), "RenderPass: {} has repeated write resource {}", _name, hnd.name());

			for (auto& e : rdgRsc->producers())
			{
				/*if (!isDuplicatedPass(e->_runBefore, this))
				{
					e->_runBefore.emplace_back(this);
				}*/
				runAfter(e);
			}
			
			rdgRsc->addProducer(this);
			_writes.emplace_back(rdgRsc);
		} break;

		default:
		{
			RDS_THROW("unknown access");
		} break;
	}
}

void 
RdgPass::cull()
{
	_isCulled = true;
}

bool
RdgPass::isDuplicatedHnd(Span<RdgResource*> dst, RdgResourceHnd src)
{
	auto it = find_if(dst.begin(), dst.end()
		, [=](RdgResource* e)
		{
			return e->id() == src.id();
		});
	return it != dst.end();
}

bool
RdgPass::isDuplicatedPass(Span<Pass*> dst, Pass* src)
{
	auto it = find_if(dst.begin(), dst.end()
		, [=](Pass* e)
		{
			return e->id() == src->id();
		});
	return it != dst.end();
}

bool 
RdgPass::isDuplicatedPasses(Span<Pass*> dst, Span<Pass*> src)
{
	for (auto& e : src)
	{
		if (isDuplicatedPass(dst, e))
		{
			return true;
		}
	}
	return false;
}

void 
RdgPass::_internal_commit()
{
	_isCommitted = true;
}


#endif

}