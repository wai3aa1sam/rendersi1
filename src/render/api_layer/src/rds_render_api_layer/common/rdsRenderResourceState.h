#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderResourceState.h"


#if 0
typedef 
enum D3D12_RESOURCE_STATES
{
    D3D12_RESOURCE_STATE_COMMON	= 0,
    D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER	= 0x1,
    D3D12_RESOURCE_STATE_INDEX_BUFFER	= 0x2,
    D3D12_RESOURCE_STATE_RENDER_TARGET	= 0x4,
    D3D12_RESOURCE_STATE_UNORDERED_ACCESS	= 0x8,
    D3D12_RESOURCE_STATE_DEPTH_WRITE	= 0x10,
    D3D12_RESOURCE_STATE_DEPTH_READ	= 0x20,
    D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE	= 0x40,
    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	= 0x80,
    D3D12_RESOURCE_STATE_STREAM_OUT	= 0x100,
    D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT	= 0x200,
    D3D12_RESOURCE_STATE_COPY_DEST	= 0x400,
    D3D12_RESOURCE_STATE_COPY_SOURCE	= 0x800,
    D3D12_RESOURCE_STATE_RESOLVE_DEST	= 0x1000,
    D3D12_RESOURCE_STATE_RESOLVE_SOURCE	= 0x2000,
    D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE	= 0x400000,
    D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE	= 0x1000000,
    D3D12_RESOURCE_STATE_GENERIC_READ	= ( ( ( ( ( 0x1 | 0x2 )  | 0x40 )  | 0x80 )  | 0x200 )  | 0x800 ) ,
    D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE	= ( 0x40 | 0x80 ) ,
    D3D12_RESOURCE_STATE_PRESENT	= 0,
    D3D12_RESOURCE_STATE_PREDICATION	= 0x200,
    D3D12_RESOURCE_STATE_VIDEO_DECODE_READ	= 0x10000,
    D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE	= 0x20000,
    D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ	= 0x40000,
    D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE	= 0x80000,
    D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ	= 0x200000,
    D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE	= 0x800000
} 	D3D12_RESOURCE_STATES;

#endif // 0


namespace rds
{


// should change to RenderGpuBufferUsageFlags
#define RenderGpuBufferTypeFlags_ENUM_LIST(E) \
	E(None, = 0) \
	E(Vertex,		= BitUtil::bit(0)) \
	E(Index,		= BitUtil::bit(1)) \
	E(Const,		= BitUtil::bit(2)) \
	E(Compute,		= BitUtil::bit(3)) \
	E(TransferSrc,	= BitUtil::bit(4)) \
	E(TransferDst,	= BitUtil::bit(5)) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderGpuBufferTypeFlags, u8);
RDS_ENUM_ALL_OPERATOR(RenderGpuBufferTypeFlags);

#define TextureUsageFlags_ENUM_LIST(E) \
	E(None, = 0) \
	E(ShaderResource,	= BitUtil::bit(0)) \
	E(UnorderedAccess,	= BitUtil::bit(1)) \
	E(RenderTarget,		= BitUtil::bit(2)) \
	E(DepthStencil,		= BitUtil::bit(3)) \
	E(BackBuffer,		= BitUtil::bit(4)) \
	E(TransferSrc,		= BitUtil::bit(5)) \
	E(TransferDst,		= BitUtil::bit(6)) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(TextureUsageFlags, u8);
RDS_ENUM_ALL_OPERATOR(TextureUsageFlags);

#define RenderResourceType_ENUM_LIST(E) \
	E(None, = 0) \
	E(RenderGpuBuffer,		) \
	E(Texture,		        ) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderResourceType, u8);


#if 0
#pragma mark --- rdsRenderResourceStateFlags-Decl ---
#endif // 0
#if 1

enum class RenderResourceStateFlags : u64;
template<>
struct TypeBitMixture_Impl<RenderResourceStateFlags>
{
    using SizeType	= RenderApiLayerTraits::SizeType;

