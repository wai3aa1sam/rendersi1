#include "rds_render_api_layer-pch.h"

#include "rdsRenderQueue.h"
#include "rds_render_api_layer/thread/rdsRenderFrame.h"
#include "rds_render_api_layer/mesh/rdsRenderMesh.h"
#include "rds_render_api_layer/command/rdsRenderRequest.h"
#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{


#if 0
#pragma mark --- rdsHashedDrawCallCommands-Impl ---
#endif // 0
#if 1

HashedDrawCallCommands::HashedDrawCallCommands()
{
	RDS_WARN_ONCE("The hashes and cmds should be a mutex allocator (don't use pushed stack)");
}

HashedDrawCallCommands::~HashedDrawCallCommands()
{
	clear();
}

void 
HashedDrawCallCommands::clear()
{
	checkValid();

	waitForReady();
	_hashs.clear();
	for (auto& cmd : _drawCallCmds)
	{
		cmd->~RenderCommand_DrawCall();
	}
	_drawCallCmds.clear();

	_drawingSettings	= 0;
	_jobHandle			= nullptr;

}

void 
HashedDrawCallCommands::resize(SizeType n)
{
	checkValid();
	_hashs.resize(n);
	_drawCallCmds.resize(n);

}

JobHandle 
HashedDrawCallCommands::sort()
{
	checkValid();

	RDS_NOT_YET_SUPPORT();

	return _jobHandle;
}

void 
HashedDrawCallCommands::waitForReady()
{
	JobSystem::instance()->waitForComplete(_jobHandle);
}

void 
HashedDrawCallCommands::checkValid() const
{
	RDS_CORE_ASSERT(_hashs.size() == _drawCallCmds.size(), "HashedDrawCallCommands different sizes");
}


#endif

#if 0
#pragma mark --- rdsRenderQueue-Impl ---
#endif // 0
#if 1

RenderQueue::~RenderQueue()
{

}

void RenderQueue::clear()
{
	_hashedDrawCmds.clear();
}

HashedDrawCallCommands& RenderQueue::prepareDrawRenderables(const DrawingSettings& drawingSettings/*, SizeType n*/)
{
	auto* p = findHashedDrawCmds(drawingSettings);
	if (!p)
	{
		p = &_hashedDrawCmds.emplace_back();
		p->_drawingSettings = drawingSettings;
	}

	/*auto* cmds = allocCommands<RenderCommand_DrawCall>(n);
	auto& hashedDrawCmds = _hashedDrawCmds.emplace_back();
	hashedDrawCmds._drawingSettings = drawingSettings;
	hashedDrawCmds.resize(n);
	for (size_t i = 0; i < n; i++)
	{
		hashedDrawCmds._drawCallCmds[i] = p++;
	}*/

	return *p;
}

HashedDrawCallCommands*
RenderQueue::drawRenderables(const DrawingSettings& drawingSettings)
{
	auto* p = findHashedDrawCmds(drawingSettings);
	if (!p)
	{
		p = &_hashedDrawCmds.emplace_back();
		p->_drawingSettings = drawingSettings;
		return p;
	}

	return p;
}

void* 
RenderQueue::alloc(SizeType n, SizeType align) 
{
	RDS_TODO("make it per rdDev");
	throwIf(true, "rework");
	return nullptr;
	//RDS_ASSERT(OsTraits::isMainThread(), "only use this in main thread"); 
	//return Renderer::renderDevice()->renderFrame(0).renderCommandAllocator().alloc(n, align); 
	//return nullptr;
}

void 
RenderQueue::recordDrawCall(Vector<RenderCommand_DrawCall*>& out, const RenderMesh& rdMesh, Material* mtl, const Mat4f& transform)
{
	_notYetSupported(RDS_SRCLOC);
	#if 0
	auto materialPassCount = 1;
	auto nDrawCalls = rdMesh.subMeshCount() * materialPassCount;
	auto it = allocCommands<RenderCommand_DrawCall>(out, nDrawCalls);

	for (auto& e : rdMesh.subMeshes())
	{
		//for (auto& pass : passes)
		{
			RenderRequest::drawSubMesh(RDS_RD_CMD_DEBUG_ARG, it, e, mtl);
			++it;
		}
	}
	#endif // 0
}

HashedDrawCallCommands*  
RenderQueue::findHashedDrawCmds(const DrawingSettings& drawingSettings)
{
	for (auto& e : _hashedDrawCmds)
	{
		if (e.drawingSettings() == drawingSettings)
		{
			return &e;
		}
	}
	return nullptr;
}


#endif
}