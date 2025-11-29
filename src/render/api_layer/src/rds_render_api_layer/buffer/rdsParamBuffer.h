#pragma once
#include "rds_render_api_layer/common/rdsRenderApi_Common.h"

namespace rds
{

template<class T>
class ParamBuffer : public RenderApiLayerCommon_Base
{
public:
	//using T = Mat4f;

public:
	ParamBuffer();
	void create(RDS_DebugLabel_PARAM, SizeType n);

	T&	 add();
	void popBack();
	void resize(SizeType n);
	void setValue(SizeType i, const T& v);
	void uploadToGpu();

public:
	SizeType	size()		const;
	bool		is_empty()	const;

	/*
	* do not store the ptr, store the index instead, since it will rotate and the cpuBuffer will resize, all ptr will be invalid
	*/
			T& at(SizeType i)						{ checkIsInBoundary(i); return reinCast<			T&>(cpuBuffer()[i * sizeof(T)]); }
	const	T& at(SizeType i) const					{ checkIsInBoundary(i); return reinCast<const		T&>(cpuBuffer()[i * sizeof(T)]); }

			RenderGpuBuffer* gpuBuffer()			{ return _gpuBufs->renderGpuBuffer(); }
	const	RenderGpuBuffer* gpuBuffer() const		{ return _gpuBufs->renderGpuBuffer(); }
	
			RenderGpuBuffer* prevGpuBuffer()		{ return _gpuBufs->previousBuffer(); }
	const	RenderGpuBuffer* prevGpuBuffer() const	{ return _gpuBufs->previousBuffer(); }

	#if 1
			Vector<u8>& cpuBuffer()					{ return _cpuBuf; }
	const	Vector<u8>& cpuBuffer() const			{ return _cpuBuf; }

	#else
			Vector<u8>& cpuBuffer()					{ return _cpuBufs[bufferIndex()]; }
	const	Vector<u8>& cpuBuffer() const			{ return _cpuBufs[bufferIndex()]; }

			Vector<u8>& prevCpuBuffer()				{ return _cpuBufs[s_previousBufferIndex(bufferIndex())]; }
	const	Vector<u8>& prevCpuBuffer() const		{ return _cpuBufs[s_previousBufferIndex(bufferIndex())]; }

	#endif // 0

	int bufferIndex() const { return _gpuBufs->bufferIndex(); }

protected:
	bool checkIsInBoundary(SizeType i) const { bool isInBoundary = i < (cpuBuffer().size() / sizeof(T)); RDS_CORE_ASSERT(isInBoundary, "out of boundary"); return isInBoundary;  }

private:
	bool _isDirty : 1;

	Vector<u8>					_cpuBuf;
	SPtr<RenderMultiGpuBuffer>	_gpuBufs;
	//Vector<Vector<u8>, s_kMaxFrameAheadCountHardLimit>	_cpuBufs;		// only 1 cpu buffer is enough
};

template<class T> inline
ParamBuffer<T>::ParamBuffer()
{
	
}

template<class T> inline
void 
ParamBuffer<T>::create(RDS_DebugLabel_PARAM, SizeType n)
{
	if (!_gpuBufs)
	{
		auto bufSize = n * sizeof(T);

		auto cDesc = RenderGpuBuffer::makeCDesc();
		cDesc.bufSize	= bufSize;
		cDesc.stride	= sizeof(T);
		cDesc.typeFlags = RenderGpuBufferTypeFlags::Compute;
		_gpuBufs = Renderer::renderDevice()->createRenderMultiGpuBuffer(RDS_DebugLabel_ARG, cDesc);
	}
}

template<class T> inline
T& 
ParamBuffer<T>::add()
{
	auto size = this->size();
	resize(size + 1);
	return at(size);		// size is the last idx
}

template<class T> inline
void
ParamBuffer<T>::popBack()
{
	auto size = this->size();
	if (size)
	{
		resize(size - 1);
	}
}

template<class T> inline
void 
ParamBuffer<T>::resize(SizeType n)
{
	auto bufSize = n * sizeof(T);
	cpuBuffer().resize(bufSize);
	_isDirty = true;
}

template<class T> inline
void 
ParamBuffer<T>::setValue(SizeType i, const T& v)
{
	at(i) = v;
	_isDirty = true;
}

template<class T> inline
void 
ParamBuffer<T>::uploadToGpu()
{
	/*if (cpuBuffer().is_empty())
	{
		return;
	}*/

	if (_isDirty)
	{
		_gpuBufs->uploadToGpu(cpuBuffer());
		_isDirty = false;
	}

	//auto prevSize = prevCpuBuffer().size();
	//cpuBuffer().resize(prevSize);
	//memory_copy(cpuBuffer().data(), prevCpuBuffer().data(), prevSize);
}

template<class T> inline typename ParamBuffer<T>::SizeType	ParamBuffer<T>::size()		const { return cpuBuffer().size() / sizeof(T); }
template<class T> inline bool								ParamBuffer<T>::is_empty()	const { return size() == 0; }


}