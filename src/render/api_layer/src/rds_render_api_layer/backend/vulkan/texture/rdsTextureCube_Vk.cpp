#include "rds_render_api_layer-pch.h"
#include "rdsTextureCube_Vk.h"

#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"
#include "rds_render_api_layer/backend/vulkan/transfer/rdsTransferFrame_Vk.h"

#if RDS_RENDER_HAS_VULKAN
namespace rds
{

#if 0
#pragma mark --- rdsTextureCube_Vk-Impl ---
#endif // 0
#if 1

TextureCube_Vk::TextureCube_Vk()
{
}

TextureCube_Vk::~TextureCube_Vk()
{
	
}

void 
TextureCube_Vk::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	Base::onUploadToGpu(cDesc, cmd);

	RDS_TODO("allocator for mage (no template), then 1 memcpy is ok");
	RDS_TODO("all image must align to gpu image requirement");
	//const auto& srcImage	= cDesc.uploadImage;
	if (!cDesc.uploadImages.is_empty())
	{
		//SizeType byteSize = cDesc.uploadImages[0].pixelByteSize() * (cDesc.size.x * cDesc.size.y * s_kFaceCount);
		SizeType imageByteSize	= cDesc.uploadImages[0].totalByteSize();
		SizeType byteSize		= imageByteSize * s_kFaceCount;

		RDS_CORE_ASSERT(imageByteSize % 64 == 0);
		
		auto& hnd = cmd->_stagingHnd;
		//auto* stagingBufVk = sCast<StagingBuffer_Vk*>(renderDevice()->transferFrame().stagingBuffer());
		auto& tsfFrameVk = transferFrameVk();
		tsfFrameVk.requestStagingHandle(hnd, byteSize);
		auto* mappedData	= tsfFrameVk.mappedStagingBufData(hnd);
		auto* p				= sCast<u8*>(mappedData);
		for (auto& uploadImage : cDesc.uploadImages)
		{
			//memory_copy(p, uploadImage.dataPtr(), imageByteSize);
			::memcpy(p, uploadImage.dataPtr(), imageByteSize);
			p += imageByteSize;
		}
	}
}

#endif


}
#endif