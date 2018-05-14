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
#include "iterate_domain.hpp"

namespace gridtools {
    template < typename T >
    struct positional_iterate_domain;

    template < typename T >
    struct is_iterate_domain : boost::mpl::false_ {};

    template < typename IterateDomainArguments >
    struct is_iterate_domain< iterate_domain< IterateDomainArguments > > : boost::mpl::true_ {};

    template < typename IterateDomainArguments >
    struct is_iterate_domain< positional_iterate_domain< IterateDomainArguments > > : boost::mpl::true_ {};

    template < typename T >
    struct is_positional_iterate_domain : boost::mpl::false_ {};

    template < typename IterateDomainArguments >
    struct is_positional_iterate_domain< positional_iterate_domain< IterateDomainArguments > > : boost::mpl::true_ {};

    template < typename T >
    struct iterate_domain_local_domain;

    template < template < template < class > class, typename > class IterateDomainImpl,
        template < class > class IterateDomainBase,
        typename IterateDomainArguments >
    struct iterate_domain_local_domain< IterateDomainImpl< IterateDomainBase, IterateDomainArguments > > {
        GRIDTOOLS_STATIC_ASSERT(
            (is_iterate_domain< IterateDomainImpl< IterateDomainBase, IterateDomainArguments > >::value),
            GT_INTERNAL_ERROR);
        typedef typename IterateDomainArguments::local_domain_t type;
    };
}