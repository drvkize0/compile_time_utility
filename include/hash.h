#pragma once

#include <type_traits>

namespace compile_time_utility
{
    #define BIG_CONSTANT( x ) x##ul
    template<typename T>
    struct is_byte : std::false_type {};
    template<>
    struct is_byte<char> : std::true_type {};
    template<>
    struct is_byte<signed char> : std::true_type {};
    template<>
    struct is_byte<unsigned char> : std::true_type {};
    // algorithms
    template<typename H, H SEED_, H PRIME_>
    struct hash_fnv
    {
        using this_type = hash_fnv<H, SEED_, PRIME_>;
        static constexpr H SEED = SEED_;
        static constexpr H PRIME = PRIME_;
        // T is not char, byte
        template<typename T, typename std::enable_if<is_byte<T>::value>::type* = nullptr>
        static constexpr H hash( const T* data, std::size_t n, H seed = SEED ) noexcept
        {
            H h = seed;
            while( n-- )
            {
                h ^= *data++;
                h *= PRIME;
            }
            return h;
        }
        // T is char, byte
        static constexpr H hash_sz( const char* data, H seed = SEED ) noexcept
        {
            H h = seed;
            while( !std::char_traits<char>::eq( *data, '\0' ) )
            {
                h ^= *data++;
                h *= PRIME;
            }
            return h;
        }
        template<typename T>
        struct can_combine : std::conjunction<is_byte<T>, std::is_convertible<const T, const unsigned char>> {};
        template<typename T, typename std::enable_if<can_combine<T>::value>::type* = nullptr>
        static inline constexpr H hash_combine( const T& data, H seed = SEED ) noexcept
        {
            return ( seed ^ static_cast<const unsigned char>( data ) ) * PRIME;
        }
        template<typename T, typename std::enable_if<!can_combine<T>::value>::type* = nullptr>
        static inline H hash_combine( const T& data, H seed = SEED ) noexcept
        {
            return this_type::hash<unsigned char>( reinterpret_cast<const unsigned char*>( std::addressof( data ) ), sizeof( T ), seed );
        }
    };
    using hash_fnv32 = hash_fnv<unsigned int, BIG_CONSTANT( 2166136261 ), BIG_CONSTANT( 16777619 )>;
    using hash_fnv64 = hash_fnv<unsigned long, BIG_CONSTANT( 14695981039346656037 ), BIG_CONSTANT( 1099511628211 )>;
    template<typename H>
    using hash_fnv_switch = typename std::conditional<sizeof( H ) <= sizeof( unsigned int ), hash_fnv32, hash_fnv64>::type;
    constexpr unsigned int hash32( const char* s )
    {
        return hash_fnv32::hash_sz( s );
    }
    constexpr unsigned long hash64( const char* s )
    {
        return hash_fnv64::hash_sz( s );
    }
}