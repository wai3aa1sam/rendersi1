#include "rds_render_api_layer-pch.h"
#include "rdsTexture2DArray.h"
#include "rds_render_api_layer/rdsRenderer.h"

#include "rds_render_api_layer/transfer/rdsTransferContext.h"
#include "rds_render_api_layer/command/rdsTransferRequest.h"

namespace rds
{

RDS_Define_TypeInfo(Texture2DArray);

SPtr<Texture2DArray> 
RenderDevice::createTexture2DArray(Texture2DArray_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateTexture2DArray(cDesc);
	return p;
}

#if 0
#pragma mark --- rdsTexture2DArray-Impl ---
#endif // 0
#if 1

Texture2DArray::CreateDesc	
Texture2DArray::makeCDesc(RDS_DEBUG_SRCLOC_PARAM)
{
	auto o = CreateDesc{ RDS_DEBUG_SRCLOC_ARG };
	o.type = RenderDataType::Texture2DArray;
	return o;
}

SPtr<Texture2DArray>
Texture2DArray::make(CreateDesc& cDesc)
{
	return Renderer::renderDevice()->createTexture2DArray(cDesc);
}

Texture2DArray::Texture2DArray()
	: Base(RenderDataType::Texture2DArray)
{
}

Texture2DArray::~Texture2DArray()
{
	
}

void 
Texture2DArray::create(CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
	onPostCreate(cDesc);
}

void 
Texture2DArray::uploadToGpu(CreateDesc& cDesc)
{
	_notYetSupported(RDS_SRCLOC);
}

void 
Texture2DArray::onCreate(CreateDesc& cDesc)
{
	Base::onCreate(cDesc);
}

void 
Texture2DArray::onPostCreate(CreateDesc& cDesc)
{

}

void 
Texture2DArray::onDestroy()
{


	Base::onDestroy();
}

void 
Texture2DArray::onUploadToGpu(CreateDesc& cDesc, TransferCommand_UploadTexture* cmd)
{
	_notYetSupported(RDS_SRCLOC);
	RDS_CORE_ASSERT(cmd, "");
}

#endif
}