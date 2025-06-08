#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsTransferCommandBuffer.h"

namespace rds
{

class RenderContext;

class TransferContext;
class TransferFrame;

class Texture2D;
class RenderGpuBuffer;

class TransferCommand_UploadTexture;


#if 0
#pragma mark --- rdsTransferRequest-Decl ---
#endif // 0
#if 1

class TransferRequest : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
	friend class Texture2D;
	friend class RenderGpuBuffer;
public:
	static bool tryPopTransferCommandSafeBuffer(TransferCommandBuffer& dst, TransferCommandSafeBuffer& src);

public:
	TransferRequest();
	~TransferRequest();

	void reset(TransferContext* tsfCtx);
	//void commit(RenderFrameParam& rdFrameParam, bool isWaitImmediate = false);
	
	void setSwapchainSize(RenderContext* rdCtx, const Tuple2f& size);

	TransferCommand_UploadTexture* uploadTexture(Texture* tex);
	//TransferCommand_UploadTexture* uploadTexture(Texture*		tex, StrView filename);
	//TransferCommand_UploadTexture* uploadTexture(Texture2D*		tex, Texture2D_CreateDesc&&		cDesc);
	//TransferCommand_UploadTexture* uploadTexture(TextureCube*	tex, TextureCube_CreateDesc&&	cDesc);

	void uploadBuffer(		RenderGpuBuffer* rdBuf, ByteSpan data, SizeType offset = 0);
	void uploadBufferAsync(	RenderGpuBuffer* rdBuf, Vector<u8>&& data); // template<class DATA, size_t N>, DATA -> u8

public:
	TransferCommandSafeBuffer& transferCommandBuffer();
	template<class TCmd> TCmd* newCommand();

private:
	TransferContext*			_tsfCtx			= nullptr;
	TransferCommandSafeBuffer	_tsfCmdBuf;
	//Vector<TransferCommandBuffer, s_kThreadCount> _perThreadTsfCmdBuf;
};


#endif

}