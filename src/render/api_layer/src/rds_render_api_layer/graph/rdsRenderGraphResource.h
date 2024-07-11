#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"
#include "rds_render_api_layer/texture/rdsTexture3D.h"
#include "rds_render_api_layer/texture/rdsTextureCube.h"
#include "rds_render_api_layer/texture/rdsTexture2DArray.h"

namespace rds
{

class RenderGraph;
class RdgPass;

enum class RdgResourceType : u8;
class RdgResource;
template<class T> class RdgResourceT;

using RdgResource_TextureT	= Texture;
using RdgResource_BufferT	= RenderGpuBuffer;

class RdgBuffer;
class RdgTexture;

class RdgTextureHnd;
class RdgBufferHnd;

struct RdgRenderTarget;
struct RdgDepthStencil;

class RdgResourceAccessor;

using RdgId = int;

template<class T> struct RdgResourceTraits;

template<class T> class RdgResourceHndT;

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

#if 1

using RdgAccess = RenderAccess;

struct RdgResourceUsage
{
	using Type = RdgResourceType;
	using BufUsage = RenderGpuBufferTypeFlags;
	using TexUsage = TextureUsageFlags;
public:
	//static bool checkValid(Type type)

	union
	{
		BufUsage buf = BufUsage::None;
		TexUsage tex;
	};

public:
	RdgResourceUsage()
	{
		static_assert(IsSame<EnumInt<BufUsage>, EnumInt<TexUsage> >, "both usage should be same size");
	}
	RdgResourceUsage(BufUsage bufUsage) : buf(bufUsage) {}
	RdgResourceUsage(TexUsage texUsage) : tex(texUsage) {}

	bool operator==(const RdgResourceUsage& rhs) const
	{
		return buf == rhs.buf || tex == rhs.tex;
	}

	bool operator!=(const RdgResourceUsage& rhs) const
	{
		return !operator==(rhs);
	}

	bool hasValue() const { return buf != BufUsage::None || tex != TexUsage::None; }
};

struct RdgResourceState
{
public:
	using Access	= RdgAccess;
	using Usage		= RdgResourceUsage;

public:
	Usage	srcUsage	= {};
	Usage	dstUsage	= {};
	Access	srcAccess	= Access::None;
	Access	dstAccess	= Access::None;

public:
	bool isSameTransition() const
	{
		return srcUsage == dstUsage && srcAccess == dstAccess;
	}
};

class RdgResourceStateTrack
{
public:
	using Access	= RdgAccess;
	using Usage		= RdgResourceUsage;
	using State		= RdgResourceState;

public:
	bool setPendingUsageAccess(Usage usage, Access access)
	{
		bool isValid = true;
		// 
		if (!isValidAccessToSet(pendingAccess(), access))
		{
			RDS_CORE_ASSERT(false, "Could not {} when the pending access is Write, only one write state exist in same pass depth", access);
			isValid = false;
			return isValid;
		}
		_state.dstAccess = access;
		_state.dstUsage	 = usage;
		return isValid;
	}

	bool setCurrentUsageAccess(Usage usage, Access access)
	{
		bool isValid = true;
		
		_state.srcAccess = access;
		_state.srcUsage	 = usage;
		return isValid;
	}

	void commit()
	{
		_state.srcAccess	= _state.dstAccess;
		_state.srcUsage		= _state.dstUsage;

		_state.dstAccess	= {};
		_state.dstUsage		= {};
	}

	//Access	currentAccess() const { return pendingAccess() != Access::None	? pendingAccess()	: _state.srcAccess; }
	//Usage	currentUsage ()	const { return pendingUsage().hasValue()		? pendingUsage()	: _state.srcUsage; }

	Access	currentAccess() const { return _state.srcAccess; }
	Usage	currentUsage ()	const { return _state.srcUsage; }

protected:
	Access	pendingAccess() const { return _state.dstAccess; }
	Usage	pendingUsage ()	const { return _state.dstUsage; }

