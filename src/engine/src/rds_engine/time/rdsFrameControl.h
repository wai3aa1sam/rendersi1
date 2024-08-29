#pragma once

/*
* references:
* ~ https://gafferongames.com/post/fix_your_timestep/
*/

#include "rds_engine/common/rds_engine_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsFrameControl-Decl ---
#endif // 0
#if 1

class FrameControl : public NonCopyable
{
public:
	bool isWaitFrame = true;

public:
	FrameControl();

	void beginFrame();
	void endFrame();

	//bool isFrameReady();
	void simulate(Function<void(double simElapsedTime, double dt)> simFn);

	void setTargetFrameRate(		u64 rate);
	void setTargetSimulationRate(	u64 rate);

public:
	double	elapsedTime()				const;
	double	frameElapsedTime()			const;
	double	simulationElapsedTime()		const;

	u32		targetFrameRate()		const;
	u32		targetSimulationRate()	const;

	double fps() const;

protected:
	HiResTimer	_timer;

	double		_elapsedTime			= 0.0;
	double		_frameTime				= 0.0;

	double		_simElapsedTime			= 0.0;
	double		_simTimeAccumlator		= 0.0;
	double		_simTargetTime			= 1.0 / 60.0;

	double		_frameElapsedTime		= 0.0;
	double		_frameTimeAccumlator	= 0.0;
	double		_frameTargetTime		= 1.0 / 60.0;
};


#endif

}