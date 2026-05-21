/*
 * sim_utils.hpp
 *
 *  Created on: Jun 16, 2013
 *      Author: benk
 */

#ifndef SIM_UTILS_HPP_
#define SIM_UTILS_HPP_

#include <string>
#include <iomanip>
#include <memory>
#include <locale>
#include <sstream>
#include <vector>
#include <math.h>
#include <set>
#include <map>
#include <unordered_map>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <stdint.h>
#include <stdlib.h>

// ========== BOOST INCLUDES ===========
//#include <boost/shared_ptr.hpp>
#include <boost/numeric/conversion/cast.hpp>

#define TICTAC_SPTR std::shared_ptr
//boost::shared_ptr

// forward declaration of the message handler
class OUT_MessageHandle;

// --------- define the primitive date types -------------
typedef signed long MYINT;
typedef unsigned long UINTEGER;
typedef int32_t INT4;
typedef uint32_t UINT4;
typedef double MYREAL;
typedef double REAL8;
typedef float REAL4;
//typedef bool BOOLEAN;
//#define BOOLEAN bool;

typedef struct { MYREAL re; MYREAL im; } COMPLEX;

/** enum type to represent the various types of variables in the unknown vector */
typedef enum {
  EXT_VOLTAGE = 0, // voltage on an external pin/port
  INT_VOLTAGE = 1, // voltage on an (device) internal pin
  INT_CURRENT = 2 // a current variable
} MNAVarType;

/** enum type to categorize the matrix entries */
typedef enum {
  DYNAMIC_ONLY = 0, // only dynamic entry
  STATIC_ONLY = 1,  // pure static entry
  STATIC_DYNAMIC = 2  // static and dynamic entry
} MatrixEntryType;

/** enum type for the different analysis types */
typedef enum{
  PREAN_TYPE = -1,
  NOANAYS_TYPE = 0,
  DC_TYPE = 1,
  TRAN_TYPE = 2
} Analysis_type;

/** enum type for the outputs ... */
typedef enum{
  SIM_OUT_TYPE_PROTOCOL = 0,
  SIM_OUT_TYPE_WARNING = 1,
  SIM_OUT_TYPE_ERROR = 2
} Output_type;

#define SIMMAX(X,Y) ((X>Y)?(X):(Y))
#define SIMMIN(X,Y) ((X>Y)?(Y):(X))
#define SIMABS(X) ((X>0)?(X):-(X))


#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#define SIM_PI             M_PI

// define as an absolute limit for diagonal matrix elements (for LU)
#define SIM_ABS_DIAG_LIMIT 1E-13

// absolute limit of the time step
#define SIM_ABS_TIME_STEP_LIMIT 1E-19
// upper limit for time steps and simulation time
#define SIM_ABS_LARGE_TIME_LIMIT 1E+4

// error macros, conditional and unconditional
// these macros should be used for internal errors ...
#define SIM_ERROR_STOP(X) { std::cout << "ERROR: " << X << std::endl; /*int *p = 0; p[3]=4;*/ exit(1); }
#define SIM_ERROR_NOSTOP(X) { std::cout << "ERROR: " << X << std::endl; }
#define SIM_ERROR_COND_NOSTOP(Y,X) { if (Y) {std::cout << "\nERROR: " << X << std::endl;\
    /*int *p = 0; p[3]=4; exit(1); */} }
#define SIM_ERROR_COND_STOP(Y,X) { if (Y) {std::cout << "\nERROR: " << X << std::endl;\
  /*int *p = 0; p[3]=4;*/ exit(1); } }

#define SIM_WARNING(X) { std::cout<< "WARNING: " << X << std::endl; }

// -------- DEBUG OUTPUT -------
#define SIM_PRINT_L0(L,X) if (L > 0) { std::cout << X << std::endl; }
#define SIM_PRINT_L1(L,X) if (L > 1) { std::cout << X << std::endl; }
#define SIM_PRINT_L2(L,X) if (L > 2) { std::cout << X << std::endl; }
#define SIM_PRINT_L3(L,X) if (L > 3) { std::cout << X << std::endl; }
#define SIM_PRINT_L4(L,X) if (L > 4) { std::cout << X << std::endl; }
#define SIM_PRINT_L5(L,X) if (L > 5) { std::cout << X << std::endl; }
#define SIM_PRINT_L6(L,X) if (L > 6) { std::cout << X << std::endl; }
#define SIM_PRINT_L7(L,X) if (L > 6) { std::cout << X << std::endl; }
#define SIM_PRINT_L8(L,X) if (L > 6) { std::cout << X << std::endl; }
#define SIM_OUT(X) { std::cout << X << std::endl; }

// ---- PROTOCOL ERROR -----
#define SIM_PROTOCOL_ERROR(AN,X) { std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocolError(AN, SIM_OUT_TYPE_ERROR, ret, 0); }
#define SIM_PROTOCOL_COND_ERROR(C,AN,X) if (C) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocolError(AN, SIM_OUT_TYPE_ERROR, ret, 0); }

// ---- PROTOCOL WARNING -----
#define SIM_PROTOCOL_WARNING(AN,X) { std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); \
    OUT_MessageHandle::writeProtocolWarning(AN, SIM_OUT_TYPE_WARNING, ret, 0); }


// -------- PROTOCOL OUTPUT -------
#define SIM_PROTOCOL_L0(AN,X) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, 0); }
#define SIM_PROTOCOL_L1(AN,X) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, 1); }
#define SIM_PROTOCOL_L2(AN,X) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, 2); }
#define SIM_PROTOCOL_L3(AN,X) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, 3); }
#define SIM_PROTOCOL_L4(AN,X) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, 4); }
#define SIM_PROTOCOL_L5(AN,X) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, 5); }
#define SIM_PROTOCOL_L6(AN,X) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, 6); }

