#include "rds_editor-pch.h"
#include "rdsEditorCameraController.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditorCameraController-Impl ---
#endif // 0
#if 1

EditorCameraController::EditorCameraController()
{

}

EditorCameraController::~EditorCameraController()
{

}

void 
EditorCameraController::update(Camera* camera, float dt, const UiMouseEvent& mouseEv, const UiInput& uiInput)
{
	auto& cam	= *camera;
	{
		using Button = UiMouseEventButton;

		const auto& ev = mouseEv;
		if (ev.isDragging()) 
		{
			switch (ev.pressedButtons) 
			{
				case Button::Left: 
				{
					auto d = ev.deltaPos * (-0.1f * dt);
					cam.dolly(d.x + d.y);
				} break;

				case Button::Middle: 
				{
					auto d = ev.deltaPos * (0.1f * dt);
					cam.move(d.x, d.y, 0);
				} break;

				case Button::Right: 
				{
					auto d = ev.deltaPos * (-0.01f * dt);
					cam.orbit(d.x, d.y);
				} break;

				// ev.scroll
			}
		}

		if (ev.pressedButtons == Button::Right && ev.isScroll())
		{
			_speed += _speedStep * dt * (ev.scroll.y / math::abs(ev.scroll.y));
			_speed = math::max(_speed, 0.0f);
		}
	}

	{
		using Button = UiKeyboardEventButton;

		auto dir = Vec3f::s_zero();

		if		(uiInput.isKeyPressed(Button::W))	dir += Vec3f::s_forward();
		else if (uiInput.isKeyPressed(Button::S))	dir -= Vec3f::s_forward();

		if		(uiInput.isKeyPressed(Button::A))	dir += Vec3f::s_right();
		else if (uiInput.isKeyPressed(Button::D))	dir -= Vec3f::s_right();

		auto speed = _speed;
		if (uiInput.isKeyPressed(Button::Shift))
			speed *= 0.0f;

		dir = dir * (speed * dt);
		if (!math::equals0(dir))
			cam.move(dir.x, dir.y, dir.z);
	}
}

void 
EditorCameraController::setSpeed(float speed)
{
	_speed = speed;
}

void 
EditorCameraController::setSpeedStep(float speedStep)
{
	_speedStep = speedStep;
}


#endif

}