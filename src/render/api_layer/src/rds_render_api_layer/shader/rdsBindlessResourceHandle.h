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
    static constexpr u32 s_kInvalid = ~u32(0);

public:

public:
    u32     getResourceIndex() const;
    bool    isValid         () const;

protected:
    u32 _hnd = s_kInvalid;
};


#endif

}