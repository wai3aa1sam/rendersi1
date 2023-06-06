#pragma once

#include "rds_core/common/rds_core_common.h"

// references:
// - https://github.com/TheCherno/Hazel

namespace rds
{
#if 0
#pragma mark --- rdsAppLayerStack-Decl ---
#endif // 0
#if 1

class AppLayer;

class AppLayerStack : public NonCopyable
{
public:
    using Stack = Vector<UPtr<AppLayer>, 8>;

public:
    
    ~AppLayerStack();

    void pushLayer(UPtr<AppLayer> p);
    void popLayer();

    Stack::iterator         begin   ();
    Stack::iterator         end     ();
    Stack::const_iterator   begin   () const;
    Stack::const_iterator   end     () const;

    Stack::reverse_iterator         rbegin  ();
    Stack::reverse_iterator         rend    ();
    Stack::const_reverse_iterator   rbegin  () const;
    Stack::const_reverse_iterator   rend    () const;

private:
    Stack _stack;
}
#endif


#if 0
#pragma mark --- rdsAppLayerStack-Impl ---
#endif // 0
#if 1

inline AppLayerStack::Stack::iterator                 begin()       { return m_layerStack.begin(); }
inline AppLayerStack::Stack::iterator                 end  ()       { return m_layerStack.end(); }
inline AppLayerStack::Stack::const_iterator           begin() const { return m_layerStack.cbegin(); }
inline AppLayerStack::Stack::const_iterator           end  () const { return m_layerStack.cend(); }

inline AppLayerStack::Stack::reverse_iterator         rbegin   ()       { return m_layerStack.rbegin(); }
inline AppLayerStack::Stack::reverse_iterator         rend     ()       { return m_layerStack.rend(); }
inline AppLayerStack::Stack::const_reverse_iterator   rbegin   () const { return m_layerStack.rbegin(); }
inline AppLayerStack::Stack::const_reverse_iterator   rend     () const { return m_layerStack.rend(); }


#endif


}

