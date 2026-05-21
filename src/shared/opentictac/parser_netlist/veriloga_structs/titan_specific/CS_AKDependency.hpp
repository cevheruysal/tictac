/*
 * CS_AKDependency.hpp
 *
 *  Created on: Jan 17, 2013
 *      Author: benk
 */

#ifndef CS_AKDEPENDENCY_HPP_
#define CS_AKDEPENDENCY_HPP_

#include "verilog_comp.hpp"

/** enum type, for at the moment two types of dependencies */
typedef enum {
  VERILOG_AK_DEP_ANY = 0,
  VERILOG_AK_DEP_NZ = 10
} AkDepEnumType;

/** class to model the dependency propagation in the expression */
class  CS_AKDependency: public UT_VerboseClass
{
public:

  /** Ctor creates an empty dependency list */
  CS_AKDependency();

  /** copy Ctor */
  CS_AKDependency(const CS_AKDependency &targetEl);

  /** copy Ctor */
  CS_AKDependency(const CS_AKDependency *targetEl);

  /** Assigment Ctor */
  CS_AKDependency& operator=(const CS_AKDependency& other);

  /** empty Dtor */
  virtual  ~CS_AKDependency() {;}

  /** returns the number of elements in the dependency list*/
  int getNrElements() const { return nrElem_; }

  /** returns the vector of system variables */
  std::vector<int>& getSysVars() { return sysVars_; }
  const std::vector<int>& getSysVars() const { return sysVars_; }

  /** returns the vector with the type of dependencies */
  std::vector<int>& getSysDepType() { return typeDeps_; }
  const std::vector<int>& getSysDepType() const { return typeDeps_; }

  /** this method combines two dependency lists */
  void uniteDependencies(CS_AKDependency &targetEl );

  /** this methods add one new item to the list,
   * if the system variable already existed then it only updates the dependency type */
  void addSystemVarIndex( int sysVarIndex, int dependencyType );

  /** this function checks weather an index is already present in the dependency list
   * if yes then it returns a value >= 0, otherwise returns -1*/
  int isSysVarIndexPresent(int sysVarIndex) const;

  /** print into string */
  std::string toString() const;

private:

  /** array of indices of the global system variables */
  std::vector<int> sysVars_;

  /** array of the type of the dependencies */
  std::vector<int> typeDeps_;

  /** number of dependencies */
  int nrElem_;
};

#endif /* CS_AKDEPENDENCY_HPP_ */
