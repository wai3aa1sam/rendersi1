#include "rds_engine-pch.h"
#include "rdsFrameControl.h"


namespace rds
{

#if 0
#pragma mark --- rdsFrameControl-Impl ---
#endif // 0
#if 1

FrameControl::FrameControl()
{
	
}

void 
FrameControl::beginFrame()
{
	double startTime	= _timer.get();
	double frameTime	= startTime - _elapsedTime;

	if (frameTime > _frameTargetTime)
		frameTime = _frameTargetTime;

	_elapsedTime	= startTime;

	_simTimeAccumlator		+= frameTime;
	_frameTimeAccumlator	+= frameTime;
}

void 
FrameControl::endFrame(bool isWait)
{
	double startTime  = _timer.get();
	double frameTime  = startTime - _elapsedTime;
	double timeToWait = _frameTargetTime - frameTime;
	while (_timer.get() < startTime + timeToWait && isWait)
	{
	}

	_frameTime = _timer.get() - _elapsedTime;
}

//bool 
//FrameControl::isFrameReady() 
//{ 
//	bool isReady = _frameTimeAccumlator >= _frameTargetTime;
//	if (isReady)
//	{
//		_frameElapsedTime	 += _frameTargetTime;
//		_frameTimeAccumlator -= _frameTargetTime;
//	}
//	return isReady; 
//}

void 
FrameControl::simulate(Function<void(double appTime, double dt)> simFn)
{
	while (_simTimeAccumlator >= _simTargetTime)
	{
		double dt = _simTargetTime; RDS_UNUSED(dt);
		simFn(_simElapsedTime, dt);

		_simElapsedTime		+= _simTargetTime;
		_simTimeAccumlator	-= _simTargetTime;
	}
}

void 
FrameControl::setTargetFrameRate(u64 rate)	
{ 
	_frameTargetTime	= 1.0 / sCast<double>(rate); 
}

void 
FrameControl::setTargetSimulationRate(u64 rate)	
{ 
	_simTargetTime		= 1.0 / sCast<double>(rate); 
}

double	FrameControl::elapsedTime()				const { return _elapsedTime; }
double	FrameControl::frameElapsedTime()		const { return math::max(0.0, _frameElapsedTime - _frameTargetTime); }
double	FrameControl::simulationElapsedTime()	const { return _simElapsedTime; }

u32		FrameControl::targetFrameRate()			const { return sCast<u32>(1.0 / _frameTargetTime); }
u32		FrameControl::targetSimulationRate()	const { return sCast<u32>(1.0 / _simTargetTime); }

double	FrameControl::fps()						const { return 1.0 / (_frameTime); }

#endif

}