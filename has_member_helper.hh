#ifndef has_member_helper_h__
#define has_member_helper_h__


#include <type_traits>

template <typename T>
T ____get_T();
template <typename T, typename... ARGS>
T _____try_and_set_return_type(ARGS&&...);


#define CREATE_TEST_FOR_MEMBER(name,member) \
template <typename... T> \
std::false_type name##impl(T&& ...) ; \
template <typename T> \
auto name##impl(const T& t) -> decltype(_____try_and_set_return_type<std::true_type>(t.member)); \
template <typename T> \
auto name##impl(T& t) -> decltype(_____try_and_set_return_type<std::true_type>(t.member)); \
template <typename T> \
auto name##impl(T&& t) -> decltype(_____try_and_set_return_type<std::true_type>(t.member)); \
template <typename T> \
auto name() ->decltype(name##impl(____get_T<T>()))

#define __ENABLE_IF(hasMember,type_) typename std::enable_if< std::is_same<decltype(hasMember), std::true_type>::value, type_>::type


#define __ENABLE_IF_ARITHMETIC(Template_ARG,return_type) std::enable_if_t<std::is_arithmetic<std::remove_all_extents_t<Template_ARG>>::value ,	return_type> 


#endif // has_member_helper_h__