#define SIM_PROTOCOL_L0_T(AN,X,T) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, T); }
#define SIM_PROTOCOL_L1_T(AN,X,T) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, T); }
#define SIM_PROTOCOL_L2_T(AN,X,T) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, T); }
#define SIM_PROTOCOL_L3_T(AN,X,T) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, T); }
#define SIM_PROTOCOL_L4_T(AN,X,T) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, T); }
#define SIM_PROTOCOL_L5_T(AN,X,T) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, T); }
#define SIM_PROTOCOL_L6_T(AN,X,T) {  std::stringstream prot; prot << X; \
    std::string ret = std::string(prot.str()); OUT_MessageHandle::writeProtocol(AN, SIM_OUT_TYPE_PROTOCOL, ret, T); }

/** the function that returns the date and time in a standard format */
const std::string sim_currentDateTime();

/** converts one double to a string*/
std::string sim_doubleToString(MYREAL val);

/** converts one integer to a string*/
std::string sim_integerToString(MYINT val);

/// converts to lower case string
std::string sim_toLowerString(const std::string &src);

/// converts to upper case string
std::string sim_toUpperString(const std::string &src);

/** tries to convert one string to an integer, returns the value and true if
 * succeeded, false otherwise */
bool sim_stringToInteger(std::string &stringIn, MYINT &val);

/** tries to convert one string to a double, returns the value and true if
 * succeeded, false otherwise */
bool sim_stringToDouble(std::string &stringIn, MYREAL &val);

/** looks for the element lookF within the sorted vector (or at least sequence)
 * vect, starting from startI till (including) endI
 * @return -1 if not found otherwise the index */
MYINT sim_binarysearch(MYINT startI, MYINT endI, MYINT lookF,
    const std::vector<MYINT>& vect);

/** looks for the element lookF within the unsorted vector vect,
 * starting from startI till (including) endI
 * @return -1 if not found otherwise the index */
MYINT sim_search(MYINT startI, MYINT endI, MYINT lookF,
    const std::vector<MYINT>& vect);

/** sorts one segment of the array starting from startI until endI*/
void sim_sort(MYINT startI, MYINT endI,
    std::vector<MYINT>& vect);

/** inserts one new element into the sorted vector segment
 * starting from startI until endI.
 * The new element is newElem*/
void sim_insert_intosorted(MYINT startI, MYINT endI, MYINT newElem,
    std::vector<MYINT>& vect);

/** prints one portion of one integer vector into string*/
std::string sim_printvector(MYINT startI, MYINT endI,
    const std::vector<MYINT>& vect);

/* Splits a string by string delimiter */
std::vector<std::string>
    sim_split(const std::string& phrase, const std::string& delimiter);

    // ========================== UTILITY CLASSES ======================

/** class that specifies which class has which verbose level (for debug info).
 * Internally this class has a list of regular expressions and the corresponding
 * verbose levels. If the called string matches one regular expression than
 * it returns the verbose level provided by the user. */
class UT_SIMVerboseLevelHandle {
public:
  /** Ctor to create the handle. The inputs are
   * a vector of regular expressions and the associated verbose levels */
  UT_SIMVerboseLevelHandle();

  virtual ~UT_SIMVerboseLevelHandle() {;}

  /** sets the vectors of verbosity with the given arrays of */
  static void setVerbLevels(
      std::vector<std::string>& regExps,
      std::vector<int>& verbLevels);

  /** adds one verbose level */
  static void addVerbLevel(std::string& regExps, int verbLevel);

  /** returns the associated verbose level corresponding the name.
   * If this string does not match any of the regular expression than the
   * function returns the lowest level "0". */
  static int getVerbLevel( std::string& className );

  /** sets the global verbose level (for all classes)*/
  static void setGlobalVerbose(int v) { globalVerbLevel_ = v; }

  /** return the global verbosity */
  static int getGlobalVerbose() { return globalVerbLevel_; }

private:
  /** vector that contains the regular expression */
  static std::vector<std::string> regExps_;

  /** array containing the associated verbose levels */
  static std::vector<int> verbLevels_;

  /** number of regular expressions */
  static int nrExps_;

  /** for general compiler verbosity we use this global integer */
  static int globalVerbLevel_;
};


/** class that only contains the verbosity level,
 * that might be needed for structured outputting */
class UT_SIMVerboseClass {
public:
  /** Ctor the argument of the constructor is the class name */
  UT_SIMVerboseClass(std::string className)
  { // set the verbosity level according to the debug information
    verb_ = UT_SIMVerboseLevelHandle::getVerbLevel(className);
  }

  virtual ~UT_SIMVerboseClass() {;}

  /// the verbosity of the object
  int verb() const { return verb_; }

  /// set the verbosity of the object
  void setVerb(int v) { verb_ = v; }

private:
  /// verbosity of the object
  volatile int verb_;
};


namespace stringUtils
{

// trim from start (in place)
static inline void
    ltrim(std::string& s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
static inline void
    rtrim(std::string& s)
{
  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(),
          s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
  ltrim(s);
  return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
  rtrim(s);
  return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
  trim(s);
  return s;
}

template <typename outType, typename T>
static inline std::string
    coutContainer(T start, T end, char const* sep = "|")
{
  std::stringstream ss;
  std::copy(start, end, std::ostream_iterator<outType>(ss, sep));
  return ss.str();
}

}  //  namespace stringUtils


#endif /* SIM_UTILS_HPP_ */
