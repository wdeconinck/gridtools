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

#include "../meta/dedup.hpp"
#include "../meta/filter.hpp"
#include "../meta/is_empty.hpp"
#include "../meta/length.hpp"
#include "../meta/logical.hpp"
#include "../meta/macros.hpp"
#include "../meta/transform.hpp"

namespace gridtools {

    namespace _impl {
        template <class Stage>
        using get_extent_from_stage = typename Stage::extent_t;
        template <class Extent>
        struct has_same_extent {
            template <class Stage>
            using apply = std::is_same<Extent, typename Stage::extent_t>;
        };

        template <class AllStages>
        struct stages_with_the_given_extent {
            template <class Extent>
            using apply = meta::filter<has_same_extent<Extent>::template apply, AllStages>;
        };

        namespace lazy {
            template <template <class...> class CompoundStage, class Stages>
            struct fuse_stages_with_the_same_extent;
            template <template <class...> class CompoundStage, template <class...> class L, class... Stages>
            struct fuse_stages_with_the_same_extent<CompoundStage, L<Stages...>> {
                using type = CompoundStage<Stages...>;
            };
            template <template <class...> class CompoundStage, template <class...> class L, class Stage>
            struct fuse_stages_with_the_same_extent<CompoundStage, L<Stage>> {
                using type = Stage;
            };
        } // namespace lazy
        GT_META_DELEGATE_TO_LAZY(fuse_stages_with_the_same_extent,
            (template <class...> class CompoundStage, class Stages),
            (CompoundStage, Stages));

        template <template <class...> class CompoundStage>
        struct fuse_stages_with_the_same_extent_f {
            template <class Stages>
            using apply = fuse_stages_with_the_same_extent<CompoundStage, Stages>;
        };

    } // namespace _impl

    namespace lazy {
        template <template <class...> class CompoundStage, class Stages>
        struct fuse_stages {
            GT_STATIC_ASSERT(meta::length<Stages>::value > 1, GT_INTERNAL_ERROR);
            using all_extents_t = meta::transform<_impl::get_extent_from_stage, Stages>;
            using extents_t = meta::dedup<all_extents_t>;
            GT_STATIC_ASSERT(!meta::is_empty<extents_t>::value, GT_INTERNAL_ERROR);
            using stages_grouped_by_extent_t =
                meta::transform<_impl::stages_with_the_given_extent<Stages>::template apply, extents_t>;
            GT_STATIC_ASSERT((!meta::any_of<meta::is_empty, stages_grouped_by_extent_t>::value), GT_INTERNAL_ERROR);
            using type = meta::transform<_impl::fuse_stages_with_the_same_extent_f<CompoundStage>::template apply,
                stages_grouped_by_extent_t>;
        };

        template <template <class...> class CompoundStage, template <class...> class L, class Stage>
        struct fuse_stages<CompoundStage, L<Stage>> {
            using type = L<Stage>;
        };
        template <template <class...> class CompoundStage, template <class...> class L>
        struct fuse_stages<CompoundStage, L<>> {
            using type = L<>;
        };
    } // namespace lazy
    /**
     *  Group the stages from the input by extent and substitute each group by compound stage.
     */
    GT_META_DELEGATE_TO_LAZY(
        fuse_stages, (template <class...> class CompoundStage, class Stages), (CompoundStage, Stages));

} // namespace gridtools
