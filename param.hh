#ifndef param_h__
#define param_h__
#include <type_traits>
#include <numeric>
#include <tuple>

template<size_t N>
struct Apply {
	template<typename F, typename T, typename... A>
	static inline auto apply(F && f, T && t, A &&... a) {
		return Apply<N - 1>::apply(::std::forward<F>(f), ::std::forward<T>(t),
			::std::get<N - 1>(::std::forward<T>(t)), ::std::forward<A>(a)...
		);
	}
};

template<>
struct Apply<0> {
	template<typename F, typename T, typename... A>
	static inline auto apply(F && f, T &&, A &&... a) {
		return ::std::forward<F>(f)(::std::forward<A>(a)...);
	}
};

template<typename F, typename T>
inline auto apply(F && f, T && t) {
	return Apply< ::std::tuple_size< ::std::decay_t<T>
	>::value>::apply(::std::forward<F>(f), ::std::forward<T>(t));
}



template <typename T>
auto make_param_impl(T&& t);

template <typename T>
class param_impl {
public:
	typename std::remove_reference<T>::type m_params;


	param_impl(const T& param_) :m_params(param_) {

	}
	template<typename T1>
	auto operator<< (T1&& t) {
		return make_param_impl(std::tuple_cat(m_params, std::make_tuple(t)));
	}

	template <typename F>
	auto operator>>=(F&& f) {
		return  apply(f, m_params);
	}

	template <typename F>
	auto operator>>=(const  F& f) {
		return  apply(f, m_params);
	}
	template <typename F>
	auto operator>>=(F& f) {
		return  apply(f, m_params);
	}
};
template <typename T>
auto make_param_impl(T&& t) {
	return param_impl<T>(std::forward<T>(t));
}
class param {
public:
	template<typename T>
	auto operator<< (T&& t) const {
		return make_param_impl(std::make_tuple(t));
	}

	template<typename F>
	auto operator>>= (F&& f) const {
		return f();
	}
};
#endif // param_h__
