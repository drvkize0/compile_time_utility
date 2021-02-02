#include "size.h"

namespace compile_time_utility
{
    // 0
    struct test0
    {
        static const char* call_with_no_reference() { return "fall back to sizeof(test1)"; }
        static const char* call_with_reference() { return "static_assert"; }
    };

    // 1
    struct test1
    {
        static const char* call_with_no_reference() { return "function specialization exist"; }
        static const char* call_with_reference() { return "static_assert"; }
    };

    template<>
    static constexpr auto size<test1>() { return 1; }

    // 2
    struct test2
    {
        static const char* call_with_no_reference() { return "static_assert"; }
        static const char* call_with_reference() { return "function specialization exist"; }
    };

    template<>
    static auto size( const test2& obj ) { return 2; }

    // 3
    struct test3
    {
        static const char* call_with_no_reference() { return "static_assert"; }
        static const char* call_with_reference() { return "member exist"; }

        std::size_t size() const { return 3; }
    };

    // 4
    struct test4
    {
        static const char* call_with_no_reference() { return "static member with no reference exist"; }
        static const char* call_with_reference() { return "static_assert"; }

        static std::size_t size() { return 4; }
    };

    // 5
    struct test5
    {
        static const char* call_with_no_reference() { return "static_assert"; }
        static const char* call_with_reference() { return "static member with reference exist"; }

        static std::size_t size( const test5& obj ) { return 5; }
    };

    // 6, test two size function
    struct test6
    {
        static const char* call_with_no_reference() { return "static_assert, because size function using reference exist, do not fallback to sizeof()"; }
        static const char* call_with_reference() { return "static member with reference exist"; }

        static constexpr std::size_t size( const test6& obj )
        {
            return 6;
        }
    };

    template<>
    static constexpr auto size<test6>()
    {
        return 6;
    }
}

using namespace compile_time_utility;

int test( int argc, char** argv )
{
    printf( "x1 = %zu\n", size<test1>() );
    printf( "x2 = %zu\n", size( test2() ) );
    printf( "x3 = %zu\n", test3().size() );
    printf( "x4 = %zu\n", test4::size() );
    printf( "x5 = %zu\n", test5::size( test5() ) );

    printf( "\n" );
    printf( "detect x0 = %zu\n", detect_size_function<test0>::value );
    printf( "detect x1 = %zu\n", detect_size_function<test1>::value );
    printf( "detect x2 = %zu\n", detect_size_function<test2>::value );
    printf( "detect x3 = %zu\n", detect_size_function<test3>::value );
    printf( "detect x4 = %zu\n", detect_size_function<test4>::value );
    printf( "detect x5 = %zu\n", detect_size_function<test5>::value );
    printf( "detect x6 = %zu\n", detect_size_function<test6>::value );

    printf( "\ncall with no reference\n" );
    printf( "x0 = %zu, %s\n", size_of<test0>(), test0::call_with_no_reference() );
    printf( "x1 = %zu, %s\n", size_of<test1>(), test1::call_with_no_reference() );
    // printf( "x2 = %zu, %s\n", size_of<test2>(), test2::call_with_no_reference() );
    // printf( "x3 = %zu, %s\n", size_of<test3>(), test3::call_with_no_reference() );
    printf( "x4 = %zu, %s\n", size_of<test4>(), test4::call_with_no_reference() );
    // printf( "x5 = %zu, %s\n", size_of<test5>(), test5::call_with_no_reference() );
    // printf( "x6 = %zu, %s\n", size_of<test6>(), test6::call_with_no_reference() );

    printf( "\ncall with reference\n" );
    // printf( "x0 = %zu, %s\n", size_of( test0() ), test0::call_with_reference() );
    // printf( "x1 = %zu, %s\n", size_of( test1() ), test1::call_with_reference() );
    printf( "x2 = %zu, %s\n", size_of( test2() ), test2::call_with_reference() );
    printf( "x3 = %zu, %s\n", size_of( test3() ), test3::call_with_reference() );
    // printf( "x4 = %zu, %s\n", size_of( test4() ), test4::call_with_reference() );
    printf( "x5 = %zu, %s\n", size_of( test5() ), test5::call_with_reference() );
    printf( "x6 = %zu, %s\n", size_of( test6() ), test6::call_with_reference() );

    return 0;
}