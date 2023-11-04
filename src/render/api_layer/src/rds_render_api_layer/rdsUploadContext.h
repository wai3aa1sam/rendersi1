#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/command/rdsTransferCommand.h"

namespace rds
{

class Texture;

#if 0
#pragma mark --- rdsUploadContext-Decl ---
#endif // 0
#if 1


class UploadContext : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static constexpr SizeType s_kLocalSize	= 64;

protected:

	struct UploadBuffer
	{

	};

	struct UploadTexture
	{
		Vector<TransferCommand_UploadTexture*, s_kLocalSize>	uploadTexCmds;
		LinearAllocator											texData;
		LinearAllocator											allocator;
		Vector<u32, s_kLocalSize>								texOffsets;
		Vector<u32, s_kLocalSize>								texSizes;
	};

};


#endif

}