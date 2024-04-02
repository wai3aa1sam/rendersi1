#pragma once

#include "rds_engine/common/rds_engine_common.h"

#include "rds_render_api_layer/graph/rdsRenderGraph.h"

#include "rdsCSystem.h"
#include "rds_engine/ecs/component/rdsCRenderable.h"


namespace rds
{

class Scene;
class ObjectTransformBuffer
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using T = Mat4f;

public:
	ObjectTransformBuffer()
	{
		_cpuBufs.resize(s_kFrameInFlightCount);
		
	}

	void resize(SizeType n)
	{
		auto bufSize = n * sizeof(T);

		if (!_gpuBufs)
		{
			auto cDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
			cDesc.bufSize	= bufSize;
			cDesc.stride	= sizeof(T);
			cDesc.typeFlags = RenderGpuBufferTypeFlags::Compute;
			_gpuBufs = Renderer::rdDev()->createRenderGpuMultiBuffer(cDesc);
		}
		
		cpuBuffer().resize(bufSize);
	}

	void setValue(SizeType i, const T& v)
	{
		at(i) = v;
	}

	void uploadToGpu()
	{
		if (cpuBuffer().is_empty())
		{
			return;
		}

		_gpuBufs->uploadToGpu(cpuBuffer());

		auto prevSize = prevCpuBuffer().size();
		cpuBuffer().resize(prevSize);
		memory_copy(cpuBuffer().data(), prevCpuBuffer().data(), prevSize);
	}

public:
			T& at(SizeType i)						{ checkIsInBoundary(i); return reinCast<			T&>(cpuBuffer()[i * sizeof(T)]); }
	const	T& at(SizeType i) const					{ checkIsInBoundary(i); return reinCast<const		T&>(cpuBuffer()[i * sizeof(T)]); }

			RenderGpuBuffer& gpuBuffer()			{ return *_gpuBufs->renderGpuBuffer(); }
	const	RenderGpuBuffer& gpuBuffer() const		{ return *_gpuBufs->renderGpuBuffer(); }
	
			RenderGpuBuffer& prevGpuBuffer()		{ return *_gpuBufs->previousBuffer(); }
	const	RenderGpuBuffer& prevGpuBuffer() const	{ return *_gpuBufs->previousBuffer(); }

			Vector<u8>& cpuBuffer()					{ return _cpuBufs[_gpuBufs->iFrame()]; }
	const	Vector<u8>& cpuBuffer() const			{ return _cpuBufs[_gpuBufs->iFrame()]; }

			Vector<u8>& prevCpuBuffer()				{ auto iPrevFrame = (sCast<int>(iFrame()) - 1) % s_kFrameInFlightCount; return _cpuBufs[iPrevFrame]; }
	const	Vector<u8>& prevCpuBuffer() const		{ auto iPrevFrame = (sCast<int>(iFrame()) - 1) % s_kFrameInFlightCount; return _cpuBufs[iPrevFrame]; }

	u32 iFrame() const { return _gpuBufs->iFrame(); }

protected:
	bool checkIsInBoundary(SizeType i) const { bool isInBoundary = i < cpuBuffer().size() / sizeof(T); RDS_CORE_ASSERT(isInBoundary, "out of boundary"); return isInBoundary;  }

private:
	SPtr<RenderGpuMultiBuffer>					_gpuBufs;
	Vector<Vector<u8>, s_kFrameInFlightCount>	_cpuBufs;
};

class CRenderable;
#if 0
#pragma mark --- rdsCRenderableSystem-Decl ---
#endif // 0
#if 1

class CRenderableSystem : public CSystemT<CRenderable> // Singleton<CRenderableSystem, CSystem>
{
	friend class CRenderable;
	RDS_ENGINE_COMMON_BODY();
public:
	using RenderableTable		= VectorMap<EntityId, SPtr<CRenderable> >;
	using FramedRenderRequest	= Vector<RenderRequest, RenderApiLayerTraits::s_kFrameInFlightCount>;

public:
	CRenderableSystem();
	~CRenderableSystem();

	void create(EngineContext* egCtx);
	void destroy();

	void update();
	void render(RenderContext* rdCtx_, RenderMesh& fullScreenTriangle, Material* mtlPresent);
	void present();

	void drawRenderables(RenderRequest& rdReq, Material* mtl);
	void drawRenderables(RenderRequest& rdReq);

public:
	Vector<CRenderable*>& renderables();

public:
	RenderGraph& renderGraph();

protected:
	RenderRequest& renderRequest();


protected:
	RenderGraph				_rdGraph;
	FramedT<RenderRequest>	_framedRdReq;
	ObjectTransformBuffer	_objTransformBuf;		// maybe use a two level index table to prevent empty space
};

inline RenderGraph&				CRenderableSystem::renderGraph()	{ return _rdGraph; }
inline RenderRequest&			CRenderableSystem::renderRequest()	{ return _framedRdReq[renderGraph().iFrame()]; }

inline Vector<CRenderable*>&	CRenderableSystem::renderables()	{ return components(); }


#endif


}