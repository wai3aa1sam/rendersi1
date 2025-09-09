#pragma once

#include "rds_fluid_simulation-pch.h"
#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim2D_ParticleDisplay-Decl ---
#endif // 0
#if 1

class FluidSim2D_ParticleDisplay
{
public:
	void s_createColorGradientTexture(SPtr<Texture2D>& oTex, const ColorGradient& colorGradient);

public:
	void create2D(const ColorGradient& colorGrad);
	void invalidateColorGradient(const ColorGradient& colorGrad);

	void draw(RenderRequest& rdReq, DrawData* drawData, const Span<Vec2f>& positions, const Span<Vec2f>& velocities, float radius)
	{
		RDS_CORE_ASSERT(_posBufGpu && _velBufGpu);

		_posBufGpu->uploadToGpu(makeByteSpan(positions));
		_velBufGpu->uploadToGpu(makeByteSpan(velocities));

		_mtlPtcDisplay->setParam("u_colorMap",		_texColorGradient);
		_mtlPtcDisplay->setParam("u_colorMap",		SamplerState::makeLinearClampToEdge());

		_mtlPtcDisplay->setParam("u_positions",		_posBufGpu->renderGpuBuffer());
		_mtlPtcDisplay->setParam("u_velocities",	_velBufGpu->renderGpuBuffer());
		_mtlPtcDisplay->setParam("u_scale",			radius);
		_mtlPtcDisplay->setParam("u_velocityMax",	6.5f);
		_mtlPtcDisplay->setParam("u_objToWorld",	Mat4f::s_identity());
		_mtlPtcDisplay->setParam("u_worldToObj",	Mat4f::s_identity());

		drawData->setupMaterial(_mtlPtcDisplay);
		rdReq.drawMesh_Instanced(RDS_SRCLOC, _rdMesh, _mtlPtcDisplay, positions.size());
	}

public:
	Texture2D*	colorGradientTexture();

private:
	SPtr<Shader>	_shaderPtcDisplay;
	SPtr<Material>	_mtlPtcDisplay;
	SPtr<Texture2D>	_texColorGradient;
	ColorGradient	_colorGradient;

	RenderMesh					_rdMesh;

	// actually do not need these, we can create on RenderGraph
	SPtr<RenderGpuMultiBuffer>	_posBufGpu;
	SPtr<RenderGpuMultiBuffer>	_velBufGpu;
};


#endif

}