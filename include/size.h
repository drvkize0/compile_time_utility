// A size function detector and a universal sizer to determine object runtime size
//
// for profiling objects memory usage that implemented following required size function
//
// all acceptable size function/specialization
// 1.
// template<>
// constexpr auto size<CustomType>() { return size; }
// 2.
// template<>
// static constexpr std::size_t CustomType::size() { return size; }
// 3.
// std::size_t CustomType::size() const { return size; }
// 4.
// auto size( const CustomType& obj ) { return size; }
// 5.
// static CustomType::size( const CustomType& obj ) { return size; }
//
// 1 and 4 need to be defined under Nickel::Platform
// when above function are defined, size can be retrieved by using
// for unknown reference
//
// sizer function ( universal sizer )
// 6.
// size_of<CustomType>()
// for known reference
//
// 7.
// size_of( const CustomType& obj )
// compile error will be reported if the function is not defined with required signature

namespace compile_time_utility
{
    template<typename T>
    static constexpr auto size();

    template<typename T>
    static auto size( const T& );
}

namespace compile_time_utility
{
    namespace
    {
        // if T has multiple size functions, size_of will call in the following orders
        enum SizeFunctionType : std::uint32_t
        {
            None = 0,
            FunctionWithNoReference = 1,
            Function = 1 << 1,
            Member = 1 << 2,
            StaticMemberWithNoReference = 1 << 3,
            StaticMember = 1 << 4,
            Last = 1 << 5
        };

        template<typename T, typename... Ts>
        struct identity { using type = T; };
        template<typename T, typename... Ts>
        using identity_t = typename identity<T, Ts...>::type;

        template<typename T>
        struct detect_size_function
        {
        private:
            template<SizeFunctionType Value>
            struct SizeTypeValue : std::integral_constant<SizeFunctionType, Value> {};

            // test size<U>()
            template<typename U>
            static auto test_function_no_reference( std::int8_t ) -> identity_t<
                SizeTypeValue<SizeFunctionType::FunctionWithNoReference>,
                std::enable_if_t<std::is_convertible<decltype( size<U>() ), std::size_t>::value>,
                decltype( static_cast<decltype( size<U>() )(*)()>( &size<U> ) )
            >;
            template<typename U>
            static SizeTypeValue<SizeFunctionType::None> test_function_no_reference( ... );

            // test size( u )
            template<typename U>
            static auto test_function( std::int64_t ) -> identity_t<
                SizeTypeValue<SizeFunctionType::Function>,
                std::enable_if_t<std::is_convertible<decltype( size( std::declval<const U&>() ) ), std::size_t>::value>,
                decltype( static_cast<decltype( size( std::declval<const U&>() ) )(*)(const U&)>( &size<U> ) )
            >;
            template<typename U>
            static SizeTypeValue<SizeFunctionType::None> test_function( ... );

            // test u.size()
            template<typename U>
            static auto test_member( std::int32_t ) -> identity_t<
                SizeTypeValue<SizeFunctionType::Member>,
                decltype( static_cast<std::size_t(U::*)() const>( &U::size ) ),
                decltype( std::declval<U>().size() )
            >;
            template<typename U>
            static SizeTypeValue<SizeFunctionType::None> test_member( ... );

            // test U::size()
            template<typename U>
            static auto test_static_member_no_reference( std::int16_t ) -> identity_t<
                SizeTypeValue<SizeFunctionType::StaticMemberWithNoReference>,
                decltype( static_cast<std::size_t(*)()>( &U::size ) ),
                decltype( U::size() )
            >;
            template<typename U>
            static SizeTypeValue<SizeFunctionType::None> test_static_member_no_reference( ... );

            // test U::size( u )
            template<typename U>
            static auto test_static_member( float ) -> identity_t<
                SizeTypeValue<SizeFunctionType::StaticMember>,
                decltype( static_cast<std::size_t(*)(const U&)>( &U::size ) ),
                decltype( U::size( std::declval<const U&>() ) )
            >;
            template<typename U>
            static SizeTypeValue<SizeFunctionType::None> test_static_member( ... );
            
