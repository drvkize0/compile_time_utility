#pragma once

#include <string_view>

namespace compile_time_utility
{
    template<typename T, std::size_t N>
    class const_array
    {
    public:
        using value_type = T;
        static constexpr std::size_t npos = std::size_t( -1 );
    protected:
        const T _items[N];
    public:
        template<typename... Ts>
        constexpr const_array( const Ts&... values )
            : _items{ static_cast<T>( values )... }
        {
        }
        constexpr std::size_t find( const T& value ) const
        {
            for( std::size_t i = 0; i < N; ++i )
            {
                if( _items[i] == value )
                {
                    return i;
                }
            }
            return npos;
        }
        inline constexpr std::size_t size() const { return N; }
        inline constexpr T operator[]( std::size_t index ) const { return _items[index]; }
    };
    // redirect deduction of string literal to std::string_view
    template<typename... Ts>
    const_array( Ts&&... values ) -> const_array<
        typename std::common_type<
            typename std::conditional<
                std::is_convertible<Ts, std::string_view>::value,
                std::string_view,
                Ts
                >::type...
            >::type,
        sizeof...( Ts )
        >;
}