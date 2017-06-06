#include "../source/timeSurfaceGenerator.hpp"
#include "../source/hotsBlocs.hpp"

#include <chrono>
#include <vector>

#define NP 2
#define NC1 8
#define NC2 32
#define NC3 128

#define R1 2
#define R2 4
#define R3 6

#define T1 100.
#define T2 200.
#define T3 300.

#define KSI1 2e-4
#define KSI2 2e-4
#define NPOW 1.

#define XSIZE 200
#define INIT_MEMORY -1000
#define LIM_INF_BATA 1000
#define LIM_ZERO 1e-6

#define CS1 (2*R1+1)*NP
#define CS2 (2*R2+1)*NC1
#define CS3 (2*R3+1)*NC2

typedef typename std::array<double, CS1> TS1;
typedef typename std::array<double, CS2> TS2;
typedef typename std::array<double, CS3> TS3;


struct Event{ // minimum field require to build 1D TS
  int64_t t;
  int64_t x;
  int64_t p;
};

struct HotsEvent{
  int64_t t;
  int64_t x;
  int64_t p;
  std::vector<int64_t> lp;
};

struct TsEvent1{
  int64_t t;
  int64_t x;
  int64_t p;
  std::vector<int64_t> lp;
  TS1 context;
};
struct TsEvent2{
  int64_t t;
  int64_t x;
  int64_t p;
  std::vector<int64_t> lp;
  TS2 context;
};
struct TsEvent3{
  int64_t t;
  int64_t x;
  int64_t p;
  std::vector<int64_t> lp;
  TS3 context;
};

double bata1(TS1::iterator beg1, TS1::iterator end1, TS1::iterator beg2);
double bata2(TS2::iterator beg1, TS2::iterator end1, TS2::iterator beg2);
double bata3(TS3::iterator beg1, TS3::iterator end1, TS3::iterator beg2);

double kernel1(HotsEvent evRef, HotsEvent evNeighbor);
double kernel2(HotsEvent evRef, HotsEvent evNeighbor);
double kernel3(HotsEvent evRef, HotsEvent evNeighbor);

HotsEvent hotsEventFromEvent(Event ev){
  return HotsEvent{ev.t, ev.x, ev.p, std::vector<int64_t>(1,ev.p)};
}

template<typename TsEvent>
HotsEvent hotsEventFromTsEvent(TsEvent tsEv, int64_t out_p){
  HotsEvent hev{tsEv.t, tsEv.x, out_p, tsEv.lp};
  hev.lp.push_back(out_p);
  return hev;
}

template<typename TsEvent, typename TS>
TsEvent tsEventFromHotsEvent(HotsEvent hev, TS context){
  return TsEvent{hev.t, hev.x, hev.p, hev.lp, context};
}


int main(void){
  std::cout << "Hello world!" << std::endl;

  auto handlerL3 = [](HotsEvent hev){
    return;
  };


  auto L3 = tarsier::make_iiwkCluster<NC3,CS3,true,false,TsEvent3,HotsEvent>
    (KSI1,KSI2,NPOW,bata3,hotsEventFromTsEvent<TsEvent3>,handlerL3);
  auto linkL2L3 = tarsier::make_timeSurfaceGenerator<XSIZE, NC2, R3,INIT_MEMORY, HotsEvent, TsEvent3>
    (kernel3,tsEventFromHotsEvent<TsEvent3, TS3>, L3);

  auto L2 = tarsier::make_iiwkCluster<NC2,CS2,true,false,TsEvent2,HotsEvent>
    (KSI1, KSI2,NPOW,bata2,hotsEventFromTsEvent<TsEvent2>,linkL2L3);
  auto linkL1L2 = tarsier::make_timeSurfaceGenerator<XSIZE, NC1, R2,INIT_MEMORY, HotsEvent, TsEvent2>
    (kernel2,tsEventFromHotsEvent<TsEvent2, TS2>, L2);

  auto L1 = tarsier::make_iiwkCluster<NC1,CS1,true,false,TsEvent1,HotsEvent>
    (KSI1,KSI2,NPOW,bata1,hotsEventFromTsEvent<TsEvent1>,linkL1L2);
  auto hots = tarsier::make_timeSurfaceGenerator<XSIZE, NP, R1,INIT_MEMORY, HotsEvent, TsEvent1>
    (kernel1,tsEventFromHotsEvent<TsEvent1, TS1>, L1);

  srand(time(NULL));
  auto Nevents = 1000000;
  auto start = std::chrono::steady_clock::now();
  for(auto i = 0; i  < Nevents; i++){
    hots(hotsEventFromEvent(Event{i*10,rand()%XSIZE,i%2}));
    // standardLayerL(TsEvent{3,std::array<double,TSSIZE>{0.576950, 0.606531, 0.637628, 0.670320, 0.704688, 0.740818, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.951229, 0.904837, 0.860708, 0.818731, 0.778801}});
  }
  auto duration = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::steady_clock::now() - start);
  std::cout << "STD learning\t-> Speed: " << static_cast<double>(Nevents)/(static_cast<double>(duration.count())/1000.) << " evs/secs" << std::endl;

  return 0;
}




























double bata1(TS1::iterator beg1, TS1::iterator end1, TS1::iterator beg2){
  double d = -log(std::inner_product(beg1,
                                     end1,
                                     beg2,
                                     0.,
                                     [](double a, double b){
                                       return a+b;
                                     },
                                     [](double a, double b){
                                       return sqrt(a*b);
                                     })
                  );
  return (std::isinf(d)) ? LIM_INF_BATA : (d < LIM_ZERO) ? 0 : d;
};

double bata2(TS2::iterator beg1, TS2::iterator end1, TS2::iterator beg2){
  double d = -log(std::inner_product(beg1,
                                     end1,
                                     beg2,
                                     0.,
                                     [](double a, double b){
                                       return a+b;
                                     },
                                     [](double a, double b){
                                       return sqrt(a*b);
                                     })
                  );
  return (std::isinf(d)) ? LIM_INF_BATA : (d < LIM_ZERO) ? 0 : d;
};

double bata3(TS3::iterator beg1, TS3::iterator end1, TS3::iterator beg2){
  double d = -log(std::inner_product(beg1,
                                     end1,
                                     beg2,
                                     0.,
                                     [](double a, double b){
                                       return a+b;
                                     },
                                     [](double a, double b){
                                       return sqrt(a*b);
                                     })
                  );
  return (std::isinf(d)) ? LIM_INF_BATA : (d < LIM_ZERO) ? 0 : d;
};

double kernel1(HotsEvent evRef, HotsEvent evNeighbor){
  auto diff = static_cast<double>(evRef.t)-static_cast<double>(evNeighbor.t);
  return (diff < 3*T1) ? exp(-(diff)/T1) : 0;
};
double kernel2(HotsEvent evRef, HotsEvent evNeighbor){
  auto diff = static_cast<double>(evRef.t)-static_cast<double>(evNeighbor.t);
  return (diff < 3*T2) ? exp(-(diff)/T2) : 0;
};
double kernel3(HotsEvent evRef, HotsEvent evNeighbor){
  auto diff = static_cast<double>(evRef.t)-static_cast<double>(evNeighbor.t);
  return (diff < 3*T3) ? exp(-(diff)/T3) : 0;
};
