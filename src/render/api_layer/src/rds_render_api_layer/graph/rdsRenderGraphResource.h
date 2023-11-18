#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"

namespace rds
{

class RenderGraph;
class RdgPass;

enum class RdgResourceType : u8;
class RdgResource;
template<class T> class RdgResourceT;

using RdgResource_BufferT	= RenderGpuBuffer;
using RdgResource_TextureT	= Texture;

using RdgId = int;

#define RdgResourceAccess_ENUM_LIST(E) \
	E(None, = 0) \
	E(Read,) \
	E(Write,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RdgResourceAccess, u8);

#if 0
#pragma mark --- rdsRdgResource-Decl ---
#endif // 0
#if 1

#define RdgResourceType_ENUM_LIST(E) \
	E(None, = 0) \
	E(Buffer,) \
	E(Texture,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RdgResourceType, u8);

template<class T> struct RdgResourceTrait;

struct RdgTexture_CreateDesc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:

};

struct RdgBuffer_CreateDesc : public RdgResource_BufferT::Desc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Base = RdgResource_BufferT::Desc;

	using RenderResourceCDesc = RdgResource_BufferT::Desc;

public:
	RdgBuffer_CreateDesc() = default;

	RdgBuffer_CreateDesc(const RenderResourceCDesc& cDesc)
		: Base(cDesc)
	{}
};


template<> 
struct RdgResourceTrait<RdgResource_BufferT>
{
	using RenderResourceT	= RdgResource_BufferT;
	using CreateDesc		= RdgBuffer_CreateDesc;
	using Info				= RdgBuffer_CreateDesc;

public:
	static constexpr RdgResourceType s_kType = RdgResourceType::Buffer;
};

template<> 
struct RdgResourceTrait<RdgResource_TextureT>
{
	using RenderResourceT	= RdgResource_TextureT;
	using CreateDesc		= RdgTexture_CreateDesc;
	using Info				= RdgTexture_CreateDesc;

public:
	static constexpr RdgResourceType s_kType = RdgResourceType::Texture;
};

class RdgResource
{
	friend class RenderGraph;
	friend class RdgPass;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Type = RdgResourceType;

	using Pass = RdgPass;

public:
	static constexpr SizeType s_kPassLocalSize = 6;

public:
	RdgResource(StrView name, RdgId id, Type type, bool isImported = false, bool isExported = false)
		: _name(name), _id(id), _type(type), _isImported(isImported), _isExported(isExported)
	{
	}

	StrView name() const;
	RdgId	id	() const;
	Type	type() const;

	bool isImported() const;
	bool isExported() const;

	Span<		Pass*> producers();
	Span<const	Pass*> producers() const;

protected:
	void addProducer(RdgPass* producer);

	void setImport(bool v);
	void setExport(bool v);

protected:
	String	_name;
	RdgId	_id = -1;
	Type	_type;
	bool	_isImported : 1;
	bool	_isExported : 1;

	Vector<Pass*, s_kPassLocalSize> _producers;
};

inline
void 
RdgResource::addProducer(RdgPass* producer)
{
	// TODO: render graph memory aliasing will help the vram bandwith;
	RDS_CORE_ASSERT(_producers.size() == 0, "only 1 write is supported, please create new resource for further process");
	_producers.emplace_back(producer);
}

inline void RdgResource::setImport(bool v) { _isImported = v; }
inline void RdgResource::setExport(bool v) { _isExported = v; }

inline StrView							RdgResource::name		() const { return _name; }
inline RdgId							RdgResource::id			() const { return _id; }
inline RdgResource::Type				RdgResource::type		() const { return _type; }
inline bool								RdgResource::isImported	() const { return _isImported; }
inline bool								RdgResource::isExported	() const { return _isExported; }

inline Span<		RdgResource::Pass*> RdgResource::producers()		{ return _producers; }
inline Span<const	RdgResource::Pass*>	RdgResource::producers() const	{ return spanConstCast<const Pass*>(_producers); }

template<class T>
class RdgResourceT : public RdgResource
{
	friend class RenderGraph;
	friend class RdgPass;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Base				= RdgResource;

	using Trait				= RdgResourceTrait<T>;
	using RenderResourceT	= T;
	using CreateDesc		= typename Trait::CreateDesc;
	using Info				= typename Trait::Info;


public:
	RdgResourceT(const CreateDesc& cDesc, StrView name, RdgId id, bool isImport = false, bool isExport = false)
		: Base(name, id, Trait::s_kType, isImport, isExport)
	{
	}

	const Info& info() const;

protected:

protected:
	Info					_info;
	RenderResourceT*		_pRenderResource	= nullptr;
	SPtr<RenderResourceT>	_spRenderResource;
};


template<class T> inline const typename RdgResourceT<T>::Info& RdgResourceT<T>::info() const { return _info; }


class RdgResourceHnd
{
	friend class RenderGraph;
	friend class RdgPass;
public:
	RdgResourceHnd() = default;
	RdgResourceHnd(RdgResource* p) : _p(p) {}

	StrView name() const { return _p->name(); }
	RdgId	id	() const { return _p->id(); }

protected:
	RdgResource* _p = nullptr;
};

template<class T>
class RdgResourceHndT : public RdgResourceHnd
{
	friend class RenderGraph;
	friend class RdgPass;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	RdgResourceHndT()
	{
		static_assert(sizeof(RdgResourceHndT<T>) == sizeof(RdgResourceHnd), "no member should be added");
	}

	RdgResourceT<T>* ptr() { return sCast<RdgResourceT<T>*>(_p); }
};

using RdgBuffer		= RdgResourceT		<RdgResource_BufferT>;
using RdgBufferHnd	= RdgResourceHndT	<RdgResource_BufferT>;

using RdgTexture	= RdgResourceT		<RdgResource_TextureT>;
using RdgTextureHnd	= RdgResourceHndT	<RdgResource_TextureT>;


#endif

}