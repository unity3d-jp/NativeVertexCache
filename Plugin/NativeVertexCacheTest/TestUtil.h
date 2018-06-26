#include "Plugin/Pcg.h"

template<typename T>
static void FillRandom(T& vec, uint64_t state = 0x900dbeef, uint64_t inc = 0x87654321) {
    using ValueType = T::value_type;
    Pcg pcg { state, inc };
    for(auto& v : vec) {
        v = static_cast<ValueType>(pcg.getUint32());
    }
}
