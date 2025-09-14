#include "rds_fluid_simulation-pch.h"
#include "rdsFluidSim_ParticleDisplay.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim_ParticleDisplay-Impl ---
#endif // 0
#if 1

void 
FluidSim_ParticleDisplay::s_createColorGradientTexture(SPtr<Texture2D>& oTex, const ColorGradient& colorGradient)
{
	int w = 64;
	int h = 4;
	auto texDesc = Texture2D::makeCDesc(RDS_SRCLOC);
	texDesc.usageFlags	= TextureUsageFlags::ShaderResource;
	texDesc.format		= ColorType::RGBAb;
	texDesc.mipCount	= 1;
	texDesc.size.set(w, h, 1);

	auto& image = texDesc.uploadImage;
	image.create(Color4b::s_kColorType, w, h);

	for (int y = 0; y < h; y++) 
	{
		auto span = image.row<Color4b>(y);
		for (int x = 0; x < w; x++) 
		{
			float t = x / (w - 1.0f);
			span[x] = colorGradient.evaluate(t).toColorRGBAb();
		}
	}

	oTex = Renderer::renderDevice()->createTexture2D(texDesc);
}

void 
FluidSim_ParticleDisplay::create2D(const ColorGradient& colorGrad)
{
	RenderMesh& rdMesh = _rdMesh;

	{
		using VtxT = Vertex_PosUv<1>;
		using IdxT = u16;
		EditMesh mesh;

		auto& pos = mesh.pos;
		pos.resize(4);
		pos[0] = Vec3f{Vec2f{ -1.0f, +1.0f }, 0.0f};
		pos[1] = Vec3f{Vec2f{ +1.0f, +1.0f }, 0.0f};
		pos[2] = Vec3f{Vec2f{ -1.0f, -1.0f }, 0.0f};
		pos[3] = Vec3f{Vec2f{ +1.0f, -1.0f }, 0.0f};

		auto& uv = mesh.uvs[0];
		uv.resize(4);
		uv[0] = Vec2f{0.0, 0.0};
		uv[1] = Vec2f{1.0, 0.0};
		uv[2] = Vec2f{0.0, 1.0};
		uv[3] = Vec2f{1.0, 1.0};

		auto& idxs = mesh.indices;
		idxs.reserve(6);
		idxs.emplace_back(0); idxs.emplace_back(2); idxs.emplace_back(1);
		idxs.emplace_back(3); idxs.emplace_back(1); idxs.emplace_back(2);

		rdMesh.create(mesh);
	}

	auto cDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
	cDesc.bufSize	= 16;
	cDesc.stride	= sizeof(Vec2f);
	cDesc.typeFlags = RenderGpuBufferTypeFlags::Vertex | RenderGpuBufferTypeFlags::Compute;
	_posBufGpu = Renderer::renderDevice()->createRenderGpuMultiBuffer(cDesc);	_posBufGpu->setDebugName("_posBufGpu");

	cDesc.typeFlags = RenderGpuBufferTypeFlags::Index | RenderGpuBufferTypeFlags::Compute;
	_velBufGpu = Renderer::renderDevice()->createRenderGpuMultiBuffer(cDesc);	_velBufGpu->setDebugName("_velBufGpu");

	RenderUtil::createMaterial(&_shaderPtcDisplay, &_mtlPtcDisplay,			"asset/shader/demo/fluid_simulation/2d/rdsFluidSim2D_ParticleDisplay.shader");
	RenderUtil::createMaterial(&_shaderPtcDisplay, &_debug.mtlPtcDisplay,	"asset/shader/demo/fluid_simulation/2d/rdsFluidSim2D_ParticleDisplay.shader");
		
	invalidateColorGradient(colorGrad);
}

void 
FluidSim_ParticleDisplay::create3D(RenderMesh& rdMesh, const ColorGradient& colorGrad)
{
	_rdMesh = rdMesh;

	RenderUtil::createMaterial(&_shaderPtcDisplay, &_mtlPtcDisplay,			"asset/shader/demo/fluid_simulation/3d/rdsFluidSim3D_ParticleDisplay.shader");
	RenderUtil::createMaterial(&_shaderPtcDisplay, &_debug.mtlPtcDisplay,	"asset/shader/demo/fluid_simulation/3d/rdsFluidSim3D_ParticleDisplay.shader");

	invalidateColorGradient(colorGrad);
}

void 
FluidSim_ParticleDisplay::invalidateColorGradient(const ColorGradient& colorGrad)
{
	_colorGradient = colorGrad;
	s_createColorGradientTexture(_texColorGradient, _colorGradient);
}

void 
FluidSim_ParticleDisplay::draw(RenderRequest& rdReq, DrawData* drawData, const Span<Vec2f>& positions, const Span<Vec2f>& velocities, float radius)
{
	RDS_CORE_ASSERT(_posBufGpu && _velBufGpu);

	_posBufGpu->uploadToGpu(makeByteSpan(positions));
	_velBufGpu->uploadToGpu(makeByteSpan(velocities));

	draw(rdReq, drawData, _posBufGpu->renderGpuBuffer(), _velBufGpu->renderGpuBuffer(), Vec3f::s_zero(), radius, sCast<u32>(positions.size()));
}

void 
FluidSim_ParticleDisplay::draw(RenderRequest& rdReq, DrawData* drawData, RenderGpuBuffer* bufPos, RenderGpuBuffer* bufVel, Vec3f pos, float radius, u32 particleCount)
{
	auto tex = 1 ? _texColorGradient : Renderer::renderDevice()->textureStock().black;
	_draw(_mtlPtcDisplay, tex, 0.9f, rdReq, drawData, bufPos, bufVel, pos, radius, particleCount);
}

void 
FluidSim_ParticleDisplay::debug_draw(RenderRequest& rdReq, DrawData* drawData, RenderGpuBuffer* bufPos, RenderGpuBuffer* bufVel, Vec3f pos, float radius, u32 particleCount)
{
	_draw(_debug.mtlPtcDisplay, Renderer::renderDevice()->textureStock().white, 1.0f, rdReq, drawData, bufPos, bufVel, pos, radius + 0.001f, particleCount);
}

void 
FluidSim_ParticleDisplay::_draw(Material* mtl, Texture2D* colorMap, float depth, RenderRequest& rdReq, DrawData* drawData, RenderGpuBuffer* bufPos, RenderGpuBuffer* bufVel, Vec3f pos, float radius, u32 particleCount)
{
	mtl->setParam("u_colorMap",		colorMap);
	mtl->setParam("u_colorMap",		SamplerState::makeLinearClampToEdge());

	mtl->setParam("u_depth",		depth);

	mtl->setParam("u_positions",	bufPos);
	mtl->setParam("u_velocities",	bufVel);
	mtl->setParam("u_scale",		radius);
	mtl->setParam("u_velocityMax",	6.5f);
	mtl->setParam("u_objToWorld",	Mat4f::s_translate(pos));

	drawData->setupMaterial(mtl);
	rdReq.drawMesh_Instanced(RDS_SRCLOC, _rdMesh, mtl, particleCount);
}

Texture2D* 
FluidSim_ParticleDisplay::colorGradientTexture()
{
	return _texColorGradient;
}

#endif

}