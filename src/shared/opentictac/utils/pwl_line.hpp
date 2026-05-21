//#pragma once

#ifndef PWL_LINE_HPP
#define PWL_LINE_HPP
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
#include <tuple>
// #include <list>

// #include "utils/sim_utils.hpp"
#include "sim_utils.hpp"

// typedef double MYREAL;
typedef std::pair<MYREAL,MYREAL> valPair;

class PwlLines
{
 public:
  PwlLines(MYREAL x1, MYREAL y1, MYREAL x2, MYREAL y2);
  PwlLines(std::vector<valPair> start);
  PwlLines() = default;

  std::vector<valPair> add(PwlLines const& add);
  PwlLines mult(MYREAL const& start, MYREAL const& end, MYREAL const& factor);
  PwlLines innerSection(MYREAL const& x1, MYREAL const& x2);

  MYREAL eval(MYREAL const& x);

  PwlLines operator+(PwlLines const& b);
  bool operator==(PwlLines const& a);
  friend std::ostream& operator<<(std::ostream& os, PwlLines const& l);

  void prune();
  void addStartAndEnd();
  bool isConstant();
  std::vector<std::tuple<MYREAL, MYREAL, MYREAL>> getConstantParts();

  std::vector<MYREAL> getPwlValues(MYREAL const& maxSlope = maxSlope_);

 private:
  static const MYREAL  maxSlope_;
  std::vector<valPair> lineContainer_;
};


inline std::ostream&
    operator<<(std::ostream& os, valPair const & p)
{
  os << '(' << std::scientific << p.first << ' '  << p.second << ')';
  return os;
}
inline std::ostream&
    operator<<(std::ostream& os, PwlLines const& l)
{
  for (auto p : l.lineContainer_) os << p <<'\n';
  // std::copy(l.lineContainer_.begin(), l.lineContainer_.end(), std::ostream_iterator<valPair>(os, "\n"));
  return os;
}


#endif
