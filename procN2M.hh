#ifndef procN2M_h__
#define procN2M_h__
#include "procReturn.hh"
#include "param.hh"
#include "proc_tools.hh"

#include <vector>


class ProcessorBase {
public:
	virtual bool init() { return true; }
	virtual bool end() { return true; }

	virtual procReturn processEvent() = 0;
	virtual procReturn processEvent(int eventNr) { return processEvent(); };

};

DEFINE_PROC1(processEvent_p, next_, e) {
  return e->processEvent();
}

DEFINE_PROC2(processEvent_p2, next_,i, e) {
  return e->processEvent(i);
}
DEFINE_PROC1(init_p, next_, e) {
  e->init(); return next_(e);
}

DEFINE_PROC1(end_p, next_, e) {
  e->end(); return next_(e);
}
using procCollection = std::vector<std::unique_ptr<ProcessorBase>>;
void run(procCollection& processors) {

	param() | proc() >> for_loop(processors) >> init_p();

	param() | proc() >> while_true() >> for_loop(processors) >> processEvent_p();

	param() | proc() >> for_loop(processors) >> end_p();
}

void run(procCollection& processors, int max_nr) {

	param() | proc() >> for_loop(processors) >> init_p();



  param() << max_nr | proc() >> for_loop() >> for_loop(processors) >> processEvent_p2();

  param() | proc() >> for_loop(processors) >> end_p();
}
template<typename T>
class ref_storage {
public:
	T m_t;
	ref_storage(T t) :m_t(t) {}

};




template<typename T>
class p_storage {
public:
	p_storage(std::vector<std::unique_ptr<ProcessorBase>>& procCollection, T t) :m_procCollection(&procCollection), m_t(t) {}
  p_storage(const p_storage<T>&rhs) :m_t(rhs.m_t), m_procCollection(rhs.m_procCollection) {}
  p_storage& operator=(const p_storage<T>&rhs) {
    m_t = rhs.m_t;
    m_procCollection=rhs.m_procCollection;
    return *this;
  }

  p_storage& operator=(p_storage<T>&& rhs) {
    m_t = rhs.m_t;
    m_procCollection = rhs.m_procCollection;
    return *this;
  }
  std::vector<std::unique_ptr<ProcessorBase>>& get_proc() {
    return *m_procCollection;
  }
  const std::vector<std::unique_ptr<ProcessorBase>>& get_proc() const {
    return *m_procCollection;
  }
	std::vector<std::unique_ptr<ProcessorBase>>* m_procCollection;
	T m_t;

	auto  operator->() ->decltype(&m_t) {
		return &m_t;
	}
};



template<typename T>
class p_storage<ref_storage<T>> {
public:
	p_storage(std::vector<std::unique_ptr<ProcessorBase>>& procCollection, ref_storage<T> t) :m_procCollection(procCollection), m_t(t) {}
	std::vector<std::unique_ptr<ProcessorBase>>& m_procCollection;
	ref_storage<T> m_t;

	auto  operator->() ->decltype(&(m_t.m_t)) {
		return &(m_t.m_t);
	}
};












template<typename T>
p_storage<typename std::remove_reference<T>::type> make_p_storage(T&& t, std::vector<std::unique_ptr<ProcessorBase>>& procColl) {
	return	p_storage<typename std::remove_reference<T>::type>(procColl, std::forward<T>(t));
}


template<typename T>
T remove_p_storage(T&& t) {
	return std::forward<T>(t);
}

template<typename T>
T& remove_p_storage(p_storage<T>&& t)   {
	return t.m_t;
}
template<typename T>
T& remove_p_storage(p_storage<T>& t) {
	return t.m_t;
}

template<typename T>
const T& remove_p_storage(const p_storage<T>& t)  {
	return t.m_t;
}

template <typename T, typename IN_t, typename... ARGS>
auto make_proc(p_storage<IN_t>&& t, ARGS&&... args)-> decltype(make_p_storage( std::declval<T>().get_Data(),t.get_proc())) {


	auto p = std::unique_ptr<T>(new T(remove_p_storage(t), remove_p_storage(args)...));
	auto ret = p->get_Data();
	t.m_procCollection->push_back(std::move(p));
	return make_p_storage(ret, t.get_proc());
}

template <typename T, typename IN_t, typename... ARGS>
auto make_proc(p_storage<IN_t>& t, ARGS&&... args)-> decltype(make_p_storage(std::declval<T>().get_Data(), t.get_proc())) {


  auto p = std::unique_ptr<T>(new T(remove_p_storage(t), remove_p_storage(args)...));
  auto ret = p->get_Data();
  t.m_procCollection->push_back(std::move(p));
  return make_p_storage(ret, t.get_proc());
}
template <typename T, typename IN_t, typename... ARGS>
auto make_proc(const p_storage<IN_t>& t, ARGS&&... args)-> decltype(make_p_storage(std::declval<T>().get_Data(), t.get_proc())) {

  auto p = std::unique_ptr<T>(new T(remove_p_storage(t), remove_p_storage(args)...));
  auto ret = p->get_Data();
  t.m_procCollection->push_back(std::move(p));
  return make_p_storage(ret, t.get_proc());
}


#endif // procN2M_h__
