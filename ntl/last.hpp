#pragma once

#include "last.hpp"

namespace ntl {
    template <typename T>
    inline bool last(const T& t)
    {
        return t.last;
    }
}
