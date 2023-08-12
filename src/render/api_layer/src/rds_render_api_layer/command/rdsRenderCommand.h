#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

class RenderMesh;
class RenderGpuBuffer;
class HashedDrawCallCommands;

using DrawingSettings = u64;

#if 0
#pragma mark --- rdsRenderCommand-Impl ---
#endif // 0
#if 1

#define RenderCommandType_ENUM_LIST(E) \
	E(None, = 0) \
	E(ClearFramebuffers,) \
	E(SwapBuffers,) \
	E(DrawCall,) \
	\
	E(DrawRenderables,) \
	\
	E(CopyBuffer,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderCommandType, u8);

class RenderCommand : public NonCopyable
{
public:
	using Type = RenderCommandType;

	using Traits = RenderApiLayerTraits;

	using SizeType = Traits::SizeType;

public:
	RenderCommand(Type type) : _type(type) {}
	virtual ~RenderCommand() {};

	Type type() const { return _type; }

protected:
	RenderCommandType _type;
};

class RenderCommand_ClearFramebuffers : public RenderCommand
{
public:
	using Base = RenderCommand;
	using This = RenderCommand_ClearFramebuffers;

public:
	Opt<Color4f>				clearColor;
	Opt<Pair<float, u32> >		clearDepthStencil;

public:
	RenderCommand_ClearFramebuffers() : Base(Type::ClearFramebuffers) {}
	virtual ~RenderCommand_ClearFramebuffers() {};

	This& setClearColor			(const Color4f& color)			{ clearColor = color; return *this; }
	This& setClearDepth			(float depth)					{ clearDepthStencil.value() = {};  clearDepthStencil.value().first	= depth;	return *this; }
	This& setClearDepthStencil	(float depth, u32 stencil)		{ auto& ret = setClearDepth(depth);clearDepthStencil.value().second = stencil;	return ret; }

	This& dontClearColor() { clearColor.reset(); }
	This& dontClearDepth() { clearDepthStencil.reset(); }
};

class RenderCommand_SwapBuffers : public RenderCommand
{
public:
	using Base = RenderCommand;
	using This = RenderCommand_SwapBuffers;

public:


public:
	RenderCommand_SwapBuffers() : Base(Type::SwapBuffers) {}
	virtual ~RenderCommand_SwapBuffers() {};
};

class RenderCommand_DrawCall : public RenderCommand
{
public:
	using Base = RenderCommand;
	using This = RenderCommand_DrawCall;

public:
	RenderDataType indexType = RenderDataType::UInt16;

	SizeType vertexCount	= 0;
	SizeType indexCount		= 0;
	SizeType vertexOffset	= 0;
	SizeType indexOffset	= 0;

	SPtr<RenderGpuBuffer> vertexBuffer;
	SPtr<RenderGpuBuffer> indexBuffer;

public:
	RenderCommand_DrawCall() : Base(Type::DrawCall) {}
	virtual ~RenderCommand_DrawCall() {};
};

class RenderCommand_DrawRenderables : public RenderCommand
{
public:
	using Base = RenderCommand;
	using This = RenderCommand_DrawRenderables;

public:
	HashedDrawCallCommands* hashedDrawCallCmds = nullptr;

public:
	RenderCommand_DrawRenderables() : Base(Type::DrawRenderables) {}
	virtual ~RenderCommand_DrawRenderables() {};
};


#endif


#if 0
#pragma mark --- rdsRenderCommandBuffer-Impl ---
#endif // 0
#if 1

class RenderCommandBuffer : public NonCopyable
{
public:
	using Traits	= RenderApiLayerTraits;
	using Util		= RenderApiUtil;
	
	using SizeType	= Traits::SizeType;

public:
	static constexpr SizeType s_kAlign				= 16;
	static constexpr SizeType s_kLocalSize			= 64;
	static constexpr SizeType s_kDrawCallLocalSize	= 512;

public:
	using DrawCallCmds = Vector<RenderCommand_DrawCall*, s_kLocalSize>;

public:
	RenderCommandBuffer();
	~RenderCommandBuffer();

	RenderCommandBuffer(RenderCommandBuffer&&)	{ throwIf(true, ""); };
	void operator=(RenderCommandBuffer&&)		{ throwIf(true, ""); };

	void clear();

	RenderCommand_ClearFramebuffers*	clearFramebuffers();
	RenderCommand_SwapBuffers*			swapBuffers();
	RenderCommand_DrawCall*				addDrawCall();

	void drawRenderables(const DrawingSettings& settings);

	Span<RenderCommand*> commands();

	const RenderCommand_ClearFramebuffers* getClearFramebuffersCmd() const;

private:
	template<class CMD> CMD* addCommand();
	void* allocCommand(SizeType n, SizeType align);

private:
	Vector<RenderCommand*, s_kLocalSize>	_commands;
	RenderCommand_ClearFramebuffers*		_clearFramebuffersCmd = nullptr;
};

template<class CMD> inline
CMD* 
RenderCommandBuffer::addCommand()
{
	void* buf = allocCommand(sizeof(CMD), s_kAlign);
	auto* cmd = new(buf) CMD();
	_commands.emplace_back(cmd);
	return cmd;
}

inline RenderCommand_ClearFramebuffers* RenderCommandBuffer::clearFramebuffers()	{ _clearFramebuffersCmd = addCommand<RenderCommand_ClearFramebuffers>(); return _clearFramebuffersCmd; }
inline RenderCommand_SwapBuffers*		RenderCommandBuffer::swapBuffers()			{ return addCommand<RenderCommand_SwapBuffers>(); }
inline RenderCommand_DrawCall*			RenderCommandBuffer::addDrawCall()			{ return addCommand<RenderCommand_DrawCall>(); }

inline Span<RenderCommand*> RenderCommandBuffer::commands() { return _commands.span(); }

inline const RenderCommand_ClearFramebuffers* RenderCommandBuffer::getClearFramebuffersCmd() const { return _clearFramebuffersCmd; }


#endif


#if 0
#pragma mark --- rdsRenderCommandPool-Impl ---
#endif // 0
#if 1

class RenderCommandPool : public NonCopyable
{
public:
	using Traits	= RenderApiLayerTraits;
	using Util		= RenderApiUtil;

	using SizeType	= Traits::SizeType;

public:
	static constexpr SizeType s_kLocalSize = 12;

public:

	void reset()
	{
		for (auto& e : _renderCmdBufs)
		{
			e->clear();
		}
		_activeRenderCmdBufCoumt = 0;
	}

	RenderCommandBuffer* requestRenderCommandBuffer()
	{
		if (_activeRenderCmdBufCoumt < _renderCmdBufs.size())
		{
			return _renderCmdBufs[_activeRenderCmdBufCoumt++];
		}
		_activeRenderCmdBufCoumt++;
		return _renderCmdBufs.emplace_back(makeUPtr<RenderCommandBuffer>());
	}

protected:
	Vector<UPtr<RenderCommandBuffer>, s_kLocalSize> _renderCmdBufs;
	u32 _activeRenderCmdBufCoumt = 0;
};

#endif



}