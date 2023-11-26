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

	void create (RenderContext* renderContext);
	void destroy();

	void onBeginRender	(RenderContext* renderContext);
	void onEndRender	(RenderContext* renderContext);

	void onDrawUI		(RenderRequest& req);
	bool onUIMouseEvent	(UIMouseEvent& ev);

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

	SPtr<RenderGpuBuffer>	_vertexBuffer;
	SPtr<RenderGpuBuffer>	_indexBuffer;

	Vector<u8>	_vertexData;
	Vector<u8>	_indexData;

	ImGuiContext*	_ctx = nullptr;
};

inline Vec2f Vec2f_make(const ImVec2& v) { return Vec2f(v.x, v.y); }
inline Vec4f Vec4f_make(const ImVec4& v) { return Vec4f(v.x, v.y, v.z, v.w); }

#endif


}