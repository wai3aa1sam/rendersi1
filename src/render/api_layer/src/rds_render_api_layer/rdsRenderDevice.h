#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "vertex/rdsVertexLayoutManager.h"
#include "thread/rdsRenderFrame.h"
#include "thread/rdsRenderFrameParam.h"

#include "rds_render_api_layer/transfer/rdsTransferFrame.h"
#include "rds_render_api_layer/shader/rdsBindlessResources.h"

#include "texture/rdsTextureStock.h"
#include "shader/rdsShaderStock.h"

#include "thread/rdsRenderThread.h"

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
	bool isMultithread		: 1;
	bool isPresent			: 1;

public:
	bool isShaderCompileMode() const;
};

class	RenderJob;

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

class	TransferFrame;
struct	TransferFrame_CreateDesc;

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

	void reset(u64 frameCount);

	void createRenderJob(u64 frameCount, bool isRetry = true);
	void submitRenderJob();
	void waitCpuIdle(int sleepMs) const;
	//void waitGpuIdle(RenderContext* rdCtx, u64 frameCount, int sleepMs) const;

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

	UPtr<TransferFrame>			createTransferFrame(				TransferFrame_CreateDesc&		cDesc);

public:
	SPtr<Texture2D>	createSolidColorTexture2D(  const Color4b& color);
	SPtr<Texture2D>	createCheckerboardTexture2D(const Color4b& color);

public:
	const	RenderAdapterInfo&		adapterInfo() const;
			ShaderStock&			shaderStock();
			TextureStock&			textureStock();

			RenderJob&				renderJob();
			TransferFrame&			transferFrame();

//			RenderFrame&			renderFrame(	u64	frameIdx);
			//TransferFrame&			transferFrame();
			//UPtr<TransferFrame>		releaseTransferFrame();
			TransferContext&		transferContext();
			//TransferRequest&		transferRequest();

			BindlessResources&	bindlessResource();

			RenderFrameParam&	renderFrameParam();
	const	RenderFrameParam&	renderFrameParam() const;

	RenderApiType	apiType()		const;

	u64				engineFrameCount()	const;
	u32				engineFrameIndex()	const;
	u64				frameCount()		const;
	u32				frameIndex()		const;

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

	virtual UPtr<TransferFrame>			onCreateTransferFrame(				TransferFrame_CreateDesc&		cDesc)	= 0;

protected:
	RenderApiType		_apiType = RenderApiType::Vulkan;

	RenderAdapterInfo	_adapterInfo;
	VertexLayoutManager _vertexLayoutManager;

	RenderFrameParam	_rdFrameParam;
	RenderThread		_rdThread;
	UPtr<RenderJob>		_rdJob = nullptr;

	/*
	* TODO: rework
	*/
	//Vector<RenderFrame,		s_kFrameInFlightCount> _rdFrames;
	//Vector<TransferFrame,	s_kFrameInFlightCount> _tsfFrames;
	//UPtr<TransferFrame>		_tsfFrame = nullptr;


	BindlessResources*		_bindlessRscs	= nullptr;
	TransferContext*		_tsfCtx			= nullptr;

	ShaderStock			_shaderStock;
	TextureStock		_textureStock;
};

inline const	RenderAdapterInfo&		RenderDevice::adapterInfo()		const		{ return _adapterInfo; }

inline			ShaderStock&			RenderDevice::shaderStock()					{ return _shaderStock; }
inline			TextureStock&			RenderDevice::textureStock()				{ return _textureStock; }

inline			RenderJob&				RenderDevice::renderJob()					{ RDS_CORE_ASSERT(_rdJob, "forgot to call createRenderJob()"); return *_rdJob; }
inline			TransferFrame&			RenderDevice::transferFrame()				{ return *_rdJob->transferFrame; }

//inline			RenderFrame&			RenderDevice::renderFrame(	u64	frameIdx)	{ _notYetSupported(RDS_SRCLOC); return _rdFrames[frameIdx]; }
//inline			TransferFrame&			RenderDevice::transferFrame()				{ checkMainThreadExclusive(RDS_SRCLOC);	RDS_ASSERT(_tsfFrame, "_tsfFrame not exist"); return *_tsfFrame; }
//inline			UPtr<TransferFrame>		RenderDevice::releaseTransferFrame()		{ checkMainThreadExclusive(RDS_SRCLOC);	RDS_ASSERT(_tsfFrame, "_tsfFrame not exist"); auto p = rds::move(_tsfFrame); return p; }

inline			TransferContext&		RenderDevice::transferContext()				{ return *_tsfCtx; }

inline			BindlessResources&		RenderDevice::bindlessResource()			{ return *_bindlessRscs; }

inline			RenderFrameParam&		RenderDevice::renderFrameParam()			{ return _rdFrameParam; }
inline const	RenderFrameParam&		RenderDevice::renderFrameParam() const		{ return _rdFrameParam; }

inline			RenderApiType			RenderDevice::apiType()		const			{ return _apiType; }

inline			u64						RenderDevice::engineFrameCount()	const	{ return renderFrameParam().engineFrameCount(); }
inline			u32						RenderDevice::engineFrameIndex()	const	{ return sCast<u32>((engineFrameCount()) % s_kFrameInFlightCount); }
inline			u64						RenderDevice::frameCount()			const	{ return renderFrameParam().frameCount(); }
inline			u32						RenderDevice::frameIndex()			const	{ return sCast<u32>((frameCount()) % s_kFrameInFlightCount); }

#endif


}