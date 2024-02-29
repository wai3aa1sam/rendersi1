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

class BindlessResources : public RenderResource
{
public:
    using Base          = RenderResource;
    using CreateDesc    = BindlessResources_CreateDesc;

public:
    static constexpr SizeType s_kSetBuffer  = 2 + 0;
    static constexpr SizeType s_kSetTexture = 2 + 1;
    static constexpr SizeType s_kSetImage   = 2 + 2;
    static constexpr SizeType s_kSetSampler = 2 + 3;
   
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

    SizeType _size = 0;
};


#endif

}