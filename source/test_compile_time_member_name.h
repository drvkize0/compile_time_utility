#include <string>
#include <source_location>


// extracted from https://github.com/getml/reflect-cpp

struct Test {
    std::string name;
    std::string address;
    int age;

    static float anything;
};

template<typename T>
struct wrapper
{
    T value;
    static const wrapper<T> obj;
};

template<typename T>
consteval const T& get_wrapped_obj() { return wrapper<std::remove_cvref_t<T>>::obj.value; }

template <class T>
struct fake_object_tuple_view_helper {
    static consteval auto tuple_view() {
        const auto& [a, b, c] = get_wrapped_obj<std::remove_cvref_t<T>>();
        const auto ref_tup = std::tie(a, b, c);
        const auto get_ptrs = [](const auto&... _refs) {
        return std::make_tuple(&_refs...);
        };
        return std::apply(get_ptrs, ref_tup);
    }
};

template<auto ptr>
consteval auto typeCall()
{
    return std::string_view( std::source_location::current().function_name() );
}

template<std::size_t N>
static consteval auto get_member_ptr()
{
    return std::get<N>( fake_object_tuple_view_helper<Test>::tuple_view() );
}

int main(int, char**)
{
    // why this is working?
    printf( "%s\n", typeCall<&Test::anything>().data() );

    printf( "%s\n", typeCall<get_member_ptr<0>()>().data() );
    printf( "%s\n", typeCall<get_member_ptr<1>()>().data() );
    printf( "%s\n", typeCall<get_member_ptr<2>()>().data() );

    return 0;
}