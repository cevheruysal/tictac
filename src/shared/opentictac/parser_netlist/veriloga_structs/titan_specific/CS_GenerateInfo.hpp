/*
 * CS_GenerateInfo.hpp
 *
 *  Created on: Jan 11, 2013
 *      Author: benk
 */

#ifndef CS_GENERATEINFO_HPP_
#define CS_GENERATEINFO_HPP_

#include "verilog_comp.hpp"
#include "CS_AKDependency.hpp"

/** This class is meant to be a super class of all nodes that might
 * need to store some general data for the AK/IP code generation.
 * Basically all the generate specific data should be dumped here
 *
 * This class has no LRM correspondence  */
class CS_GenerateInfo
{
public :

  /** Ctor to set the fields */
  CS_GenerateInfo();

  virtual ~CS_GenerateInfo() { ; }

 /** returns the name that is potentially used in the AK generate code*/
  std::string getAkName() const { return akName_; }
  /** sets the name of the AK generate */
  void setAkName( std::string akName) { akName_ = akName; }

  /** returns the name that is potentially used in the AK generate code*/
   std::string getAkGenerateName() const { return akGenerateName_; }
   /** sets the name of the AK generate */
   void setAkGenerateName( std::string akGenerateName) { akGenerateName_ = akGenerateName; }

  /** returns the index of this Value (Parameter/Port/Variable) in the AK generate,
   * that might be different from the linear indexing */
  int getAkIndex() const { return akIndex_; }

  /** sets the AK index of this Value (e.g., Parameter) */
  void setAkIndex( int akInd ) { akIndex_ = akInd; }

  /** returns the index in the eval list */
  int getAkEvalIndex() const { return akEvalIndex_; }

  /** sets the index of the evaluation list of the AK*/
  void setAkEvalIndex(int akEvalIndex) { akEvalIndex_ = akEvalIndex;}

  /** gets the system variable index for the AK generate */
  int getAkSystemVarIndex() const { return akSystemVarIndex_; }

  /** sets the system variable index for the AK generate */
  void setAkSystemVarIndex(int varIndex) { akSystemVarIndex_ = varIndex; }

  /** function returns the set of dependencies */
  CS_AKDependency& getActVarDependency() { return generalVarDependencies_; }
  const CS_AKDependency& getActVarDependency() const { return generalVarDependencies_; }

  /** sets the dependencies of this node */
  void setActVarDependency(CS_AKDependency& varDeps) { generalVarDependencies_ = varDeps; }

  /** returns the dependency type that should be propagated further */
  int getDependencyType() const { return dependencyType_; }

  /** sets the dependency type to a chosen one */
  void setDependencyType(int newDepType) { dependencyType_ = newDepType;}

  /** returns the context where this node is used */
  virtual int getNodeContext() const { return variableContext_; }
  /** sets the context of the node */
  void setNodeContext(int v) { variableContext_ = v; }
  /** adds flags to the context of the node */
  void addtoNodeContext(int v) { variableContext_ = variableContext_ | v; }

  /** returns the general flag*/
  virtual int getGeneralFlag() const { return generalFlag_; }
  /** sets the general flag of the node */
  void setGeneralFlag(int v) { generalFlag_ = v; }
  /** adds flags to the general flag */
  void addtoGeneralFlag(int v) { generalFlag_ = generalFlag_ | v; }

protected:

  /** AK generate name, used in the code generation process */
  std::string akName_;

  /** AK generate name, used in the code generation process */
  std::string akGenerateName_;

  /** for the AK generate one might need an indexing */
  int akIndex_;

  /** index in the global evaluation list of a module,
   * this might be used for a function or for its arguments */
  int akEvalIndex_;

  /** some entities have an AK specific variable index, */
  int akSystemVarIndex_;

  /** this fields stores the dependency type that should be propagated further */
  int dependencyType_;

  /** for each node we might interested in which contex is this used:
   * e.g., in bias dependent, only */
  int variableContext_;

  /** the set of dependencies */
  CS_AKDependency generalVarDependencies_;

  /** this integer is for general purpose to mark specific parts */
  int generalFlag_;

};

#endif /* CS_GENERATEINFO_HPP_ */
