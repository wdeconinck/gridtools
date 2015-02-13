#pragma once

#include <gridtools.h>
#include <common/halo_descriptor.h>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <stencil-composition/interval.h>
#include <stencil-composition/make_computation.h>
#include <storage/parallel_storage.h>
#include <storage/partitioner_trivial.h>
#include <storage/io.h>

#ifdef CUDA_EXAMPLE
#include <stencil-composition/backend_cuda.h>
#else
#include <stencil-composition/backend_host.h>
#endif

#ifdef CUDA_EXAMPLE
#include <boundary-conditions/apply_gpu.h>
#else
#include <boundary-conditions/apply.h>
#endif

#define BACKEND_BLOCK 1
/*
  @file
  @brief This file shows an implementation of the "shallow water" stencil using the CXX03 interfaces, with periodic boundary conditions.
  It is an horrible crappy unreadable solution, but it is more portable for the moment
 */

using gridtools::level;
using gridtools::arg_type;
using gridtools::range;
using gridtools::arg;

using gridtools::direction;
using gridtools::sign;
using gridtools::minus_;
using gridtools::zero_;
using gridtools::plus_;

using namespace gridtools;
using namespace enumtype;
using namespace expressions;

namespace shallow_water{
// This is the definition of the special regions in the "vertical" direction
    typedef interval<level<0,-1>, level<1,-1> > x_interval;
    typedef interval<level<0,-2>, level<1,1> > axis;

/**@brief This traits class defined the necessary typesand functions used by all the functors defining the shallow water model*/

    template<uint_t Component=0, uint_t Snapshot=0>
    struct bc_periodic {

        typedef Dimension<5> comp;
	/**@brief space discretization step in direction i */
	GT_FUNCTION
        static float_type dx(){return 1.;}
	/**@brief space discretization step in direction j */
	GT_FUNCTION
        static float_type dy(){return 1.;}

        // periodic boundary conditions in I
        template <sign I, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<I, minus_, K, typename boost::enable_if_c<I!=minus_>::type>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<Component, Snapshot>()[data_field0._index(i,j,k)] = data_field0.template get<Component, Snapshot>()[data_field0._index(i,data_field0.template dims<1>()-1-j,k)];
        }

        // periodic boundary conditions in J
        template <sign J, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<minus_, J, K>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<Component, Snapshot>()[data_field0._index(i,j,k)] = data_field0.template get<Component, Snapshot>()[data_field0._index(data_field0.template dims<0>()-1-i,j,k)];
        }

	// default: do nothing
        template <sign I, sign J, sign K, typename P, typename DataField0>
        GT_FUNCTION
        void operator()(direction<I, J, K, P>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
        }

