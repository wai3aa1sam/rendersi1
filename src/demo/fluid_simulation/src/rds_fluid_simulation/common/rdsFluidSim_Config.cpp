#include "rds_fluid_simulation-pch.h"
#include "rdsFluidSim_Config.h"
#include "../2d/rdsFluidSim2D_Cpu.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim_Config-Impl ---
#endif // 0
#if 1

FluidSim_Config::FluidSim_Config()
{

}

void 
FluidSim_Config::create(FluidSimDemo_Base* sim)
{
	_fluSim = sim;
}

ColorGradient 
FluidSim_Config::makeColorGradient()
{
	ColorGradient colGrad;
	colGrad.addColorKey(ColorGradientKey{colorGradientKey0, 0.05f});
	colGrad.addColorKey(ColorGradientKey{colorGradientKey1, 0.5f});
	colGrad.addColorKey(ColorGradientKey{colorGradientKey2, 0.6f});
	colGrad.addColorKey(ColorGradientKey{colorGradientKey3, 1.0f});
	return colGrad;
}

void
FluidSim_Config::drawGui(EditorUiDrawRequest& uiDrawReq)
{
	if (!_fluSim)
		return;

	auto wnd = uiDrawReq.makeWindow("config");
	uiDrawReq.makeCheckbox("useSpatialOptimization",	&useSpatialOptimization);
	uiDrawReq.makeCheckbox("useDebugSpatial",			&useDebugSpatial);
	uiDrawReq.makeCheckbox("useDebugLog",				&useDebugLog);

	
	uiDrawReq.dragInt("simulationCountPerFrame",		&simulationCountPerFrame,		1, 1, 6);
	uiDrawReq.dragFloat("timeMultiplier",				&timeMultiplier,				0.01f);

	uiDrawReq.dragFloat("gravity",						&gravity,						0.01f);
	uiDrawReq.drawVec3f("gravityDir",					&gravityDir);

	uiDrawReq.dragFloat("particleSize",					&particleSize,					0.01f);
	uiDrawReq.dragFloat("particleMass",					&particleMass,					0.01f);
	uiDrawReq.dragFloat("smoothingRadius",				&smoothingRadius,				0.1f, 0.1f);
	uiDrawReq.dragFloat("collisionDamping",				&collisionDamping,				0.1f);
	uiDrawReq.dragFloat("targetDensity",				&targetDensity,					0.1f);
	uiDrawReq.dragFloat("pressureMultiplier",			&pressureMultiplier,			1.0f);
	uiDrawReq.dragFloat("viscosityStrength",			&viscosityStrength,				0.01f);
	uiDrawReq.dragFloat("maxValue",						&maxValue,						0.1f);

	uiDrawReq.dragFloat("interactionRadius",			&interactionRadius,		0.01f, 0.01f);
	uiDrawReq.dragFloat("interactionStrength",			&interactionStrength,	1.0f);
	
	uiDrawReq.drawVec3f("forceFieldDir",				&forceFieldDir);
	uiDrawReq.dragFloat("forceFieldStrength",			&forceFieldStrength,	1.0f);

	auto makeColorPicker4 = [](const char* label, Color4f* oColor)
		{
			float v[4];
			v[0] = oColor->r;
			v[1] = oColor->g;
			v[2] = oColor->b;
			v[3] = oColor->a;

			ImGui::ColorEdit4(label, v);

			oColor->r = v[0];
			oColor->g = v[1];
			oColor->b = v[2];
			oColor->a = v[3];
		};
	makeColorPicker4("particleColor",		&particleColor);
	makeColorPicker4("colorGradientKey0",	&colorGradientKey0);
	makeColorPicker4("colorGradientKey1",	&colorGradientKey1);
	makeColorPicker4("colorGradientKey2",	&colorGradientKey2);
	makeColorPicker4("colorGradientKey3",	&colorGradientKey3);

	uiDrawReq.makeCheckbox("isInvalidateColorMap", &isInvalidateColorMap);
	uiDrawReq.showText("colorGradientTexture:");
	//uiDrawReq.showImage(_fluSim->particleDisplay().colorGradientTexture());
	RDS_TODO("sample this texture should use clamp sampler");

	uiDrawReq.showText("debugDensity: {}",				debugDensity);
	uiDrawReq.showText("debugMousePosViewport: {}",		debugMousePosViewport);
	uiDrawReq.showText("debugMousePosWorld: {}",		debugMousePosWorld);
	uiDrawReq.showText("debugMouseDirWorld: {}",		debugMouseDirWorld);

	uiDrawReq.showText("debugParticleCount: {}",		debugParticleCount);
	uiDrawReq.showText("debugWithinRadiusCount: {}",	debugWithinRadiusCount);
	uiDrawReq.showText("debugMouseCellCoord: {}",		FluidSim2D_Cpu::positionToCellCoord(debugMousePosWorld.toVec2(), smoothingRadius));

	if (_fluSim && _fluSim->getDebugSpatialTransform())
	{
		_fluSim->getDebugSpatialTransform()->setLocalScale(Vec3f::s_one() * smoothingRadius);
	}
}

float 
FluidSim_Config::calcPressureByDensity(float dens)
{
	float densityDiff = dens - targetDensity;
	float pressure = densityDiff * pressureMultiplier;
	return pressure;
}

float 
FluidSim_Config::calcNearPressureByDensity(float nearDens)
{
	float nearPressure	= nearDens * nearPressureMultiplier;
	return nearPressure;
}

Vec2f 
FluidSim_Config::calcPressureByDensityData(const Vec2f& densData)
{
	float pressure		= calcPressureByDensity(densData.x);
	float nearPressure	= calcNearPressureByDensity(densData.y);
	return Vec2f{pressure, nearPressure};
}

#endif

}