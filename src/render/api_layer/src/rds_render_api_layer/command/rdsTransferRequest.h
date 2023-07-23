#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsTransferCommand.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferCommand-Impl ---
#endif // 0
#if 1

class TransferRequest : public NonCopyable
{
public:
	using Traits	= RenderApiLayerTraits;
	using SizeType	= Traits::SizeType;

public:
	static constexpr SizeType s_kThreadCount		= RenderApiLayerTraits::s_kThreadCount;
	static constexpr SizeType s_kFrameInFlightCount	= RenderApiLayerTraits::s_kFrameInFlightCount;

public:
	TransferRequest();
	~TransferRequest();

	void clear();

	TransferCommandBuffer& transferCommandBuffer();

private:
	Vector<TransferCommandBuffer, s_kThreadCount>	_transferCommandBuffers;
};

inline
TransferCommandBuffer& TransferRequest::transferCommandBuffer()
{
	auto tlid = OsTraits::threadLocalId();
	return _transferCommandBuffers[tlid];
}

#endif

}