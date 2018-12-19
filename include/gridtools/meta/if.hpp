/*
  GridTools Libraries

  Copyright (c) 2017, ETH Zurich and MeteoSwiss
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  For information: http://eth-cscs.github.io/gridtools/
*/

#pragma once

#include <type_traits>

#include "macros.hpp"

namespace gridtools {
    namespace meta {
        GT_META_LAZY_NAMESPACE {
            /**
             *  Normalized std::conditional version, which is proper function in the terms of meta library.
             *
             *  Note: `std::conditional` should be named `if_c` according to `meta` name convention.
             */
            template <class Cond, class Lhs, class Rhs>
            GT_META_DEFINE_ALIAS(if_, std::conditional, (Cond::value, Lhs, Rhs));

            template <bool Cond, class Lhs, class Rhs>
            GT_META_DEFINE_ALIAS(if_c, std::conditional, (Cond, Lhs, Rhs));
        }
#if !GT_BROKEN_TEMPLATE_ALIASES
        template <class Cond, class Lhs, class Rhs>
        using if_ = typename std::conditional<Cond::value, Lhs, Rhs>::type;

        template <bool Cond, class Lhs, class Rhs>
        using if_c = typename std::conditional<Cond, Lhs, Rhs>::type;
#endif
    } // namespace meta
} // namespace gridtools