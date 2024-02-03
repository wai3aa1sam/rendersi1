#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "vertex/rdsVertex.h"
#include "buffer/rdsRenderGpuMultiBuffer.h"
#include "shader/rdsShader.h"
#include "shader/rdsMaterial.h"

#include <imgui.h>

/*
references:
~ src/render/ImGui_SGE.h in [https://github.com/SimpleTalkCpp/SimpleGameEngine]
*/

#define RDS_RENDER_ENABLE_UI 1

namespace rds
{

class RenderContext;
class RenderDevice;

#if 0
#pragma mark --- rdsRenderUiContext-Decl ---
#endif // 0
#if 1

class RenderUiContext : public NonCopyable
{
public:
	using Vertex = Vertex_ImGui;

public:
	RenderUiContext();
	~RenderUiContext();

	#if RDS_RENDER_ENABLE_UI
	void create (RenderContext* renderContext);
	void destroy();

	void onBeginRender	(RenderContext* renderContext);
	void onEndRender	(RenderContext* renderContext);

	void onDrawUI		(RenderRequest& req);
	bool onUIMouseEvent	(UIMouseEvent& ev);
	#else
	void create (RenderContext* renderContext) {};
	void destroy() {};

	void onBeginRender	(RenderContext* renderContext) {};
	void onEndRender	(RenderContext* renderContext) {};

	void onDrawUI		(RenderRequest& req) {};
	bool onUIMouseEvent	(UIMouseEvent& ev) {return true;};
	#endif // 0


protected:
	void _createFontTexture();
	int _mouseButton(UIMouseEventButton v);

	void _setDarkTheme();

protected:
	RenderDevice* renderDevice();

protected:
	RenderContext* _rdCtx = nullptr; // temp, must be raw ptr, since the RenderContext is holding it

	SPtr<Shader>	_shader;
	SPtr<Material>	_material;
	SPtr<Texture2D>	_fontTex;

	const VertexLayout*		_vertexLayout = nullptr;

	SPtr<RenderGpuMultiBuffer>	_vtxBuf;
	SPtr<RenderGpuMultiBuffer>	_idxBuf;

	Vector<u8>	_vertexData;
	Vector<u8>	_indexData;

	ImGuiContext*	_ctx = nullptr;
};

inline Vec2f makeVec2f(const ImVec2& v) { return Vec2f(v.x, v.y); }
inline Vec4f makeVec4f(const ImVec4& v) { return Vec4f(v.x, v.y, v.z, v.w); }

inline Rect2f makeRect2f(const ImVec2& pos, const ImVec2& size) { Rect2f o; o.set(makeVec2f(pos), makeVec2f(size)); return o; }

#endif


}