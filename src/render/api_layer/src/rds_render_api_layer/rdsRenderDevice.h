#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

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

class	VertexLayoutManager;
class	RenderGraph;

class	RenderContext;
struct	RenderContext_CreateDesc;

class	RenderGpuBuffer;
struct	RenderGpuBuffer_CreateDesc;
class	RenderMultiGpuBuffer;

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

class	RenderJob;
struct	RenderJob_CreateDesc;
class	TransferContext;
struct	TransferContext_CreateDesc;
class	TransferFrame;
struct	TransferFrame_CreateDesc;
class	BindlessResources;
struct	BindlessResources_CreateDesc;

class RenderDevice : public RefCount_Base, public RenderApiLayerCommon_Base
{
	friend class ShaderStock;
public:
	using Base			= RefCount_Base;
	using CreateDesc	= RenderDevice_CreateDesc;

public:
	static CreateDesc makeCDesc();

public:
	RenderDevice();
	virtual ~RenderDevice();

	void create(const CreateDesc& cDesc);
	void destroy();

public:
	class RenderInputFrameParam;
	UPtr<RenderJob> newRenderJob(RenderContext* rdCtx, i64 frameCount);
	void submitRenderJob(UPtr<RenderJob> rdJob);

	//void reset(u64 frameCount);
	//void resetEngineFrame(u64 engineFrameCount);

	void waitIdle();
	//void waitIdle();
	void waitCpuIdle();
	void waitGpuIdle();
	void waitRenderThreadIdle();

public:
	SPtr<RenderContext>			createContext(					RDS_DebugLabel_PARAM,		const	RenderContext_CreateDesc&		cDesc);
	SPtr<RenderGpuBuffer>		createRenderGpuBuffer(			RDS_DebugLabel_PARAM,				RenderGpuBuffer_CreateDesc&		cDesc);
	SPtr<RenderMultiGpuBuffer>	createRenderMultiGpuBuffer(		RDS_DebugLabel_PARAM,				RenderGpuBuffer_CreateDesc&		cDesc);
	SPtr<Texture>				createTexture(					RDS_DebugLabel_PARAM,				Texture_CreateDesc&				cDesc);
	SPtr<Texture2D>				createTexture2D(				RDS_DebugLabel_PARAM,				Texture2D_CreateDesc&			cDesc);
	SPtr<Texture3D>				createTexture3D(				RDS_DebugLabel_PARAM,				Texture3D_CreateDesc&			cDesc);
	SPtr<TextureCube>			createTextureCube(				RDS_DebugLabel_PARAM,				TextureCube_CreateDesc&			cDesc);
	SPtr<Texture2DArray>		createTexture2DArray(			RDS_DebugLabel_PARAM,				Texture2DArray_CreateDesc&		cDesc);
	SPtr<Shader>				createShader(					RDS_DebugLabel_PARAM,		const	Shader_CreateDesc&				cDesc);
	SPtr<Shader>				createShader(					RDS_DebugLabel_PARAM,				StrView							filename);
	SPtr<Shader>				createShader(					RDS_DebugLabel_PARAM,				StrView							filename, const ShaderPermutations& permuts);
	SPtr<Material>				createMaterial(					RDS_DebugLabel_PARAM,		const	Material_CreateDesc&			cDesc);
	SPtr<Material>				createMaterial(					RDS_DebugLabel_PARAM,				Shader*							shader);
	SPtr<Material>				createMaterial(					RDS_DebugLabel_PARAM);

	UPtr<RenderJob>				createRenderJob(				RDS_DebugLabel_PARAM, RenderJob_CreateDesc&			cDesc);
	SPtr<TransferContext>		createTransferContext(			RDS_DebugLabel_PARAM, TransferContext_CreateDesc&		cDesc);
	SPtr<TransferFrame>			createTransferFrame(			RDS_DebugLabel_PARAM, TransferFrame_CreateDesc&		cDesc);	// frame only exist 1 for 1 frame, Request could have many
	SPtr<BindlessResources>		createBindlessResources(		RDS_DebugLabel_PARAM, BindlessResources_CreateDesc&	cDesc);

public:
	const	RenderAdapterInfo&		adapterInfo() const;
			ShaderStock&			shaderStock();
			TextureStock&			textureStock();
//			RenderFrame&			renderFrame(	u64	frameIdx);
			TransferFrame&			transferFrame();
			TransferContext&		transferContext();
			TransferContext*		transferContextPtr();
			
			TransferRequest&		transferRequest();

			BindlessResources&	bindlessResource();

	RenderApiType	apiType()		const;

	bool			isQuit();

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onDestroy();
	virtual void onResetFrame(i64 frameCount);

protected:
	virtual SPtr<RenderContext>			onCreateContext(			const	RenderContext_CreateDesc&		cDesc)	= 0;
	virtual SPtr<RenderGpuBuffer>		onCreateRenderGpuBuffer(			RenderGpuBuffer_CreateDesc&		cDesc)	= 0;
	virtual SPtr<Texture2D>				onCreateTexture2D(					Texture2D_CreateDesc&			cDesc)	= 0;
	virtual SPtr<Texture3D>				onCreateTexture3D(					Texture3D_CreateDesc&			cDesc)	= 0;
	virtual SPtr<TextureCube>			onCreateTextureCube(				TextureCube_CreateDesc&			cDesc)	= 0;
	virtual SPtr<Texture2DArray>		onCreateTexture2DArray(				Texture2DArray_CreateDesc&		cDesc)	= 0;
	virtual SPtr<Shader>				onCreateShader(				const	Shader_CreateDesc&				cDesc)	= 0;
	virtual SPtr<Material>				onCreateMaterial(			const	Material_CreateDesc&			cDesc)	= 0;