	bool isValidAccessToSet(Access src, Access dst)
	{
		return !(src== Access::Write || (dst == Access::Write && src == Access::Read));
	}

protected:
	RdgResourceState _state = {};

	/*	
	// probably is useless
	// later for same depth async compute
	Access	_pendingAccess	= Access::None;
	Usage	_pendingUsage	= {};
	
	u32 _lastAccessPassDepth = 0;

	*/

	/*Access	_curAccess		= Access::None;
	Usage	_curUsage		= {};
	Access	_pendingAccess	= Access::None;
	Usage	_pendingUsage	= {};*/
};

#endif // 0

#if 0
#pragma mark --- rdsRdgResource_CreateDesc-Decl ---
#endif // 0
#if 1

//using RdgTexture_CreateDesc = RdgResource_TextureT::CreateDesc;
using RdgTexture_CreateDesc =  Texture_CreateDesc;		// this support all texture type
using RdgBuffer_CreateDesc	=  RdgResource_BufferT::CreateDesc;

using RdgTexture_Desc	= RdgResource_TextureT::Desc;
using RdgBuffer_Desc	=  RdgResource_BufferT::Desc;

#endif // 1

template<> 
struct RdgResourceTraits<RdgResource_TextureT>
{
	using RenderResource	= RdgResource_TextureT;
	using CreateDesc		= RdgTexture_CreateDesc;
	using Desc				= RdgTexture_Desc;
	using Hnd				= RdgTextureHnd;
	using ResourceT			= RdgTexture;
	using RenderResourceT	= RdgResource_TextureT;

	using Usage				= TextureUsageFlags;

public:
	static constexpr RdgResourceType s_kType = RdgResourceType::Texture;
};

template<> 
struct RdgResourceTraits<RdgResource_BufferT>
{
	using RenderResource	= RdgResource_BufferT;
	using CreateDesc		= RdgBuffer_CreateDesc;
	using Desc				= RdgBuffer_Desc;
	using Hnd				= RdgBufferHnd;
	using ResourceT			= RdgBuffer;
	using RenderResourceT	= RdgResource_BufferT;

	using Usage				= RenderGpuBufferTypeFlags;

public:
	static constexpr RdgResourceType s_kType = RdgResourceType::Buffer;
};

class RdgResource
{
	friend class RenderGraph;
	friend class RdgPass;
	friend class RdgResourcePool;
	template<class T> friend class RdgResourceHndT;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Type = RdgResourceType;

	using Pass			= RdgPass;
	using Access		= RdgAccess;
	using State			= RdgResourceState;
	using StateTrack	= RdgResourceStateTrack;
	//using PendingState	= RdgPendingResourceState;

public:
	static constexpr SizeType s_kPassLocalSize = 6;

public:
	using Passes = Vector<Pass*, s_kPassLocalSize>;

public:
	RdgResource() = default;
	RdgResource(const RenderGraph& rdGraph, Type type, StrView name, RdgId id, bool isImported = false, bool isExported = false);
	~RdgResource();

	void create(const RenderGraph& rdGraph, Type type, StrView name, RdgId id, bool isImported, bool isExported);

public:
	StrView name() const;
	RdgId	id	() const;
	Type	type() const;

	bool isImported() const;
	bool isExported() const;

	Span<		Pass*> producers();
	Span<const	Pass*> producers() const;

	const StateTrack&	stateTrack() const;

	RenderResource* renderResource();

protected:
	void addProducer(RdgPass* producer);

	void setImport(bool v);
	void setExport(bool v);
	//void setState(const RdgResourceState& state);

	void commitRenderResouce(RenderResource* rdRsc);

	template<size_t N> bool appendUnqiueProducerTo(Vector<Pass*, N>& dst, RdgPass* producer);
	bool isUniqueProducer(RdgPass* producer) const;

