#pragma once

#include "rds_render_api_layer/mesh/rdsRenderMesh.h"
#include "rds_render_api_layer/command/rdsRenderRequest.h"
#include "rds_render_api_layer/rdsRenderFrame.h"

#include "rds_editor.h"

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
		auto& rdQueue = Renderer::instance()->renderFrame().renderQueue();
		auto& hashedCmds = rdQueue.prepareDrawRenderables(settings);
		Span<RenderCommand_DrawCall*> cmds = hashedCmds.drawCallCmds();

		// 1. sort renderables first (require a extra store for this)
		// , then pararllel reocrd, when each batch finish, store to its own chunk (seems no need to wait) / wait for the submission order and push to a whole big chunk
		// 2. parallel record first (each renderables may add >1 drawcalls, so cannot use ParJob_Base)
		// , store the recorded cmds to a local chunk, end then push to a big chunk, no need to follow the order but need a mutex for it
		// , then sort it 

		// --- both method should only wait when it is actually use to record
		recordDrawCallCommands2(rdQueue, hashedCmds);
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
		JobHandle sortCmdJobHandle = JobSystem::instance()->createEmptyJob();
		RDS_WARN_ONCE("TODO: sort the render cmds / sort the mesh");

		JobFlow jf;

		out._internal_jobHandle() = sortCmdJobHandle;
		jf.emplace(sortCmdJobHandle, scatterJobParent);
		jf.xDependOnY(sortCmdJobHandle, scatterJobParent);

		SizeType remain = n;
		for (SizeType i = 0; i < jobCount; ++i)
		{
			auto& job		= recordJobs[i];

			auto batchSize = math::min(batchSizePerThread, remain);
			remain -= batchSize;
			
			job = makeUPtr<ParRecordRenderCommandJob>(batchSizePerThread * i, batchSize, &out, _cRenderables.span(), &rdQueue, &mtx);

			auto handle = job->prepareDispatch(scatterJobParent, nullptr);
			//handle->setParent(scatterJobParent);
			JobSystem::instance()->submit(handle);
		}

		#if 1

		jf.runAndWait();

		#else

		// the parent use as spwan task is better, the local variable should use a class and store to the CRenderableSystem per frame
		JobSystem::instance()->submit(scatterJobParent);

		RDS_WARN_ONCE("TODO: remove the wait, must keep now since the jobs are local variable");
		JobSystem::instance()->waitForComplete(sortCmdJobHandle);

		#endif // 0


		return sortCmdJobHandle;
	}

	JobHandle recordDrawCallCommands2(RenderQueue& rdQueue, HashedDrawCallCommands& out)
	{
		//auto& outDrawCmds = out._interal_drawCallCmds();
		using DrawCallCmds = RenderCommandBuffer::DrawCallCmds;

		class ParRecordRenderCommandJob : public JobCluster_Base<Job_Base>
		{
		public:
			ParRecordRenderCommandJob(HashedDrawCallCommands* outCmds, Span<SPtr<Test_CRenderable> > cRenderables
										, RenderQueue* rdQueue, Mutex* mtx)
			{
				_cRenderables	= cRenderables;
				_outCmds		= outCmds;
				_rdQueue		= rdQueue;
				_mtx			= mtx;
			}

			virtual void onBegin() override
			{
				Base::onBegin();
			}

			virtual void onEnd() override
			{
				RDS_PROFILE_SCOPED();

				Base::onEnd();

				ULock<Mutex> lock{*_mtx};
				auto oldSize = _outCmds->size();
				_outCmds->resize(_outCmds->size() + _cmds.size());
				auto** dst = _outCmds->_internal_drawCallCmds().data() + oldSize; RDS_UNUSED(dst);
				memory_copy(dst, _cmds.data(), _cmds.size());
			}

			virtual void execute() override
			{
				RDS_PROFILE_SCOPED();
				for (size_t i = 0; i < clusterArgs().clusterSize; i++)
				{
					auto id = clusterArgs().clusterOffset + i;
					auto& cRenderables = _cRenderables[id];
					_rdQueue->recordDrawCall(_cmds, cRenderables->_rdMesh, Mat4f::s_identity());
				}
			}

		private:
			Span<SPtr<Test_CRenderable> >		_cRenderables;
			HashedDrawCallCommands*				_outCmds		= nullptr;
			RenderQueue*						_rdQueue		= nullptr;
			Mutex*								_mtx			= nullptr;

			Vector<RenderCommand_DrawCall*> _cmds;
		};

		Vector<UPtr<ParRecordRenderCommandJob>,	s_kThreadCount> recordJobs;
		Mutex mtx;

		u32 minBatchSize = 200;
		const auto loopCount = sCast<u32>(_cRenderables.size());
		//auto jobClusterHnd = JobCluster::dispatch(recordJobs, loopCount, minBatchSize, &out, _cRenderables.span(), &rdQueue, &mtx);
		auto jobClusterHnd = JobCluster::prepare(recordJobs, loopCount, minBatchSize, &out, _cRenderables.span(), &rdQueue, &mtx);

		RDS_WARN_ONCE("TODO: impl both methoed sort the render cmds first / sort the mesh first");
		JobFlow jf;

		RDS_WARN_ONCE("TODO: impl multi-threaded sort");
		JobHandle sortCmdJobHandle = JobSystem::instance()->createEmptyJob();

		out._internal_jobHandle() = sortCmdJobHandle;
		jf.emplace(sortCmdJobHandle, jobClusterHnd);
		jf.xDependOnY(sortCmdJobHandle, jobClusterHnd);

		jf.runAndWait();

		return sortCmdJobHandle;
	}


	void createTest(SizeType n, RenderMesh& rdMesh)
	{
		_cRenderables.resize(n);
		for (auto& e : _cRenderables)
		{
			e = makeSPtr<Test_CRenderable>(rdMesh);
		}
	}

	void reserve(SizeType n)
	{
		_cRenderables.reserve(n);
	}

	void addRenderable(RenderMesh& rdMesh)
	{
		_cRenderables.emplace_back(makeSPtr<Test_CRenderable>(rdMesh));
	}

