#include "rds_fluid_simulation-pch.h"
#include "rdsFluidSim3D_Gpu.h"
#include "rdsFluidSim3D_Base.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim3D_Gpu-Impl ---
#endif // 0
#if 1

void 
FluidSim3D_Base::onCreate(GraphicsDemo* parentDemo) 
{ 
	Base::onCreate(parentDemo);

	//_simConfig.spawnRegion3D.min = AABBox3T_center(_simConfig.boundingRegion3D) - AABBox3T_halfSize(_simConfig.boundingRegion3D);
	//_simConfig.spawnRegion3D.max = AABBox3T_center(_simConfig.boundingRegion3D) + AABBox3T_halfSize(_simConfig.boundingRegion3D);

	auto& camera = parentDemo->app().mainWindow().camera();
	//camera.setOrthographic(4.0f);
	camera.setPos(Vec3f{0.385f, 11.446f, 22.212f});
	camera.setAim(Vec3f{0.385f, 5.10f, 0.0f});

	camera.setPos(Vec3f{0.0f, 0.0f, 8.0f});
	camera.setAim(Vec3f{0.0f, 0.0f, 0.0f});

	_particleSpawner.spawnDensity		= 32.0f;
	
	_simConfig.smoothingRadius			= 0.2f;
	_simConfig.targetDensity			= 630.0f;
	_simConfig.pressureMultiplier		= 288.0f;
	_simConfig.nearPressureMultiplier	= 2.25f;
	_simConfig.viscosityStrength		= 0.001f;

	_simConfig.create(this);
	_ptcDisplay.create3D(_parentDemo->meshAssets().sphere->renderMesh, _simConfig.makeColorGradient());
	_particleSpawner.create3D(_simConfig.spawnRegion3D);
}

void 
FluidSim3D_Base::onUpdate(float dt, RenderPassPipeline* renderPassPipeline)
{
	_simState.interactionInputStrength = 0.0f;
	if (_simState.isPullInteraction) _simState.interactionInputStrength += _simConfig.interactionStrength;
	if (_simState.isPushInteraction) _simState.interactionInputStrength -= _simConfig.interactionStrength;

	{
		_simConfig.debugParticleCount		= _particleSpawner.particleCount;
		_simConfig.debugMousePosViewport	= _mousePosViewport;
		_simConfig.debugMousePosWorld		= _mouseRayWorld.origin;
		_simConfig.debugMouseDirWorld		= _mouseRayWorld.dir;
	}

	Base::onUpdate(dt, renderPassPipeline);
}

void 
FluidSim3D_Base::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);

}

void FluidSim3D_Base::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);
}

void 
FluidSim3D_Base::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	Base::onDrawGui(uiDrawReq);

	_simConfig.drawGui(uiDrawReq);
}

void
rds::FluidSim3D_Base::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);

	_simState.isPullInteraction = isFocusOnEditorViewport() && ev.isDown(UiMouseEventButton::Left);
	_simState.isPushInteraction = isFocusOnEditorViewport() && ev.isDown(UiMouseEventButton::Right);
}

void FluidSim3D_Base::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);
}

void FluidSim3D_Base::debug_drawBoundary(RenderRequest& rdReq)
{
	// now use scene object as boundary
	#if 0
	AABBox3f newBox;
	newBox.min = _simConfig.boundingRegion3D.min - AABBox3T_halfSize(_simConfig.boundingRegion3D);
	newBox.max = _simConfig.boundingRegion3D.max - AABBox3T_halfSize(_simConfig.boundingRegion3D);
	//anchorTransf()->setLocalPosition(newBox.min);
	rdReq.drawAABBox(newBox, Color4f(0.0f, 1.0f, 0.0f, 1.0f));
	#endif // 0
}

void FluidSim3D_Base::debug_drawSpatialGrid(RenderRequest& rdReq)
{
	Color4f color = Color4f(0.2f, 0.6f, 0.2f, 1.0f);

	auto boundingRegionSize = AABBox3T_size(_simConfig.boundingRegion3D);
	auto gridCellCount = boundingRegionSize / _simConfig.smoothingRadius;
	gridCellCount.x = math::ceil(gridCellCount.x);
	gridCellCount.y = math::ceil(gridCellCount.y);
	gridCellCount.z = math::ceil(gridCellCount.z);

	auto cellSize = boundingRegionSize / gridCellCount;

	auto minExtent = AABBox3T_center(_simConfig.boundingRegion3D) - boundingRegionSize / 2.0f - cellSize / 2.0f;
	for (int z = -2; z < gridCellCount.z + 2; z++)
	{
		float posZ = minExtent.z + cellSize.z * z;
		auto zLinePt0 = Vec3f{ minExtent.x, 0.0f, posZ };
		auto zLinePt1 = zLinePt0 + Vec3f::s_forward() * boundingRegionSize.z * 2.0f;
		rdReq.drawLine(zLinePt0, zLinePt1, color);

		for (int y = -2; y < gridCellCount.y + 2; y++)
		{
			float posY = minExtent.y + cellSize.y * y;
			auto yLinePt0 = Vec3f{ minExtent.x, posY, posZ };
			auto yLinePt1 = yLinePt0 + Vec3f::s_right() * boundingRegionSize.y * 2.0f;
			rdReq.drawLine(yLinePt0, yLinePt1, color);

			for (int x = -2; x < gridCellCount.x + 2; x++)
			{
				float posX = minExtent.x + cellSize.x * x;
				auto xLinePt0 = Vec3f{ posX, minExtent.y, posZ };
				auto xLinePt1 = xLinePt0 + Vec3f::s_up() * boundingRegionSize.x * 2.0f;
				rdReq.drawLine(xLinePt0, xLinePt1, color);
			}
		}
	}
}


void 
FluidSim3D_Base::debug_drawMouseInteraction(RenderRequest& rdReq)
{
	if (_simState.isPullInteraction)
	{
		//rdReq.drawCircle(_mouseRayWorld.origin.toVec2(), _simConfig.interactionRadius, Color4f(0.2f, 0.8f, 0.2f, 0.005f));
	}
	if (_simState.isPushInteraction)
	{
		//rdReq.drawCircle(_mouseRayWorld.origin.toVec2(), _simConfig.interactionRadius, Color4f(0.2f, 0.2f, 0.8f, 0.005f));
	}
}

void 
FluidSim3D_Base::debug_drawSmoothRadius(RenderRequest& rdReq)
{
	if (_simConfig.useDebugSmoothRadius)
	{
		//rdReq.drawCircle(_mouseRayWorld.origin.toVec2(), _simConfig.smoothingRadius, Color4f(0.8f, 0.2f, 0.2f, 0.005f));
	}
}

#endif

}