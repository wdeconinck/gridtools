#pragma once

#include "../iterate_domain.hpp"
#include "../iterate_domain_metafunctions.hpp"
#include "shared_iterate_domain.hpp"
#include "../caches/iterate_domain_cache.hpp"

namespace gridtools {

/**
 * @brief iterate domain class for the CUDA backend
 */
template<template<class> class IterateDomainBase, typename IterateDomainArguments>
class iterate_domain_cuda : public IterateDomainBase<iterate_domain_cuda<IterateDomainBase, IterateDomainArguments> > //CRTP
{
    DISALLOW_COPY_AND_ASSIGN(iterate_domain_cuda);
    GRIDTOOLS_STATIC_ASSERT((is_iterate_domain_arguments<IterateDomainArguments>::value), "Internal error: wrong type");

    typedef IterateDomainBase<iterate_domain_cuda<IterateDomainBase, IterateDomainArguments> > super;
    typedef typename IterateDomainArguments::local_domain_t local_domain_t;
    typedef typename super::data_pointer_array_t data_pointer_array_t;
    typedef typename super::strides_cached_t strides_cached_t;

    typedef shared_iterate_domain<data_pointer_array_t, strides_cached_t> shared_iterate_domain_t;

    typedef iterate_domain_cache<IterateDomainArguments> iterate_domain_cache_t;
private:
    const uint_t m_block_size_i;
    const uint_t m_block_size_j;

    shared_iterate_domain_t* RESTRICT m_pshared_iterate_domain;

public:
    GT_FUNCTION
    explicit iterate_domain_cuda(local_domain_t const& local_domain, const uint_t block_size_i, const uint_t block_size_j)
        : super(local_domain), m_block_size_i(block_size_i), m_block_size_j(block_size_j) {}

    GT_FUNCTION
    uint_t thread_position_x() const
    {
        return threadIdx.x;
    }

    GT_FUNCTION
    uint_t thread_position_y() const
    {
        return threadIdx.y;
    }

    /**
     * @brief determines whether the current (i,j) position is within the block size
     */
    GT_FUNCTION
    bool is_thread_in_domain() const
    {
        return threadIdx.x < m_block_size_i && threadIdx.y < m_block_size_j ;
    }

    /**
     * @brief determines whether the current (i,j) position + an offset is within the block size
     */
    GT_FUNCTION
    bool is_thread_in_domain(const int_t i_offset, const int_t j_offset) const
    {
        return is_thread_in_domain_x(i_offset) &&  is_thread_in_domain_y(j_offset);
    }

    /**
     * @brief determines whether the current (i) position is within the block size
     */
    GT_FUNCTION
    bool is_thread_in_domain_x() const
    {
        return threadIdx.x < m_block_size_i;
    }

    /**
     * @brief determines whether the current (i) position + an offset is within the block size
     */
    GT_FUNCTION
    bool is_thread_in_domain_x(const int_t i_offset) const
    {
        return (int_t)threadIdx.x + i_offset >= 0 && (int_t)threadIdx.x + i_offset < m_block_size_i;
    }

    /**
     * @brief determines whether the current (j) position is within the block size
     */
    GT_FUNCTION
    bool is_thread_in_domain_y(const int_t j_offset) const
    {
        return (int_t)threadIdx.y + j_offset >= 0 && (int_t)threadIdx.y + j_offset < m_block_size_j;
    }

    GT_FUNCTION
    uint block_size_i()
    {
        return m_block_size_i;
    }
    GT_FUNCTION
    uint block_size_j()
    {
        return m_block_size_j;
    }

    GT_FUNCTION
    void set_shared_iterate_domain_pointer_impl(shared_iterate_domain_t* ptr)
    {
        m_pshared_iterate_domain = ptr;
    }

    GT_FUNCTION
    data_pointer_array_t const & RESTRICT data_pointer_impl() const
    {
//        assert(m_pshared_iterate_domain);
        return m_pshared_iterate_domain->data_pointer();
    }

    GT_FUNCTION
    data_pointer_array_t & RESTRICT data_pointer_impl()
    {
//        assert(m_pshared_iterate_domain);
        return m_pshared_iterate_domain->data_pointer();
    }

    GT_FUNCTION
    strides_cached_t const & RESTRICT strides_impl() const
    {
//        assert(m_pshared_iterate_domain);
        return m_pshared_iterate_domain->strides();
    }
    GT_FUNCTION
    strides_cached_t & RESTRICT strides_impl()
    {
//        assert(m_pshared_iterate_domain);
        return m_pshared_iterate_domain->strides();
    }
};

template<
    template<class> class IterateDomainBase, typename IterateDomainArguments>
struct is_iterate_domain<
    iterate_domain_cuda<IterateDomainBase, IterateDomainArguments>
> : public boost::mpl::true_{};

template<
    template<class> class IterateDomainBase,
    typename IterateDomainArguments
>
struct is_positional_iterate_domain<iterate_domain_cuda<IterateDomainBase, IterateDomainArguments> > :
    is_positional_iterate_domain<IterateDomainBase<iterate_domain_cuda<IterateDomainBase, IterateDomainArguments> > > {};

}