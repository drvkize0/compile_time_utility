#pragma once

#include "const_array.h"
#include "hash.h"

namespace compile_time_utility
{
    class string_enum_item
    {
        const std::string_view _name;
        const unsigned int _hash;
    
    public:
        constexpr string_enum_item( const char* name )
            : _name{ name }
            , _hash{ hash32( name ) }
        {
        }

        constexpr string_enum_item( const std::string_view& name )
            : _name{ name }
            , _hash{ hash32( name.data() ) }
        {
        }

        constexpr std::string_view name() const { return _name; }
        constexpr unsigned int hash() const { return _hash; }
    };

    template<std::size_t N>
    class string_enum : public const_array<string_enum_item, N>
    {
    public:
        using base = const_array<string_enum_item, N>;
        using base::operator[];

    public:
        template<typename... Ts>
        constexpr string_enum( const Ts&... values )
            : base{ values... }
        {
        }

        constexpr std::size_t find( unsigned int hash ) const
        {
            for( std::size_t i = 0; i < N; ++i )
            {
                if( hash == base::_items[i].hash() )
                {
                    return i;
                }
            }

            return base::npos;
        }

        inline constexpr std::size_t find( const char* name ) const
        {
            return find( hash32( name ) );
        }

        inline constexpr unsigned int operator[]( const char* name ) const
        {
            return base::_items[find( name )].hash();
        }
    };

    template<typename... Ts>
    string_enum( const Ts&... values ) -> string_enum<sizeof...( Ts )>;
}