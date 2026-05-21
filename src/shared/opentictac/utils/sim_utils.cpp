/*
 * sim_utils.cpp
 *
 *  Created on: Jun 16, 2013
 *      Author: benk
 */

#include "sim_utils.hpp"
#include <algorithm>

// this functions returns the actual time and date in a string
const std::string sim_currentDateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

std::string sim_doubleToString(MYREAL val)
{
  // convert double to string
  std::ostringstream s;
  s.precision(15);
  s << val;
  std::string ret = std::string(s.str());
  // avoid plotting a double as an integer
  size_t dot = ret.find('.');
  size_t esmall = ret.find('e');
  size_t ebig = ret.find('E');
  // add .0 if this is not already in a double format
  if (    (dot == std::string::npos) && (esmall == std::string::npos)
       && (ebig == std::string::npos) )
    ret += ".0";
  return ret;
}

std::string sim_integerToString(MYINT val)
{
  // convert int to string
  std::ostringstream s;
  s << val;
  return std::string(s.str());
}

std::string sim_toLowerString(const std::string &src){
  // converts to lower case string
  std::string tmp = src;
  std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
  return tmp;
}

std::string sim_toUpperString(const std::string &src){
  // converts to upper case string
  std::string tmp = src;
  std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
  return tmp;
}

bool sim_stringToInteger(std::string &stringIn, MYINT &val)
{
  std::stringstream ss(stringIn);
  if( (ss >> val).fail() )
    return false;
  else
    return true;
}

bool sim_stringToDouble(std::string &stringIn, MYREAL &val)
{
  std::stringstream ss(stringIn);
  if( (ss >> val).fail() )
    return false;
  else
    return true;
}

MYINT sim_binarysearch(MYINT startI, MYINT endI, MYINT lookF,
    const std::vector<MYINT>& vect)
{
  MYINT res = -1, t = 0;
  // early stop
  if (vect[startI] == lookF) return startI;
  if (vect[endI] == lookF) return endI;
  // loop
  t = (startI + endI)/2;
  while (startI != endI)
    {
      // restrict the interval
      if (vect[t] < lookF)
        startI = t + ((t==startI)?1:0);
      else
        endI = t;
      // early stop
      if (vect[t] == lookF)
        {
          res = t;
          break;
        }
      // compute the new middle point
      t = (startI + endI)/2;
      //std::cout << "t=" << t << " startI=" << startI << " endI=" << endI <<
      //    " vect[t]=" <<  vect[t] << " lookF=" << lookF << "\n";
    }
  return res;
}


MYINT sim_search(MYINT startI, MYINT endI, MYINT lookF,
    const std::vector<MYINT>& vect)
{
  MYINT res = -1;
  for (MYINT t = startI; t <= endI; t++)
    { if (vect[t] == lookF) { res = t; break;} }
  return res;
}


void sim_sort(MYINT startI, MYINT endI,
    std::vector<MYINT>& vect)
{
  bool needsswap = true;
  // TODO: improve this with better sorting algorithms
  // this is now a dummy bubble sort :-(
  while (needsswap)
    {
      needsswap = false;
      for (MYINT i = startI+1; i < endI; i++)
        if (vect[i-1] > vect[i])
          {
            needsswap = true;
            MYINT tmp = vect[i-1]; vect[i-1] = vect[i]; vect[i] = tmp;
          }
    }
}


void sim_insert_intosorted(MYINT startI, MYINT endI, MYINT newElem,
    std::vector<MYINT>& vect)
{
  MYINT i = 0;
  std::vector<MYINT>::iterator it;

  it = vect.begin();
  for (i = 0; i < startI; i++, it++);
  for (i = startI; i < endI; i++)
    {
      if (vect[i] < newElem)
        break;
      it++;
    }
  // insert the element at this actual iterator position
  vect.insert(it,newElem);
}


std::string sim_printvector(MYINT startI, MYINT endI,
    const std::vector<MYINT>& vect)
{
  std::string ret = "{";
  MYINT i = 0;
  for (i = startI; i < endI; i++)
    ret += sim_integerToString(vect[i]) + ",";
  ret += "}";
  return ret;
}

// ================== CLASSES ==============================


// the static fields initializations
std::vector<std::string> UT_SIMVerboseLevelHandle::regExps_;
std::vector<int> UT_SIMVerboseLevelHandle::verbLevels_;
int UT_SIMVerboseLevelHandle::nrExps_ = 0;

int UT_SIMVerboseLevelHandle::globalVerbLevel_ = 0;

UT_SIMVerboseLevelHandle::UT_SIMVerboseLevelHandle()
{
  // only static fields, nothing to do here locally
}

void UT_SIMVerboseLevelHandle::setVerbLevels(
    std::vector<std::string>& regExps,
    std::vector<int>& verbLevels)
{
  // store the lists
  regExps_ = regExps;
  verbLevels_ = verbLevels;
  nrExps_ = (int)regExps_.size();
}

void UT_SIMVerboseLevelHandle::addVerbLevel(std::string& regExps, int verbLevel)
{
  // add a new reg expression
  regExps_.resize(nrExps_+1);
  verbLevels_.resize(nrExps_+1);
  regExps_[nrExps_] = regExps;
  verbLevels_[nrExps_] = verbLevel;
  nrExps_++;
}

int UT_SIMVerboseLevelHandle::getVerbLevel(std::string& className)
{
  for (int i = 0; i < nrExps_; i++)
    {
      // if we have a math than
      // todo: implement also for regular expressions match, such as *Visitor*
      // we might use ... <regex>, but this needs additional informations
      if (className == regExps_[i])
        {
          //std::cout << "VERB:" << verbLevels_[i] << std::endl;
          return verbLevels_[i];
        }
    }
  // return the lowes level
  //std::cout << "VERB:" << globalVerbLevel_ << std::endl;
  return globalVerbLevel_;
}

std::vector<std::string>
    sim_split(const std::string& phrase, const std::string& delimiter)
{
  std::vector<std::string> list;
  std::string s = std::string(phrase);
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos)
    {
      token = s.substr(0, pos);
      list.push_back(token);
      s.erase(0, pos + delimiter.length());
    }
  list.push_back(s);
  return list;
}
