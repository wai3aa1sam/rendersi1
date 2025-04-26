#pragma once

#include "rds_editor/common/rds_editor_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditorCameraController-Decl ---
#endif // 0
#if 1

class EditorCameraController : public NonCopyable
{
public:
	using Camera = math::Camera3f;

public:
	EditorCameraController();
	~EditorCameraController();

	void update(Camera* camera, float dt, const UiMouseEvent& mouseEv, const UiInput& uiInput);

	void setSpeed(		float speed);
	void setSpeedStep(	float speedStep);

public:
	float speed() const;

private:
	float _speed		= 0.9f;
	float _speedStep	= 0.2f;
};

inline float EditorCameraController::speed() const { return _speed; }

#endif

}