#ifndef proc_tools_h__
#define proc_tools_h__
#include "proc.hh"
#include <vector>
#include <type_traits>
#include "TRandom.h"


template <typename T>
std::vector<T> make_vec(std::initializer_list<T> l) {
	return std::vector<T>(l);
}

template <typename T1, typename T2>
struct ___IS_BOTH_INT {
	using type = typename std::conditional<std::is_same<T1, T2>::value, T1, double>::type;
};


template <typename T1, typename T2, typename T3>
struct ___IS_ALL_INT {
	using type = typename ___IS_BOTH_INT<T1, typename ___IS_BOTH_INT<T2, T3>::type>::type;
};



class for_loop_imple_0 {

public:
	template <typename NEXT_T, typename T>
	procReturn operator()(NEXT_T&& next, T t) {
		for (typename std::remove_all_extents<T>::type i = 0; i < t;++i) {
			if (next(i) == stop_) {
				return stop_;
			}
		}

		return success;
	}

	template <typename NEXT_T, typename T>
	procReturn operator()(NEXT_T&& next, const std::vector<T>& vec) {
		for (auto& e : vec) {
			if (next(e) == stop_) {
				return stop_;
			}
		}

		return success;
	}
	template <typename NEXT_T, typename T1, typename T2>
	procReturn  operator()(NEXT_T&& next, T1 start_, T2 end_) {
		for (typename ___IS_BOTH_INT<
							typename std::remove_all_extents<T1>::type, 
							typename std::remove_all_extents<T2>::type
						>::type i = start_; i < end_;++i) {
			if (next(i) == stop_) {
				return stop_;
			}
		}

		return success;
	}






	template <typename NEXT_T, typename T1, typename T2, typename T3>
	procReturn operator()(NEXT_T&& next, T1 start_, T2 end_, T3 step) {

		for (typename ___IS_ALL_INT<
							typename std::remove_all_extents<T1>::type, 
							typename std::remove_all_extents<T2>::type, 
							typename std::remove_all_extents<T3>::type
						>::type i = start_; i < end_;i += step) {
			if (next(i) == stop_) {
				return stop_;
			}
		}

		return success;
	}
};
for_loop_imple_0 for_loop() {
	return for_loop_imple_0{};
}


template<typename END_T>
class for_loop_imple_1 {
	using param_t = typename std::remove_all_extents<END_T>::type;
public:
	const param_t m_end;
	for_loop_imple_1<END_T>(const END_T& end__) :m_end(end__) {}
	for_loop_imple_1<END_T>(END_T&& end__) : m_end(std::move(end__)) {}
	template <typename NEXT_T , typename... ARGS>
	procReturn operator()(NEXT_T&& next, ARGS&&... args) {
		for ( param_t i = 0; i < m_end;++i) {
			if (next(std::forward<ARGS>(args)..., i) == stop_) {
				return stop_;
			}
		}

		return success;
	}


};


template <typename T>
for_loop_imple_1<typename std::remove_all_extents<T>::type> for_loop(T&& t) {
	return for_loop_imple_1<typename std::remove_all_extents<T>::type>(std::forward<T>(t));
}

template<typename T>
class for_loop_imple_2 {
	using param_t = typename std::remove_all_extents<T>::type;
public:
	const param_t m_end;
	const param_t m_start;
	for_loop_imple_2<T>(const T& start__,const T& end__) : m_start(start__),m_end(end__) {}
	template <typename NEXT_T, typename... ARGS>
	procReturn operator()(NEXT_T&& next, ARGS&&... args) {
		for ( param_t i = m_start; i < m_end;++i) {
			if (next(std::forward<ARGS>(args)..., i) == stop_) {
				return stop_;
			}
		}

		return success;
	}


};



template <typename T1, typename T2>
for_loop_imple_2<typename ___IS_BOTH_INT<typename std::remove_all_extents<T1>::type, typename std::remove_all_extents<T2>::type>::type> for_loop(T1&& start_ , T2&& end_) {
	return for_loop_imple_2<typename ___IS_BOTH_INT<typename std::remove_all_extents<T1>::type, typename std::remove_all_extents<T2>::type>::type>(std::forward<T1>(start_), std::forward<T2>(end_));
}


template<typename T>
class for_loop_imple_3 {
	using param_t = typename std::remove_all_extents<T>::type;
public:
	const param_t m_end;
	const param_t m_start;
	const param_t m_step;
	for_loop_imple_3<T>(const T& start__, const T& end__ ,const T& step_) : m_start(start__), m_end(end__) , m_step(step_){}
	template <typename NEXT_T, typename... ARGS>
	procReturn operator()(NEXT_T&& next, ARGS&&... args) {
		for ( param_t i = m_start; i < m_end; i += m_step) {
			if (next(std::forward<ARGS>(args)..., i) == stop_) {
				return stop_;
			}
		}

		return success;
	}


};

