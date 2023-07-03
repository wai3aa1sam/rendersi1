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

	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static CreateDesc makeCDesc();
	static SPtr<RenderContext> make(const CreateDesc& cDesc);

public:
	RenderContext();
	virtual ~RenderContext();

	void create(const CreateDesc& cDesc);
	void destroy();

	void beginRender();
	void endRender();

protected:
	virtual void onCreate(const CreateDesc& cDesc);
	virtual void onPostCreate(const CreateDesc& cDesc);
	virtual void onDestroy();

	virtual void onBeginRender()	{};
	virtual void onEndRender()		{};

protected:
	//RenderContext(const CreateDesc&)

private:

};



#endif
}