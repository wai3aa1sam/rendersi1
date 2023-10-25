#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "vertex/rdsVertexLayoutManager.h"
#include "rdsRenderFrame.h"

namespace rds
{

#if 0
#pragma mark --- rdsRenderer-Decl ---
#endif // 0
#if 1

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

struct Renderer_CreateDesc
{
public:
	Renderer_CreateDesc();

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

class Renderer : public VirtualSingleton<Renderer>
{
public:
    using Base = VirtualSingleton<Renderer>;

	using CreateDesc = Renderer_CreateDesc;

	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kFrameInFlightCount = RenderApiLayerTraits::s_kFrameInFlightCount;

public:
	static CreateDesc makeCDesc();

public:
	Renderer();
	virtual ~Renderer();

	void create(const CreateDesc& cDesc);
	void destroy();

	SPtr<RenderContext>			createContext				(const RenderContext_CreateDesc&	cDesc);
	SPtr<RenderGpuBuffer>		createRenderGpuBuffer		(const RenderGpuBuffer_CreateDesc&	cDesc);
	SPtr<RenderGpuMultiBuffer>	createRenderGpuMultiBuffer	(const RenderGpuBuffer_CreateDesc&	cDesc);
	SPtr<Texture2D>				createTexture2D				(const Texture2D_CreateDesc&		cDesc);
	SPtr<Shader>				createShader				(const Shader_CreateDesc&			cDesc);
	SPtr<Shader>				createShader				(StrView							filename);
	SPtr<Material>				createMaterial				();
	SPtr<Material>				createMaterial				(Shader*							shader);

	const RenderAdapterInfo& adapterInfo() const;

protected:
	Renderer* _init(const CreateDesc& cDesc);
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

protected:
	virtual SPtr<RenderContext>			onCreateContext				(const RenderContext_CreateDesc&	cDesc)	= 0;
	virtual SPtr<RenderGpuBuffer>		onCreateRenderGpuBuffer		(const RenderGpuBuffer_CreateDesc&	cDesc)	= 0;
	virtual SPtr<Texture2D>				onCreateTexture2D			(const Texture2D_CreateDesc&		cDesc)	= 0;
	virtual SPtr<Shader>				onCreateShader				(const Shader_CreateDesc&			cDesc)	= 0;
	virtual SPtr<Material>				onCreateMaterial			()											= 0;
	virtual SPtr<Material>				onCreateMaterial			(Shader*							shader) = 0;

protected:
	RenderAdapterInfo	_adapterInfo;
	VertexLayoutManager _vertexLayoutManager;
	RenderFrameContext  _renderFrameCtx;
};

inline const RenderAdapterInfo& Renderer::adapterInfo() const { return _adapterInfo; }


#endif


}