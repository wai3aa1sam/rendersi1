#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

class BindlessResources;

#if 0
#pragma mark --- rdsBindlessResourceHandle-Decl ---
#endif // 0
#if 1

struct BindlessResourceHandle
{
    friend class BindlessResources;
    RDS_RENDER_API_LAYER_COMMON_BODY();
public:
    using IndexT = u32;

public:
    static constexpr IndexT s_kInvalid = ~IndexT(0);

public:

public:
    IndexT  getResourceIndex()              const;
    IndexT  getResourceIndex(u32 mipLevel)  const;
    bool    isValid         () const;

    u32     subResouceCount () const;

protected:
    void setValue(u32 v);
    void setSubResourceCount(u32 v);

    u32 getValue() const;

protected:
    u32 _value          = s_kInvalid;
    u8  _subRscCount    = sCast<u8>(1);
};


#endif

}