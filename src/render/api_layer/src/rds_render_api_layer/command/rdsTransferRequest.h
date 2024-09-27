#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsTransferCommand.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferRequest-Decl ---
#endif // 0
#if 1

class TransferContext;
class TransferFrame;

class Texture2D;
class RenderGpuBuffer;

class TransferRequest : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
	friend class Texture2D;
	friend class RenderGpuBuffer;
public:
	TransferRequest();
	~TransferRequest();

	void reset(TransferContext* tsfCtx, TransferFrame* tsfFrame);
	void commit(RenderFrameParam& rdFrameParam, bool isWaitImmediate = false);

	void createBuffer(	RenderGpuBuffer*	buffer);
	void createTexture(	Texture*			texture);

	void destroyBuffer(	RenderGpuBuffer*	buffer);
	void destroyTexture(Texture*			texture);
	
	TransferCommand_UploadTexture* uploadTexture(Texture* tex);
	//TransferCommand_UploadTexture* uploadTexture(Texture*		tex, StrView filename);
	//TransferCommand_UploadTexture* uploadTexture(Texture2D*		tex, Texture2D_CreateDesc&&		cDesc);
	//TransferCommand_UploadTexture* uploadTexture(TextureCube*	tex, TextureCube_CreateDesc&&	cDesc);

	void uploadBuffer		(RenderGpuBuffer* rdBuf, ByteSpan data, SizeType offset, RenderGpuMultiBuffer* rdMultiBuf);
	void uploadBuffer		(RenderGpuBuffer* rdBuf, ByteSpan data, SizeType offset = 0);
	void uploadBufferAsync	(RenderGpuBuffer* rdBuf, Vector<u8>&& data); // template<class DATA, size_t N>, DATA -> u8

	TransferCommandBuffer& transferCommandBuffer();

private:
	TransferContext*		_tsfCtx	= nullptr;
	TransferCommandBuffer	_tsfCmdBuf;

	//Vector<TransferCommandBuffer, s_kThreadCount> _perThreadTsfCmdBuf;
};


#endif

}