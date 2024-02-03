#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/shader/rdsMaterial.h"

namespace rds
{

class RenderMesh;
class RenderGpuBuffer;
class HashedDrawCallCommands;
class RenderRequest;

class VertexLayout;
class Material;

class RenderSubMesh;
class RenderMesh;

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
	E(SetScissorRect, ) \
	E(SetViewport, ) \
	\
	E(DrawRenderables,) \
	\
	E(Dispatch,) \
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

	#if RDS_DEVELOPMENT
	void setDebugSrcLoc(const SrcLoc& srcLoc)	{ _debugSrcLoc	= srcLoc; }
	void setDebugName  (StrView name)			{ _debugName	= name; }
	void setDebugColor (const Color4f& color)	{ _debugColor	= color; }

	const char*		debugName()		const { return _debugName.c_str(); }
	const Color4f&	debugColor()	const { return _debugColor; }

	#else
	void setDebugSrcLoc(const SrcLoc& srcLoc)	{  }
	void setDebugName  (StrView name)			{  }
	void setDebugColor (const Color4f& color)	{  }

	const char*		debugName()		const { return ""; }
	const Color4f&	debugColor()	const { return Color4f { 0.1f, 0.2f, 0.3f, 1.0f }; }

	#endif
protected:
	RenderCommandType _type;

	#if RDS_DEVELOPMENT
	RDS_DEBUG_SRCLOC_DECL;
	TempString	_debugName;
	Color4f		_debugColor = Color4f { 0.1f, 0.2f, 0.3f, 1.0f };
	#endif // RDS_DEVELOPMENT
};

#endif

#if 0
#pragma mark --- rdsGraphics-RenderCommand-Impl ---
#endif // 0
#if 1

class RenderCommand_ClearFramebuffers : public RenderCommand
{
public:
	using Base = RenderCommand;
	using This = RenderCommand_ClearFramebuffers;

public:
	Opt<Color4f>				color;
	Opt<Pair<float, u32> >		depthStencil;

public:
	RenderCommand_ClearFramebuffers() : Base(Type::ClearFramebuffers) {}
	virtual ~RenderCommand_ClearFramebuffers() {};

	This& setClearColor			(const Color4f& color_)			{ color.emplace(color_);	return *this; }
	This& setClearDepth			(float depth)					{ depthStencil.emplace();	depthStencil.value().first	= depth;			return *this; }
	This& setClearDepthStencil	(float depth, u32 stencil)		{ auto& ret = setClearDepth(depth);	depthStencil.value().second = stencil;	return ret; }

	This& dontClearColor() { color.reset(); }
	This& dontClearDepth() { depthStencil.reset(); }

	void reset() { color.reset(); depthStencil.reset(); }

	bool isValid() const { return color.has_value() || depthStencil.has_value(); }
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
	RenderDataType		indexType			= RenderDataType::UInt16;
	RenderPrimitiveType renderPrimitiveType = RenderPrimitiveType::Triangle;
	const VertexLayout* vertexLayout		= nullptr;

	SizeType vertexCount		= 0;
	SizeType indexCount			= 0;
	SizeType vertexOffset		= 0;
	SizeType indexOffset		= 0;
	SizeType materialPassIdx	= 0;

	SPtr<RenderGpuBuffer>	vertexBuffer;
	SPtr<RenderGpuBuffer>	indexBuffer;

	SPtr<Material>			material;

public:
	Material::Pass* getMaterialPass() { return material ? material->getPass(materialPassIdx) : nullptr; }

	void setSubMesh(RenderSubMesh* subMesh, SizeType vtxOffset = 0, SizeType idxOffset = 0);

public:
	RenderCommand_DrawCall() : Base(Type::DrawCall) {}
	virtual ~RenderCommand_DrawCall() {};
};

class RenderCommand_SetScissorRect : public RenderCommand 
{
public:
	using Base = RenderCommand;
	using This = RenderCommand_SetScissorRect;

public:
	Rect2f rect;

public:
	RenderCommand_SetScissorRect() : Base(Type::SetScissorRect) {}
};

class RenderCommand_SetViewport : public RenderCommand 
{
public:
	using Base = RenderCommand;
	using This = RenderCommand_SetViewport;

public:
	Rect2f rect;

public:
	RenderCommand_SetViewport() : Base(Type::SetViewport) {}
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
#pragma mark --- rdsCompute-RenderCommand-Impl ---
#endif // 0
#if 1

class RenderCommand_Dispatch : public RenderCommand
{
public:
	using Base = RenderCommand;
	using This = RenderCommand_Dispatch;

public:
	Tuple3u			threadGroups	= Tuple3u{1, 1, 1};
	SizeType		materialPassIdx	= 0;
	SPtr<Material>	material;

public:
	RenderCommand_Dispatch() : Base(Type::Dispatch) {}
	virtual ~RenderCommand_Dispatch() {};

	Material::Pass* getMaterialPass() { return material ? material->getPass(materialPassIdx) : nullptr; }
};

#endif // 0


#if 0
#pragma mark --- rdsRenderCmdIter-Impl ---
#endif // 0
#if 1

template<typename T>
class RenderCmdIter
{
public:
	using ValueType = T;

	RenderCmdIter(T* p)
		: _p(p)
	{}

			ValueType& operator*()			{ return *_p; }
	const	ValueType& operator*() const	{ return *_p; }

	operator		ValueType*()			{ return _p; }
	operator const	ValueType*() const		{ return _p; }

	constexpr		ValueType* operator->() 		{ return &(operator*()); }
	constexpr const ValueType* operator->() const	{ return &(operator*()); }

