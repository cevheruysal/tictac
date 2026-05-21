/*
 * CS_NatureAndDisciplineCollection.hpp
 *
 *  Created on: Nov 23, 2015
 *      Author: benk
 */

#ifndef CS_NATUREANDDISCIPLINECOLLECTION_HPP_
#define CS_NATUREANDDISCIPLINECOLLECTION_HPP_

#include "verilog_comp.hpp"
#include "CS_DisciplineDeclaration.hpp"
#include "CS_NatureDeclaration.hpp"

/** there should be one instance of this class for one compilation
 * which collects all the natures and disciplines within one compilation */
class CS_NatureAndDisciplineCollection
{
public:

  /// empty Ctor
  CS_NatureAndDisciplineCollection();

  /// empty Dtor
  virtual  ~CS_NatureAndDisciplineCollection() {;}

  /// adds one nature decalration
  void addNature( MY_SMPO<const CS_NatureDeclaration> &n);

  /// adds one discipline declaration
  void addDiscipline( MY_SMPO<const CS_DisciplineDeclaration> &d);

  /// returns true if the nature exists, false otherwise
  bool hasNature(std::string natureName) const;

  /// returns true if the discipline exists, false otherwise
  bool hasDiscipline(std::string discpName) const;

  /// returns the nature, if non-existent, than it will be an empty smaprt pointer
  MY_SMPO<const CS_NatureDeclaration> getNature(std::string natureName);

  /// returns the discipline, if non-existent, than it will be an empty smaprt pointer
  MY_SMPO<const CS_DisciplineDeclaration> getDiscipline(std::string disciplineName);

  /// returns the global container of natures and disciplines
  static MY_SMPO<CS_NatureAndDisciplineCollection>& getDiscAndNatures();

  /** after collection all disciplines and natures we look if electrical & co.
   * are there, if not then, for compatibility purposes, we define by hand those
   * natures and disciplines */
  static void addElectricalIfNonExistent();

private:

  /// all natures
  std::map< std::string, MY_SMPO<const CS_NatureDeclaration> > natures_;

  /// all disciplines
  std::map< std::string, MY_SMPO<const CS_DisciplineDeclaration> > disciplines_;

  /// the one and only collection for one compilation
  static MY_SMPO<CS_NatureAndDisciplineCollection> staticCollection_;

};

#endif /* CS_NATUREANDDISCIPLINECOLLECTION_HPP_ */
