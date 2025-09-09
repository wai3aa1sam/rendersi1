#include "rds_fluid_simulation-pch.h"
#include "rdsFluidSim2D_ParticleDisplay.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim2D_ParticleDisplay-Impl ---
#endif // 0
#if 1

void 
FluidSim2D_ParticleDisplay::s_createColorGradientTexture(SPtr<Texture2D>& oTex, const ColorGradient& colorGradient)
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
FluidSim2D_ParticleDisplay::create2D(const ColorGradient& colorGrad)
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

	GraphicsDemo::createMaterial(&_shaderPtcDisplay, &_mtlPtcDisplay, "asset/shader/demo/fluid_simulation/2d/rdsFluidSim2D_ParticleDisplay.shader");

	invalidateColorGradient(colorGrad);
}

void 
FluidSim2D_ParticleDisplay::invalidateColorGradient(const ColorGradient& colorGrad)
{
	_colorGradient = colorGrad;
	s_createColorGradientTexture(_texColorGradient, _colorGradient);
}

Texture2D* 
FluidSim2D_ParticleDisplay::colorGradientTexture()
{
	return _texColorGradient;
}

#endif

}