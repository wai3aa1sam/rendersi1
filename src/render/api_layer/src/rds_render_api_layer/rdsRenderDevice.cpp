#include "rds_render_api_layer-pch.h"

#include "rdsRenderDevice.h"
//#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rdsRenderer.h"

#include "vertex/rdsVertexLayoutManager.h"

#include "rds_render_api_layer/graph/rdsRenderGraph.h"
#include "rds_render_api_layer/thread/rdsRenderJob.h"
#include "rds_render_api_layer/rdsRenderContext.h"

#include "rds_render_api_layer/transfer/rdsTransferContext.h"
#include "rds_render_api_layer/transfer/rdsTransferFrame.h"

#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rds_render_api_layer/texture/rdsTexture2DArray.h"
#include "rds_render_api_layer/texture/rdsTexture3D.h"
#include "rds_render_api_layer/texture/rdsTextureCube.h"

#include "rds_render_api_layer/shader/rdsShader.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderDevice-Impl ---
#endif // 0
#if 1

RenderDevice_CreateDesc::RenderDevice_CreateDesc()
{
	info.apiType				= RenderApiType::Vulkan;
	info.isPresent				= true;
	info.isMultithread			= true;
	info.isCompileShaderMode	= false;
	info.isDebug				= RDS_DEBUG;
}

bool 
RenderDevice_CreateDesc::isShaderCompileMode() const
{
	return info.isCompileShaderMode;
}

#endif

#if 0
#pragma mark --- rdsRenderDevice-Impl ---
#endif // 0
#if 1

RenderDevice::CreateDesc RenderDevice::makeCDesc() { return CreateDesc{}; }

RenderDevice::RenderDevice()
	: Base()
{
}

RenderDevice::~RenderDevice()
{
	//RDS_LOG_DEBUG("~RenderDevice()");
}

void 
RenderDevice::create(const CreateDesc& cDesc)
{
	_adapterInfo.create(cDesc.info);

	_vertexLayoutManager = makeUPtr<VertexLayoutManager>();

	onCreate(cDesc);

	auto bindlessRscVk_cDesc = BindlessResources::makeCDesc();
	_bindlessRscs = createBindlessResources(bindlessRscVk_cDesc);

	auto TransferContext_cDesc = TransferContext::makeCDesc();
	_tsfCtx = createTransferContext(TransferContext_cDesc);

	if (cDesc.isShaderCompileMode())
		return;
	
	_createRenderJobs();

	auto rdThreadCDesc = RenderThread::makeCDesc(this, JobSystem::instance());
	_rdThread.create(rdThreadCDesc);
	if (!_adapterInfo.isMultithread)
	{
		_rdThread.quit();
	}

	_shaderStock.create(this);
	_textureStock.create(this);
}

void 
RenderDevice::destroy()
{
	onDestroy();
}

void 
RenderDevice::onCreate(const CreateDesc& cDesc)
{

}

void 
RenderDevice::onDestroy()
{
	_shaderStock.destroy();
	_textureStock.destroy();

	_debug.rdRscs.clear();

	#if 0	// old impl
	RDS_TODO("revise, kind of weird, may be tsfCtx could just run destroyRenderResources is ok");
	//_rdThread.waitIdle();
	//_freeRdJobs.clear();	// ensure all the resources will be free
	//_createRenderJobs();	// destroy need to submit, so need to create
	_rdThread.destroy();	
	_freeRdJobs.clear();	// release TransferFrame before destroy TransferContext
	#else
	_rdThread.destroy();
	_freeRdJobs.clear();
	#endif // 0

	if (_tsfCtx)
	{
		_tsfCtx->destroy();
		_tsfCtx = nullptr;
	}

	if (_bindlessRscs)
	{
		_bindlessRscs->destroy();
		_bindlessRscs = nullptr;
	}

	RDS_CORE_ASSERT(!_bindlessRscs,			"forgot to call destroy() _bindlessRscs");
	RDS_CORE_ASSERT(!_tsfCtx,				"forgot to call destroy() _tsfCtx");
	//RDS_CORE_ASSERT(_rdFrames.is_empty(),	"forgot to clear RenderFrame in derived class");
	//RDS_CORE_ASSERT(_tsfFrames.is_empty(),	"forgot to clear TransferFrame in derived class");
}

