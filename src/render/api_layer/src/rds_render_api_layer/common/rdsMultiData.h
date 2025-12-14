#pragma once

#include <rds_core.h>
#include "rds_render_api_layer_traits.h"

namespace rds
{

#if 0
#pragma mark --- rdsMultiData-Decl ---
#endif // 0
#if 1

// class MultiXXX, resizeToLocalSize(), reserveToLocalSize(), nextBufferIndex()

template<class T, int N = RenderApiLayerTraits::s_kMaxFrameAheadCountHardLimit>
class MultiData : public Vector<T, N>
{
public:
	void resizeToLocalSize();
	void reserveToLocalSize();

	//auto idx = s_bufferIndex(_i_buffer);
	//_renderGpuBuffers.resize(s_kMaxBufferCount);

private:

};

#endif

}