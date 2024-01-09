#include "rds_render_api_layer-pch.h"
#include "rdsRenderGraphPass.h"

namespace rds
{

#if 0
#pragma mark --- rdsRdgPass-Impl ---
#endif // 0
#if 1

RdgPass::RdgPass(RenderGraph* rdGraph, StrView name, int id, RdgPassTypeFlags typeFlag, RdgPassFlag flag)
{
	_rdGraph	= rdGraph;
	_name		= name;
	_id			= id;
	_typeFlags	= typeFlag;
	_flag		= _flag;

	_isCulled		= false;
	_isExecuted		= false;
	_isCommitted	= false;
}

RdgPass::~RdgPass()
{
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
	_executeFunc = nmsp::move(func);
}

void 
RdgPass::setRenderTarget(RdgTextureHnd hnd, RenderTargetLoadOp loadOp, RenderTargetStoreOp storeOp)
{
	RDS_CORE_ASSERT(BitUtil::has(_typeFlags, TypeFlag::Graphics));
	//accessResource(hnd, Access::Write);
	//RDS_CORE_ASSERT(!isDuplicatedHnd(_rdTargets[0].span(), hnd), "RenderPass: {} has repeated read resource {}", _name, hnd.name());
	
	RDS_TODO("check is unqiue");

	auto& dst = _rdTargets.emplace_back();
	dst.loadOp		= loadOp;
	dst.storeOp		= storeOp;
	dst.targetHnd	= hnd;
	dst._localId	= sCast<int>(_rscAccesses.size());

	auto usage = Usage{ TextureFlags::RenderTarget };
	accessResource(hnd, usage, Access::Write, true);
}

//void 
//RdgPass::setRenderTarget(RdgTextureHndSpan hnds)
//{
//	//accessResources(toHndSpan(hnds), Access::Write);
//
//
//}

void 
RdgPass::setDepthStencil(RdgTextureHnd hnd, Access access, RenderTargetLoadOp depthLoadOp, RenderTargetLoadOp stencilLoadOp)
{
	RDS_CORE_ASSERT(!_depthStencil, "depthStencil already set");
	RDS_CORE_ASSERT(BitUtil::has(_typeFlags, TypeFlag::Graphics));
	RDS_CORE_ASSERT(BitUtil::has(hnd.usageFlags(), TextureFlags::DepthStencil));

	_depthStencil.loadOp		= depthLoadOp;
	_depthStencil.stencilLoadOp = stencilLoadOp;
	_depthStencil.targetHnd		= hnd;
	_depthStencil.access		= access;
	_depthStencil._localId		= sCast<int>(_rscAccesses.size());

	auto usage = Usage{ TextureFlags::DepthStencil };
	accessResource(hnd, usage, access, true);
}

void 
RdgPass::readTexture(RdgTextureHnd	hnd)
{
	RDS_CORE_ASSERT(BitUtil::has(hnd.usageFlags(), TextureFlags::ShaderResource), "must have ShaderResource flag");
	auto usage = Usage{ TextureFlags::ShaderResource };
	accessResource(hnd, usage, Access::Read);
}
void 
RdgPass::readTextures(RdgTextureHndSpan hnds)
{
	RDS_CORE_ASSERT(BitUtil::has(hnds[0].usageFlags(), TextureFlags::ShaderResource), "must have ShaderResource flag");
	auto usage = Usage{ TextureFlags::ShaderResource };
	accessResources(toHndSpan(hnds), usage, Access::Read);
}

void
RdgPass::writeTexture(RdgTextureHnd hnd)
{
	auto usage = Usage{ TextureFlags::UnorderedAccess };
	accessResource(hnd, usage, Access::Write);
}

void 
RdgPass::writeTextures(RdgTextureHndSpan hnds)
{
	auto usage = Usage{ TextureFlags::UnorderedAccess };
	accessResources(toHndSpan(hnds), usage, Access::Write);
}

void 
RdgPass::readBuffer(RdgBufferHnd hnd, ShaderStageFlag useStages)
{
	auto usage = Usage{ RenderGpuBufferTypeFlags::Compute };
	if		(BitUtil::has(useStages, ShaderStageFlag::Vertex))	usage.buf = RenderGpuBufferTypeFlags::Vertex;
	else if (BitUtil::has(useStages, ShaderStageFlag::Pixel))	usage.buf = RenderGpuBufferTypeFlags::Const;
	accessResource(hnd, usage, Access::Read);
}

void 
RdgPass::readBuffers(RdgBufferHndSpan hnds, ShaderStageFlag useStages)
{
	auto usage = Usage{ RenderGpuBufferTypeFlags::Compute };
	if		(BitUtil::has(useStages, ShaderStageFlag::Vertex))	usage.buf = RenderGpuBufferTypeFlags::Vertex;
	else if (BitUtil::has(useStages, ShaderStageFlag::Pixel))	usage.buf = RenderGpuBufferTypeFlags::Const;
	accessResources(toHndSpan(hnds), usage, Access::Read);
}

void 
RdgPass::writeBuffer(RdgBufferHnd hnd)
{
	auto usage = Usage{ RenderGpuBufferTypeFlags::Compute };
	accessResource(hnd, usage, Access::Write);
}

void 
RdgPass::writeBuffers(RdgBufferHndSpan hnds)
{
	auto usage = Usage{ RenderGpuBufferTypeFlags::Compute };
	accessResources(toHndSpan(hnds), usage, Access::Write);
}

void 
RdgPass::accessResources(RdgResourceHndSpan hnds, Usage usage, Access access, bool isRenderTarget)
{
	for (auto& e : hnds)
	{
		accessResource(e, usage, access, isRenderTarget);
	}
}

void 
RdgPass::accessResource(RdgResourceHnd hnd, Usage usage, Access access, bool isRenderTarget)
{
	using SRC = Access;

	RDS_TODO("check unique resource");

	auto* rdgRsc = hnd._rdgRsc;

	auto& rscAccess = _rscAccesses.emplace_back();
	rscAccess.rsc				= rdgRsc;
	rscAccess.state.dstUsage	= usage;
	rscAccess.state.dstAccess	= access;

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
				if (!isDuplicatedPass(e->_runBefore, this))
				{
					e->_runBefore.emplace_back(this);
				}
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