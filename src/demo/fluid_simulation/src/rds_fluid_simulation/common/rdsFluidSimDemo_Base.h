#pragma once

#include "rds_fluid_simulation-pch.h"
#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim2D_SimState-Decl ---
#endif // 0
#if 1

struct FluidSim_SimState
{
	bool isStop			= true;		// : 1
	bool isStepForward	= false;
	bool isStepBackward	= false;
	bool hasSimulated	= false;

	bool	isPullInteraction = false;
	bool	isPushInteraction = false;

	float interactionInputStrength = 0.0f;
};

#endif

#if 0
#pragma mark --- rdsFluidSim2D_Gpu-Decl ---
#endif // 0
#if 1

class FluidSimDemo_Base : public NonCopyable
{
public:
	using SimState = FluidSim_SimState;

public:
	virtual void onCreate(GraphicsDemo* parentDemo);

	virtual void onUpdate(float dt, RenderPassPipeline* renderPassPipeline);

	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline);
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline);

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq);

	virtual void onUiMouseEvent(	UiMouseEvent&		ev);
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev);

public:
	virtual void simulate(float dt, RenderPassPipeline* renderPassPipeline);

protected:
			DemoEditorLayer* demoLayer()		{ return _parentDemo ? _parentDemo->demoLayer() : nullptr; }
	const	DemoEditorLayer* demoLayer() const	{ return _parentDemo ? _parentDemo->demoLayer() : nullptr; }

protected:
	bool isFocusOnEditorViewport()	const	{ return demoLayer() ? demoLayer()->isFocusOnEditorViewport() : false; }

protected:
	GraphicsDemo*	_parentDemo = nullptr;
	Vec2f			_mousePosViewport;
	Ray3f			_mouseRayWorld;

protected:
	SimState _simState;
};

#endif

#if 0
#pragma mark --- rdsMaterialPool-Decl ---
#endif // 0
#if 1
class MaterialPool
{
public:
	MaterialPool()
	{
		_data.resize(RenderApiLayerTraits::s_kFrameInFlightCount);
		RDS_TODO("this class also in RenderUiContext, please separate it as a file");
		RDS_TODO("this will trigger un - freed block in vma in mt mode, please check later");
	}

	void			reset();
	SPtr<Material>	newObject(Shader* shader);		// general objectPool should return *
	//void			deleteObject(SPtr<Material> obj);

private:
	struct Data
	{
	public:
		void			reset();
		SPtr<Material>	newObject(Shader* shader);		// general objectPool should return *
		//void			deleteObject(SPtr<Material> obj);

	public:
		Vector<SPtr<Material>, 16> _freedObjs;
		Vector<SPtr<Material>, 16> _objs;
	};
	FramedT<Data> _data;

protected:
	Data& data() { return _data[Renderer::renderDevice()->engineFrameIndex()]; }
};
#endif

}