	constexpr RenderCmdIter& operator++()		{ advance(); return *this; }
	constexpr RenderCmdIter  operator++(int)	{ RenderCmdIter it(*this); advance(); return it; }
	//constexpr RenderCmdIter& operator--()		{ --_p; return *this; }
	//constexpr RenderCmdIter  operator--(int)	{ RenderCmdIter it(*this); --_p; return it; }

protected:
	void advance() { _p = reinCast<T*>(reinCast<u8*>(_p) + sizeof(T)); }

private:
	T* _p = nullptr;
};

#endif

#if 0
#pragma mark --- rdsRenderCommandBuffer-Impl ---
#endif // 0
#if 1

class RenderCommandBuffer : public NonCopyable
{
	friend class RenderRequest;
public:
	using Traits	= RenderApiLayerTraits;
	using Util		= RenderApiUtil;
	
	using SizeType	= Traits::SizeType;

public:
	static constexpr SizeType s_kAlign				= 16;
	static constexpr SizeType s_kLocalSize			= 64;
	static constexpr SizeType s_kDrawCallLocalSize	= 512;

public:
	using DrawCallCmds = Vector<RenderCommand_DrawCall*, s_kLocalSize, Mallocator>;

public:
	RenderCommandBuffer();
	~RenderCommandBuffer();

	RenderCommandBuffer(RenderCommandBuffer&&)	{ throwIf(true, ""); };
	void operator=(RenderCommandBuffer&&)		{ throwIf(true, ""); };

	void reset();

	RenderCommand_Dispatch*				dispatch();

	RenderCommand_ClearFramebuffers*	clearFramebuffers();
	RenderCommand_SwapBuffers*			swapBuffers();
	RenderCommand_DrawCall*				addDrawCall();

	void setScissorRect(const Rect2f& rect);
	void setViewport(const Rect2f& rect);
	void setViewportReverse(const Rect2f& rect);

	Span<RenderCommand*> commands();

	const Rect2f& scissorRect	() const;
	const Rect2f& viewport		() const;

	const RenderCommand_ClearFramebuffers* getClearValue() const;

private:
	template<class CMD> CMD* newCommand();
	void* alloc(SizeType n, SizeType align);

private:
	LinearAllocator							_alloc;
	Vector<RenderCommand*, s_kLocalSize>	_commands;
	RenderCommand_ClearFramebuffers			_clearFramebufCmd;
	
	Rect2f	_scissorRect	= {};
	Rect2f	_viewport		= {};

};

template<class CMD> inline
CMD* 
RenderCommandBuffer::newCommand()
{
	void* buf = alloc(sizeof(CMD), s_kAlign);
	auto* cmd = new(buf) CMD();
	_commands.emplace_back(cmd);
	return cmd;
}

inline RenderCommand_Dispatch*			RenderCommandBuffer::dispatch()				{ return newCommand<RenderCommand_Dispatch>(); }

inline RenderCommand_ClearFramebuffers* RenderCommandBuffer::clearFramebuffers()	{ return _clearFramebufCmd.isValid() ? newCommand<RenderCommand_ClearFramebuffers>() : &_clearFramebufCmd; }
inline RenderCommand_SwapBuffers*		RenderCommandBuffer::swapBuffers()			{ return newCommand<RenderCommand_SwapBuffers>(); }
inline RenderCommand_DrawCall*			RenderCommandBuffer::addDrawCall()			{ return newCommand<RenderCommand_DrawCall>(); }

inline Span<RenderCommand*> RenderCommandBuffer::commands() { return _commands.span(); }

inline const Rect2f&	RenderCommandBuffer::scissorRect()	const { return _scissorRect; }
inline const Rect2f&	RenderCommandBuffer::viewport()		const { return _viewport; }

inline const RenderCommand_ClearFramebuffers* RenderCommandBuffer::getClearValue() const { return _clearFramebufCmd.isValid() ? &_clearFramebufCmd : nullptr; }


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
			e->reset();
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

#if 0
#pragma mark --- rdsRenderScissorRectScope-Impl ---
#endif // 0
#if 1

class RenderScissorRectScope : public NonCopyable 
{
public:
	RenderScissorRectScope() = default;

	RenderScissorRectScope(RenderScissorRectScope && r) 
	{
		_cmdBuf = r._cmdBuf;
		_rect	= r._rect;
		r._cmdBuf = nullptr;
	}

	RenderScissorRectScope(RenderCommandBuffer* cmdBuf) 
	{
		if (!cmdBuf) return;
		_rect	= cmdBuf->scissorRect();
		_cmdBuf = cmdBuf;
	}

	~RenderScissorRectScope() { detach(); }

	void detach() 
	{
		if (!_cmdBuf) return;
		_cmdBuf->setScissorRect(_rect);
		_cmdBuf = nullptr;
	}

private:
	RenderCommandBuffer* _cmdBuf = nullptr;
	Rect2f _rect;
};

#endif // 1

#if 0
#pragma mark --- rdsRenderViewportScope-Impl ---
#endif // 0
#if 1

class RenderViewportScope : public NonCopyable 
{
public:
	RenderViewportScope() = default;

	RenderViewportScope(RenderViewportScope && r) 
	{
		_cmdBuf = r._cmdBuf;
		_rect	= r._rect;
		r._cmdBuf = nullptr;
	}

	RenderViewportScope(RenderCommandBuffer* cmdBuf) 
	{
		if (!cmdBuf) return;
		_rect	= cmdBuf->scissorRect();
		_cmdBuf = cmdBuf;
	}

	~RenderViewportScope() { detach(); }

	void detach() 
	{
		if (!_cmdBuf) return;
		_cmdBuf->setViewport(_rect);
		_cmdBuf = nullptr;
	}

private:
	RenderCommandBuffer* _cmdBuf = nullptr;
	Rect2f _rect;
};

#endif // 1

}