	virtual UPtr<RenderJob>				onCreateRenderJob(					RenderJob_CreateDesc&			cDesc)	= 0;
	virtual SPtr<TransferContext>		onCreateTransferContext(			TransferContext_CreateDesc&		cDesc)	= 0;
	virtual SPtr<TransferFrame>			onCreateTransferFrame(				TransferFrame_CreateDesc&		cDesc)	= 0;
	virtual SPtr<BindlessResources>		onCreateBindlessResources(			BindlessResources_CreateDesc&	cDesc)	= 0;

public:
	void			_internal_freeRenderJob(UPtr<RenderJob> rdJob);
	virtual void	_internal_waitGpuIdle() = 0;
	void			_internal_createRenderResource(RenderResource* rdRsc);

private:
	void _createRenderJobs();

protected:
	RenderApiType		_apiType = RenderApiType::Vulkan;

	// TODO: RenderThread version layer for all resource, and specific type of that resource
	// eg. Render_RenderResource, Render_RenderDevice
	// Render_RenderDevice* _rd_rdDev = nullptr;

	//UPtr<RenderGraph>			_rdGraph = nullptr;	// TODO: temp
	RenderThread				_rdThread;			// Consumer inside, _pendingRdJobs; 
	// no need processingRdJobs, this is for check the gpu side is completed or not
	CondQueue<UPtr<RenderJob> >	_freeRdJobs;		// Producer

	//RenderFrameParam			_rdFrameParam;

	RenderAdapterInfo	_adapterInfo;
	UPtr<VertexLayoutManager> _vertexLayoutManager;

	/*
	* TODO: rework
	*/
	//Vector<RenderFrame,		s_kMaxFrameAheadCountHardLimit> _rdFrames;
	//Vector<TransferFrame,	s_kMaxFrameAheadCountHardLimit> _tsfFrames;
	SPtr<BindlessResources>		_bindlessRscs	= nullptr;
	SPtr<TransferContext>		_tsfCtx			= nullptr;

	ShaderStock			_shaderStock;
	TextureStock		_textureStock;

	struct Debug
	{
		Vector<SPtr<RenderResource> > rdRscs;
	} _debug;
};

inline const	RenderAdapterInfo&		RenderDevice::adapterInfo()		const		{ return _adapterInfo; }

inline			ShaderStock&			RenderDevice::shaderStock()					{ return _shaderStock; }
inline			TextureStock&			RenderDevice::textureStock()				{ return _textureStock; }

//inline			RenderFrame&			RenderDevice::renderFrame(	u64	frameIdx)	{ _notYetSupported(RDS_SRCLOC); return _rdFrames[frameIdx]; }
//inline			UPtr<TransferFrame>		RenderDevice::releaseTransferFrame()		{ checkMainThreadExclusive(RDS_SRCLOC);	RDS_ASSERT(_tsfFrame, "_tsfFrame not exist"); auto p = rds::move(_tsfFrame); return p; }

inline			TransferContext&		RenderDevice::transferContext()				{ return *_tsfCtx; }
inline			TransferContext*		RenderDevice::transferContextPtr()			{ return _tsfCtx; }

inline			BindlessResources&		RenderDevice::bindlessResource()			{ return *_bindlessRscs; }

inline			RenderApiType			RenderDevice::apiType()		const			{ return _apiType; }

#endif

}

#define RDS_RENDER_DEVICE_INTERFACE_ON_CREATE(T) \
virtual SPtr<RenderContext>			onCreateContext(			const	RenderContext_CreateDesc&		cDesc)	override;	\
virtual SPtr<RenderGpuBuffer>		onCreateRenderGpuBuffer(			RenderGpuBuffer_CreateDesc&		cDesc)	override;	\
virtual SPtr<Texture2D>				onCreateTexture2D(					Texture2D_CreateDesc&			cDesc)	override;	\
virtual SPtr<Texture3D>				onCreateTexture3D(					Texture3D_CreateDesc&			cDesc)	override;	\
virtual SPtr<TextureCube>			onCreateTextureCube(				TextureCube_CreateDesc&			cDesc)	override;	\
virtual SPtr<Texture2DArray>		onCreateTexture2DArray(				Texture2DArray_CreateDesc&		cDesc)	override;	\
virtual SPtr<Shader>				onCreateShader(				const	Shader_CreateDesc&				cDesc)	override;	\
virtual SPtr<Material>				onCreateMaterial(			const	Material_CreateDesc&			cDesc)	override;	\
\
virtual UPtr<RenderJob>				onCreateRenderJob(					RenderJob_CreateDesc&			cDesc)	override;	\
virtual SPtr<TransferContext>		onCreateTransferContext(			TransferContext_CreateDesc&		cDesc)	override;	\
virtual SPtr<TransferFrame>			onCreateTransferFrame(				TransferFrame_CreateDesc&		cDesc)	override;	\
virtual SPtr<BindlessResources>		onCreateBindlessResources(			BindlessResources_CreateDesc&	cDesc)	override;	\
// ---