#define height 2.
	GT_FUNCTION
    	static float_type droplet(uint_t const& i, uint_t const& j, uint_t const& k){
            if(i>1 && j>1 && i<4 && j<4)
                return 1.+2. * std::exp(-5*(((i-2)*dx())*(((i-2)*dx()))+((j-2)*dy())*((j-2)*dy())));
            else
                return 1.;
       }

};


    template<uint_t Component, uint_t Snapshot=0>
    struct bc_reflective;

    template<uint_t Snapshot>
    struct bc_reflective<0, Snapshot> {

        template <sign I, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<I, minus_, K>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<0, Snapshot>()[data_field0._index(i,j,k)] = data_field0.template get<0, Snapshot>()[data_field0._index(i,j+1,k)];
        }

        // periodic boundary conditions in I
        template <sign I, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<I, plus_, K, typename boost::enable_if_c<I!=minus_>::type>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<0, Snapshot>()[data_field0._index(i,j,k)] = data_field0.template get<0, Snapshot>()[data_field0._index(i,j-1,k)];
        }

        // periodic boundary conditions in J (I=0) for H
        template <sign J, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<minus_, J, K, typename boost::enable_if_c<J!=plus_&&J!=minus_>::type>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<0, Snapshot>()[data_field0._index(i,j,k)] = data_field0.template get<0, Snapshot>()[data_field0._index(i+1,j,k)];
        }

                // periodic boundary conditions in J (I=0) for H
        template <sign J, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<plus_, J, K, typename boost::enable_if_c<J!=minus_&&J!=plus_>::type>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<0, Snapshot>()[data_field0._index(i,j,k)] = data_field0.template get<0, Snapshot>()[data_field0._index(i-1,j,k)];
        }

	// default: do nothing
        template <sign I, sign J, sign K, typename P, typename DataField0>
        GT_FUNCTION
        void operator()(direction<I, J, K, P>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
        }

};

    template<uint_t Snapshot>
    struct bc_reflective<1, Snapshot> {

        // periodic boundary conditions in I (J=0)
        template <sign I, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<I, minus_, K>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<1, Snapshot>()[data_field0._index(i,j,k)] = data_field0.template get<1, Snapshot>()[data_field0._index(i,j+1,k)];
        }

        // periodic boundary conditions in I (J=N)
        template <sign I, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<I, plus_, K, typename boost::enable_if_c<I!=minus_>::type>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<1, Snapshot>()[data_field0._index(i,j,k)] = data_field0.template get<1, Snapshot>()[data_field0._index(i,j-1,k)];
        }

        // periodic boundary conditions in J (I=0) for H
        template <sign J, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<minus_, J, K, typename boost::enable_if_c<J!=minus_&&J!=plus_>::type>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<1, Snapshot>()[data_field0._index(i,j,k)] = -data_field0.template get<1, Snapshot>()[data_field0._index(i+1,j,k)];
        }

        // periodic boundary conditions in J (I=0) for H
        template <sign J, sign K, typename DataField0, typename boost::enable_if_c<J!=minus_&&J!=plus_>::type>
        GT_FUNCTION
        void operator()(direction<plus_, J, K, typename boost::enable_if_c<J!=minus_&&J!=plus_>::type>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<1, Snapshot>()[data_field0._index(i,j,k)] = -data_field0.template get<1, Snapshot>()[data_field0._index(i-1,j,k)];
        }

	// default: do nothing
        template <sign I, sign J, sign K, typename P, typename DataField0>
        GT_FUNCTION
        void operator()(direction<I, J, K, P>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
        }
};

    template<uint_t Snapshot>
    struct bc_reflective<2, Snapshot> {

        // periodic boundary conditions in I
        template <sign I, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<I, minus_, K>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<2, Snapshot>()[data_field0._index(i,j,k)] = -data_field0.template get<2, Snapshot>()[data_field0._index(i,j+1,k)];
        }

        // periodic boundary conditions in I
        template <sign I, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<I, plus_, K, typename boost::enable_if_c<I!=minus_>::type>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<2, Snapshot>()[data_field0._index(i,j,k)] = -data_field0.template get<2, Snapshot>()[data_field0._index(i,j-1,k)];
        }

        // periodic boundary conditions in J (I=0) for H
        template <sign J, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<minus_, J, K, typename boost::enable_if_c<J!=minus_&&J!=plus_>::type>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<2, Snapshot>()[data_field0._index(i,j,k)] = data_field0.template get<2, Snapshot>()[data_field0._index(i+1,j,k)];
        }

        // periodic boundary conditions in J (I=0) for H
        template <sign J, sign K, typename DataField0>
        GT_FUNCTION
        void operator()(direction<plus_, J, K, typename boost::enable_if_c<J!=minus_&&J!=plus_>::type>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
	    data_field0.template get<2, Snapshot>()[data_field0._index(i,j,k)] = data_field0.template get<2, Snapshot>()[data_field0._index(i-1,j,k)];
        }

	// default: do nothing
        template <sign I, sign J, sign K, typename P, typename DataField0>
        GT_FUNCTION
        void operator()(direction<I, J, K, P>,
                        DataField0 & data_field0,
                        uint_t i, uint_t j, uint_t k) const {
        }
};