    // Types<0> - reserved
    using Types								= Tuple<u8, RenderResourceType,     RenderAccess,   RenderGpuBufferTypeFlags,                       TextureUsageFlags                       >; 
    static constexpr size_t s_kBitCounts[]	= {     1,  8,                      3,              BitUtil::bitSize<RenderGpuBufferTypeFlags>(),   BitUtil::bitSize<TextureUsageFlags>(),  };
    
    static constexpr SizeType s_kSlotIdxResourceType	= 1;
    static constexpr SizeType s_kSlotIdxRenderAccess	= 2;
    static constexpr SizeType s_kSlotIdxBufferUsage		= 3;
    static constexpr SizeType s_kSlotIdxTextureUsage	= 4;
};

struct RenderResourceStateFlagsUtil
{
public:
    using ValueType = RenderResourceStateFlags;
    using IntType	= EnumInt<RenderResourceStateFlags>;
    using TBM_T     = TBM<RenderResourceStateFlags>;

    //using SemanticT	= VertexSemanticType;

public:
    RenderResourceStateFlagsUtil() = delete;

public:
    static constexpr ValueType make(RenderGpuBufferTypeFlags        flags, RenderAccess access) { return sCast<ValueType>(makeInt(flags, access)); }
    static constexpr ValueType make(TextureUsageFlags               flags, RenderAccess access) { return sCast<ValueType>(makeInt(flags, access)); }

    static constexpr IntType makeInt(RenderGpuBufferTypeFlags       flags, RenderAccess access) { return (sCast<IntType>(RenderResourceType::RenderGpuBuffer)   << TBM_T::offsets(TBM_T::s_kSlotIdxResourceType))  |(sCast<IntType>(flags) << TBM_T::offsets(TBM_T::s_kSlotIdxBufferUsage))  | (sCast<IntType>(access) << TBM_T::offsets(TBM_T::s_kSlotIdxRenderAccess)); }
    static constexpr IntType makeInt(TextureUsageFlags              flags, RenderAccess access) { return (sCast<IntType>(RenderResourceType::Texture)           << TBM_T::offsets(TBM_T::s_kSlotIdxResourceType)) |(sCast<IntType>(flags) << TBM_T::offsets(TBM_T::s_kSlotIdxTextureUsage)) | (sCast<IntType>(access) << TBM_T::offsets(TBM_T::s_kSlotIdxRenderAccess)); }

    static constexpr bool	                    isBuffer		        (RenderResourceStateFlags x) { return TBM_T::getElementValue<TBM_T::s_kSlotIdxResourceType>(x) == RenderResourceType::RenderGpuBuffer; }
    static constexpr bool	                    isTexture		        (RenderResourceStateFlags x) { return TBM_T::getElementValue<TBM_T::s_kSlotIdxResourceType>(x) == RenderResourceType::Texture; }

    static constexpr RenderAccess	            getRenderAccess		    (RenderResourceStateFlags x) { return TBM_T::getElementValue<TBM_T::s_kSlotIdxRenderAccess>(x); }
    static constexpr RenderGpuBufferTypeFlags	getBufferUsageFlags		(RenderResourceStateFlags x) { return TBM_T::getElementValue<TBM_T::s_kSlotIdxBufferUsage> (x); }
    static constexpr TextureUsageFlags		    getTextureUsageFlags    (RenderResourceStateFlags x) { return TBM_T::getElementValue<TBM_T::s_kSlotIdxTextureUsage>(x); }

    static String toString();
};

