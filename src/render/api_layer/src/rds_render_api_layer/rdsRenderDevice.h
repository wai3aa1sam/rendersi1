#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "vertex/rdsVertexLayoutManager.h"
#include "thread/rdsRenderFrame.h"
#include "thread/rdsRenderFrameParam.h"

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
class	Texture2DArray;
struct	Texture_CreateDesc;
struct	Texture2D_CreateDesc;
struct	Texture3D_CreateDesc;
struct	TextureCube_CreateDesc;
struct	Texture2DArray_CreateDesc;

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

	void resetEngineFrame(u64 engineFrameCount);

	virtual void waitIdle();

public:
	SPtr<RenderContext>			createContext(				const	RenderContext_CreateDesc&		cDesc);
	SPtr<RenderGpuBuffer>		createRenderGpuBuffer(				RenderGpuBuffer_CreateDesc&		cDesc);
	SPtr<RenderGpuMultiBuffer>	createRenderGpuMultiBuffer(			RenderGpuBuffer_CreateDesc&		cDesc);
	SPtr<Texture>				createTexture(						Texture_CreateDesc&				cDesc);
	SPtr<Texture2D>				createTexture2D(					Texture2D_CreateDesc&			cDesc);
	SPtr<Texture3D>				createTexture3D(					Texture3D_CreateDesc& cDesc);
	SPtr<TextureCube>			createTextureCube(					TextureCube_CreateDesc&			cDesc);
	SPtr<Texture2DArray>		createTexture2DArray(				Texture2DArray_CreateDesc&			cDesc);
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
	const	RenderAdapterInfo&		adapterInfo() const;
			ShaderStock&			shaderStock();
			TextureStock&			textureStock();
			RenderFrame&			renderFrame(	u64	frameIdx);
			TransferFrame&			transferFrame(	u64	frameIdx);
			TransferContext&		transferContext();
			TransferRequest&		transferRequest(u64 frameIdx);

			BindlessResources&	bindlessResource();

			RenderFrameParam&	renderFrameParam();
	const	RenderFrameParam&	renderFrameParam() const;

	RenderApiType	apiType()		const;

	u64				engineFrameCount()	const;
	u32				engineFrameIndex()	const;
	u64				frameCount()		const;
	u32				frameIndex()		const;

	bool hasCreated() const;

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onDestroy();
	virtual void onResetFrame(u64 frameCount);

protected:
	virtual SPtr<RenderContext>			onCreateContext(			const	RenderContext_CreateDesc&		cDesc)	= 0;
	virtual SPtr<RenderGpuBuffer>		onCreateRenderGpuBuffer(			RenderGpuBuffer_CreateDesc&		cDesc)	= 0;
	virtual SPtr<Texture2D>				onCreateTexture2D(					Texture2D_CreateDesc&			cDesc)	= 0;
	virtual SPtr<Texture3D>				onCreateTexture3D(					Texture3D_CreateDesc&			cDesc)	= 0;
	virtual SPtr<TextureCube>			onCreateTextureCube(				TextureCube_CreateDesc&			cDesc)	= 0;
	virtual SPtr<Texture2DArray>		onCreateTexture2DArray(				Texture2DArray_CreateDesc&		cDesc)	= 0;
	virtual SPtr<Shader>				onCreateShader(				const	Shader_CreateDesc&				cDesc)	= 0;
	virtual SPtr<Material>				onCreateMaterial(			const	Material_CreateDesc&			cDesc)	= 0;

protected:
	RenderApiType		_apiType = RenderApiType::Vulkan;

	RenderAdapterInfo	_adapterInfo;
	VertexLayoutManager _vertexLayoutManager;

	RenderFrameParam	_rdFrameParam;
	/*
	* TODO: rework
	*/
	Vector<RenderFrame,		s_kFrameInFlightCount> _rdFrames;
	Vector<TransferFrame,	s_kFrameInFlightCount> _tsfFrames;

	BindlessResources*		_bindlessRscs	= nullptr;
	TransferContext*		_tsfCtx			= nullptr;

	ShaderStock			_shaderStock;
	TextureStock		_textureStock;
};

inline const	RenderAdapterInfo&		RenderDevice::adapterInfo()		const		{ return _adapterInfo; }

inline			ShaderStock&			RenderDevice::shaderStock()					{ return _shaderStock; }
inline			TextureStock&			RenderDevice::textureStock()				{ return _textureStock; }

inline			RenderFrame&			RenderDevice::renderFrame(	u64	frameIdx)	{ return _rdFrames[frameIdx]; }
inline			TransferFrame&			RenderDevice::transferFrame(u64 frameIdx)	{ return _tsfFrames[frameIdx]; }

inline			TransferContext&		RenderDevice::transferContext()				{ return *_tsfCtx; }
inline			TransferRequest&		RenderDevice::transferRequest(u64 frameIdx)	{ return transferFrame(frameIdx).transferRequest(); }

inline			BindlessResources&		RenderDevice::bindlessResource()			{ return *_bindlessRscs; }

inline			RenderFrameParam&		RenderDevice::renderFrameParam()			{ return _rdFrameParam; }
inline const	RenderFrameParam&		RenderDevice::renderFrameParam() const		{ return _rdFrameParam; }

inline			RenderApiType			RenderDevice::apiType()		const			{ return _apiType; }

inline			u64						RenderDevice::engineFrameCount()	const	{ return renderFrameParam().engineFrameCount(); }
inline			u32						RenderDevice::engineFrameIndex()	const	{ return sCast<u32>((engineFrameCount()) % s_kFrameInFlightCount); }
inline			u64						RenderDevice::frameCount()			const	{ return renderFrameParam().frameCount(); }
inline			u32						RenderDevice::frameIndex()			const	{ return sCast<u32>((frameCount()) % s_kFrameInFlightCount); }
inline			bool					RenderDevice::hasCreated()			const	{ return !_rdFrames.is_empty(); }

#endif


}