/*
 * GridTools
 *
 * Copyright (c) 2014-2019, ETH Zurich
 * All rights reserved.
 *
 * Please, refer to the LICENSE file in the root directory.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "length.hpp"
#include "macros.hpp"
#include "type_traits.hpp"

namespace gridtools {
    namespace meta {
        template <class T>
        using is_empty = bool_constant<length<T>::value == 0>;
    } // namespace meta
} // namespace gridtools
