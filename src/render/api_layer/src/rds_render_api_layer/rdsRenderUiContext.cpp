#include "rds_render_api_layer-pch.h"
#include "rdsRenderUiContext.h"
#include "rdsRenderContext.h"
#include "command/rdsRenderRequest.h"
#include "rdsRenderer.h"

namespace rds
{

RenderUiContext::RenderUiContext()
{
}

RenderUiContext::~RenderUiContext()
{
	destroy();
}

#if RDS_RENDER_ENABLE_UI
void
RenderUiContext::create(RenderContext* renderContext)
{
	if (!IMGUI_CHECKVERSION()) RDS_THROW("ImGui version error");
	_ctx = ImGui::CreateContext();
	if (!_ctx) RDS_THROW("ImGui error create context");

	_rdCtx			= renderContext;
	_vertexLayout	= Vertex::vertexLayout();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererUserData	 = this;
	io.BackendRendererName		 = "RenderUiContext";
	io.BackendFlags				|= ImGuiBackendFlags_RendererHasVtxOffset;
	io.ConfigFlags				|= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags				|= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags				|= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	io.ConfigViewportsNoDefaultParent = true;

	//_setDarkTheme();
	RDS_DUMP_VAR(Path::getCurrentDir());
	auto* rdDev = _rdCtx->renderDevice();
	_shader   = rdDev->createShader("asset/shader/ui/imgui.shader");
	_material = rdDev->createMaterial();
	_material->setShader(_shader);
	_createFontTexture();

	auto totalVertexDataSize	= 10000;
	auto totalIndexDataSize		= 10000;

	if (!_vtxBuf /*|| _vtxBuf->bufSize() < totalVertexDataSize*/) 
	{
		RenderGpuBuffer_CreateDesc desc = {RDS_SRCLOC};
		desc.typeFlags	= RenderGpuBufferTypeFlags::Vertex;
		desc.bufSize	= totalVertexDataSize;
		_vtxBuf = rdDev->createRenderGpuMultiBuffer(desc);
		_vtxBuf->setDebugName("ui-vtxBuf");
	}

	if (!_idxBuf /*|| _idxBuf->bufSize() < totalIndexDataSize*/) 
	{
		RenderGpuBuffer_CreateDesc desc = {RDS_SRCLOC};
		desc.typeFlags	= RenderGpuBufferTypeFlags::Index;
		desc.bufSize	= totalIndexDataSize;
		_idxBuf = rdDev->createRenderGpuMultiBuffer(desc);
		_idxBuf->setDebugName("ui-idxBuf");
	}
}

void
RenderUiContext::destroy()
{
	if (!_ctx) return;

	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererUserData	= nullptr;
	io.BackendRendererName		= nullptr;

	ImGui::DestroyContext(_ctx);
	_ctx = nullptr;

	_vtxBuf.reset(nullptr);
	_idxBuf.reset(nullptr);
	_material.reset(nullptr);
	_shader.reset(nullptr);

	_rdCtx = nullptr;
}

void
RenderUiContext::onBeginRender(RenderContext* renderContext)
{
	RDS_CORE_ASSERT(_rdCtx == renderContext, "must be same RenderContext");

	ImGuiIO& io = ImGui::GetIO();

	auto s = renderContext->framebufferSize();

	io.DisplaySize = ImVec2(s.x, s.y);
	io.DeltaTime = 1.0f / 60.0f;

	ImGui::NewFrame();
}

void
RenderUiContext::onEndRender(RenderContext* renderContext)
{
	RDS_CORE_ASSERT(_rdCtx == renderContext, "must be same RenderContext");
}

void
RenderUiContext::onDrawUI(RenderRequest& req)
{
	ImGui::Render();

	if (!_material) return;

	auto* data = ImGui::GetDrawData();
	if (!data) return;

	if (data->DisplaySize.x <= 0 || data->DisplaySize.y <= 0)
		return;

	if (data->TotalVtxCount <= 0 || data->TotalIdxCount <= 0)
		return;

	int fb_width	= sCast<int>((data->DisplaySize.x * data->FramebufferScale.x));
	int fb_height	= sCast<int>((data->DisplaySize.y * data->FramebufferScale.y));
	if (fb_width <= 0 || fb_height <= 0)
		return;

	{
		float L = data->DisplayPos.x;
		float R = data->DisplayPos.x + data->DisplaySize.x;
		float T = data->DisplayPos.y;
		float B = data->DisplayPos.y + data->DisplaySize.y;

		Mat4f mat(
			{ 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
			{ 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
			{ 0.0f,         0.0f,           0.5f,       0.0f },
			{ (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f }
		);
		//mat[1][1] *= -1;

		_material->setParam("rds_matrix_proj",	mat);
		_material->setParam("texture0",			_fontTex);
	}

	auto vertexSize = sizeof(ImDrawVert);
	auto indexSize  = sizeof(ImDrawIdx);

	RDS_CORE_ASSERT(vertexSize == _vertexLayout->stride());
	RDS_CORE_ASSERT(indexSize  == sizeof(u16));

	auto* rdDev = renderDevice(); RDS_UNUSED(rdDev);
	
	/*
		this has bug, since _vtxBuf will be destroy and the purpose of vtxBuf(MultiGpuBuffer) is to maintain the framed concept
		, also, when the ptr is released
	*/
	#if 0

	auto totalVertexDataSize = data->TotalVtxCount * vertexSize;
	auto totalIndexDataSize  = data->TotalIdxCount * indexSize;

	if (!_vtxBuf || _vtxBuf->bufSize() < totalVertexDataSize) 
	{
		RenderGpuBuffer_CreateDesc desc = {RDS_SRCLOC};
		desc.typeFlags	= RenderGpuBufferTypeFlags::Vertex;
		desc.bufSize	= totalVertexDataSize;
		desc.stride		= vertexSize;
		_vtxBuf = rdDev->createRenderGpuMultiBuffer(desc);
}

	if (!_idxBuf || _idxBuf->bufSize() < totalIndexDataSize) 
	{
		RenderGpuBuffer_CreateDesc desc = {RDS_SRCLOC};
		desc.typeFlags	= RenderGpuBufferTypeFlags::Index;
		desc.bufSize	= totalIndexDataSize;
		desc.stride		= indexSize;
		_idxBuf = rdDev->createRenderGpuMultiBuffer(desc);
	}
	#else
	
	#endif // 0

	// need set set viewport too
	// vkCmdSetViewport(command_buffer, 0, 1, &viewport);

	auto viewportScope		= req.viewportScope();
	auto scissorRectScope	= req.scissorRectScope();

	{
		_vertexData.clear();
		_indexData.clear();

		ImVec2 clip_off		= data->DisplayPos;
		ImVec2 clip_scale	= data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)
		
		Rect2f viewport = {};
		viewport.set(makeVec2f(clip_off), _rdCtx->framebufferSize());
		req.setViewport(viewport);

		// upload data to vtx and idx buf first, since it is using multi-buffer
		{
			for (int i = 0; i < data->CmdListsCount; i++) 
			{
				auto* srcCmd = data->CmdLists[i];

				_vertexData.appendRange(Span<const u8>(reinCast<const u8*>(srcCmd->VtxBuffer.Data), srcCmd->VtxBuffer.Size * vertexSize));
				 _indexData.appendRange(Span<const u8>(reinCast<const u8*>(srcCmd->IdxBuffer.Data), srcCmd->IdxBuffer.Size * indexSize ));
			}
			_vtxBuf->uploadToGpu(_vertexData);
			_idxBuf->uploadToGpu(_indexData);
		}

		int vertexStart = 0;
		int indexStart  = 0;

		ImVec2 clipOff = data->DisplayPos;

		for (int i = 0; i < data->CmdListsCount; i++) 
		{
			auto* srcCmd = data->CmdLists[i];

			for (int j = 0; j < srcCmd->CmdBuffer.Size; j++) 
			{
				auto& srcBuf = srcCmd->CmdBuffer[j];

				if (srcBuf.UserCallback)
				{
					srcBuf.UserCallback(srcCmd, &srcBuf);
				}
				else
				{
					// Project scissor/clipping rectangles into frame buffer space

					ImVec2 clip_min((srcBuf.ClipRect.x - clip_off.x) * clip_scale.x, (srcBuf.ClipRect.y - clip_off.y) * clip_scale.y);
					ImVec2 clip_max((srcBuf.ClipRect.z - clip_off.x) * clip_scale.x, (srcBuf.ClipRect.w - clip_off.y) * clip_scale.y);

					if (clip_min.x < 0.0f)		{ clip_min.x = 0.0f; }
					if (clip_min.y < 0.0f)		{ clip_min.y = 0.0f; }
					if (clip_max.x > fb_width)	{ clip_max.x = sCast<float>(fb_width); }
					if (clip_max.y > fb_height) { clip_max.y = sCast<float>(fb_height); }
					if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
						continue;

					// Apply scissor/clipping rectangle
					auto a = makeVec2f(clip_min);
					auto b = makeVec2f(clip_max);

					req.setScissorRect(Rect2f{a, b - a});

					auto* cmd = req.commandBuffer().addDrawCall(sizeof(PerObjectParam));

					#if RDS_DEVELOPMENT
					cmd->setDebugSrcLoc(RDS_SRCLOC);
					cmd->setDebugName("draw imgui");
					#endif

					cmd->setMaterial(_material);

					cmd->renderPrimitiveType	= RenderPrimitiveType::Triangle;

					cmd->vertexLayout			= _vertexLayout;
					cmd->vertexBuffer			= _vtxBuf->renderGpuBuffer();
					cmd->vertexOffset			= (vertexStart + srcBuf.VtxOffset) * vertexSize;
					cmd->vertexCount			= 0;

					cmd->indexType				= RenderDataTypeUtil::get<ImDrawIdx>();
					cmd->indexBuffer			= _idxBuf->renderGpuBuffer();
					cmd->indexOffset			= (indexStart + srcBuf.IdxOffset) * indexSize;
					cmd->indexCount				= srcBuf.ElemCount;

					cmd->vertexBuffer->setDebugName("imgui vtx buf");
					cmd->indexBuffer ->setDebugName("imgui idx buf");

					// temporary solution
					PerObjectParam perObjParam;
					perObjParam.id = _fontTex->bindlessHandle().getResourceIndex();
					if (auto texId = srcBuf.GetTexID())
					{
						perObjParam.id = reinCast<Texture*>(texId)->bindlessHandle().getResourceIndex();
					}
					cmd->setExtraData(perObjParam);
				}
			}

			vertexStart += srcCmd->VtxBuffer.Size;
			indexStart  += srcCmd->IdxBuffer.Size;

			//_vertexData.appendRange(Span<const u8>(reinCast<const u8*>(srcCmd->VtxBuffer.Data), srcCmd->VtxBuffer.Size * vertexSize));
			// _indexData.appendRange(Span<const u8>(reinCast<const u8*>(srcCmd->IdxBuffer.Data), srcCmd->IdxBuffer.Size * indexSize ));
		}
	}
}

bool
RenderUiContext::onUiMouseEvent(UiMouseEvent& ev)
{
	RDS_CORE_ASSERT(_ctx, "not yet init");

	ImGuiIO& io = ImGui::GetIO();

	using Type = UiMouseEventType;
	switch (ev.type) 
	{
		case Type::Move: 
		{
			io.AddMousePosEvent(ev.pos.x, ev.pos.y);
		} break;

		case Type::Down: 
		{
			io.AddMouseButtonEvent(_mouseButton(ev.button), true);
		} break;

		case Type::Up: 
		{
			io.AddMouseButtonEvent(_mouseButton(ev.button), false);
		} break;

		case Type::Scroll: 
		{
			io.AddMouseWheelEvent(ev.scroll.x, ev.scroll.y);
		} break;
	}

	return io.WantCaptureMouse;
}

bool 
RenderUiContext::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	RDS_CORE_ASSERT(_ctx, "not yet init");

	ImGuiIO& io = ImGui::GetIO();

	using Type = UiKeyboardEventType;

	//io.AddKeyEvent(); // lazy, impl a fn for our key to imgui key set

	return io.WantCaptureKeyboard;
}


//virtual void onUiMouseEvent(UiMouseEvent& ev)		{};


#endif // RDS_RENDER_ENABLE_UI


void
RenderUiContext::_createFontTexture()
{
	ImGuiIO& io = ImGui::GetIO();

	unsigned char* pixels = nullptr;
	int width  = 0;
	int height = 0;
	#if 0
	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
	using Color = ColorRb;
	#else
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	using Color = ColorRGBAb;
	#endif // 0

	auto cDesc = Texture2D::makeCDesc(RDS_SRCLOC);
	cDesc.create(pixels, width, height, Color::s_kColorType);

	auto* rdDev = renderDevice();
	_fontTex = rdDev->createTexture2D(cDesc);
}

int
RenderUiContext::_mouseButton(UiMouseEventButton v)
{
	using Button = UiMouseEventButton;
	switch (v) 
	{
		case Button::Left:		{ return 0; } break;
		case Button::Right:		{ return 1; } break;
		case Button::Middle:	{ return 2; } break;
		case Button::Button4:	{ return 3; } break;
		case Button::Button5:	{ return 4; } break;
	}
	return 0;
}

void 
RenderUiContext::_setDarkTheme()
{
	auto& style = ImGui::GetStyle();

	style.FrameRounding = 4.0f;
	style.GrabRounding = 4.0f;

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Text]					= ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
	colors[ImGuiCol_TextDisabled]			= ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
	colors[ImGuiCol_WindowBg]				= ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_ChildBg]				= ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_PopupBg]				= ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border]					= ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_BorderShadow]			= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg]				= ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgHovered]			= ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
	colors[ImGuiCol_FrameBgActive]			= ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_TitleBg]				= ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
	colors[ImGuiCol_TitleBgActive]			= ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]		= ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg]				= ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]			= ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
	colors[ImGuiCol_ScrollbarGrab]			= ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]	= ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
	colors[ImGuiCol_CheckMark]				= ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab]				= ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]		= ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
	colors[ImGuiCol_Button]					= ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonHovered]			= ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_ButtonActive]			= ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header]					= ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
	colors[ImGuiCol_HeaderHovered]			= ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive]			= ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator]				= ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_SeparatorHovered]		= ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive]		= ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip]				= ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered]		= ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive]		= ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab]					= ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabHovered]				= ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_TabActive]				= ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_TabUnfocused]			= ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive]		= ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_PlotLines]				= ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]		= ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]			= ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]	= ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]			= ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget]			= ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]			= ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]	= ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]		= ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]		= ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

RenderDevice* RenderUiContext::renderDevice() { return _rdCtx->renderDevice(); }


}