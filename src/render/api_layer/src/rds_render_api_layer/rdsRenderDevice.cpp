#include "rds_render_api_layer-pch.h"

#include "rdsRenderDevice.h"
//#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/transfer/rdsTransferContext.h"

#include "rds_render_api_layer/texture/rdsTexture.h"

namespace rds
{
#if 0
#pragma mark --- rdsRenderDevice-Impl ---
#endif // 0
#if 1

RenderDevice_CreateDesc::RenderDevice_CreateDesc()
{
	apiType		= RenderApiType::Vulkan;
	isPresent	= true;

	#if RDS_DEBUG
	isDebug = true;
	#else
	isDebug = false;
	#endif // RDS_DEBUG
}

RenderDevice::CreateDesc RenderDevice::makeCDesc() { return CreateDesc{}; }

RenderDevice::RenderDevice()
	: Base()
{
}

RenderDevice::~RenderDevice()
{
	RDS_LOG_DEBUG("~RenderDevice()");
}

void 
RenderDevice::create(const CreateDesc& cDesc)
{
	onCreate(cDesc);
	{
		// wait for UploadContext, since currently use RenderContext to upload
		//rdr->_textureStock.white	= rdr->createSolidColorTexture2D(Color4b(255, 255, 255, 255));
		//rdr->_textureStock.black	= rdr->createSolidColorTexture2D(Color4b(0,   0,   0,   255));
		//rdr->_textureStock.red		= rdr->createSolidColorTexture2D(Color4b(255, 0,   0,   255));
		//rdr->_textureStock.green	= rdr->createSolidColorTexture2D(Color4b(0,   255, 0,   255));
		//rdr->_textureStock.blue		= rdr->createSolidColorTexture2D(Color4b(0,   0,   255, 255));
		//rdr->_textureStock.magenta	= rdr->createSolidColorTexture2D(Color4b(255, 0,   255, 255));
		//rdr->_textureStock.error	= rdr->createSolidColorTexture2D(Color4b(255, 0,   255, 255));
	}

	_tsfCtx->transferRequest().reset(_tsfCtx);

	RDS_CORE_ASSERT(_bindlessRscs, "");
}


void 
RenderDevice::destroy()
{
	waitIdle();

	onDestroy();

	RDS_LOG_DEBUG("{}", RDS_SRCLOC);

	RDS_CORE_ASSERT(_rdFrames.is_empty(),	"forgot to clear RenderFrame in derived class");
	RDS_CORE_ASSERT(_tsfFrames.is_empty(),	"forgot to clear TransferFrame in derived class");
}

void 
RenderDevice::onCreate(const CreateDesc& cDesc)
{
	_adapterInfo.isDebug = cDesc.isDebug;

	_rdFrames.resize(s_kFrameInFlightCount);
	for (auto& e : _rdFrames)
	{
		e.create();
	}
	_tsfFrames.resize(s_kFrameInFlightCount);
	for (auto& e : _tsfFrames)
	{
		e.create();
	}
}

void 
RenderDevice::onDestroy()
{
	
}

void 
RenderDevice::onNextFrame()
{
}

void 
RenderDevice::nextFrame()
{
	_iFrame = (_iFrame + 1) % s_kFrameInFlightCount;

	// throwIf(true, "all reset should after beginRender, as it will wait that frame to finish");

	renderFrame().reset();
	transferFrame().reset();
	_tsfReq.reset(_tsfCtx);

	onNextFrame();
}

void 
RenderDevice::waitIdle()
{

}

SPtr<Texture2D>	
RenderDevice::createSolidColorTexture2D(const Color4b& color)
{
	#if 0
	int w = 4;
	int h = 4;
	Texture2D_CreateDesc texDesc;
	texDesc.format		= ColorType::RGBAb;
	texDesc.mipCount	= 1;
	//texDesc._size.set(w, h);

	auto& image = texDesc._uploadImage;
	image.create(Color4b::s_kColorType, w, h);

	for (int y = 0; y < h; y++) 
	{
		auto span = image.row<Color4b>(y);
		for (int x = 0; x < w; x++) 
		{
			span[x] = color;
		}
	}
	return createTexture2D(texDesc);
	#else
	return nullptr;
	#endif // 0
}

#endif
}
