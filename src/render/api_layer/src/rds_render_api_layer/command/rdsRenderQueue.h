#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#include "rdsRenderCommand.h"

namespace rds
{


class RenderQueue;

#if 0
#pragma mark --- rdsHashedDrawCallCommands-Decl ---
#endif // 0
#if 1

class HashedDrawCallCommands
{
	friend class RenderQueue;
public:
	using SizeType		= RenderApiLayerTraits::SizeType;
	using DrawCallCmds	= RenderCommandBuffer::DrawCallCmds;

public:
	static constexpr SizeType s_kLocalSize = RenderCommandBuffer::s_kLocalSize;

public:
	HashedDrawCallCommands();
	~HashedDrawCallCommands();

	void clear();
	void resize(SizeType n);
	JobHandle sort();

	Span<RenderCommand_DrawCall*> drawCallCmds();

	bool isReadyToUse() const;
	void waitForReady();

	const DrawingSettings& drawingSettings() const;

	SizeType size() const;

public:
	DrawCallCmds&	_internal_drawCallCmds()	{ return _drawCallCmds; };
	JobHandle&		_internal_jobHandle()		{ return _jobHandle; };

protected:
	void checkValid() const;

protected:
	DrawingSettings				_drawingSettings;
	JobHandle					_jobHandle = nullptr;
	Vector<u32, s_kLocalSize>	_hashs;
	DrawCallCmds				_drawCallCmds;
};

inline 
Span<RenderCommand_DrawCall*> 
HashedDrawCallCommands::drawCallCmds()
{
	if (!isReadyToUse())
	{
		JobSystem::instance()->waitForComplete(_jobHandle);
	}
	return _drawCallCmds.span();
}

inline bool								HashedDrawCallCommands::isReadyToUse()		const { return !_jobHandle || _jobHandle->isCompleted(); }
inline const DrawingSettings&			HashedDrawCallCommands::drawingSettings()	const { return _drawingSettings; }
inline HashedDrawCallCommands::SizeType HashedDrawCallCommands::size()				const { return _drawCallCmds.size(); };

#endif

#if 0
#pragma mark --- rdsRenderQueue-Impl ---
#endif // 0
#if 1

class RenderQueue : public NonCopyable
{
public:
	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kLocalSize = 8;

public:
	RenderQueue() = default;
	~RenderQueue();

	RenderQueue(RenderQueue&&)		{ throwIf(true, ""); };
	void operator=(RenderQueue&&)	{ throwIf(true, ""); };

	void clear();

	HashedDrawCallCommands& prepareDrawRenderables(const DrawingSettings& drawingSettings/*, SizeType n*/);

	HashedDrawCallCommands* drawRenderables(const DrawingSettings& drawingSettings);

	
	void recordDrawCall(Vector<RenderCommand_DrawCall*>& out, const RenderMesh& rdMesh, Material* mtl, const Mat4f& transform);

protected:
	void* alloc(SizeType n, SizeType align);

	template<class T>
	RenderCmdIter<T> allocCommands(Vector<T*>& out, SizeType n);

	template<class T>
	T* allocCommands(SizeType n);

	HashedDrawCallCommands* findHashedDrawCmds(const DrawingSettings& drawingSettings);

protected:
	Vector<HashedDrawCallCommands, s_kLocalSize> _hashedDrawCmds;
};

template<class T> inline 
RenderCmdIter<T>
RenderQueue::allocCommands(Vector<T*>& out, SizeType n)
{
	if (n < 1)
		return nullptr;

	auto* p		= allocCommands<T>(n);
	auto beg	= RenderCmdIter<T>{p};
	auto it		= beg;

	for (size_t i = 0; i < n; i++)
	{
		out.emplace_back(it);
		++it;
	}
	return beg;
}

template<class T> inline
T* 
RenderQueue::allocCommands(SizeType n)
{
	auto* p = sCast<T*>(alloc(sizeof(T) * n, RenderCommandBuffer::s_kAlign));
	for (size_t i = 0; i < n; i++)
	{
		new(&(p[i])) T();
	}
	return p;
}

#endif


}