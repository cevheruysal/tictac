#include "pwl_line.hpp"
#include <map>
#include <cassert>
#include <iterator>

const MYREAL PwlLines::maxSlope_ = 1e5;

PwlLines::PwlLines(MYREAL x1, MYREAL y1, MYREAL x2, MYREAL y2)
    : lineContainer_{
          {x1, y1},
          {x2, y2},
      }
{
  addStartAndEnd();
  prune();
}

PwlLines::PwlLines(std::vector<valPair> start) : lineContainer_(start)
{
        addStartAndEnd();
        prune();
}

void
    PwlLines::addStartAndEnd()
{
  if (lineContainer_.front().second != 0.0)
    {
      lineContainer_.insert(lineContainer_.begin(), valPair(lineContainer_.front().first, 0.0));
    }
  if (lineContainer_.back().second != 0.0)
    {
      lineContainer_.push_back(valPair(lineContainer_.back().first, 0.0));
    }
}

MYREAL
interpol(valPair const &x1, valPair const &x2, MYREAL const &x)
{
  auto res =  ((x2.second - x1.second) / (x2.first - x1.first)) * (x - x1.first) + x1.second;
  // std::cout << x1.first << '\t' << x2.first << '\t' << x1.second << '\t' << x2.second << " | " << x
            // << " = " << res << std::endl;
  return res;
}

std::vector<valPair>
    PwlLines::add(PwlLines const &add)
{
  auto                 l      = lineContainer_.begin();
  auto                 a      = add.lineContainer_.begin();
  auto                 l_e    = lineContainer_.end();
  auto                 a_e    = add.lineContainer_.end();
  MYREAL               offset = 0.0;
  std::vector<valPair> result;

  if (l_e == l) return add.lineContainer_;
  if (a_e == a) return lineContainer_;

  auto a_buffer = a++;
  auto l_buffer = l++;


  while (a_buffer->first <= l_buffer->first && a_buffer != a_e)
    {
      result.push_back(*a_buffer);
      // std::cout << "preceding " << result[result.size() - 1] << " " << a_buffer->first << " "
                // << l->first << std::endl;
      if (l_buffer->first < a->first)
        {
          offset = interpol(*a_buffer, *a, l_buffer->first);
          // std::cout << "computed offset" << std::endl;
        }
      a_buffer = a++;
    }
  // std::cout << "finished preceding points" << std::endl;

  while (l != l_e)
    {
      result.push_back(valPair(l_buffer->first, l_buffer->second + offset));
      // std::cout << "added " << result[result.size() - 1] << std::endl;

      while (a_buffer->first <= l->first && a_buffer != a_e)
        {
          // std::cout << a_buffer->first << "  " << l->first << "  "<<a_buffer->second<<std::endl;
          result.push_back(valPair(a_buffer->first,
                                   a_buffer->second + interpol(*l_buffer, *l, a_buffer->first)));
          // std::cout << "added 2 " << result[result.size() - 1] << std::endl;
          if (a == a_e)
            offset = 0.0;
          else
            offset = interpol(*a_buffer, *a, l->first);
          a_buffer = a++;
        }
      l_buffer = l++;
    }

  result.push_back(valPair(l_buffer->first, l_buffer->second + offset));
  // std::cout << "added 3 " << result[result.size() - 1] << std::endl;

  while (a_buffer != a_e)
    {
      result.push_back(*a_buffer);
      a_buffer = a++;
    }

  /*
   *   if (a == a_e) offset = 0.0;
   *   result.push_back(valPair(l_buffer->first, l_buffer->second + offset));
   *
   *   while (a != a_e)
   *     {
   *       result.push_back(*a);
   *       a_buffer = a++;
   *     }
   */

  return result;
}


PwlLines
    PwlLines::mult(MYREAL const &x1, MYREAL const &x2, MYREAL const &factor)
{
  PwlLines sec(innerSection(x1, x2));
  for (auto && p : sec.lineContainer_)
    {
      p.second *= (factor - 1.0);
    }
  return *this + sec;
}


PwlLines
    PwlLines::innerSection(MYREAL const &x1, MYREAL const &x2)
{
  auto cmp = [](MYREAL const &x, valPair const &p) {
    if (x <= p.first)
      return true;
    else
      return false;
  };
  auto start = std::upper_bound(lineContainer_.begin(), lineContainer_.end(), x1, cmp);
  //
  /*
   * auto start = lineContainer_.begin();
   * for (auto p = start; p != lineContainer_.end(); ++p)
   *   {
   *     if (x1 >= p->first)
   *       {
   *         start = p;
   *       }
   *   }
   */
  auto cmp2 = [](MYREAL const &x, valPair const &p) {
    if (x < p.first)
      return true;
    else
      return false;
  };
  auto end = std::upper_bound(lineContainer_.begin(), lineContainer_.end(), x2, cmp2);
  // if (end != lineContainer_.end()) ++end;

  // if (std::next(start)->first == start->first) ++start;
  /*
   * if (std::next(start) != lineContainer_.end() && std::next(start)->first == start->first)
   *   start = std::next(start);
   */
      // if (std::prev(end)->first == end->first) end = std::prev(start);
      // if (end != lineContainer_.end() && std::prev(end)->first == end->first)
      // end = std::prev(end);
  /*
   * auto buffer = start++;
   * if (!(start != lineContainer_.end() && x1 == start->first && start->first == buffer->first))
   * {
   *         start = buffer;
   * }
   * else
   *   std::cout << "moved buffer" << std::endl;
   */

  std::vector<valPair> ret(start, end);

  /*
   * std::cout << "-------------" << std::endl;
   * std::cout << x1 << "    " << x2 << std::endl;
   * std::cout << *this << std::endl;
   * for (auto &&p : ret) std::cout << p << std::endl;
   */
  
  // if (ret.size() > 1 && ret[0].first == ret[1].first) ret.erase(ret.begin());
  if (start != lineContainer_.begin() ) ret.insert(ret.begin(), valPair(x1, eval(x1)));
  if (end != lineContainer_.end()) ret.push_back(valPair(x2, eval(x2)));

  return PwlLines(ret);
}

