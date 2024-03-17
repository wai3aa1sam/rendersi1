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


struct BindlessResources_CreateDesc : public RenderResource_CreateDesc
{
    u32             size    = 512 * 100;
};

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
    static CreateDesc makeCDesc();

public:
    BindlessResources();
    ~BindlessResources();

    void create(const CreateDesc& cDesc);
    void destroy();

    BindlessResourceHandle allocTexture(Texture*            rsc);
    BindlessResourceHandle allocBuffer (RenderGpuBuffer*    rsc);


    void free(BindlessResourceHandle hnd, RenderResourceType type);
    void freeBuffer (RenderGpuBuffer*   rsc);
    void freeTexture(Texture*           rsc);

    void commit();

    u32 findSamplerIndex(const SamplerState& samplerState) const;

public:
    SizeType samplerCount()         const;
    SizeType bindlessTypeCount()    const;

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
        Vector<SPtr<RSC>,   s_kLocalSize> rscs;
        Vector<u32,         s_kLocalSize> freedIdxs;
        u32                               totalRscCount = 0;

    public:
        BindlessResourceHandle alloc(RSC* rsc, BindlessResources* bindlessRscs)
        {
            auto oHnd = BindlessResourceHandle{};

            auto& dst = rscs.emplace_back();
            dst.reset(rsc);

            if (freedIdxs.is_empty())
            {
                oHnd._hnd = totalRscCount;
                totalRscCount++;
            }
            else
            {
                oHnd._hnd = freedIdxs.moveBack();
            }

            throwIf(totalResourceCount() > bindlessRscs->_size, "bindless descriptor count overflow, limit: {}", bindlessRscs->_size);
            return oHnd;
        }

        void free(BindlessResourceHandle hnd)
        {
            freedIdxs.emplace_back(hnd._hnd);
        }

    public:
        u32 totalResourceCount() const { return totalRscCount; }
    };

protected:
    MutexProtected<ResourceAlloc<Texture> >             _texAlloc;
    MutexProtected<ResourceAlloc<RenderGpuBuffer> >     _bufAlloc;

    // VectorTable
    #if 1

    // this could keep the index same
    // template<class T, class VALUE, size_t LOCAL_SIZE = 0, class PRED = Less<typename rds::Hash<T>::Value>, class ALLOC = DefaultAllocator>
    class SamplerStateListTable
    {
    public:
        static constexpr size_t s_kLocalSize = 64;

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


    SamplerStateListTable _samplerStateListTable;

    SizeType _size = 0;
};

inline BindlessResources::SizeType BindlessResources::samplerCount()        const { return _samplerStateListTable.size(); }
inline BindlessResources::SizeType BindlessResources::bindlessTypeCount()   const 
{ 
    bool isSupportAcceralteStruct = false; 
    auto typeCount = enumInt(BindlessResourceType::_kCount); 
    return isSupportAcceralteStruct ? typeCount : typeCount - 1; 
}

inline u32 BindlessResources::findSamplerIndex(const SamplerState& samplerState) const 
{ 
    auto idx = _samplerStateListTable.findIndex(samplerState);
    RDS_CORE_ASSERT(idx != _samplerStateListTable.s_kInvalid, "not found");
    return sCast<u32>(idx);
}


#endif

}