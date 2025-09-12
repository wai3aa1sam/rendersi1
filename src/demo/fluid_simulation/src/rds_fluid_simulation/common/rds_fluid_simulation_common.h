#pragma once

#include <rds_demo.h>

#include "rds_fluid_simulation-config.h"
#include "rds_fluid_simulation_traits.h"

#include "rdsColorGradient.h"

namespace rds
{


#if 0
#pragma mark --- rdsMaterialPool-Decl ---
#endif // 0
#if 1

class MaterialPool
{
public:
	MaterialPool();
	~MaterialPool();

public:
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