UPtr<RenderJob> 
RenderDevice::newRenderJob(RenderContext* rdCtx, i64 frameCount)
{
	UPtr<RenderJob> o;
	for (;;)
	{
		_freeRdJobs.try_pop(o);
		if (o)
			break;
		_rdThread._checkUploadCompletedJob();
		OsUtil::sleep_ms(0); RDS_TODO("pass a param here");
	}
	//o->_renderGraph = _rdGraph;
	o->reset(this, rdCtx, frameCount);
	return o;
}

void 
RenderDevice::_internal_freeRenderJob(UPtr<RenderJob> rdJob)
{
	//transferContext()._internal_freeTransferFrame(rds::move(rdJob->transferFrame));
	_freeRdJobs.push(rds::move(rdJob));
}

void 
RenderDevice::_internal_createRenderResource(RenderResource* rdRsc)
{
	_debug.rdRscs.emplace_back(rdRsc);
}

void 
RenderDevice::_createRenderJobs()
{
	for (size_t i = 0; i < s_kMaxFrameAheadCountHardLimit; i++)
	{
		RenderJob_CreateDesc rdJob_cDesc = {};
		rdJob_cDesc.renderDevice = this;
		auto o = createRenderJob(rdJob_cDesc);
		_freeRdJobs.push(rds::move(o));
	}
}

void 
RenderDevice::submitRenderJob(UPtr<RenderJob> rdJob)
{
	RDS_TODO("**** must wait all async upload stuff when submit");
	_tsfCtx->submit(rdJob);
	if (adapterInfo().isMultithread)
	{
		_rdThread.requestRender(rds::move(rdJob));
	}
	else
	{
		_rdThread.render(rds::move(rdJob));
	}
}

//void 
//RenderDevice::reset(u64 frameCount)
//{
//	RDS_TODO("remove, make a Base class FrameParam and separate frame, also, do not use this frame for Material");
//
//	checkRenderThreadExclusive(RDS_SRCLOC);
//
//	//renderFrameParam().reset(frameCount);
//	onResetFrame(frameCount);
//}

void 
RenderDevice::waitIdle()
{
	_rdThread.waitIdle();
}

void 
RenderDevice::waitCpuIdle()
{
	while (_freeRdJobs.size() != s_kMaxFrameAheadCountHardLimit)
	{
		OsUtil::sleep_ms(1);
	};
}

void
RenderDevice::waitGpuIdle()
{
	_rdThread.waitGpuIdle();
}

void 
RenderDevice::waitRenderThreadIdle()
{
	_rdThread.waitCpuIdle();
}

void 
RenderDevice::onResetFrame(i64 frameCount)
{
}

#endif

#if 0
#pragma mark --- rdsRenderDevice::RenderResource-Impl ---
#endif // 0
#if 1

UPtr<RenderJob> 
RenderDevice::createRenderJob(RenderJob_CreateDesc& cDesc)
{
	auto p = onCreateRenderJob(cDesc);
	p->create(cDesc);
	return p;
}

SPtr<RenderContext> 
RenderDevice::createContext(const RenderContext_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateContext(cDesc);
	p->create(cDesc);
	return p;
}

SPtr<TransferContext>
RenderDevice::createTransferContext(TransferContext_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateTransferContext(cDesc);
	p->create(cDesc);
	return p;
}

SPtr<TransferFrame> 
RenderDevice::createTransferFrame(TransferFrame_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateTransferFrame(cDesc);
	p->create(cDesc);
	return p;
}

SPtr<BindlessResources> 
RenderDevice::createBindlessResources(BindlessResources_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateBindlessResources(cDesc);
	p->create(cDesc);
	return p;
}

SPtr<RenderGpuBuffer> 
RenderDevice::createRenderGpuBuffer(RenderGpuBuffer_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateRenderGpuBuffer(cDesc);
	p->create(cDesc);
	return p;
}


SPtr<RenderMultiGpuBuffer> 
RenderDevice::createRenderMultiGpuBuffer(RenderGpuBuffer_CreateDesc& cDesc)
{
	auto p = makeSPtr<RenderMultiGpuBuffer>();
	cDesc._internal_create(this);
	p->create(cDesc);
	return p;
}

