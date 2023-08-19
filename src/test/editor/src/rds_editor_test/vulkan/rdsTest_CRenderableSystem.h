#pragma once

#include "rds_render_api_layer/mesh/rdsRenderMesh.h"
#include "rds_render_api_layer/command/rdsRenderRequest.h"
#include "rds_render_api_layer/rdsRenderFrame.h"

namespace rds
{


class Test_System_Base {};

class Test_CRenderable : public RefCount_Base
{
public:
	Test_CRenderable(RenderMesh& rdMesh) : _rdMesh(rdMesh) {}

public:
	RenderMesh& _rdMesh;
};

class Test_CRenderableSystem : public Test_System_Base
{
public:
	using Traits = RenderApiLayerTraits;

	using SizeType = Traits::SizeType;

public:
	static constexpr SizeType s_kSwapchainImageLocalSize	= Traits::s_kSwapchainImageLocalSize;
	static constexpr SizeType s_kFrameInFlightCount			= Traits::s_kFrameInFlightCount;
	static constexpr SizeType s_kThreadCount				= Traits::s_kThreadCount;

public:

	~Test_CRenderableSystem()
	{
		clear();
	}

	void clear()
	{
		_cRenderables.clear();
	}

	void execute(/*RenderRequest& rdReq*/)
	{
		DrawingSettings settings = {};
		auto& rdQueue = RenderFrameContext::instance()->renderFrame().renderQueue();
		auto& hashedCmds = rdQueue.prepareDrawRenderables(settings);
		Span<RenderCommand_DrawCall*> cmds = hashedCmds.drawCallCmds();

		// 1. sort renderables first (require a extra store for this)
		// , then pararllel reocrd, when each batch finish, store to its own chunk (seems no need to wait) / wait for the submission order and push to a whole big chunk
		// 2. parallel record first (each renderables may add >1 drawcalls, so cannot use ParJob_Base)
		// , store the recorded cmds to a local chunk, end then push to a big chunk, no need to follow the order but need a mutex for it
		// , then sort it 

		// --- both method should only wait when it is actually use to record
		recordDrawCallCommands(rdQueue, hashedCmds);
	}

	JobHandle recordDrawCallCommands(RenderQueue& rdQueue, HashedDrawCallCommands& out)
	{
		//auto& outDrawCmds = out._interal_drawCallCmds();
		using DrawCallCmds = RenderCommandBuffer::DrawCallCmds;

		SizeType	s_kMinBatchSize		= 200;
		auto		n					= _cRenderables.size();
		auto		batchSizePerThread	= math::max(s_kMinBatchSize, n / OsTraits::logicalThreadCount());
		auto		jobCount			= sCast<SizeType>(math::ceil((float)n / batchSizePerThread));

		class ParRecordRenderCommandJob : public Job_Base
		{
		public:
			ParRecordRenderCommandJob(SizeType batchOffset, SizeType batchSize, HashedDrawCallCommands* outCmds, Span<SPtr<Test_CRenderable> > cRenderables
									, RenderQueue* rdQueue, Mutex* mtx)
			{
				_batchOffset	= batchOffset;
				_batchSize		= batchSize;
				_cRenderables	= cRenderables;
				_outCmds		= outCmds;
				_rdQueue		= rdQueue;
				_mtx			= mtx;
			}

			virtual void execute() override
			{
				RDS_PROFILE_SCOPED();

				// begin
				Vector<RenderCommand_DrawCall*> cmds;

				// execute
				for (size_t i = 0; i < _batchSize; i++)
				{
					auto id = _batchOffset + i;
					auto& cRenderables = _cRenderables[id];
					_rdQueue->recordDrawCall(cmds, cRenderables->_rdMesh, Mat4f::s_identity());
				}

				// end
				if (true)
				{
					ULock<Mutex> lock{*_mtx};
					auto oldSize = _outCmds->size();
					_outCmds->resize(_outCmds->size() + cmds.size());
					auto** dst = _outCmds->_internal_drawCallCmds().data() + oldSize; RDS_UNUSED(dst);
					memory_copy(dst, cmds.data(), cmds.size());
				}
			}

		private:
			Span<SPtr<Test_CRenderable> >		_cRenderables;
			HashedDrawCallCommands*				_outCmds		= nullptr;
			RenderQueue*						_rdQueue		= nullptr;
			Mutex*								_mtx			= nullptr;
			SizeType _batchOffset	= 0;
			SizeType _batchSize		= 0;
		};

		Vector<UPtr<ParRecordRenderCommandJob>,	s_kThreadCount> recordJobs;
		Mutex mtx;

		recordJobs.resize(jobCount);

		JobHandle scatterJobParent = JobSystem::instance()->createEmptyJob();
		JobHandle sortCmdJobHandle = JobSystem::instance()->createEmptyJob();;

		out._internal_jobHandle() = sortCmdJobHandle;
		scatterJobParent->runBefore(sortCmdJobHandle);

		SizeType remain = n;
		for (SizeType i = 0; i < jobCount; ++i)
		{
			auto& job		= recordJobs[i];

			auto batchSize = math::min(batchSizePerThread, remain);
			remain -= batchSize;
			
			job = makeUPtr<ParRecordRenderCommandJob>(batchSizePerThread * i, batchSize, &out, _cRenderables.span(), &rdQueue, &mtx);

			auto handle = job->delayDispatch();
			handle->setParent(scatterJobParent);
			JobSystem::instance()->submit(handle);
		}

		// the parent use as spwan task is better, the local variable should use a class and store to the CRenderableSystem per frame
		JobSystem::instance()->submit(scatterJobParent);

		RDS_WARN_ONCE("TODO: remove the wait, must keep now since the jobs are local variable");
		JobSystem::instance()->waitForComplete(sortCmdJobHandle);

		return sortCmdJobHandle;
	}

	template<class T, class ALLOC = DefaultAllocator, size_t N = OsTraits::s_kEstLogicalThreadCount, class... ARGS>
	JobHandle prepareGatherJobs(Vector<UPtr<T>, N, ALLOC>& jobs, SizeType loopCount, SizeType minBatchSize, ARGS&&... ctor_args)
	{
		static_assert(IsBaseOf<Job_Base, T>);

		auto		n					= loopCount;
		auto		batchSizePerThread	= math::max(minBatchSize, n / OsTraits::logicalThreadCount());
		auto		jobCount			= sCast<SizeType>(math::ceil((float)n / batchSizePerThread));

		jobs.clear();
		jobs.resize(jobCount);

		JobHandle parent = JobSystem::instance()->createEmptyJob();

		SizeType remain = n;
		for (int i = 0; i < jobCount; ++i)
		{
			auto& job		= jobs[i];
			job = makeUPtr<T>(nmsp::forward<ARGS>(ctor_args)...);

			auto batchSize = math::min(batchSizePerThread, remain);
			remain -= batchSize;

			auto handle = job->delayDispatch();
			// handle->setParent(parent);
			JobSystem::submit(handle);
		}

		return parent;
	}


	void createTest(SizeType n, RenderMesh& rdMesh)
	{
		
		_cRenderables.resize(n);
		for (auto& e : _cRenderables)
		{
			e = makeSPtr<Test_CRenderable>(rdMesh);
		}
	}

public:
	Vector<SPtr<Test_CRenderable>> _cRenderables;
};


}