	//void	commitState();
	StateTrack&	stateTrack();

protected:
	TempString	_name;
	RdgId		_id = -1;
	Type		_type;
	bool		_isImported : 1;
	bool		_isExported : 1;

	Passes	_producers;

	StateTrack _stateTrack;
	//PendingState	_pendingState;

	RenderResource*			_pRdRsc	= nullptr;
	SPtr<RenderResource>	_spRdRsc;
};

inline void RdgResource::setImport(bool v) { _isImported = v; }
inline void RdgResource::setExport(bool v) { _isExported = v; }

inline 
void 
RdgResource::commitRenderResouce(RenderResource* rdRsc)
{
	_pRdRsc		= rdRsc;
	_spRdRsc	= rdRsc;
}
//inline void	RdgResource::commitState() { state().commit(); }

inline const	RdgResource::StateTrack& RdgResource::stateTrack() const	{ return _stateTrack; }
inline			RdgResource::StateTrack& RdgResource::stateTrack()			{ return _stateTrack; }

//inline void RdgResource::setState(const RdgResourceState& state) { _state = state; }

//inline RdgResource::PendingState		RdgResource::pendingState()		 { return _pendingState; }

inline StrView							RdgResource::name		() const { return _name; }
inline RdgId							RdgResource::id			() const { return _id; }
inline RdgResource::Type				RdgResource::type		() const { return _type; }
inline bool								RdgResource::isImported	() const { return _isImported; }
inline bool								RdgResource::isExported	() const { return _isExported; }

inline RenderResource*					RdgResource::renderResource()	 { return _pRdRsc; }

inline Span<		RdgResource::Pass*> RdgResource::producers()		{ return _producers; }
inline Span<const	RdgResource::Pass*>	RdgResource::producers() const	{ return spanConstCast<const Pass*>(_producers); }

//inline			RdgResource::State&	RdgResource::state()		{ return _state; }
//inline const	RdgResource::State&	RdgResource::state() const	{ return _state; }


template<class T>
class RdgResourceT : public RdgResource
{
	friend class RenderGraph;
	friend class RdgPass;
	friend class RdgResourceAccessor;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Base				= RdgResource;

	using ResourceTraits	= RdgResourceTraits<T>;
	using RenderResourceT	= T;
	using CreateDesc		= typename ResourceTraits::CreateDesc;
	using Desc				= typename ResourceTraits::Desc;

public:
	RdgResourceT() = default;

	RdgResourceT(const RenderGraph& rdGraph, const CreateDesc& cDesc, StrView name, RdgId id, bool isImport, bool isExport)
		//: Base(ResourceTraits::s_kType, name, id, isImport, isExport)
	{
		create(rdGraph, cDesc, name, id, isImport, isExport);
	}

	void create(const RenderGraph& rdGraph, const CreateDesc& cDesc, StrView name, RdgId id, bool isImport, bool isExport)
	{
		Base::create(rdGraph, ResourceTraits::s_kType, name, id, isImport, isExport);
		_desc = cDesc;
	}

	const Desc&			desc()	const;

protected:
			RenderResourceT*	access()		{ return sCast<		 RenderResourceT*>(_pRdRsc); }
	const	RenderResourceT*	access() const	{ return sCast<const RenderResourceT*>(_pRdRsc); }

	//void commitResouce(RenderResourceT* rsc);

protected:
	Desc					_desc;
};

template<class T> inline const typename RdgResourceT<T>::Desc& RdgResourceT<T>::desc() const { return _desc; }

//template<class T> inline 
//void 
//RdgResourceT<T>::commitResouce(RenderResourceT* rsc)
//{
//	_pRdRsc		= rsc;
//	_spRdRsc	= rsc;
//}

class RdgResourceHnd
{
	friend class RenderGraph;
	friend class RdgPass;
public:
	using Type = RdgResourceType;

public:
	RdgResourceHnd() = default;
	RdgResourceHnd(RdgResource* rdgRsc) : _rdgRsc(rdgRsc) {}

