#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "vertex/rdsVertexLayoutManager.h"
#include "rdsRenderFrame.h"
#include "rds_render_api_layer/transfer/rdsTransferFrame.h"

namespace rds
{

#define RenderApiType_ENUM_LIST(E) \
	E(None, = 0) \
	E(OpenGL,) \
	E(Dx11,) \
	E(Metal,) \
	E(Vulkan,) \
	E(Dx12,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderApiType, u8);

#if 0
#pragma mark --- rdsRenderDevice-Decl ---
#endif // 0
#if 1

struct RenderDevice_CreateDesc : public RenderResource
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	RenderDevice_CreateDesc();

public:
	RenderApiType apiType;

	bool isDebug	: 1;
	bool isPresent	: 1;
};

class	RenderContext;
struct	RenderContext_CreateDesc;

class	RenderGpuBuffer;
struct	RenderGpuBuffer_CreateDesc;
class	RenderGpuMultiBuffer;

class	Texture;
class	Texture2D;
struct	Texture_CreateDesc;
struct	Texture2D_CreateDesc;

class	Shader;
struct	Shader_CreateDesc;
class	Material;
struct	Material_CreateDesc;

struct TextureStock
{
	SPtr<Texture2D>	white;
	SPtr<Texture2D>	black;
	SPtr<Texture2D>	red;
	SPtr<Texture2D>	green;
	SPtr<Texture2D>	blue;
	SPtr<Texture2D>	magenta;
	SPtr<Texture2D>	error;
};

class RenderDevice : public RenderResource
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Base			= RenderResource;
	using CreateDesc	= RenderDevice_CreateDesc;

public:
	static CreateDesc makeCDesc();

public:
	RenderDevice();
	virtual ~RenderDevice();

	void create(const CreateDesc& cDesc);
	void destroy();

	void nextFrame();

	virtual void waitIdle();

	SPtr<RenderContext>			createContext				(const	RenderContext_CreateDesc&		cDesc);
	SPtr<RenderGpuBuffer>		createRenderGpuBuffer		(		RenderGpuBuffer_CreateDesc&		cDesc);
	SPtr<RenderGpuMultiBuffer>	createRenderGpuMultiBuffer	(		RenderGpuBuffer_CreateDesc&		cDesc);
	SPtr<Texture2D>				createTexture2D				(		Texture2D_CreateDesc&			cDesc);
	SPtr<Shader>				createShader				(const	Shader_CreateDesc&				cDesc);
	SPtr<Shader>				createShader				(StrView								filename);
	SPtr<Material>				createMaterial				(const	Material_CreateDesc&			cDesc);
	SPtr<Material>				createMaterial				(Shader*								shader);
	SPtr<Material>				createMaterial				();

public:
	SPtr<Texture2D>	createSolidColorTexture2D(const Color4b& color);

public:
	const	RenderAdapterInfo&	adapterInfo() const;
			TextureStock&		textureStock();
			RenderFrame&		renderFrame();
			TransferFrame&		transferFrame();
			TransferContext&	transferContext();
			TransferRequest&	transferRequest();

	u32 iFrame() const;

protected:
	virtual void onCreate	(const CreateDesc& cDesc);
	virtual void onDestroy	();
	virtual void onNextFrame();

protected:
	virtual SPtr<RenderContext>			onCreateContext				(const	RenderContext_CreateDesc&	cDesc)	= 0;
	virtual SPtr<RenderGpuBuffer>		onCreateRenderGpuBuffer		(		RenderGpuBuffer_CreateDesc&	cDesc)	= 0;
	virtual SPtr<Texture2D>				onCreateTexture2D			(		Texture2D_CreateDesc&		cDesc)	= 0;
	virtual SPtr<Shader>				onCreateShader				(const	Shader_CreateDesc&			cDesc)	= 0;
	virtual SPtr<Material>				onCreateMaterial			(const	Material_CreateDesc&		cDesc)	= 0;

protected:
	RenderAdapterInfo	_adapterInfo;
	VertexLayoutManager _vertexLayoutManager;

	Vector<RenderFrame,		s_kFrameInFlightCount> _rdFrames;
	Vector<TransferFrame,	s_kFrameInFlightCount> _tsfFrames;
	u32 _iFrame = 0;

	TransferContext* _tsfCtx = nullptr;
	TransferRequest	 _tsfReq;

	TextureStock _textureStock;
};

inline const	RenderAdapterInfo&		RenderDevice::adapterInfo()		const	{ return _adapterInfo; }
inline			TextureStock&			RenderDevice::textureStock()			{ return _textureStock; }

inline			RenderFrame&			RenderDevice::renderFrame()				{ return _rdFrames[iFrame()]; }
inline			TransferFrame&			RenderDevice::transferFrame()			{ return _tsfFrames[iFrame()]; }

inline			TransferContext&		RenderDevice::transferContext()			{ return *_tsfCtx; }
inline			TransferRequest&		RenderDevice::transferRequest()			{ return _tsfReq; }

inline			u32						RenderDevice::iFrame() const			{ return _iFrame; }

#endif


}