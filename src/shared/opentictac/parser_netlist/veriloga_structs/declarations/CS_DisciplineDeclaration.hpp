/*
 * CS_DisciplineDeclaration.hpp
 *
 *  Created on: Nov 21, 2015
 *      Author: benk
 */

#ifndef CS_DISCIPLINEDECLARATION_HPP_
#define CS_DISCIPLINEDECLARATION_HPP_

#include "verilog_comp.hpp"
#include "CS_ErrorInfo.hpp"
#include "CS_NatureDeclaration.hpp"

class CS_ModuleDeclaration;

/** This class represents a discipline declaration
 *
 * in LRM this is discipline_declaration
 */
class CS_DisciplineDeclaration : public CS_ErrorInfo
{
public:

  /// Ctor with the discipline name and the info where this was declared
  CS_DisciplineDeclaration(std::string disciplineName,
      std::string filename, int sR, int sC);

  /// empty Dtor
  virtual  ~CS_DisciplineDeclaration() {;}

  /** returns the module, this is just a dummy for error message*/
  const CS_ModuleDeclaration* getModule() const ;

  /** line nr in the Verilog file*/
  int getStartLine() const { return startR_;}

  /** column number in the Verilog file */
  int getStartColumn() const { return startC_; }

  /// returns the discipline name
  const std::string& getDisciplineName() const { return disciplineName_; }

  /// sets the flow nature of the discipline
  void setFlowNature(MY_SMPO<const CS_NatureDeclaration> &n) {flowNature_ = n;}
  /// returns the flow nature of the discipline, empty smart ptr if non-existent
  const MY_SMPO<const CS_NatureDeclaration>& getFlowNature() const { return flowNature_; }
  /// returns true if flow nature is existent, false otherwise
  bool hasFlowNature() const { return (flowNature_.get() != 0); }

  /// sets the potential nature of this discipline
  void setPotentialNature(MY_SMPO<const CS_NatureDeclaration> n) {potentialNature_ = n;}
  /// returns the potential nature of the discipline, empty smart ptr if non-existent
  const MY_SMPO<const CS_NatureDeclaration>& getPotentialNature() const { return potentialNature_; }
  /// returns potential if flow nature is existent, false otherwise
  bool hasPotentialNature() const { return (potentialNature_.get() != 0); }

  /// returns true if this discipline has continious binding
  bool isDisciplineContinous() const { return isContinuous_; }
  /// sets the continous or discrete binding of a discipline
  void setDisciplineContinous(bool b) { isContinuous_ = b; }

  /// Overwrites the flow attributes
  void addFlowAtributes(std::string atribName, std::string atribValue);
  /// Overwrites the flow attributes
  void addPotentialAtributes(std::string atribName, std::string atribValue);

  /// returns a given attribute from the flow
  std::string getFlowAtributes(std::string atribName) const;

  /// returns a given attribute from the potential
  std::string getPotentialAtributes(std::string atribName) const;

private:

  /// name of the discipline
  std::string  disciplineName_;

  /// name of the flow nature
  MY_SMPO<const CS_NatureDeclaration>  flowNature_;

  /// name of the potential nature
  MY_SMPO<const CS_NatureDeclaration> potentialNature_;

  /** true, when this is continuous (default),
      false if this is set explicitly to discrete */
  bool isContinuous_;

  /// if some attributes of flow are overwritten than they are here
  std::map< std::string, std::string > flowAttributes_;
  /// if some attributes of potential are overwritten than they are here
  std::map< std::string, std::string > potentialAttributes_;

  /// the file where this was declared
  std::string filename_;
  /// start row of the declaration
  int startR_;
  /// start column of the declaration
  int startC_;

  /// the dummy module just for error messages in case of problems
  MY_SMPO<const CS_ModuleDeclaration> dummyModule_;
};

#endif /* CS_DISCIPLINEDECLARATION_HPP_ */
