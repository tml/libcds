//$$CDS-header$$

#include "set/hdr_striped_set.h"
#include <cds/container/striped_set/std_hash_set.h>
#include <cds/container/striped_set.h>
#include <cds/lock/spinlock.h>

#if !((CDS_COMPILER == CDS_COMPILER_MSVC || (CDS_COMPILER == CDS_COMPILER_INTEL && CDS_OS_INTERFACE == CDS_OSI_WINDOWS)) && _MSC_VER < 1600)

namespace set {

    namespace {
        struct equal_item
        {
            template <typename T>
            bool operator ()( T const& i1, T const& i2) const
            {
                return i1.key() == i2.key();
            }
        };

        typedef std::unordered_set< StripedSetHdrTest::item, StripedSetHdrTest::hash_int, equal_item > set_t;

        struct my_copy_policy {
            typedef set_t::iterator iterator;

            void operator()( set_t& set, iterator itWhat )
            {
                set.insert( std::make_pair(itWhat->key(), itWhat->val()) );
            }
        };
    }

    void StripedSetHdrTest::Refinable_hashset()
    {
        CPPUNIT_MESSAGE( "cmp");
        typedef cc::StripedSet< set_t
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            ,co::mutex_policy< cc::striped_set::refinable<> >
        >   set_cmp;
        test_striped< set_cmp >();

        CPPUNIT_MESSAGE( "less");
        typedef cc::StripedSet< set_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less<item> >
        >   set_less;
        test_striped< set_less >();

        CPPUNIT_MESSAGE( "cmpmix");
        typedef cc::StripedSet< set_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::less< less<item> >
        >   set_cmpmix;
        test_striped< set_cmpmix >();

        // Spinlock as lock policy
        CPPUNIT_MESSAGE( "spinlock");
        typedef cc::StripedSet< set_t
            ,co::mutex_policy< cc::striped_set::refinable<cds::lock::ReentrantSpin> >
            , co::hash< hash_int >
            , co::less< less<item> >
        >   set_spin;
        test_striped< set_spin >();

        // Resizing policy
        CPPUNIT_MESSAGE( "load_factor_resizing<0>(1024)");
        {
            typedef cc::StripedSet< set_t
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , co::hash< hash_int >
                , co::less< less<item> >
                , co::resizing_policy< cc::striped_set::load_factor_resizing<0> >
            >   set_less_resizing_lf;
            set_less_resizing_lf s(30, cc::striped_set::load_factor_resizing<0>(1024));
            test_striped_with(s);
        }

        CPPUNIT_MESSAGE( "load_factor_resizing<256>");
        typedef cc::StripedSet< set_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
        >   set_less_resizing_lf16;
        test_striped< set_less_resizing_lf16 >();

        CPPUNIT_MESSAGE( "single_bucket_size_threshold<0>(1024)");
        {
            typedef cc::StripedSet< set_t
                ,co::mutex_policy< cc::striped_set::refinable<> >
                , co::hash< hash_int >
                , co::less< less<item> >
                , co::resizing_policy< cc::striped_set::single_bucket_size_threshold<0> >
            >   set_less_resizing_sbt;
            set_less_resizing_sbt s(30, cc::striped_set::single_bucket_size_threshold<0>(1024) );
            test_striped_with(s);
        }

        CPPUNIT_MESSAGE( "single_bucket_size_threshold<256>");
        typedef cc::StripedSet< set_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::less< less<item> >
            , co::resizing_policy< cc::striped_set::single_bucket_size_threshold<256> >
        >   set_less_resizing_sbt16;
        test_striped< set_less_resizing_sbt16 >();

        // Copy policy
        CPPUNIT_MESSAGE( "load_factor_resizing<256>, copy_item");
        typedef cc::StripedSet< set_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< cc::striped_set::copy_item >
        >   set_copy_item;
        test_striped< set_copy_item >();

        CPPUNIT_MESSAGE( "load_factor_resizing<256>, swap_item");
        typedef cc::StripedSet< set_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< cc::striped_set::swap_item >
        >   set_swap_item;
        test_striped< set_swap_item >();

        CPPUNIT_MESSAGE( "load_factor_resizing<256>, move_item");
        typedef cc::StripedSet< set_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< cc::striped_set::move_item >
        >   set_move_item;
        test_striped< set_move_item >();

        CPPUNIT_MESSAGE( "load_factor_resizing<256>, special copy_item");
        typedef cc::StripedSet< set_t
            ,co::mutex_policy< cc::striped_set::refinable<> >
            , co::hash< hash_int >
            , co::compare< cmp<item> >
            , co::resizing_policy< cc::striped_set::load_factor_resizing<256> >
            , co::copy_policy< my_copy_policy >
        >   set_special_copy_item;
        test_striped< set_special_copy_item >();
    }

}   // namespace set
#endif // #if !(CDS_COMPILER == CDS_COMPILER_MSVC && CDS_COMPILER_VERSION < 1600)