SPtr<Texture>
RenderDevice::createTexture(Texture_CreateDesc& cDesc)
{
	SPtr<Texture> o;
	switch (cDesc.type)
	{
		case RenderDataType::Texture2D:			{ auto p = createTexture2D(			sCast<Texture2D_CreateDesc&>(		cDesc)); o.reset(p.ptr()); } break;
		case RenderDataType::Texture3D:			{ auto p = createTexture3D(			sCast<Texture3D_CreateDesc&>(		cDesc)); o.reset(p.ptr()); } break;
		case RenderDataType::TextureCube:		{ auto p = createTextureCube(		sCast<TextureCube_CreateDesc&>(		cDesc)); o.reset(p.ptr()); } break;
		case RenderDataType::Texture2DArray:	{ auto p = createTexture2DArray(	sCast<Texture2DArray_CreateDesc&>(	cDesc)); o.reset(p.ptr()); } break;
		default: { RDS_THROW("invalid texture type"); } break;
	}
	return o;
}

SPtr<Texture2D> 
RenderDevice::createTexture2D(Texture2D_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateTexture2D(cDesc);
	p->create(cDesc);
	return p;
}

SPtr<Texture2DArray> 
RenderDevice::createTexture2DArray(Texture2DArray_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateTexture2DArray(cDesc);
	p->create(cDesc);
	return p;
}

SPtr<Texture3D> 
RenderDevice::createTexture3D(Texture3D_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateTexture3D(cDesc);
	p->create(cDesc);
	return p;
}

SPtr<TextureCube> 
RenderDevice::createTextureCube(TextureCube_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateTextureCube(cDesc);
	p->create(cDesc);
	return p;
}

SPtr<Shader> 
RenderDevice::createShader(const Shader_CreateDesc& cDesc)
{
	auto& ss = shaderStock();
	if (auto p = ss.findShader(cDesc))
	{
		return p;
	}

	cDesc._internal_create(this);
	auto p = onCreateShader(cDesc);
	p->create(cDesc);

	ss.appendUnqiueShader(p);
	return p;
}

SPtr<Shader> 
RenderDevice::createShader(StrView filename)
{
	auto cDesc = Shader::makeCDesc();
	cDesc.filename	= filename;

	SPtr<Shader> p = createShader(cDesc);
	return p;
}

SPtr<Shader>
RenderDevice::createShader(StrView filename, const ShaderPermutations& permuts)
{
	auto cDesc = Shader::makeCDesc();
	cDesc.filename	= filename;
	cDesc.permuts	= &permuts;

	SPtr<Shader> p = createShader(cDesc);
	return p;
}

SPtr<Material> 
RenderDevice::createMaterial(const Material_CreateDesc& cDesc)
{
	cDesc._internal_create(this);
	auto p = onCreateMaterial(cDesc);
	p->create(cDesc);
	return p;
}

SPtr<Material> 
RenderDevice::createMaterial(Shader* shader)
{
	auto cDesc = Material::makeCDesc();
	cDesc.shader = shader;
	return createMaterial(cDesc);
}

SPtr<Material> 
RenderDevice::createMaterial()
{
	auto cDesc = Material::makeCDesc();
	auto p = createMaterial(cDesc);
	return p;
}


#endif // 1

TransferRequest&		RenderDevice::transferRequest()				{ checkMainThreadExclusive(RDS_SRCLOC); return transferFrame().transferRequest(); }
TransferFrame&			RenderDevice::transferFrame()				{ checkMainThreadExclusive(RDS_SRCLOC);	return transferContext().transferFrame(); }
bool					RenderDevice::isQuit()						{ return _rdThread.isQuit(); }

template<class T>
void 
RdsDeleter<T, EnableIf<IsBaseOf<RenderResource, T> > >::rds_delete(T* p) RDS_NOEXCEPT
{
	if (p)
	{
		RDS_TODO("some type are using SPtr<RenderResource>, it will make it fail to call _internal_requestDestroyObject() \n"
			"it is ok to block the usage of SPtr<RenderResource> later"
		);

		#if 0

		// if constexpr (IsBaseOf<Texture, T> || IsSame<T, RenderGpuBuffer>) {}	// this work btw, but not work when T is RenderResource
		p->_internal_requestDestroyObject();

		#else

		RDS_TODO("no RenderResource then all can call T::destroy()"
			"/ just use RenderResource::destroy to call onDestroy()"
			"call destroy() in here is better? Derived class no need to call destroy() in dtor"
		);
		using SRC = RenderResourceType;
		switch (p->renderResourceType())
		{
			case SRC::RenderContext:	{ reinCast<RenderContext*>(		p)->destroy(); } break;
			case SRC::RenderGpuBuffer:	{ reinCast<RenderGpuBuffer*>(	p)->destroy(); } break;
			case SRC::Texture:			{ reinCast<Texture*>(			p)->destroy(); } break;
			default: { rds_delete_impl(p); } break;
		}

		#endif // 0
	}
}
}
