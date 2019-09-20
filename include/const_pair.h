#pragma once

#include <type_traits>
#include <string_view>

namespace compile_time_utility
{
    template<typename Key, typename Value>
    class const_pair
    {
        const Key _key;
        const Value _value;
    public:
        constexpr const_pair( const Key& key, const Value& value )
            : _key{ key }
            , _value{ value }
        {
        }
        inline constexpr Key key() const { return _key; }
        inline constexpr Value value() const { return _value; }
    };
    // redirect deduction of string literal to std::string_view
    template<typename Key, typename Value>
    const_pair( const Key&, const Value& ) -> const_pair<
        typename std::conditional<std::is_convertible<Key, std::string_view>::value, std::string_view, Key>::type,
        typename std::conditional<std::is_convertible<Value, std::string_view>::value, std::string_view, Value>::type
        >;
}