	StrView name() const { return _rdgRsc->name(); }
	RdgId	id	() const { return _rdgRsc->id(); }
	Type	type() const { return _rdgRsc->type(); }

protected:
	RdgResource* _rdgRsc = nullptr;
};

template<class T>
class RdgResourceHndT : public RdgResourceHnd
{
	friend class	RenderGraph;
	friend class	RdgPass;
	friend class	RdgResourceAccessor;
	friend struct	RdgTarget;
	friend struct	RdgRenderTarget;
	friend struct	RdgDepthStencil;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using ResourceTraits	= RdgResourceTraits<T>;
	using ResourceT			= typename ResourceTraits::ResourceT;
	using RenderResourceT	= typename ResourceTraits::RenderResourceT;

	using Desc				= typename ResourceTraits::Desc;

public:
	RdgResourceHndT()
	{
		static_assert(sizeof(RdgResourceHndT<T>) == sizeof(RdgResourceHnd), "no member should be added");
	}

	/*		RdgResourceT<T>* resource()			{ return sCast<			RdgResourceT<T>*>(_rdgRsc); }
	const	RdgResourceT<T>* resource() const	{ return sCast<const	RdgResourceT<T>*>(_rdgRsc); }
	*/

public:
	explicit operator bool() const { return resource(); }

public:
	const Desc& desc() const			{ return resource()->desc(); }

	RenderResourceT* renderResource()		{ return sCast<RenderResourceT*>(resource()->renderResource()); }
	RenderResourceT* renderResource() const	{ return sCast<RenderResourceT*>(_rdgRsc->renderResource()); }

protected:
			ResourceT* resource()		{ return sCast<			ResourceT*>(_rdgRsc); }
			ResourceT* resource() const	{ return sCast<			ResourceT*>(_rdgRsc); }

	//		RenderResourceT* renderResource()		{ return sCast<			RenderResourceT*>(resource()->access()); }
	//const	RenderResourceT* renderResource() const	{ return sCast<const	RenderResourceT*>(resource()->access()); }
};

//using RdgBuffer		= RdgResourceT		<RdgResource_BufferT>;
//using RdgBufferHnd	= RdgResourceHndT	<RdgResource_BufferT>;

//using RdgTexture	= RdgResourceT		<RdgResource_TextureT>;
//using RdgTextureHnd	= RdgResourceHndT	<RdgResource_TextureT>;


class RdgTexture : public RdgResourceT<RdgResource_TextureT>
{
	friend class RenderGraph;
	friend class RdgPass;
	friend class RdgTextureHnd;
	friend class RdgResourceAccessor;
public:
	using Size = RdgResource_TextureT::Size;

public:
	RdgTexture()	{};
	~RdgTexture()	{};

	const Size&		size()		const { return access()->size(); }
	ColorType		format()	const { return desc().format; }
};


class RdgBuffer : public RdgResourceT<RdgResource_BufferT>
{
	friend class RenderGraph;
	friend class RdgPass;
	friend class RdgBufferHnd;
	friend class RdgResourceAccessor;
public:
	RdgBuffer() {};
	~RdgBuffer() {};

public:

};

class RdgTextureHnd : public RdgResourceHndT<RdgResource_TextureT>
{
	friend class RenderGraph;
	friend class RdgPass;
	friend class RdgBufferHnd;
	friend class RdgResourceAccessor;
public:
	using RenderResource	= RdgResource_TextureT;
	using Size				= RenderResource::Size;

public:
	RdgTextureHnd() = default;

	Size				size()				const	{ return desc().size; }
	ColorType			format()			const	{ return desc().format; }
	TextureUsageFlags	usageFlags()		const	{ return desc().usageFlags; }

	Texture*			texture()					{ return sCast<Texture*>(resource()->renderResource()); }
	
