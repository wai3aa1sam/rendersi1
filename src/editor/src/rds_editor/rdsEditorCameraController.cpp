#include "rds_editor-pch.h"
#include "rdsEditorCameraController.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditorCameraController-Impl ---
#endif // 0
#if 1

void 
EditorCameraController::update(Camera* camera, float dt, const UiMouseEvent& mouseEv, const UiInput& uiInput)
{
	auto& cam	= *camera;
	{
		const auto& ev = mouseEv;
		if (ev.isDragging()) 
		{
			using Button = UiMouseEventButton;
			switch (ev.pressedButtons) 
			{
				case Button::Left: 
				{
					auto d = ev.deltaPos * (0.01f * dt);
					cam.orbit(d.x, d.y);
				} break;

				case Button::Middle: 
				{
					auto d = ev.deltaPos * (0.1f * dt);
					cam.move(d.x, d.y, 0);
				} break;

				case Button::Right: 
				{
					auto d = ev.deltaPos * (-0.1f * dt);
					cam.dolly(d.x + d.y);
				} break;
			}
		}
	}

	{
		using Button = UiKeyboardEventButton;

		auto dir = Vec3f::s_zero();

		if		(uiInput.isKeyPressed(Button::W))	dir += Vec3f::s_forward();
		else if (uiInput.isKeyPressed(Button::S))	dir -= Vec3f::s_forward();

		if		(uiInput.isKeyPressed(Button::A))	dir += Vec3f::s_right();
		else if (uiInput.isKeyPressed(Button::D))	dir -= Vec3f::s_right();

		dir = dir * (_speed * dt);
		if (!math::equals0(dir))
			cam.move(dir.x, dir.y, dir.z);
	}
}

void 
EditorCameraController::setSpeed(float speed)
{
	_speed = speed;
}


#endif

}