public:
	Vector<SPtr<Test_CRenderable>> _cRenderables;
};



class Test_MultiThreadDrawCalls
{
public:
	static EditMesh makeEditMesh(Vec3f translate, Vec3f scale, float z = 0.0f)
	{
		static size_t s_kVtxCount = 4;
		EditMesh editMesh;
		auto rnd = Random{};
		
		{
			auto& e = editMesh.pos;
			e.reserve(s_kVtxCount);
			//v = 0.5f;

			auto transform = Mat4f::s_TS(translate, scale);

			e.emplace_back(-0.5f, -0.5f, z); e.back() = transform.mulPoint4x3(e.back()).toTuple3();
			e.emplace_back(0.5f,  -0.5f, z); e.back() = transform.mulPoint4x3(e.back()).toTuple3();
			e.emplace_back(0.5f,   0.5f, z); e.back() = transform.mulPoint4x3(e.back()).toTuple3();
			e.emplace_back(-0.5f,  0.5f, z); e.back() = transform.mulPoint4x3(e.back()).toTuple3();
		}
		{
			auto& e = editMesh.color;
			e.reserve(s_kVtxCount);
			auto r0 = sCast<u8>(rnd.range(0, 255));
			//auto r1 = sCast<u8>(rnd.range(0, 255));
			e.emplace_back(r0, 0, 0, 255);
			e.emplace_back(0, r0, 0, 255);
			e.emplace_back(0, 0, r0, 255);
			e.emplace_back(255, 255, 255, 255);
		}
		{
			auto& e = editMesh.uvs[0];
			e.reserve(s_kVtxCount);
			e.emplace_back(1.0f, 0.0f);
			e.emplace_back(0.0f, 0.0f);
			e.emplace_back(0.0f, 1.0f);
			e.emplace_back(1.0f, 1.0f);
		}

		editMesh.indices = { 0, 2, 1, 2, 0, 3 };

		RDS_ASSERT(editMesh.getVertexLayout() == Vertex_PosColorUv<1>::vertexLayout(), "");
		return editMesh;
	}

	void create(size_t n)
	{
		_rdMeshes.clear();
		_cRdSys.clear();

		auto count = n * n;

		_cRdSys.reserve(count);
		_rdMeshes.reserve(count);

		float dx = 1 / sCast<float>(n);
		float dy = 1 / sCast<float>(n);

		float offsetX = sCast<float>(n) / -2;
		float offsetY = sCast<float>(n) / -2;

		offsetX = 0.0f;
		offsetY = 0.0f;

		auto scaleFactor	= 1 / sCast<float>(n);
		auto scale			= Vec3f{ scaleFactor, scaleFactor, scaleFactor };

		for (size_t iy = 0; iy < n; iy++)
		{
			for (size_t ix = 0; ix < n; ix++)
			{
				auto& rdMesh = _rdMeshes.emplace_back();

				rdMesh.create(makeEditMesh(Vec3f{ sCast<float>(ix * dx + offsetX),  sCast<float>(iy * dy + offsetY), 0.0f }, scale, 0.3f));
				_cRdSys.addRenderable(rdMesh);
			}
		}
	}

	void createFixed(size_t n)
	{
		_rdMeshes.clear();
		_cRdSys.clear();

		_cRdSys.reserve(n);
		_rdMeshes.reserve(1);

		auto mesh = makeEditMesh(Vec3f{ 0.0f, 1.0f, 0.0f }, Vec3f::s_one());
		auto& rdMesh = _rdMeshes.emplace_back();
		rdMesh.create(mesh);

		for (size_t ix = 0; ix < n; ix++)
		{
			_cRdSys.addRenderable(rdMesh);
		}
	}

	void execute()
	{
		_cRdSys.execute();
	}

private:
	Vector<RenderMesh>		_rdMeshes;
	Test_CRenderableSystem	_cRdSys;
};

}