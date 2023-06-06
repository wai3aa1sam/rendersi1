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

    Stack::iterator         begin();
    Stack::iterator         end();
    Stack::const_iterator   begin() const;
    Stack::const_iterator   end()   const;

    Stack::reverse_iterator         rbegin();
    Stack::reverse_iterator         rend();
    Stack::const_reverse_iterator   rbegin()    const;
    Stack::const_reverse_iterator   rend()      const;

private:
    Stack _stack;
};
#endif


#if 0
#pragma mark --- rdsAppLayerStack-Impl ---
#endif // 0
#if 1

inline AppLayerStack::Stack::iterator                 AppLayerStack::begin()            { return _stack.begin(); }
inline AppLayerStack::Stack::iterator                 AppLayerStack::end  ()            { return _stack.end(); }
inline AppLayerStack::Stack::const_iterator           AppLayerStack::begin() const      { return _stack.cbegin(); }
inline AppLayerStack::Stack::const_iterator           AppLayerStack::end  () const      { return _stack.cend(); }

inline AppLayerStack::Stack::reverse_iterator         AppLayerStack::rbegin   ()        { return _stack.rbegin(); }
inline AppLayerStack::Stack::reverse_iterator         AppLayerStack::rend     ()        { return _stack.rend(); }
inline AppLayerStack::Stack::const_reverse_iterator   AppLayerStack::rbegin   () const  { return _stack.rbegin(); }
inline AppLayerStack::Stack::const_reverse_iterator   AppLayerStack::rend     () const  { return _stack.rend(); }


#endif


}

