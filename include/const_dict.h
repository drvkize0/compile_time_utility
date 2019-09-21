#pragma once

#include "const_tuple.h"
#include "const_pair.h"

namespace compile_time_utility
{
    template<typename Key, typename... Ts>
    class const_dict : public const_tuple<const_pair<Key, Ts>...>
    {
    public:
        using base = const_tuple<const_pair<Key, Ts>...>;

        using key_type = Key;

        using array_type = const_array<key_type, base::SIZE>;
        const array_type _index;

        template<std::size_t I>
        using value_type = const_pair<Key, decltype( std::get<I>( std::declval<std::tuple<Ts...>>() ) )>;

    public:
        constexpr const_dict( const const_pair<Key, Ts>&... values )
            : base{ values... }
            , _index{ values.key()... }
        {
        }
        constexpr const_dict( const const_dict& other )
            : base{ other }
            , _index{ other._index }
        {
        }

        constexpr std::size_t find( const key_type& key ) const
        {
            return _index.find( key ); 
        }
    };

    // redirect deduction of string literal to std::string_view
    template<typename... Ks, typename... Vs>
    const_dict( const const_pair<Ks, Vs>&... ) -> const_dict<typename std::common_type<typename const_pair<Ks, Vs>::Key...>::type, Vs...>;

    // find
    namespace detail
    {
        template<std::size_t I, std::size_t N>
        struct find_impl
        {
            template<typename Key, typename... Ts>
            static constexpr std::size_t find( const const_dict<Key, Ts...>& dict, const typename const_dict<Key, Ts...>::key_type& key )
            {
                return get_element<I>( dict ).key() == key ? I : find_impl<I + 1, N>::find( dict, key );
            }
        };

        template<std::size_t N>
        struct find_impl<N, N>
        {
            template<typename Key, typename... Ts>
            static constexpr std::size_t find( const const_dict<Key, Ts...>& dict, const typename const_dict<Key, Ts...>::key_type& key )
            {
                return std::size_t( -1 );
            }
        };
    }

    template<typename Key, typename... Ts>
    constexpr std::size_t find( const const_dict<Key, Ts...>& dict, const typename const_dict<Key, Ts...>::key_type& key )
    {    
        return detail::find_impl<0, dict.size()>::find( dict, key );
    }

    // get_value
    namespace detail
    {
        template<std::size_t I>
        struct get_value_impl
        {
            template<typename Key, typename... Ts>
            static constexpr auto get( const const_dict<Key, Ts...>& dict )
            {
                return get_element<I>( dict ).value();
            }
        };

        template<>
        struct get_value_impl<std::size_t( -1 )>
        {
            template<typename Key, typename... Ts>
            static constexpr auto get( const const_dict<Key, Ts...>& dict )
            {
                return nullptr;
            }
        };
    }

    // // template<typename Key, typename... Ts>
    // // constexpr auto get_value_impl = []( const const_dict<Key, Ts...>& dict, const typename const_dict<Key, Ts...>::key_type& key )
    // // {
    // //     return get_element<dict.find( key )>( dict ).value();
    // // };

    // // template<typename Key, typename... Ts>
    // // constexpr auto get_value( const const_dict<Key, Ts...>& dict, const typename const_dict<Key, Ts...>::key_type& key )
    // // {
    // //     return get_value_impl<Key, Ts...>( dict, key );
    // // }

    // template<std::size_t I>
    // constexpr auto get_ele = []( const detail::const_tuple_leaf<I, auto>& leaf )
    // {
    //     return leaf.value();
    // };

    // // constexpr auto get_ele = []( const auto& index, const auto& tuple ){ return static_cast<detail::const_tuple_leaf<index, U>>( tuple ).value(); }
    // constexpr auto get_value = []( const auto& dict, const auto& key ){ return get_ele<find( dict, "glossary" )>( dict ).value(); };

    // // #define get_value( dict, key ) ::compile_time_utility::detail::get_value_impl<find( dict, key )>::get( dict )
}