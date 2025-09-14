#pragma once

#include "rds_fluid_simulation-pch.h"
#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim_ParticleDisplay-Decl ---
#endif // 0
#if 1

class FluidSim_ParticleDisplay : public NonCopyable
{
public:
	void s_createColorGradientTexture(SPtr<Texture2D>& oTex, const ColorGradient& colorGradient);

public:
	void create2D(const ColorGradient& colorGrad);
	void create3D(RenderMesh& rdMesh, const ColorGradient& colorGrad);

public:
	void draw(RenderRequest& rdReq, DrawData* drawData, const Span<Vec2f>& positions, const Span<Vec2f>& velocities, float radius);
	void draw(RenderRequest& rdReq, DrawData* drawData, RenderGpuBuffer* bufPos, RenderGpuBuffer* bufVel, Vec3f pos, float radius, u32 particleCount);

	void invalidateColorGradient(const ColorGradient& colorGrad);

public:
	void debug_draw(RenderRequest& rdReq, DrawData* drawData, RenderGpuBuffer* bufPos, RenderGpuBuffer* bufVel, Vec3f pos, float radius, u32 particleCount);

private:
	void _draw(Material* mtl, Texture2D* colorMap, float depth, RenderRequest& rdReq, DrawData* drawData, RenderGpuBuffer* bufPos, RenderGpuBuffer* bufVel, Vec3f pos, float radius, u32 particleCount);

public:
	Texture2D*	colorGradientTexture();

private:

private:
	SPtr<Shader>	_shaderPtcDisplay;
	SPtr<Material>	_mtlPtcDisplay;

	struct Debug
	{
		SPtr<Material>	mtlPtcDisplay;
	} _debug;

	SPtr<Texture2D>	_texColorGradient;
	ColorGradient	_colorGradient;

	RenderMesh		_rdMesh;

	// actually do not need these, we can create on RenderGraph
	SPtr<RenderGpuMultiBuffer>	_posBufGpu;
	SPtr<RenderGpuMultiBuffer>	_velBufGpu;
};


#endif

}