// These are the stencil operators that compose the multistage stencil in this test
    struct first_step_x {

        typedef Dimension<5> comp;
	/**@brief space discretization step in direction i */
	GT_FUNCTION
        static float_type dx(){return 1.;}
	/**@brief space discretization step in direction j */
	GT_FUNCTION
        static float_type dy(){return 1.;}
	/**@brief time discretization step */
	GT_FUNCTION
        static float_type dt(){return .02;}
	/**@brief gravity acceleration */
	GT_FUNCTION
        static float_type g(){return 9.81;}

        static x::Index i;
        static y::Index j;

        typedef arg_type<0, range<0, 0, 0, 0>, 5>::type tmpx;
        typedef arg_type<1, range<0, 0, 0, 0>, 5>::type sol;
        using arg_list=boost::mpl::vector<tmpx, sol> ;

        template <typename Evaluation>
        GT_FUNCTION
        static void Do(Evaluation const & eval, x_interval) {

            // eval(tmpx(comp(1), x(-1), y(-2)))=eval(sol(comp(+5), x(+6), y(+7)))+3;
            eval(tmpx())= (eval(sol(comp(0),i+1,j+1)) +eval(sol(comp(0),j+1)))/2. -
                (eval(sol(comp(1),i+1,j+1)) - eval(sol(comp(1),j+1)))*(dt()/(2*dx()));

            eval(tmpx(comp(1)))=eval(sol(comp(1),i+1, j+1)) +
                eval(sol(comp(1),j+1))/2.-
                (((eval(sol(comp(1),i+1,j+1))*eval(sol(comp(1),i+1,j+1)))/eval(sol(comp(0),i+1,j+1))+(eval(sol(comp(0),i+1,j+1))*eval(sol(comp(0),i+1,j+1)))*g()/2.)  -
                 (eval(sol(comp(1),j+1))*eval(sol(comp(1),j+1))/eval(sol(comp(0),j+1)) +
                  eval(sol(comp(0),j+1))*eval(sol(comp(0),j+1))*(g()/2.)
                     ))*(dt()/(2.*dx()));

            eval(tmpx(comp(2)))= (eval(sol(comp(2),i+1,j+1)) +
                         eval(sol(comp(2),j+1)))/2. -
                (eval(sol(comp(1),i+1,j+1))*eval(sol(comp(2),i+1,j+1))/eval(sol(comp(0),i+1,j+1)) -
                 eval(sol(comp(1),j+1))*eval(sol(comp(2),j+1))/eval(sol(comp(0),j+1)))*(dt()/(2*dx())) ;
        }
    };
    x::Index first_step_x::i;
    y::Index first_step_x::j;

    struct second_step_y {

        typedef Dimension<5> comp;
	/**@brief space discretization step in direction i */
	GT_FUNCTION
        static float_type dx(){return 1.;}
	/**@brief space discretization step in direction j */
	GT_FUNCTION
        static float_type dy(){return 1.;}
	/**@brief time discretization step */
	GT_FUNCTION
        static float_type dt(){return .02;}
	/**@brief gravity acceleration */
	GT_FUNCTION
        static float_type g(){return 9.81;}

        static x::Index i;
        static y::Index j;

        //using xrange=range<0,-1,0,0>;
        typedef arg_type<0,range<0, 0, 0, 0>, 5>::type tmpy;
        typedef arg_type<1,range<0, 0, 0, 0>, 5>::type sol;
        using arg_list=boost::mpl::vector<tmpy, sol> ;

        template <typename Evaluation>
        GT_FUNCTION
        static void Do(Evaluation const & eval, x_interval) {

        eval(tmpy(comp(0)))= (eval(sol(comp(0),i+1,j+1)) + eval(sol(comp(0),i+1)))/2. -
            (eval(sol(comp(2),i+1,j+1)) - eval(sol(comp(2),i+1)))*(dt()/(2*dy()));

        eval(tmpy(comp(1)))=(eval(sol(comp(1),i+1,j+1)) +
                    eval(sol(comp(1),i+1)))/2. -
            (eval(sol(comp(2),i+1,j+1))*eval(sol(comp(1),i+1,j+1))/eval(sol(comp(0),i+1,j+1)) -
             eval(sol(comp(2),i+1))*eval(sol(comp(1),i+1))/eval(sol(comp(0),i+1)))*(dt()/(2*dy()));

        eval(tmpy(comp(2)))=eval(sol(comp(2),i+1, j+1)) +
            eval(sol(comp(2),i+1))/2.-
            (((eval(sol(comp(2),i+1,j+1))*eval(sol(comp(2),i+1,j+1)))/eval(sol(comp(0),i+1,j+1))+(eval(sol(comp(0),i+1,j+1))*eval(sol(comp(0),i+1,j+1)))*g()/2.)  -
             (eval(sol(comp(2),i+1))*eval(sol(comp(2),i+1))/eval(sol(comp(0),i+1)) +
              (eval(sol(comp(0),i+1))*eval(sol(comp(0),i+1)))*(g()/2.)
                 ))*(dt()/(2.*dy()));
        }
    };
    x::Index second_step_y::i;
    y::Index second_step_y::j;

    struct final_step {

        typedef Dimension<5> comp;
	/**@brief space discretization step in direction i */
	GT_FUNCTION
        static float_type dx(){return 1.;}
	/**@brief space discretization step in direction j */
	GT_FUNCTION
        static float_type dy(){return 1.;}
	/**@brief time discretization step */
	GT_FUNCTION
        static float_type dt(){return .02;}
	/**@brief gravity acceleration */
	GT_FUNCTION
        static float_type g(){return 9.81;}

        static x::Index i;
        static y::Index j;

        //using xrange=range<0,-1,0,0>;
        typedef arg_type<0,range<0, 0, 0, 0>, 5>::type tmpx;
        typedef arg_type<1,range<0, 0, 0, 0>, 5>::type tmpy;
        typedef arg_type<2,range<-1, 1, -1, 1>, 5>::type sol;
        using arg_list=boost::mpl::vector<tmpx, tmpy, sol> ;
        static uint_t current_time;

        //########## FINAL STEP #############
        //data dependencies with the previous parts
        //notation: alias<tmp, comp, step>(0, 0) is ==> tmp(comp(0), step(0)).
        //Using a strategy to define some arguments beforehand

        template <typename Evaluation>
        GT_FUNCTION
        static void Do(Evaluation const & eval, x_interval) {

            eval(sol()) = eval(sol())-
                (eval(tmpx(comp(1),j-1)) - eval(tmpx(comp(1),i-1, j-1)))*(dt()/dx())
                -
                (eval(tmpy(comp(2),i-1)) - eval(tmpy(comp(2),i-1, j-1)))*(dt()/dy())
                ;

            eval(sol(comp(1))) =  eval(sol(comp(1))) -
                ((eval(tmpx(comp(1),j-1))*eval(tmpx(comp(1),j-1)))                / eval(tmpx(comp(0),j-1))      + eval(tmpx(comp(0),j-1))*eval(tmpx(comp(0),j-1))*((g()/2.))                 -
                 ((eval(tmpx(comp(1),i-1,j-1))*eval(tmpx(comp(1),i-1,j-1)))            / eval(tmpx(comp(0),i-1, j-1)) +(eval(tmpx(comp(0),i-1,j-1))*eval(tmpx(comp(0),i-1,j-1)) )*((g()/2.))))*((dt()/dx())) -
                (eval(tmpy(comp(2),i-1))*eval(tmpy(comp(1),i-1))          / eval(tmpy(comp(0),i-1))                                                   -
                 eval(tmpy(comp(2),i-1, j-1))*eval(tmpy(comp(1),i-1,j-1)) / eval(tmpy(comp(0),i-1, j-1))) *(dt()/dy());

            eval(sol(comp(2))) = eval(sol(comp(2))) -
                (eval(tmpx(comp(1),j-1))    *eval(tmpx(comp(2),j-1))       /eval(tmpx(comp(0),j-1)) -
                 (eval(tmpx(comp(1),i-1,j-1))*eval(tmpx(comp(2),i-1, j-1))) /eval(tmpx(comp(0),i-1, j-1)))*((dt()/dx()))-
                ((eval(tmpy(comp(2),i-1))*eval(tmpy(comp(2),i-1)))                /eval(tmpy(comp(0),i-1))      +(eval(tmpy(comp(0),i-1))*eval(tmpy(comp(0),i-1))     )*((g()/2.)) -
                 ((eval(tmpy(comp(2),i-1, j-1))*eval(tmpy(comp(2),i-1, j-1)))           /eval(tmpy(comp(0),i-1, j-1)) +(eval(tmpy(comp(0),i-1, j-1))*eval(tmpy(comp(0),i-1, j-1)))*((g()/2.))   ))*((dt()/dy()));
        }

    };
    x::Index final_step::i;
    y::Index final_step::j;


    uint_t final_step::current_time=0;

