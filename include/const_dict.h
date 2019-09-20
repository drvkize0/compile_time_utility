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

        using array_type = const_array<Key, base::SIZE>;
        static constexpr auto npos = array_type::npos;

    private:
        const array_type _index;

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
        constexpr std::size_t find( const Key& key ) const
        {
            return _index.find( key );
        }
    };
    // redirect deduction of string literal to std::string_view
    template<typename... Ks, typename... Vs>
    const_dict( const const_pair<Ks, Vs>&... ) -> const_dict<typename std::common_type<typename const_pair<Ks, Vs>::Key...>::type, Vs...>;
}