MYREAL
PwlLines::eval(MYREAL const &x)
{
  if (x < lineContainer_.front().first)
    return 0.0;
  else if (x > lineContainer_.back().first)
    return 0.0;
  else
    {
      auto next    = lineContainer_.begin();
      auto current = next++;

      while (next != lineContainer_.end())
        {
          if (next->first == x) return next->second;
          if (next->first > x && x >= current->first) return interpol(*current, *next, x);
          current = next++;
        }
      assert(0);
    }
}

void
    PwlLines::prune()
{
  std::map<MYREAL, std::vector<valPair>> buffer;
  std::vector<valPair> result;

  for (auto p : lineContainer_)
    {
      if (buffer.count(p.first))
        buffer[p.first].push_back(p);
      else
        {
          buffer[p.first] = std::vector<valPair>();
          buffer[p.first].push_back(p);
        }
    }

  for (auto p : buffer)
    {
      if (p.second.size() == 1)
        result.push_back(p.second[0]);
      else
        {
          result.push_back(p.second.front());
          if (p.second.front() != p.second.back()) result.push_back(p.second.back());
        }
    }
  lineContainer_ = result;
}

std::vector<MYREAL>
    PwlLines::getPwlValues(MYREAL const &maxSlope)
{
  std::vector<MYREAL> ret;

  auto p = lineContainer_.begin();

  if (p != lineContainer_.end())
    {
      ret.push_back(p->first);
      ret.push_back(p->second);
    }
  ++p;

  while (p != lineContainer_.end())
    {
      MYREAL currentSlope;
      MYREAL y2 = p->second;
      MYREAL y1 = ret.back();
      MYREAL x2 = p->first;
      MYREAL x1 = ret[ret.size() - 2];
      assert(x2 >= x1);

      if (y2 - y1 >= 0.0)
        {
          if (x1 == x2)
            currentSlope = 1e50;
          else
            currentSlope = (y2 - y1) / (x2 - x1);

          if (currentSlope > maxSlope)
            ret.push_back(x1 + (y2 - y1) / (maxSlope));
          else
            ret.push_back(x2);

          ret.push_back(y2);
        }
      else
        {
          if (x1 == x2)
            currentSlope = -1e50;
          else
            currentSlope = (y2 - y1) / (x2 - x1);

          if (currentSlope < -maxSlope)
            ret.push_back(x1 + (y2 - y1) / (-maxSlope));
          else
            ret.push_back(x2);

          ret.push_back(y2);
        }
      ++p;
    }
  return ret;
  /*
   *       if (p->first != ret.back())
   *         currentSlope = (p->second - ret[ret.size() - 2]) / (p->first - ret.back());
   *       else
   *         currentSlope = (p->second - ret[ret.size() - 2]) >= 0.0 ? 1e50 : -1e50;
   *
   *       if (std::abs(currentSlope) > maxSlope)
   *         {
   *
   *         }
   *       else
   *         {
   *         }
   */
}

bool
    PwlLines::isConstant()
{
  // check if the lines are horizontal or vertical
  auto next = lineContainer_.begin();
  auto prev = next++;

  while (next != lineContainer_.end())
    {
      if (next->second != prev->second)
        if (next->first != prev->first) return false;
      prev = next++;
    }

  return true;
}

std::vector<std::tuple<MYREAL, MYREAL, MYREAL>>
    PwlLines::getConstantParts()
{
  auto next = lineContainer_.begin();
  auto prev = next++;
  std::vector<std::tuple<MYREAL, MYREAL, MYREAL>> ret;

  while (next != lineContainer_.end())
    {
      if (prev->second == next->second)
        {
          ret.push_back(std::make_tuple(prev->first, next->first, prev->second));
        }
      prev = next++;
    }
  return ret;
}

PwlLines
    PwlLines::operator+(PwlLines const &b)
{
  PwlLines buf(add(b));
  buf.prune();
  return buf;
}

bool
    PwlLines::operator==(PwlLines const &in)
{
  return lineContainer_ == in.lineContainer_;
}

