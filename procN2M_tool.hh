#ifndef procN2_tool_h__
#define procN2_tool_h__
#include <vector>
#include <memory>
#include "procN2M.hh"
#include "TFile.h"
#include "TTree.h"
#include <functional>
#include "proc.hh"
#include <exception>
#include "TMath.h"


class collection {
public:
  collection(const std::vector<double> * x_, const std::vector<double> * y_, const std::vector<double> *ID_) : x(x_), y(y_), ID(ID_) {}

  const std::vector<double> *x;
  const std::vector<double> *y;
  const std::vector<double> *ID;

};







class get_collection : public ProcessorBase {
public:
  get_collection(std::shared_ptr<TFile> file, const char* collectionName) :m_file(file) {
    m_GBL = dynamic_cast<TTree*>(m_file->Get(collectionName));

    m_GBL->SetBranchAddress("x", &m_gbl_x);
    m_GBL->SetBranchAddress("y", &m_gbl_y);
    m_GBL->SetBranchAddress("ID", &m_ids);

  }




  virtual bool init() override {
    m_count = 0;
    return true;
  }



  virtual procReturn processEvent() override {

    if (++m_count >= m_GBL->GetEntries()) {
      return stop_;
    }

    m_GBL->GetEntry(m_count);

    return success;
  }

  virtual procReturn processEvent(int event_nr) override {

    if (event_nr >= m_GBL->GetEntries()) {
      return stop_;
    }

    m_GBL->GetEntry(event_nr);

    return success;
  }

  collection get_Data() const {
    return collection(m_gbl_x, m_gbl_y, m_ids);
  }


private:
  std::vector<double> *m_gbl_x = nullptr, *m_gbl_y = nullptr, *m_ids = nullptr;




  std::shared_ptr<TFile> m_file;
  TTree *m_GBL = nullptr;
  int m_count = 0;

  
};


class plane {
public:
  plane(const std::vector<double> * x_, const std::vector<double> * y_) : x(x_), y(y_) {}

  const std::vector<double> *x;
  const std::vector<double> *y;


};

class axes {
public:
  axes(const std::vector<double> * x_) : x(x_) {}

  const std::vector<double> *x;



};


class hit_xy {
public: 
  hit_xy(double x_, double y_) :x(x_), y(y_) {}
  double x, y;
};

class getPlane :public ProcessorBase {
public:
  getPlane(collection coll, double ID) :m_id(ID), m_coll(coll) {

  }

  virtual procReturn processEvent() override {
    m_Tracks_x.clear();
    m_Tracks_y.clear();

    for (size_t i = 0; i < m_coll.ID->size();++i) {
      if (m_coll.ID->at(i) == m_id) {
        m_Tracks_x.push_back(m_coll.x->at(i));
        m_Tracks_y.push_back(m_coll.y->at(i));
      }
    }
    return success;
  }
  plane get_Data() const {
    return  plane(&m_Tracks_x, &m_Tracks_y);
  }

private:
  std::vector<double> m_Tracks_x, m_Tracks_y;
  const double m_id;
  collection m_coll;
};



enum axes_t {
  x_axis,
  y_axis
};

p_storage<axes> get_axis(p_storage<plane> plane_, axes_t ax) {

  switch (ax) {
  case x_axis:
    return p_storage<axes>(plane_.get_proc(), plane_.m_t.x);
    break;
  case y_axis:
    return p_storage<axes>(plane_.get_proc(), plane_.m_t.y);
    break;
  }

}



class find_closest :public ProcessorBase {
public:
  find_closest(plane x1, plane x2, double cut_off) :m_x1(x1), m_x2(x2), m_cut_off(cut_off) {}

  virtual procReturn processEvent() override {
    double r = m_cut_off;
    double r1 = m_cut_off;
    m_Tracks_x.clear();
    m_Tracks_y.clear();
    hit_xy h1(0,0);
    for (size_t i = 0; i < m_x1.x->size(); ++i) {
      for (size_t j = 0; j < m_x2.x->size(); ++j) {

        double x1_x = m_x1.x->at(i);
        double x1_y = m_x1.y->at(i);

        double x2_x = m_x2.x->at(j);
        double x2_y = m_x2.y->at(j);

        r1 = TMath::Sqrt( (x1_x -x2_x)*(x1_x - x2_x) + (x1_y - x2_y)*(x1_y - x2_y));

        if (r1<r){
          h1 = hit_xy(m_x1.x->at(i), m_x1.y->at(i));
          r = r1;
        }
      }
    }

    if (r<m_cut_off){
      m_Tracks_x.push_back(h1.x);
      m_Tracks_y.push_back(h1.y);
    }
    return success;
  }
  plane get_Data() const {
    return  plane(&m_Tracks_x, &m_Tracks_y);
  }
private:
  std::vector<double> m_Tracks_x, m_Tracks_y;
  plane m_x1;
  plane m_x2;
  const double m_cut_off;
};

