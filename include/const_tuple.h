#pragma once

#include "const_pair.h"
#include <utility>

namespace compile_time_utility
{
    namespace detail
    {
        template<std::size_t I, typename T>
        class const_tuple_leaf
        {
        public:
            using value_type = T;
            const value_type _value;
            constexpr const_tuple_leaf( const T& value )
               : _value{ value }
            {
            }
            inline constexpr value_type value() const { return _value; }
        };
        template<typename, typename... Ts>
        struct const_tuple_base;
        template<std::size_t... Is, typename... Ts>
        class const_tuple_base<std::index_sequence<Is...>, Ts...>
            : public const_tuple_leaf<Is, Ts>...
        {
        public:
            constexpr const_tuple_base( const Ts&... values )
                : const_tuple_leaf<Is, Ts>{ values }...
            {
            }
            static constexpr std::size_t SIZE = sizeof...( Is );
            static constexpr const std::size_t size() noexcept { return SIZE; }
        };
        template<typename... Ts>
        const_tuple_base( const Ts&... ) -> const_tuple_base<Ts...>;
        template<>
        class const_tuple_base<std::index_sequence<>>
        {
            static constexpr std::size_t SIZE = 0;
            static constexpr const std::size_t size() noexcept { return SIZE; }
        };
    }
    template<typename... Ts>
    class const_tuple : public detail::const_tuple_base<std::index_sequence_for<Ts...>, typename detail::const_tuple_leaf<0, Ts>::value_type...>
    {
    public:
        using base = detail::const_tuple_base<std::index_sequence_for<Ts...>, typename detail::const_tuple_leaf<0, Ts>::value_type...>;
        using base::SIZE;
        using base::size;
        constexpr const_tuple( const typename detail::const_tuple_leaf<0, Ts>::value_type&... values )
            : base{ values... }
        {
        }
    };
    // redirect deduction of string literal to std::string_view
    template<typename... Ts>
    const_tuple( const Ts&... ) -> const_tuple<typename std::conditional<std::is_convertible<Ts, std::string_view>::value, std::string_view, Ts>::type...>;
    template<std::size_t I, typename U, typename = std::enable_if<I != std::size_t( -1 )>>
    static inline constexpr U get_element( const detail::const_tuple_leaf<I, U>& leaf )
    {
        return leaf.value();
    }
}