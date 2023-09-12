#include "rds_render_api_layer-pch.h"
#include "rdsRenderGraphPass.h"

namespace rds
{

#if 0
#pragma mark --- rdsRdgPass-Impl ---
#endif // 0
#if 1

RdgPass::RdgPass(RenderGraph* rdGraph, StrView name, int id, RdgPassTypeFlag typeFlag, RdgPassFlag flag)
{
	_rdGraph	= rdGraph;
	_name		= name;
	_id			= id;
	_typeFlag	= typeFlag;
	_flag		= _flag;

	_isCulled	= false;
}

RdgPass::~RdgPass()
{
}

void RdgPass::execute()
{
	//RDS_ASSERT(_executeFunc, "setExecuteFunc");
	if (!_executeFunc)
	{
		RDS_THROW("RenderGraphPass has not call setExecuteFunc()");
	}
	_executeFunc();
}

void 
RdgPass::setExecuteFunc(ExecuteFunc&& func)
{
	_executeFunc = nmsp::move(func);
}

void 
RdgPass::setRenderTarget(RdgTextureHnd hnd)
{
	RDS_CORE_ASSERT(BitUtil::has(_typeFlag, TypeFlag::Graphics));
	accessResource(hnd, Access::Write);
}

void 
RdgPass::setRenderTarget(RdgTextureHndSpan hnds)
{
	accessResources(toHndSpan(hnds), Access::Write);
}

void 
RdgPass::setDepthStencil(RdgTextureHnd hnd, Access access)
{
	RDS_CORE_ASSERT(BitUtil::has(_typeFlag, TypeFlag::Graphics));
	accessResource(hnd, access);
}

void 
RdgPass::readTexture(RdgTextureHnd	hnd)
{
	accessResource(hnd, Access::Read);
}
void 
RdgPass::readTextures(RdgTextureHndSpan hnds)
{
	accessResources(toHndSpan(hnds), Access::Read);
}

void RdgPass::writeTexture(RdgTextureHnd hnd)
{
	accessResource(hnd, Access::Write);
}

void RdgPass::writeTextures(RdgTextureHndSpan hnds)
{
	accessResources(toHndSpan(hnds), Access::Write);
}

void 
RdgPass::accessResources(RdgResourceHndSpan hnds, Access access)
{
	for (auto& e : hnds)
	{
		accessResource(e, access);
	}
}

void RdgPass::accessResource(RdgResourceHnd hnd, Access access)
{
	using SRC = Access;

	auto* resource = hnd._p;

	auto doIfNotduplicatedPass = [resource, this](auto passes)
		{
			for (auto& e : resource->producers())
			{
				if (!isDuplicatedPass(passes, e))
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
			//RDS_CORE_ASSERT(), "RenderPass: {} has repeated read resource {}", _name, hnd.name());
			_reads.emplace_back(resource);
			doIfNotduplicatedPass(_runBefore);
		} break;

		case SRC::Write:
		{
			RDS_CORE_ASSERT(!isDuplicatedHnd(_writes.span(), hnd), "RenderPass: {} has repeated write resource {}", _name, hnd.name());

			resource->addProducer(this);
			_writes.emplace_back(resource);
			doIfNotduplicatedPass(_runAfter);
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

bool RdgPass::isDuplicatedPass(Span<Pass*> dst, Pass* src)
{
	auto it = find_if(dst.begin(), dst.end()
		, [=](Pass* e)
		{
			return e->id() == src->id();
		});
	return it != dst.end();
}

bool RdgPass::isDuplicatedPasses(Span<Pass*> dst, Span<Pass*> src)
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


#endif

}