#define RenderResourceStateFlags_ENUM_LIST(E) \
	E(None,     = 0) \
    \
	E(Index_Read,           = RenderResourceStateFlagsUtil::makeInt(RenderGpuBufferTypeFlags::Index,    RenderAccess::Read)) \
	E(Index_Write,          = RenderResourceStateFlagsUtil::makeInt(RenderGpuBufferTypeFlags::Index,    RenderAccess::Write)) \
	E(Vertex_Read,          = RenderResourceStateFlagsUtil::makeInt(RenderGpuBufferTypeFlags::Vertex,   RenderAccess::Read)) \
	E(Vertex_Write,         = RenderResourceStateFlagsUtil::makeInt(RenderGpuBufferTypeFlags::Vertex,   RenderAccess::Write)) \
	E(Constant,             = RenderResourceStateFlagsUtil::makeInt(RenderGpuBufferTypeFlags::Const,    RenderAccess::Read)) \
	E(Compute_Read,         = RenderResourceStateFlagsUtil::makeInt(RenderGpuBufferTypeFlags::Compute,  RenderAccess::Read)) \
	E(Compute_Write,        = RenderResourceStateFlagsUtil::makeInt(RenderGpuBufferTypeFlags::Compute,  RenderAccess::Write)) \
    \
    E(Transfer_Src,         = RenderResourceStateFlagsUtil::makeInt(RenderGpuBufferTypeFlags::TransferSrc, RenderAccess::Read)  | RenderResourceStateFlagsUtil::makeInt(TextureUsageFlags::TransferSrc, RenderAccess::Read)) \
    E(Transfer_Dst,         = RenderResourceStateFlagsUtil::makeInt(RenderGpuBufferTypeFlags::TransferDst, RenderAccess::Write) | RenderResourceStateFlagsUtil::makeInt(TextureUsageFlags::TransferDst, RenderAccess::Write)) \
	\
    E(ShaderResource,       = RenderResourceStateFlagsUtil::makeInt(TextureUsageFlags::ShaderResource,  RenderAccess::Read)) \
    E(UnorderedAccess,      = RenderResourceStateFlagsUtil::makeInt(TextureUsageFlags::UnorderedAccess, RenderAccess::Write)) \
    E(RenderTarget_Read,    = RenderResourceStateFlagsUtil::makeInt(TextureUsageFlags::RenderTarget,    RenderAccess::Read)) \
    E(RenderTarget,         = RenderResourceStateFlagsUtil::makeInt(TextureUsageFlags::RenderTarget,    RenderAccess::Write)) \
    E(DepthStencil_Read,    = RenderResourceStateFlagsUtil::makeInt(TextureUsageFlags::DepthStencil,    RenderAccess::Read)) \
    E(DepthStencil_Write,   = RenderResourceStateFlagsUtil::makeInt(TextureUsageFlags::DepthStencil,    RenderAccess::Write)) \
    E(BackBuffer,           = RenderResourceStateFlagsUtil::makeInt(TextureUsageFlags::BackBuffer,      RenderAccess::Read)) \
    \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(RenderResourceStateFlags, u64);

//inline
//String 
//RenderResourceStateFlagsUtil::toString()
//{
//
//}


#if 0



#endif // 0

#endif // 0

#if 0
#pragma mark --- rdsRenderResourceState-Decl ---
#endif // 0
#if 1

class RenderResourceState : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:

public:
	static constexpr SizeType   s_kMaxMipCount      = 15;
    static constexpr u32        s_kAllSubResource  = ~u32(0);

public:
	using StateFlags = Vector<RenderResourceStateFlags, s_kMaxMipCount>;

public:
    static bool isAllSubResource(u32 subResource);
    static bool isTransitionNeeded(RenderResourceStateFlags src, RenderResourceStateFlags dst);

public:
    RenderResourceState();
    ~RenderResourceState();

    void setSubResourceCount(SizeType n);

    void setState       (RenderResourceStateFlags state, u32 subResource = s_kAllSubResource);
   // void setPendingState(RenderResourceStateFlags state, u32 subResource = s_kAllSubResource);

    RenderResourceStateFlags state(u32 subResource = s_kAllSubResource) const;


protected:
	StateFlags				    _stateFlags;
    //StateFlags				    _stateFlagsPending;
	RenderResourceStateFlags	_stateFlagsCommon = RenderResourceStateFlags::None;
    bool                        _isCommonState : 1;
};

inline RenderResourceStateFlags RenderResourceState::state(u32 subResource) const { return isAllSubResource(subResource) ? _stateFlagsCommon : _stateFlags[subResource]; }

inline bool RenderResourceState::isAllSubResource(u32 subResource) { return subResource == s_kAllSubResource; }


#endif

}