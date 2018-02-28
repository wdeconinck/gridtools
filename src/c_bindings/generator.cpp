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

#include "c_bindings/generator.hpp"

#include <cassert>

namespace gridtools {
    namespace c_bindings {
        namespace _impl {
            void declarations::add(char const *name, generator_t generator) {
                bool ok = m_generators.emplace(name, std::move(generator)).second;
                assert(ok);
            }

            std::ostream &operator<<(std::ostream &strm, declarations const &declarations) {
                for (auto &&item : declarations.m_generators)
                    item.second(strm, item.first);
                return strm;
            }

            char const c_traits::m_prologue[] = R"?(
struct gt_handle;

#ifdef __cplusplus
extern "C" {
#else
typedef struct gt_handle gt_handle;
#endif

void gt_release(gt_handle*);
)?";
            char const c_traits::m_epilogue[] = R"?(
#ifdef __cplusplus
}
#endif
)?";

            // TODO(anstaf): To figure out if it makes sense to export functions under the different module name.
            char const fortran_traits::m_prologue[] = R"?(
module gt_import
implicit none
  interface

    subroutine gt_release(h) bind(c)
      use iso_c_binding
      type(c_ptr), value :: h
    end
)?";
            char const fortran_traits::m_epilogue[] = R"?(
  end interface
end
)?";
            template <>
            char const fortran_kind_name< bool >::value[] = "c_bool";
            template <>
            char const fortran_kind_name< int >::value[] = "c_int";
            template <>
            char const fortran_kind_name< short >::value[] = "c_short";
            template <>
            char const fortran_kind_name< long >::value[] = "c_long";
            template <>
            char const fortran_kind_name< long long >::value[] = "c_long_long";
            template <>
            char const fortran_kind_name< float >::value[] = "c_float";
            template <>
            char const fortran_kind_name< double >::value[] = "c_double";
            template <>
            char const fortran_kind_name< long double >::value[] = "c_long_double";
            template <>
            char const fortran_kind_name< signed char >::value[] = "c_signed_char";
        }
    }
}