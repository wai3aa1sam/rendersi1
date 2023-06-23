#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"


namespace rds
{

#if 0
#pragma mark --- rdsRenderContext-Decl ---
#endif // 0
#if 1

struct RenderContext_CreateDesc
{
	NativeUIWindow* window = nullptr;
};

class RenderContext : public RefCount_Base
{
public:
	using Base = RefCount_Base;
	using CreateDesc = RenderContext_CreateDesc;

	using Util = RenderApiUtil;

public:
	static CreateDesc makeCDesc();
	static SPtr<RenderContext> make(const CreateDesc& cDesc);

public:
	RenderContext();
	virtual ~RenderContext();

	void create(const CreateDesc& cDesc);
	void destroy();

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

protected:
	//RenderContext(const CreateDesc&)

private:

};



#endif
}