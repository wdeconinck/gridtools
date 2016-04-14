#include "gtest/gtest.h"
#include <stencil-composition/stencil-composition.hpp>

namespace test_conditionals{
    using namespace gridtools;


#ifdef CUDA_EXAMPLE
#define BACKEND backend<enumtype::Cuda, enumtype::Block >
#else
#ifdef BACKEND_BLOCK
#define BACKEND backend<enumtype::Host, enumtype::Block >
#else
#define BACKEND backend<enumtype::Host, enumtype::Naive >
#endif
#endif

    typedef gridtools::interval<level<0,-1>, level<1,-1> > x_interval;
    typedef gridtools::interval<level<0,-2>, level<1,1> > axis;

    template<uint_t Id>
    struct functor{

        typedef accessor<0, enumtype::inout> p_dummy;
        typedef boost::mpl::vector1<p_dummy> arg_list;

        template <typename Evaluation>
        GT_FUNCTION
        static void Do(Evaluation const & eval, x_interval) {
            eval(p_dummy())=+Id;
        }
    };

    bool test(){

        conditional<0> cond(false);
        conditional<1> cond2(true);

        grid<axis> grid_({0,0,0,1,2},{0,0,0,1,2});
        grid_.value_list[0] = 0;
        grid_.value_list[1] = 2;

        typedef gridtools::layout_map<2,1,0> layout_t;//stride 1 on i
        typedef BACKEND::storage_info<__COUNTER__, layout_t> meta_data_t;
        typedef BACKEND::storage_type<float_type, meta_data_t >::type storage_t;
        meta_data_t meta_data_(3,3,3);
        storage_t dummy(meta_data_, 0., "dummy");
        typedef arg<0, storage_t > p_dummy;

        typedef boost::mpl::vector1<p_dummy> arg_list;
        domain_type< arg_list > domain_(boost::fusion::make_vector(&dummy));

#ifdef CXX11_ENABLED
        auto
#else
            boost::shared_ptr<computation>
#endif
            comp_ = make_computation < BACKEND > (
                domain_, grid_,
                if_(cond
                    ,
                    make_mss(
                        enumtype::execute<enumtype::forward>()
                        , make_esf<functor<0> >( p_dummy() ))
                    , if_( cond2
                           , make_mss(
                               enumtype::execute<enumtype::forward>()
                               , make_esf<functor<1> >( p_dummy() ))
                           , make_mss(
                               enumtype::execute<enumtype::forward>()
                               , make_esf<functor<2> >( p_dummy() ))
                        )
                    )
                );

        bool result=true;
        comp_->ready();
        comp_->steady();
        comp_->run();
        comp_->finalize();
        result = result && (dummy(0,0,0)==1);
        return result;
    }
}//namespace test_conditional

TEST(stencil_composition, conditionals){
    EXPECT_TRUE(test_conditionals::test());
}