            using function_no_reference_t = decltype( test_function_no_reference<T>( 0 ) );
            using function_t = decltype( test_function<T>( 0 ) );
            using member_t = decltype( test_member<T>( 0 ) );
            using static_member_no_reference_t = decltype( test_static_member_no_reference<T>( 0 ) );
            using static_member_t = decltype( test_static_member<T>( 0 ) );

        public:
            static constexpr std::uint32_t value =
                function_no_reference_t::value
                | function_t::value
                | member_t::value
                | static_member_no_reference_t::value
                | static_member_t::value;
        };

        // require with reference
        constexpr bool size_function_require_reference( std::uint32_t x )
        {
            switch( static_cast<SizeFunctionType>( x ) )
            {
                case SizeFunctionType::Function:
                case SizeFunctionType::Member:
                case SizeFunctionType::StaticMember:
                    return true;
            }
            return false;
        }

        // next_index
        // get next function mask need to test
        template<std::uint32_t Mask, bool WithReference, std::uint32_t I = 1>
        struct next_index
        {
            static constexpr bool ExistSizeFunctionWithReference = ( ( Mask & ( SizeFunctionType::Function | SizeFunctionType::Member | SizeFunctionType::StaticMember ) ) != 0 );
            static_assert( !( ExistSizeFunctionWithReference && !WithReference ), "size function with reference exist, use size_of( const T& ) instead" );

            static constexpr bool use_this_index = ( ( Mask & I ) != 0 ) && ( WithReference == size_function_require_reference( I ) );
            static constexpr std::uint32_t value = use_this_index ? I : next_index<Mask, WithReference, I << 1>::value;
        };
        template<std::uint32_t Mask, bool WithReference>
        struct next_index<Mask, WithReference, (std::uint32_t)SizeFunctionType::Last>
        {
            static constexpr std::uint32_t value = 0;
        };

        // implement function call
        template<typename T>
        struct size_of_impl
        {
        private:
            static constexpr std::uint32_t FunctionMask = detect_size_function<T>::value;

            template<std::uint32_t Mask>
            static constexpr std::size_t size_impl()
            {
                // fall back to sizeof( T ) if no no-reference size funciton is defined
                return sizeof( T );
            }
            template<>
            static constexpr std::size_t size_impl<(std::uint32_t)SizeFunctionType::FunctionWithNoReference>() { return size<T>(); }
            template<>
            static constexpr std::size_t size_impl<(std::uint32_t)SizeFunctionType::StaticMemberWithNoReference>() { return T::size(); }
            template<std::uint32_t Mask>
            static std::size_t size_impl( const T& obj )
            {
                static_assert( 0, "T missing size function with reference" );
            }
            template<>
            static std::size_t size_impl<(std::uint32_t)SizeFunctionType::Function>( const T& obj ) { return size( obj ); }
            template<>
            static std::size_t size_impl<(std::uint32_t)SizeFunctionType::Member>( const T& obj ) { return obj.size(); }
            template<>
            static std::size_t size_impl<(std::uint32_t)SizeFunctionType::StaticMember>( const T& obj ) { return T::size( obj ); }
            static constexpr std::size_t try_size_function()
            {
                // try next no reference size function
                return size_impl<next_index<FunctionMask, false>::value>();
            }
            static std::size_t try_size_function( const T& obj )
            {
                // try next size function
                return size_impl<next_index<FunctionMask, true>::value>( obj );
            }  
        public:
            static constexpr std::size_t size_of()
            {
                return try_size_function();
            }
            static std::size_t size_of( const T& obj )
            {
                return try_size_function( obj );
            }
        };
    }

    template<typename T>
    constexpr static std::size_t size_of()
    {
        return size_of_impl<T>::size_of();
    }

    template<typename T>
    static std::size_t size_of( const T& obj )
    {
        return size_of_impl<T>::size_of( obj );
    }
}