template <typename T1, typename T2,typename T3>
for_loop_imple_3<typename ___IS_ALL_INT<typename std::remove_all_extents<T1>::type, typename std::remove_all_extents<T2>::type, typename std::remove_all_extents<T3>::type>::type> for_loop(T1&& start_, T2&& end_,T3&& step_) {
	return for_loop_imple_3<typename ___IS_ALL_INT<typename std::remove_all_extents<T1>::type, typename std::remove_all_extents<T2>::type, typename std::remove_all_extents<T3>::type>::type>(std::forward<T1>(start_), std::forward<T2>(end_),std::forward<T3>(step_));
}


class push_impl {
	T* m_graph;
public:

	push_impl(T* t) :m_graph(t) {
		m_graph->Set(0);
	}



	template <typename NEXT_T, typename... ARGS>
	procReturn operator()(NEXT_T&& next, ARGS... args) {
		m_graph->SetPoint(m_graph->GetN(), args...);
		return next(args...);
	}
};

template<typename T>
push_impl<T> push(T* f) {
	return push_impl<T>(f);
}

class push_first {
public:
	push_first(std::vector<double>& vec_) :vec(vec_) {}
	template <typename NEXT_T, typename T, typename... args>
	procReturn operator()(NEXT_T&& next, T x, args&&... ar) const {

		vec.push_back(x);

		return next(ar...);
	}
private:
	std::vector<double>& vec;
};

class get_element {
public:
	get_element(const std::vector<double>& vec_) :vec(vec_) {}
	template <typename NEXT_T, typename T, typename... args>
	procReturn operator()(NEXT_T&& next, T i, args&&... ar) {

		return next(i, ar..., vec[i]);
	}
private:
	const	std::vector<double>& vec;
};
class remove_first {
public:

	template <typename NEXT_T, typename T, typename... args>
	procReturn operator()(NEXT_T&& next, T i, args... ar) const {

		return next(ar...);
	}

};

template<typename T>
class calc_impl {
public:
	T m_f;
	calc_impl(const T & f) : m_f(f) {

	}
	template <typename NEXT_T, typename... ARGS>
	procReturn operator()(NEXT_T&& next, ARGS... i) const {

		return next(i..., m_f(i...));
	}

};

template<typename T>
calc_impl<typename std::remove_all_extents<T>::type > calc(T&& t) {
	return calc_impl<typename std::remove_all_extents<T>::type >(std::forward<T>(t));
}

template<typename T>
class loop_impl {
public:
	const T& m_vec;
	loop_impl(const T& vec) :m_vec(vec) {

	}

	template <typename NEXT_T, typename... ARGS>
	procReturn operator()(NEXT_T&& next, ARGS&&... args) {
		for (auto& e : m_vec) {
			auto ret = next(args..., e);
			if (ret != success) {
				return ret;
			}
		}

		return success;
	}


};

template <typename T>
loop_impl<T> loop(const T& vec) {
	return loop_impl<T>(vec);
}


template<typename T>
class THFill_impl {
public:
	T& m_histo__;
	THFill_impl(T& vec) :m_histo__(vec) {

	}

	template <typename NEXT_T, typename... ARGS>
	procReturn operator()(NEXT_T&& next, ARGS&&... args) {

		m_histo__.Fill(args...);
		return next(args...);
	}


};

template <typename T>
THFill_impl<T> THFill(T& histo__) {
	return THFill_impl<T>(histo__);
}


class add_random {
	static unsigned getSeed() {
		static unsigned m_seed = 0;
		return ++m_seed;
	}
public:
	TRandom m_rand;
	double m_start = 0, m_end = 1;
	add_random(double start_ = 0, double end__ = 1) : m_rand(add_random::getSeed()), m_start(start_), m_end(end__) {}
	add_random() : m_rand(add_random::getSeed()) {}


	template <typename NEXT_T, typename... ARGS>
	procReturn operator()(NEXT_T&& next, ARGS&&... args) {
		return next(args..., (m_end - m_start)*m_rand.Rndm() + m_start);
	}

};
template<typename T>
void print__(T&& t) {
	std::cout << t << std::endl;
}
template<typename T, typename... ARGS>
void print__(T&& t, ARGS&&... args) {
	std::cout << t << "  ";
	print__(args...);
}

DEFINE_PROC_V(display, nextP, inPut) {

	print__(inPut...);
	return nextP(inPut...);
}

DEFINE_PROC1(square, nextP, inPut) {


	return nextP(inPut, inPut*inPut);
}

DEFINE_PROC_V(while_true, nextP, input_) {


	return nextP(input_...);
}

#endif // proc_tools_h__