/*
 * The following operators and structs are for debugging only
 */
    std::ostream& operator<<(std::ostream& s, first_step_x const) {
        return s << "initial step 1: ";
	// initiali_step.to_string();
    }

    std::ostream& operator<<(std::ostream& s, second_step_y const) {
        return s << "initial step 2: ";
    }

/*
 * The following operators and structs are for debugging only
 */
    std::ostream& operator<<(std::ostream& s, final_step const) {
        return s << "final step";
    }

    bool test(uint_t x, uint_t y, uint_t z) {

        uint_t d1 = x;
        uint_t d2 = y;
        uint_t d3 = z;

#ifdef CUDA_EXAMPLE
#define BACKEND backend<Cuda, Naive >
#else
#ifdef BACKEND_BLOCK
#define BACKEND backend<Host, Block >
#else
#define BACKEND backend<Host, Naive >
#endif
#endif
        //                      dims  z y x
        //                   strides xy x 1
        typedef layout_map<2,1,0> layout_t;
        typedef gridtools::BACKEND::storage_type<float_type, layout_t >::type storage_type;
        typedef gridtools::BACKEND::temporary_storage_type<float_type, layout_t >::type tmp_storage_type;

        /* The nice interface does not compile today (CUDA 6.5) with nvcc (C++11 support not complete yet)*/
        typedef field<storage_type, 1, 1, 1>::type sol_type;
        typedef field<tmp_storage_type, 1, 1, 1>::type tmp_type;
        typedef sol_type::original_storage::pointer_type ptr;

        // Definition of placeholders. The order of them reflects the order the user will deal with them
        // especially the non-temporary ones, in the construction of the domain
        typedef arg<0, tmp_type > p_tmpx;
        typedef arg<1, tmp_type > p_tmpy;
        typedef arg<2, sol_type > p_sol;
        typedef boost::mpl::vector<p_tmpx, p_tmpy, p_sol> arg_type_list;

        MPI_3D_process_grid_t<gridtools::boollist<3> > comm(gridtools::boollist<3>(true,true,true), GCL_WORLD);
        ushort_t halo[3]={1,1,1};
        typedef partitioner_trivial<sol_type> partitioner_t;
        typedef sol_type::original_storage::pointer_type pointer_type;
        partitioner_t part(comm.ntasks(), comm.coordinates(), comm.dimensions(), halo);
        parallel_storage<partitioner_t> sol(part, d1, d2, d3);

        typedef gridtools::halo_exchange_dynamic_ut<gridtools::layout_map<0, 1, 2>,
                                                    gridtools::layout_map<0, 1, 2>,
                                                    pointer_type::pointee_t, 3,
                                                    gridtools::gcl_cpu,
                                                    gridtools::version_manual> pattern_type;

        pattern_type he(pattern_type::grid_type::period_type(true, true, true), comm.communicator());

        he.add_halo<0>(part.template get_halo_descriptor<0>());
        he.add_halo<1>(part.template get_halo_descriptor<1>());
        he.add_halo<2>(0, 0, 0, d3 - 1, d3);

        he.setup(3);

        ptr out7(sol.size()), out8(sol.size()), out9(sol.size());
        if(!comm.pid())
            sol.set<0>(out7, &bc_periodic<0,0>::droplet);//h
        else
            sol.set<0>(out7, 1.);//h
        sol.set<1>(out8, 0.);//u
        sol.set<2>(out9, 0.);//v

        std::cout<<"INITIALIZED VALUES"<<std::endl;
        sol.print();
        std::cout<<"#####################################################"<<std::endl;

        // construction of the domain. The domain is the physical domain of the problem, with all the physical fields that are used, temporary and not
        // It must be noted that the only fields to be passed to the constructor are the non-temporary.
        // The order in which they have to be passed is the order in which they appear scanning the placeholders in order. (I don't particularly like this)
        domain_type<arg_type_list> domain
            (boost::fusion::make_vector(&sol));

        // Definition of the physical dimensions of the problem.
        // The constructor takes the horizontal plane dimensions,
        // while the vertical ones are set according the the axis property soon after
        // coordinates<axis> coords(2,d1-2,2,d2-2);
        uint_t di2[5] = {1, 0, 1, d1-4, d1};
        uint_t dj2[5] = {1, 0, 1, d2-4, d2};
        coordinates<axis> coords(di2, dj2);
        coords.value_list[0] = 0;
        coords.value_list[1] = d3-1;

        auto shallow_water_stencil =
            make_computation<gridtools::BACKEND, layout_t>
            (
                make_mss // mss_descriptor
                (
                    execute<forward>(),
                    make_independent(
                        make_esf<first_step_x> (p_tmpx(), p_sol() ),
                        make_esf<second_step_y>(p_tmpy(), p_sol() )),
                    make_esf<final_step>(p_tmpx(), p_tmpy(), p_sol() )
                    ),
                domain, coords
                );

        shallow_water_stencil->ready();

        shallow_water_stencil->steady();

        array<halo_descriptor, 3> halos;
        halos[0] = halo_descriptor(1,0,1,d1-1,d1);
        halos[1] = halo_descriptor(1,0,1,d2-1,d2);
        halos[2] = halo_descriptor(0,0,1,d3-1,d3);

        //the following might be runtime value
        uint_t total_time=1;

        for (;final_step::current_time < total_time; ++final_step::current_time)
        {
#ifdef CUDA_EXAMPLE
            /*                        component,snapshot */
            boundary_apply_gpu< bc_reflective<0,0> >(halos, bc_reflective<0,0>()).apply(sol);
            boundary_apply_gpu< bc_reflective<1,0> >(halos, bc_reflective<1,0>()).apply(sol);
            boundary_apply_gpu< bc_reflective<2,0> >(halos, bc_reflective<2,0>()).apply(sol);
#else
            /*                    component,snapshot */
            boundary_apply< bc_reflective<0,0> >(halos, bc_reflective<0,0>()).apply(sol);
            boundary_apply< bc_reflective<1,0> >(halos, bc_reflective<1,0>()).apply(sol);
            boundary_apply< bc_reflective<2,0> >(halos, bc_reflective<2,0>()).apply(sol);
#endif
            shallow_water_stencil->run();

            std::vector<pointer_type::pointee_t*> vec(3);
            vec[0]=sol.fields()[0].get();
            vec[1]=sol.fields()[1].get();
            vec[2]=sol.fields()[2].get();

            he.pack(vec);
            he.exchange();
            he.unpack(vec);

            sol.print();
        }

        shallow_water_stencil->finalize();

        // hdf5_driver<decltype(sol)> out("out.h5", "h", sol);
        // out.write(sol.get<0,0>());

        he.wait();

        GCL_Finalize();

        return true;

    }

}//namespace shallow_water