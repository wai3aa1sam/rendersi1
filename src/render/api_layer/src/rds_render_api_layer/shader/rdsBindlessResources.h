#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsBindlessResourceHandle.h"

#include "rds_render_api_layer/common/rdsRenderResourceState.h"

#include "rds_render_api_layer/buffer/rdsRenderGpuBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"

namespace rds
{

class RenderGpuBuffer;
class Texture;
class SamplerStateListTable;

struct BindlessResources_CreateDesc : public RenderResource_CreateDesc
{
    u32             size    = 512 * 10;
};

// VectorTable
#if 0
#pragma mark --- rdsSamplerStateListTable-Decl ---
#endif // 0
#if 1

#if 1

// this could keep the index same
// template<class T, class VALUE, size_t LOCAL_SIZE = 0, class PRED = Less<typename rds::Hash<T>::Value>, class ALLOC = DefaultAllocator>
class SamplerStateListTable
{
public:
    using SizeType = RenderApiLayerTraits::SizeType;

public:
    static constexpr SizeType s_kLocalSize = 64;

public:
    //struct 
    //using T = SamplerState;

    using T = SamplerState;


    using Key      = T::HashValue;      // rds::Hash<T>::Value
    //using Value    = T*;
    using Value    = SizeType;

    using List  = Vector<T*, s_kLocalSize>;
    using Table = VectorMap<Key, Value>;
    using Alloc = LinearAllocator;		// VectorSet<T> and store its ptr will fucked up, since resize will re-alloc

    using Iter = List::Iter;

public:
    static constexpr Value s_kInvalid = ~Value(0);
    //static constexpr Value s_kInvalid = nullptr;

public:
    void add(const T& s)
    {
        RDS_CORE_ASSERT(!find(s), "already exist");
        auto* buf = _alloc.alloc(sizeof(T));
        auto* p = new(buf) T(s);
        _list.emplace_back(p);
        //_table.emplace(p->hash(), Value(p));
        _table.emplace(p->hash(), size() - 1);
    }

    SamplerState* find(const T& s) const
    {
        auto it = _table.find(s.hash());
        return it != _table.end() ? _list[it->second] : nullptr;
    }

    SizeType findIndex(const T& s) const
    {
        auto it = _table.find(s.hash());
        return it != _table.end() ? it->second : s_kInvalid;
    }

    void clear()
    {
        _list.clear();
        _table.clear();
        _alloc.clear();
    }

    SizeType size() const { return _list.size(); }

    Iter begin()  { return _list.begin(); }
    Iter end()    { return _list.end(); }

protected:
    List    _list;
    Table   _table;
    Alloc	_alloc;
};

#else

class SamplerStateTable : protected VectorMap<SamplerState, u32>
{
public:
    using HandleType = u32;

    using Base = VectorMap<SamplerState, HandleType>;

    using T = SamplerState;

    using Iter = Base::iterator;

public:
    void emplace(const T& s)
    {
        //auto& table = *this;
        Base::emplace(s, sCast<HandleType>(size()));
    }

    HandleType findIndex(const T& s)
    {
        auto& table = *this;
        auto it = table.find(s);
        RDS_CORE_ASSERT(it != table.end(), "not found");
        return it != table.end() ? it->second : ~HandleType(0);
    }

    void clear()
    {
        Base::clear();
    }

    SizeType size() const { return Base::size(); }

    Iter begin()  { return Base::begin(); }
    Iter end()    { return Base::end(); }
};

#endif // 0

#endif // 1

#if 0
#pragma mark --- rdsBindlessResources-Decl ---
#endif // 0
#if 1

enum class BindlessResourceType
{
    Buffer,
    Texture,
    Image,
    AccelerationStruct,
    _kCount,
};

class BindlessResources : public RenderResource
{
public:
    using Base          = RenderResource;
    using CreateDesc    = BindlessResources_CreateDesc;

public:
    static constexpr SizeType s_kTypeCount = enumInt(BindlessResourceType::_kCount);

public:
    static CreateDesc   makeCDesc();
    static SizeType     supportSamplerCount();
    static SizeType     createSamplerListTable(SamplerStateListTable& o);
    static SizeType     bindlessTypeCount();
    static bool         isSupportAccelerationStruct();


public:
    BindlessResources();
    ~BindlessResources();

