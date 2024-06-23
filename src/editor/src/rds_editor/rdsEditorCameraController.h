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

	void setSpeed(float speed);

public:
	float speed() const;

private:
	float _speed = 2.5f;
};

inline float EditorCameraController::speed() const { return _speed; }

#endif

}