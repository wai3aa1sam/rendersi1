#pragma once
#include "rds_render_api_layer/common/rdsRenderApi_Common.h"

namespace rds
{

template<class T>
class ParamBuffer
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	//using T = Mat4f;

public:
	ParamBuffer();

	void resize(SizeType n);
	void setValue(SizeType i, const T& v);
	void uploadToGpu();

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

template<class T> inline
ParamBuffer<T>::ParamBuffer()
{
	_cpuBufs.resize(s_kFrameInFlightCount);
}

template<class T> inline
void 
ParamBuffer<T>::resize(SizeType n)
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

template<class T> inline
void 
ParamBuffer<T>::setValue(SizeType i, const T& v)
{
	at(i) = v;
}

template<class T> inline
void 
ParamBuffer<T>::uploadToGpu()
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

}