class find_closest_strip :public ProcessorBase {
public:
  find_closest_strip(plane x1, plane x2, double cut_off,axes_t ax) :m_x1(x1), m_x2(x2), m_cut_off(cut_off) {
  if (ax== x_axis){
    Axes_of_int_1 = x1.x;
    Axes_of_int_2 = x2.x;
  } else if (ax == y_axis) {
    Axes_of_int_1 = x1.y;
    Axes_of_int_2 = x2.y;
  } else {
    std::cout << "unknown Axis \n";
  }

  }

  virtual procReturn processEvent() override {
    double r = m_cut_off;
    double r1 = m_cut_off;
    m_Tracks_x.clear();
    m_Tracks_y.clear();
    hit_xy h1(0, 0);
    for (size_t i = 0; i < Axes_of_int_1->size(); ++i) {
      for (size_t j = 0; j < Axes_of_int_2->size(); ++j) {

        double x1_x = Axes_of_int_1->at(i);

        double x2_x = Axes_of_int_2->at(j);

        r1 = TMath::Abs( x1_x-x2_x );

        if (r1 < r) {
          h1 = hit_xy(m_x1.x->at(i), m_x1.y->at(i));
          r = r1;
        }
      }
    }

    if (r < m_cut_off) {
      m_Tracks_x.push_back(h1.x);
      m_Tracks_y.push_back(h1.y);
    }
    return success;
  }
  plane get_Data() const {
    return  plane(&m_Tracks_x, &m_Tracks_y);
  }
private:
  std::vector<double> m_Tracks_x, m_Tracks_y;
  plane m_x1;
  plane m_x2;
  const std::vector<double>* Axes_of_int_1;
  const std::vector<double>* Axes_of_int_2;
  const double m_cut_off;
};

class Correlation :public ProcessorBase {
public:
  Correlation(axes vec_x, axes vec_y) :
    m_intput_x(vec_x.x),
    m_intput_y(vec_y.x){
  }
  
  virtual procReturn processEvent() override {
    x.clear();
    y.clear();
   param() | proc() >> for_loop(*m_intput_x) >> for_loop(*m_intput_y)  >> push_first(x)>> push_first(y);

    return success;
  }


  plane get_Data() const {
    return  plane(&x, &y);
  }
private:
  const std::vector<double>* m_intput_x;
  const std::vector<double>* m_intput_y;
  std::vector<double> x, y;

};

template<typename HIST_T>
class Fill_HIST :public ProcessorBase{
public:
  template<typename... ARGGS>
  Fill_HIST(plane pl, ARGGS&&... args) :
    m_intput_x(pl.x), 
    m_intput_y(pl.y),
    h2(new HIST_T(std::forward<ARGGS>(args)...)),
    m_push(push(h2.get()))
  {}



  virtual procReturn processEvent() override {

   m_intput_x->size() | proc()>> for_loop()
     >>get_element(*m_intput_x)
     >>get_element(*m_intput_y) 
     >> drop<0>()
      >> m_push ;


    return success;
  }

  ref_storage<HIST_T&> get_Data() {
    return ref_storage<HIST_T&>(*h2);
  }


private:
  const std::vector<double>* m_intput_x;
  const std::vector<double>* m_intput_y;
  std::shared_ptr<HIST_T> h2;
  push_impl<HIST_T> m_push;
};


template<>
class Fill_HIST<TH2D> :public ProcessorBase {
public:
  template<typename... ARGGS>
  Fill_HIST(plane pl, ARGGS&&... args) :
    m_intput_x(pl.x),
    m_intput_y(pl.y),
    h2(new TH2D(std::forward<ARGGS>(args)...)),
    m_push(push(h2.get())) {
  }



