/*
 * CS_NatureDeclaration.hpp
 *
 *  Created on: Nov 21, 2015
 *      Author: benk
 */

#ifndef CS_NATUREDECLARATION_HPP_
#define CS_NATUREDECLARATION_HPP_

#include "verilog_comp.hpp"
#include "CS_ErrorInfo.hpp"

class CS_ModuleDeclaration;

/**
 * This class represents the declaration of a nature
 *
 * in LRM this represents nature_declaration*/
class CS_NatureDeclaration : public CS_ErrorInfo
{
public:

  /// Ctor with the nature name and the info where this was declared
  CS_NatureDeclaration(std::string natureName,
      std::string filename, int sR, int sC);

  /// empty Dtor
  virtual ~CS_NatureDeclaration() {;}

  /** returns the module, this is just a dummy for error message*/
  const CS_ModuleDeclaration* getModule() const ;

  /** line nr in the Verilog file*/
  int getStartLine() const { return startR_;}

  /** column number in the Verilog file */
  int getStartColumn() const { return startC_; }

  /// returns the parent nature if exists, empty smart ptr if non-existent
  const MY_SMPO<const CS_NatureDeclaration>& getParentNature() const { return parentNature_; }
  /// sets the parent nature
  void setParentNature(MY_SMPO<const CS_NatureDeclaration> &nature) { parentNature_ = nature; }
  /// returns true if it has a parent nature
  bool hasParent() const { return (parentNature_.get() != 0);}

  /// returns abstol
  double getAbsTol() const { return abstol_; }
  /// sets abstol
  void setAbsTol(double v) { abstol_ = v; }

  /// returns the name of the access function
  const std::string& getAccess() const { return access_; }
  /// sets the name of the access function
  void setAccess(std::string v)  { access_ = v; }

  /// returns the ddt_nature, if non-existent than empty string
  const std::string& getDDTNature() const { return ddt_nature_; }
  /// sets the ddt_nature
  void setDDTNature(std::string v)  { ddt_nature_ = v; }

  /// returns the idt_nature, if non-existent than empty string
  const std::string& getIDTNature() const { return idt_nature_; }
  /// sets the idt_nature
  void setIDTNature(std::string v)  { idt_nature_ = v; }

  /// returns the units of the nature
  const std::string& getUnits() const { return  units_; }
  /// sets the nature of the units
  void setUnits(std::string v)  { units_ = v; }

  /// adds a general attribute
  void addAttribute(std::string attribName,std::string attribValue);

  /// returns a general attribute, abstol, units, or similar can used here as well
  std::string getAttribute(std::string attribName) const;

  /// returns the name of the nature
  const std::string& getName() const { return natureName_; }

private:


  /// name of the nature
  std::string natureName_;

  /// name of the parent nature if there is any
  MY_SMPO<const CS_NatureDeclaration> parentNature_;
  /// absolute tolerance
  double abstol_;
  /// access function
  std::string access_;
  /// ddt nature if given
  std::string ddt_nature_;
  /// idt nature if given
  std::string idt_nature_;
  /// units of the nature
  std::string units_;
  /// here we store futher contributions
  std::map< std::string, std::string > furtherAttributes_;

  /// the file where this was declared
  std::string filename_;
  /// start row of the declaration
  int startR_;
  /// start column of the declaration
  int startC_;

  /// the dummy module just for error messages in case of problems
  MY_SMPO<const CS_ModuleDeclaration> dummyModule_;
};

#endif /* CS_NATUREDECLARATION_HPP_ */
