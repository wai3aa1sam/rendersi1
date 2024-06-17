#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "vertex/rdsVertexLayoutManager.h"
#include "rdsRenderFrame.h"
#include "rds_render_api_layer/transfer/rdsTransferFrame.h"
#include "rds_render_api_layer/shader/rdsBindlessResources.h"

#include "texture/rdsTextureStock.h"
#include "shader/rdsShaderStock.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderDevice-Decl ---
#endif // 0
#if 1

struct RenderDevice_CreateDesc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	RenderDevice_CreateDesc();

public:
	RenderApiType apiType;

	bool isDebug			: 1;
	bool isPresent			: 1;

public:
	bool isShaderCompileMode() const;
};

class	RenderContext;
struct	RenderContext_CreateDesc;

class	RenderGpuBuffer;
struct	RenderGpuBuffer_CreateDesc;
class	RenderGpuMultiBuffer;

class	Texture;
class	Texture2D;
class	TextureCube;
struct	Texture_CreateDesc;
struct	Texture2D_CreateDesc;
struct	Texture3D_CreateDesc;
struct	TextureCube_CreateDesc;

class	Shader;
struct	Shader_CreateDesc;
class	ShaderPermutations;
class	Material;
struct	Material_CreateDesc;

class RenderDevice : public RenderResource
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
	friend class ShaderStock;
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

	SPtr<RenderContext>			createContext(				const	RenderContext_CreateDesc&		cDesc);
	SPtr<RenderGpuBuffer>		createRenderGpuBuffer(				RenderGpuBuffer_CreateDesc&		cDesc);
	SPtr<RenderGpuMultiBuffer>	createRenderGpuMultiBuffer(			RenderGpuBuffer_CreateDesc&		cDesc);
	SPtr<Texture>				createTexture(						Texture_CreateDesc&				cDesc);
	SPtr<Texture2D>				createTexture2D(					Texture2D_CreateDesc&			cDesc);
	SPtr<Texture3D>				createTexture3D(					Texture3D_CreateDesc& cDesc);
	SPtr<TextureCube>			createTextureCube(					TextureCube_CreateDesc&			cDesc);
	SPtr<Shader>				createShader(				const	Shader_CreateDesc&				cDesc);
	SPtr<Shader>				createShader(						StrView							filename);
	SPtr<Shader>				createShader(						StrView							filename, const ShaderPermutations& permuts);
	SPtr<Material>				createMaterial(				const	Material_CreateDesc&			cDesc);
	SPtr<Material>				createMaterial(						Shader*							shader);
	SPtr<Material>				createMaterial();

public:
	SPtr<Texture2D>	createSolidColorTexture2D(  const Color4b& color);
	SPtr<Texture2D>	createCheckerboardTexture2D(const Color4b& color);

public:
	const	RenderAdapterInfo&	adapterInfo() const;
			ShaderStock&		shaderStock();
			TextureStock&		textureStock();
			RenderFrame&		renderFrame();
			TransferFrame&		transferFrame();
			TransferContext&	transferContext();
			TransferRequest&	transferRequest();

			BindlessResources&	bindlessResource();

	RenderApiType	apiType()	const;
	u32				iFrame()	const;

protected:
	virtual void onCreate	(const CreateDesc& cDesc);
	virtual void onDestroy	();
	virtual void onNextFrame();

protected:
	virtual SPtr<RenderContext>			onCreateContext(			const	RenderContext_CreateDesc&	cDesc)	= 0;
	virtual SPtr<RenderGpuBuffer>		onCreateRenderGpuBuffer(			RenderGpuBuffer_CreateDesc&	cDesc)	= 0;
	virtual SPtr<Texture2D>				onCreateTexture2D(					Texture2D_CreateDesc&		cDesc)	= 0;
	virtual SPtr<Texture3D>				onCreateTexture3D(					Texture3D_CreateDesc&		cDesc)	= 0;
	virtual SPtr<TextureCube>			onCreateTextureCube(				TextureCube_CreateDesc&		cDesc)	= 0;
	virtual SPtr<Shader>				onCreateShader(				const	Shader_CreateDesc&			cDesc)	= 0;
	virtual SPtr<Material>				onCreateMaterial(			const	Material_CreateDesc&		cDesc)	= 0;

protected:
	RenderApiType		_apiType = RenderApiType::Vulkan;

	RenderAdapterInfo	_adapterInfo;
	VertexLayoutManager _vertexLayoutManager;

	Vector<RenderFrame,		s_kFrameInFlightCount> _rdFrames;
	Vector<TransferFrame,	s_kFrameInFlightCount> _tsfFrames;
	u32 _iFrame = 0;

	TransferContext* _tsfCtx = nullptr;
	TransferRequest	 _tsfReq;

	BindlessResources*	_bindlessRscs = nullptr;

	ShaderStock			_shaderStock;
	TextureStock		_textureStock;
};

inline const	RenderAdapterInfo&		RenderDevice::adapterInfo()		const	{ return _adapterInfo; }

inline			ShaderStock&			RenderDevice::shaderStock()				{ return _shaderStock; }
inline			TextureStock&			RenderDevice::textureStock()			{ return _textureStock; }

inline			RenderFrame&			RenderDevice::renderFrame()				{ return _rdFrames[iFrame()]; }
inline			TransferFrame&			RenderDevice::transferFrame()			{ return _tsfFrames[iFrame()]; }

inline			TransferContext&		RenderDevice::transferContext()			{ return *_tsfCtx; }
inline			TransferRequest&		RenderDevice::transferRequest()			{ return _tsfReq; }

inline			BindlessResources&		RenderDevice::bindlessResource()		{ return *_bindlessRscs; }

inline			RenderApiType			RenderDevice::apiType() const			{ return _apiType; }
inline			u32						RenderDevice::iFrame()	const			{ return _iFrame; }

#endif


}