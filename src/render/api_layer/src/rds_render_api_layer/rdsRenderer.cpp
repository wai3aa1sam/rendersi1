#include "rds_render_api_layer-pch.h"

#include "rdsRenderer.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderer_Vk.h"

#include "rds_render_api_layer/texture/rdsTexture.h"

namespace rds
{
#if 0
#pragma mark --- rdsRenderer-Impl ---
#endif // 0
#if 1

Renderer* Renderer::s_instance = nullptr;

Renderer_CreateDesc::Renderer_CreateDesc()
{
	apiType		= RenderApiType::Vulkan;
	isPresent	= true;

	#if RDS_DEBUG
	isDebug = true;
	#else
	isDebug = false;
	#endif // RDS_DEBUG

}

Renderer::CreateDesc Renderer::makeCDesc() { return CreateDesc{}; }

Renderer::Renderer()
	: Base()
{
}

Renderer::~Renderer()
{

}

void 
Renderer::create(const CreateDesc& cDesc)
{
	auto* rdr = _init(cDesc); RDS_UNUSED(rdr);
	rdr->onCreate(cDesc);

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

	rdr->_tsfCtx->transferRequest().reset(rdr->_tsfCtx);
}


void 
Renderer::destroy()
{

	onDestroy();

	RDS_CORE_ASSERT(_rdFrames.is_empty(),	"forgot to clear RenderFrame in derived class");
	RDS_CORE_ASSERT(_tsfFrames.is_empty(),	"forgot to clear TransferFrame in derived class");
}

void 
Renderer::onCreate(const CreateDesc& cDesc)
{
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
Renderer::onDestroy()
{
	
}

void 
Renderer::onNextFrame()
{
}

void 
Renderer::nextFrame()
{
	_iFrame = (_iFrame + 1) % s_kFrameInFlightCount;

	renderFrame().clear();
	transferFrame().clear();
	_tsfReq.reset(_tsfCtx);

	onNextFrame();
}

SPtr<Texture2D>	
Renderer::createSolidColorTexture2D(const Color4b& color)
{
	int w = 4;
	int h = 4;
	Texture2D_CreateDesc texDesc;
	texDesc.format		= ColorType::RGBAb;
	texDesc.mipCount	= 1;
	texDesc._size.set(w, h);

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
}

Renderer*
Renderer::_init(const CreateDesc& cDesc)
{
	Renderer* rdr = nullptr;
	switch (cDesc.apiType)
	{
		using EM = RenderApiType;
		case EM::Vulkan: { rdr = RDS_NEW(Renderer_Vk)(); } break;

		default: { throwIf(false, "unsupported render api"); } break;
	}
	s_instance = rdr;
	
	s_instance->_adapterInfo.isDebug = cDesc.isDebug;

	return rdr;
}

#endif
}
