#include "rds_fluid_simulation-pch.h"
#include "rdsFluidSim2D_Gpu.h"
#include "rdsFluidSim2D_Base.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim2D_Gpu-Impl ---
#endif // 0
#if 1


void 
FluidSim2D_Base::onCreate(GraphicsDemo* parentDemo) 
{ 
	Base::onCreate(parentDemo); 
}

void 
FluidSim2D_Base::onUpdate(float dt)
{
	Base::onUpdate(dt);
}

void 
FluidSim2D_Base::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);

}

void FluidSim2D_Base::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);
}

void 
FluidSim2D_Base::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	Base::onDrawGui(uiDrawReq);

	_simConfig.drawGui(uiDrawReq);
}

void
rds::FluidSim2D_Base::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);

	_simState.isPullInteraction = isFocusOnEditorViewport() && ev.isDown(UiMouseEventButton::Left);
	_simState.isPushInteraction = isFocusOnEditorViewport() && ev.isDown(UiMouseEventButton::Right);
}

void FluidSim2D_Base::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);

	if (ev.isUp(UiKeyboardEventButton::Space))
	{
		_simState.isStop = !_simState.isStop;
	}

	if (_simState.isStop)
	{
		if (ev.isUp(UiKeyboardEventButton::E))
		{
			_simState.isStepForward = !_simState.isStepForward;
		}

		if (ev.isUp(UiKeyboardEventButton::Q))
		{
			_simState.isStepBackward = !_simState.isStepBackward;
		}
	}
}

void FluidSim2D_Base::debug_drawBoundary(RenderRequest& rdReq)
{
	#if 0
	if (_parentDemo)
	{
		static Ray3f ray;
		if (_parentDemo->uiMouseState.isDown(UiMouseEventButton::Left))
		{
			ray = _mouseRayWorld;
		}
		rdReq.drawLine(ray.origin, ray.origin + ray.dir * 9999.0f, Color4f(1.0f, 0.0f, 0.0f, 1.0f));
		rdReq.drawCircle(ray.origin.toVec2(), _simConfig.smoothingRadius, Color4f(0.2f, 0.0f, 0.0f, 0.005f));
	}
	#endif // 0

	rdReq.drawAABBox(Vec3f{ _simConfig.boundingRegion.pos, 0.0f }, Vec3f{_simConfig.boundingRegion.size, 0.001f} / 2.0f);
}

void FluidSim2D_Base::debug_drawSpatialGrid(RenderRequest& rdReq)
{
	Color4f color = Color4f(0.2f, 0.6f, 0.2f, 1.0f);

	auto gridCellCount = Vec2f{_simConfig.boundingRegion.size} / _simConfig.smoothingRadius;
	gridCellCount.x = math::ceil(gridCellCount.x);
	gridCellCount.y = math::ceil(gridCellCount.y);

	const auto& region = _simConfig.boundingRegion;
	auto cellSize = Vec2f{_simConfig.boundingRegion.size} / gridCellCount;

	auto minExtent = Vec2f{ region.pos } - Vec2f{ region.size } / 2.0f -cellSize / 2.0f;
	for (int y = -2; y < gridCellCount.y + 2; y++)
	{
		float posY = minExtent.y + cellSize.y * y;

		auto yLinePt0 = Vec3f{ minExtent.x, posY, 0.0f };
		auto yLinePt1 = yLinePt0 + Vec3f::s_right() * region.w * 2.0f;
		rdReq.drawLine(yLinePt0, yLinePt1, color);

		for (int x = -2; x < gridCellCount.x + 2; x++)
		{
			float posX = minExtent.x + cellSize.x * x;

			auto xLinePt0 = Vec3f{ posX, minExtent.y, 0.0f };
			auto xLinePt1 = xLinePt0 + Vec3f::s_up() * region.h * 2.0f;
			rdReq.drawLine(xLinePt0, xLinePt1, color);
		}
	}
}


void 
FluidSim2D_Base::debug_drawMouseInteraction(RenderRequest& rdReq)
{
	if (_simState.isPullInteraction)
	{
		rdReq.drawCircle(_mouseRayWorld.origin.toVec2(), _simConfig.interactionRadius, Color4f(0.2f, 0.8f, 0.2f, 0.005f));
	}
	if (_simState.isPushInteraction)
	{
		rdReq.drawCircle(_mouseRayWorld.origin.toVec2(), _simConfig.interactionRadius, Color4f(0.2f, 0.2f, 0.8f, 0.005f));
	}
}

#endif

}