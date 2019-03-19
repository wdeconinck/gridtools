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

#include <boost/fusion/include/as_map.hpp>
#include <boost/fusion/include/as_vector.hpp>
#include <boost/fusion/include/pair.hpp>

#include "../common/array.hpp"
#include "../common/defs.hpp"
#include "../meta.hpp"
#include "./arg.hpp"
#include "./extent.hpp"

namespace gridtools {

    namespace _impl {
        namespace local_domain_details {
            template <class Arg>
            GT_META_DEFINE_ALIAS(
                get_data_ptrs_elem, meta::id, (boost::fusion::pair<Arg, typename Arg::data_store_t::data_t *>));

            template <class Arg, class StorageInfo = typename Arg::data_store_t::storage_info_t>
            GT_META_DEFINE_ALIAS(get_storage_info_ptr, meta::id, StorageInfo const *);

            template <class Args>
            GT_META_DEFINE_ALIAS(
                get_storage_info_ptrs, meta::dedup, (GT_META_CALL(meta::transform, (get_storage_info_ptr, Args))));
        } // namespace local_domain_details
    }     // namespace _impl

    /**
     * This class extract the proper iterators/storages from the full domain
     * to adapt it for a particular functor. This version does not provide grid
     * to the function operator
     *
     */
    template <class EsfArgs, class MaxExtentForTmp, bool IsStateful>
    struct local_domain {
        GT_STATIC_ASSERT(is_extent<MaxExtentForTmp>::value, GT_INTERNAL_ERROR);
        GT_STATIC_ASSERT((meta::all_of<is_plh, EsfArgs>::value), GT_INTERNAL_ERROR);

        using type = local_domain;

        using esf_args = EsfArgs;
        using max_extent_for_tmp_t = MaxExtentForTmp;

        using arg_to_data_ptr_map_t = GT_META_CALL(
            meta::transform, (_impl::local_domain_details::get_data_ptrs_elem, EsfArgs));

        using storage_info_ptr_list = GT_META_CALL(_impl::local_domain_details::get_storage_info_ptrs, EsfArgs);

        using tmp_storage_info_ptr_list = GT_META_CALL(
            _impl::local_domain_details::get_storage_info_ptrs, (GT_META_CALL(meta::filter, (is_tmp_arg, EsfArgs))));

        using data_ptr_fusion_map = typename boost::fusion::result_of::as_map<arg_to_data_ptr_map_t>::type;
        using storage_info_ptr_fusion_list = typename boost::fusion::result_of::as_vector<storage_info_ptr_list>::type;
        using size_array = array<uint_t, meta::length<storage_info_ptr_list>::value>;

        data_ptr_fusion_map m_local_data_ptrs;
        storage_info_ptr_fusion_list m_local_storage_info_ptrs;
        size_array m_local_padded_total_lengths;
    };

    template <class>
    struct is_local_domain : std::false_type {};

    template <class EsfArgs, class MaxExtentForTmp, bool IsStateful>
    struct is_local_domain<local_domain<EsfArgs, MaxExtentForTmp, IsStateful>> : std::true_type {};

    template <class>
    struct local_domain_is_stateful;

    template <class EsfArgs, class MaxExtentForTmp, bool IsStateful>
    struct local_domain_is_stateful<local_domain<EsfArgs, MaxExtentForTmp, IsStateful>> : bool_constant<IsStateful> {};
} // namespace gridtools

#ifdef GT_USE_GPU
#include "../common/cuda_util.hpp"

namespace gridtools {
    // Force cloning to cuda device, even though local_domain is not trivially copyable because of boost fusion
    // containers implementation.
    namespace cuda_util {
        template <class EsfArgs, class MaxExtentForTmp, bool IsStateful>
        struct is_cloneable<local_domain<EsfArgs, MaxExtentForTmp, IsStateful>> : std::true_type {};
    } // namespace cuda_util
} // namespace gridtools

#endif