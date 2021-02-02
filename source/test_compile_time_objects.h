#include "const_array.h"
#include "const_dict.h"
#include "const_pair.h"
#include "const_tuple.h"
#include "string_enum.h"

using namespace compile_time_utility;

template<typename T>
struct test_struct
{
    const T _value;

    constexpr test_struct( const T& value )
        : _value( value )
    {
    }

    constexpr T value() const { return _value; }
};

int test( int argc, char** argv )
{
    constexpr auto x0 = const_array{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    for( std::size_t i = 0; i < x0.size(); ++i )
    {
        printf( "x0[%lu] = %d\n", i, x0[i] );
    }

    constexpr auto x1 = const_array{ "Building", "CXX", "object", "CMakeFiles" };
    for( std::size_t i = 0; i < x1.size(); ++i )
    {
        printf( "x1[%lu] = %s\n", i, x1[i].data() );
    }

    constexpr auto x2 = const_tuple{ 10, 11u, 12ul, 13.0f, 14.0, "15", test_struct<std::string_view>{ "16" } };
    printf( "x2[0] = %d\n", get_element<0>( x2 ) );
    printf( "x2[1] = %u\n", get_element<1>( x2 ) );
    printf( "x2[2] = %lu\n", get_element<2>( x2 ) );
    printf( "x2[3] = %f\n", get_element<3>( x2 ) );
    printf( "x2[4] = %f\n", get_element<4>( x2 ) );
    printf( "x2[5] = %s\n", get_element<5>( x2 ).data() );
    printf( "x2[6] = %s\n", get_element<6>( x2 ).value().data() );

    constexpr auto x3 = string_enum{
            "R8u",
            "R8s",
            "R8i",
            "R8ui",
            "R8srgb",
            "R16u",
            "R16s",
            "R16i",
            "R16ui",
            "R16f",
            "R32u",
            "R32s",
            "R32i",
            "R32ui",
            "R32f",
            "RG8u",
            "RG8s",
            "RG8i",
            "RG8ui",
            "RG8srgb",
            "RG16u",
            "RG16s",
            "RG16i",
            "RG16ui",
            "RG16f"
    };

    std::string format( "RG8srgb" );
    switch( hash32( format.c_str() ) )
    {
        case x3["R8u"]:
        {
            printf( "format = x3.R8u\n" );
        }
        break;
        case x3["RG8srgb"]:
        {
            printf( "format = x3.RG8srgb\n" );
        }
        break;
    }

    for( std::size_t i = 0; i < x3.size(); ++i )
    {
        printf( "x3[%lu] = %s, %u\n", i, x3[i].name().data(), x3[i].hash() );
    }

    constexpr auto root = const_dict{
        const_pair{
            "glossary",
            const_dict{
                const_pair{
                    "title",
                    "example glossary"
                },
                const_pair{
                    "GlossDiv",
                    const_dict{
                        const_pair{ "title", "S" },
                        const_pair{
                            "GlossList",
                            const_dict{
                                const_pair{
                                    "GlossEntry",
                                    const_dict{
                                        const_pair{ "ID", "SGML" },
                                        const_pair{ "SortAs", "SGML" },
                                        const_pair{ "GlossTerm", "Standard Generalized Markup Language" },
                                        const_pair{ "Acronym", "SGML" },
                                        const_pair{ "Abbrev", "ISO 8879:1986" },
                                        const_pair{
                                            "GlossDef",
                                            const_dict{
                                                const_pair{
                                                "para",
                                                "A meta-markup language, used to create markup languages such as DocBook."
                                                },
                                                const_pair{
                                                    "GlossSeeAlso",
                                                    const_array{
                                                        "GML",
                                                        "XML"
                                                    }
                                                }
                                            }
                                        },
                                        const_pair{ "GlossSee", "markup" }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    };

    constexpr std::size_t I = find( root, "glossary" );
    printf( "find return: %lu\n", I );

    // have to be like this, not working otherwise
    #define get_value( dict, key ) get_element<find( dict, key )>( dict ).value()

    constexpr auto glossary = get_value( root, "glossary" );
        constexpr auto glossary_title = get_value( glossary, "title" );
        constexpr auto glossary_GlossDiv = get_value( glossary, "GlossDiv" );
            constexpr auto GlossDiv_title = get_value( glossary_GlossDiv, "title" );
            constexpr auto GlossDiv_GlossList = get_value( glossary_GlossDiv, "GlossList" );
                constexpr auto GlossList_GlossEntry = get_value( GlossDiv_GlossList, "GlossEntry" );
                    constexpr auto GlossEntry_ID = get_value( GlossList_GlossEntry, "ID" );
                    constexpr auto GlossEntry_SortAs = get_value( GlossList_GlossEntry, "SortAs" );
                    constexpr auto GlossEntry_GlossTerm = get_value( GlossList_GlossEntry, "GlossTerm" );
                    constexpr auto GlossEntry_Acronym = get_value( GlossList_GlossEntry, "Acronym" );
                    constexpr auto GlossEntry_Abbrev = get_value( GlossList_GlossEntry, "Abbrev" );
                    constexpr auto GlossEntry_GlossDef = get_value( GlossList_GlossEntry, "GlossDef" );
                        constexpr auto GlossDef_para = get_value( GlossEntry_GlossDef, "para" );
                        constexpr auto GlossDef_GlossSeeAlso = get_value( GlossEntry_GlossDef, "GlossSeeAlso" );
                    constexpr auto GlossEntry_GlossSee = get_value( GlossList_GlossEntry, "GlossSee" );

    printf( "glossary[%lu]:\n", glossary.size() );
        printf( "\tglossary_title = %s\n", glossary_title.data() );
        printf( "\tglossary_GlossDiv[%lu]:\n", glossary_GlossDiv.size() );
            printf( "\t\tGlossDiv_title = %s\n", GlossDiv_title.data() );
            printf( "\t\tGlossDiv_GlossList[%lu]:\n", GlossDiv_GlossList.size() );
                printf( "\t\t\tGlossList_GlossEntry[%lu]:\n", GlossList_GlossEntry.size() );
                    printf( "\t\t\t\tGlossEntry_ID = %s\n", GlossEntry_ID.data() );
                    printf( "\t\t\t\tGlossEntry_SortAs = %s\n", GlossEntry_SortAs.data() );
                    printf( "\t\t\t\tGlossEntry_GlossTerm = %s\n", GlossEntry_GlossTerm.data() );
                    printf( "\t\t\t\tGlossEntry_Acronym = %s\n", GlossEntry_Acronym.data() );
                    printf( "\t\t\t\tGlossEntry_Abbrev = %s\n", GlossEntry_ID.data() );
                    printf( "\t\t\t\tGlossEntry_GlossDef[%lu]:\n", GlossEntry_GlossDef.size() );
                        printf( "\t\t\t\t\tGlossDef_para = %s\n", GlossDef_para.data() );
                        printf( "\t\t\t\t\tGlossDef_GlossSeeAlso[%lu]:\n", GlossDef_GlossSeeAlso.size() );
                            printf( "\t\t\t\t\t\tGlossDef_GlossSeeAlso[0] = %s\n", GlossDef_GlossSeeAlso[0].data() );
                            printf( "\t\t\t\t\t\tGlossDef_GlossSeeAlso[1] = %s\n", GlossDef_GlossSeeAlso[1].data() );
                    printf( "\t\t\t\tGlossEntry_GlossSee = %s\n", GlossEntry_GlossSee.data() );

    #undef get_value

    return 0;
}