    void create(const CreateDesc& cDesc);
    void destroy();

    BindlessResourceHandle allocBuffer (RenderGpuBuffer*    rsc);
    BindlessResourceHandle allocTexture(Texture*            rsc);
    BindlessResourceHandle allocImage(  Texture*            rsc);

    void freeBuffer (   RenderGpuBuffer*   rsc);
    void freeTexture(   Texture*           rsc);
    void freeImage(     Texture*           rsc);

    void commit();

    u32 findSamplerIndex(const SamplerState& samplerState) const;

public:
    SizeType samplerCount()         const;
    SizeType size()                 const;


protected:
    virtual void onCreate(const CreateDesc& cDesc);
    virtual void onDestroy();

    virtual void onCommit();

public:
    template<class RSC>
    struct ResourceAlloc
    {
    public:
        static constexpr SizeType s_kLocalSize = 64;

    public:
        using Freelist      = Vector<u32, s_kLocalSize>;
        using FreelistTable = VectorMap<u32, Freelist>;

    public:
        Vector<SPtr<RSC>,   s_kLocalSize>   rscs;
        Vector<u32,         s_kLocalSize>   freedIndices;        // for mipCount == 1
        u32                                 nextRscId = 0;
        FreelistTable                       freelistTable;      // for mipCount > 1

    public:
        BindlessResourceHandle alloc(RSC* rsc, BindlessResources* bindlessRscs, u32 count, BindlessResourceType type)
        {
            auto oHnd = BindlessResourceHandle{};

            auto& dst = rscs.emplace_back();
            dst.reset(rsc);
            
            auto& freelist = getFreelist(count, type);
            if (freelist.is_empty())
            {
                oHnd.setValue(nextRscId);
                nextRscId += count;
            }
            else
            {
                oHnd.setValue(freelist.moveBack());
            }
            oHnd.setSubResourceCount(count);

            throwIf(totalResourceCount() > bindlessRscs->size(), "bindless descriptor count overflow, limit: {}", bindlessRscs->size());
            return oHnd;
        }

        void free(BindlessResourceHandle hnd, u32 count, BindlessResourceType type)
        {
            auto& freelist = getFreelist(count, type);
            freelist.emplace_back(hnd.getValue());
        }

        Freelist& getFreelist(u32 count, BindlessResourceType type) 
        { 
            bool isImage = type == BindlessResourceType::Image; 
            return isImage && count != 1 ? freelistTable[count] : freedIndices; 
        }

    public:
        u32 totalResourceCount() const { return nextRscId; }
    };

public:
    template<class T>
    BindlessResourceHandle alloc(T* rsc, MutexProtected<ResourceAlloc<T> >& alloc, u32 count, BindlessResourceType type)
    {
        auto oHnd = BindlessResourceHandle{};
        {
            auto data = alloc.scopedULock();
            oHnd = data->alloc(rsc, this, count, type);
        }
        return oHnd;
    }

    template<class T>
    void free(BindlessResourceHandle hnd, MutexProtected<ResourceAlloc<T> >& alloc, u32 count, BindlessResourceType type)
    {
        if (!hnd.isValid())
            return;
        { 
            auto data = alloc.scopedULock(); 
            data->free(hnd, count, type); 
        }
    }

protected:
    MutexProtected<ResourceAlloc<Texture> >             _texAlloc;
    MutexProtected<ResourceAlloc<Texture> >             _imgAlloc;
    MutexProtected<ResourceAlloc<RenderGpuBuffer> >     _bufAlloc;

    SamplerStateListTable _samplerStateListTable;

    SizeType _size = 0;
};

inline BindlessResources::SizeType BindlessResources::samplerCount()        const { return _samplerStateListTable.size(); }


inline BindlessResources::SizeType BindlessResources::size() const { return _size; }

inline u32 BindlessResources::findSamplerIndex(const SamplerState& samplerState) const 
{ 
    auto idx = _samplerStateListTable.findIndex(samplerState);
    RDS_CORE_ASSERT(idx != _samplerStateListTable.s_kInvalid, "not found");
    return sCast<u32>(idx);
}


#endif

}