	Texture2D*			texture2D()					{ RDS_CORE_ASSERT(desc().type == RenderDataType::Texture2D);		return sCast<Texture2D*>(resource()->renderResource()); }
	Texture2D*			texture2D()			const	{ RDS_CORE_ASSERT(desc().type == RenderDataType::Texture2D);		return sCast<Texture2D*>(resource()->renderResource()); }

	Texture3D*			texture3D()					{ RDS_CORE_ASSERT(desc().type == RenderDataType::Texture3D);		return sCast<Texture3D*>(resource()->renderResource()); }
	Texture3D*			texture3D()			const	{ RDS_CORE_ASSERT(desc().type == RenderDataType::Texture3D);		return sCast<Texture3D*>(resource()->renderResource()); }

	TextureCube*		textureCube()				{ RDS_CORE_ASSERT(desc().type == RenderDataType::TextureCube);		return sCast<TextureCube*>(resource()->renderResource()); }
	TextureCube*		textureCube()		const	{ RDS_CORE_ASSERT(desc().type == RenderDataType::TextureCube);		return sCast<TextureCube*>(resource()->renderResource()); }

	Texture2DArray*		texture2DArray()			{ RDS_CORE_ASSERT(desc().type == RenderDataType::Texture2DArray);	return sCast<Texture2DArray*>(resource()->renderResource()); }
	Texture2DArray*		texture2DArray()	const	{ RDS_CORE_ASSERT(desc().type == RenderDataType::Texture2DArray);	return sCast<Texture2DArray*>(resource()->renderResource()); }
};

class RdgBufferHnd : public RdgResourceHndT<RdgResource_BufferT>
{
	friend class RenderGraph;
	friend class RdgPass;
	friend class RdgBufferHnd;
	friend class RdgResourceAccessor;
public:
	using RenderResource	= RdgResource_BufferT;

public:
	RdgBufferHnd() = default;

	RenderGpuBufferTypeFlags	usageFlags()		const	{ return desc().typeFlags; }
};

class RdgResourceAccessor
{
public:
	static RdgResource_TextureT*	access(RdgTextureHnd hnd)	{ return hnd.resource()->access(); }
	static RdgResource_BufferT*		access(RdgBufferHnd hnd)	{ return hnd.resource()->access(); }

	static RdgResource_TextureT*	access(RdgTexture*	tex)	{ return tex->access(); }
	static RdgResource_BufferT*		access(RdgBuffer*	buf)	{ return buf->access(); }

	template<class RD_RSC_API> static RD_RSC_API access(RdgTextureHnd	tex)	{ static_assert(IsSame<typename RemovePtr<RD_RSC_API>::Base, RdgTexture::RenderResourceT>, ""); return sCast<RD_RSC_API>(tex.resource()->access()); }
	template<class RD_RSC_API> static RD_RSC_API access(RdgBufferHnd	buf)	{ static_assert(IsSame<typename RemovePtr<RD_RSC_API>::Base,  RdgBuffer::RenderResourceT>, ""); return sCast<RD_RSC_API>(buf.resource()->access()); }

	template<class RD_RSC_API> static RD_RSC_API access(RdgTexture*		tex)	{ static_assert(IsSame<typename RemovePtr<RD_RSC_API>::Base, RdgTexture::RenderResourceT>, ""); return sCast<RD_RSC_API>(tex->access()); }
	template<class RD_RSC_API> static RD_RSC_API access(RdgBuffer*		buf)	{ static_assert(IsSame<typename RemovePtr<RD_RSC_API>::Base,  RdgBuffer::RenderResourceT>, ""); return sCast<RD_RSC_API>(buf->access()); }

	//template<class RD_RSC_API> static RD_RSC_API access(RdgResource*	rsc)	{ static_assert(IsSame<typename RemovePtr<RD_RSC_API>::Base,  RdgBuffer::RenderResourceT>, ""); return sCast<RD_RSC_API>(buf->access()); }

private:
	//RdgResource* _rdgRsc = nullptr;
};

#endif

}