  virtual procReturn processEvent() override {
    m_intput_x->size() | proc() 
      >> for_loop()      
      >> get_element(*m_intput_x)
      >> get_element(*m_intput_y)
      >> drop<0>()
      >> m_push ;
    
    return success;
  }

  ref_storage<TH2D&> get_Data() {
    return ref_storage<TH2D&>(*h2);
  }


private:
  const std::vector<double>* m_intput_x;
  const std::vector<double>* m_intput_y;
  std::shared_ptr<TH2D> h2;
  push_impl<TH2D> m_push;
};

template<>
class Fill_HIST<TH1D> :public ProcessorBase {
public:
  template<typename... ARGGS>
  Fill_HIST(axes ax, ARGGS&&... args) :
    m_intput_x(ax.x),
    h2(new TH1D(std::forward<ARGGS>(args)...)),
    m_push(push(h2.get())) {
  }



  virtual procReturn processEvent() override {

param()| proc() >> for_loop(*m_intput_x) >> m_push ;


    return success;
  }

  ref_storage<TH1D&> get_Data() {
    return ref_storage<TH1D&>(*h2);
  }


private:
  const std::vector<double>* m_intput_x;
  const std::vector<double>* m_intput_y;
  std::shared_ptr<TH1D> h2;
  push_impl<TH1D> m_push;
};

DEFINE_PROC2(Residual_proc__, next__, x, y) {
  return next__(x - y);
}

class Residual :public ProcessorBase {
public:
  Residual(axes vec_x, axes vec_y) :
    m_intput_x(vec_x.x),
    m_intput_y(vec_y.x){
  }


  virtual bool init() override {
   
    return true;
  }


  virtual bool end() override {
 //   H2->Draw("");
    return true;
  }



  virtual procReturn processEvent() override {

    res.clear();
    param() | proc() >> for_loop(*m_intput_x) >> for_loop(*m_intput_y) >> Residual_proc__() >> push_first(res);


    return success;
  }


  axes get_Data() const {
    return  axes(&res);
  }
private:


  
  std::vector<double> res;
  const std::vector<double>* m_intput_x;
  const std::vector<double>* m_intput_y;
  
};




class cut_op {
public:
  cut_op(std::function<bool(const hit_xy&)>& f) :m_f(f) {}
  std::function<bool(const hit_xy&)>& m_f;
  template<typename NEXT_T>
  procReturn operator()(NEXT_T&& next, double x, double y) {
    if (!m_f(hit_xy(x, y))) {
      return skip;
    }

    return	next(x, y);
  }

};
class cut : public ProcessorBase {
public:
  cut(plane pl, std::function<bool(const hit_xy&)> f) :m_f(std::move(f)), m_pl(pl) {}




  virtual procReturn processEvent() override {

    x.clear();
    y.clear();

    m_pl.x->size() | proc()
      >> for_loop()
      >> get_element(*m_pl.x)
      >> get_element(*m_pl.y)
      >> remove_first()
      >> cut_op(m_f)
      >> push_first(x)
      >> push_first(y);

    return success;
  }

  plane get_Data() const {
    return  plane(&x, &y);
  }
private:
  std::function<bool(const hit_xy&)> m_f;
  plane m_pl;

  std::vector<double> x, y;

};



class transform_hits : public ProcessorBase {
public:

  class transform_proc {
  public:
    transform_proc(std::function<hit_xy(const hit_xy&)>& f) :m_f(f) {}
    std::function<hit_xy(const hit_xy&)>& m_f;
    template<typename NEXT_T>
    procReturn operator()(NEXT_T&& next, double x, double y) {
      auto h = m_f(hit_xy(x, y));

      return	next(h.x,h.y);
    }
  };
  transform_hits(plane pl, std::function<hit_xy(const hit_xy&)> f) :m_f(std::move(f)), m_pl(pl) {}

  virtual procReturn processEvent() override {

    x.clear();
    y.clear();

    m_pl.x->size() | proc()
      >> for_loop()
      >> get_element(*m_pl.x)
      >> get_element(*m_pl.y)
      >> remove_first()
      >> transform_proc(m_f)
      >> push_first(x)
      >> push_first(y);

    return success;
  }

  plane get_Data() const {
    return  plane(&x, &y);
  }
private:
  std::function<hit_xy(const hit_xy&)> m_f;
  plane m_pl;
  std::vector<double> x, y;

};

#endif // procN2_tool_h__
