/*
 * CS_NatureAndDisciplineCollection.cpp
 *
 *  Created on: Nov 23, 2015
 *      Author: benk
 */

#include "CS_NatureAndDisciplineCollection.hpp"

MY_SMPO<CS_NatureAndDisciplineCollection> CS_NatureAndDisciplineCollection::staticCollection_
 = MY_SMPO<CS_NatureAndDisciplineCollection>(new CS_NatureAndDisciplineCollection());

CS_NatureAndDisciplineCollection::CS_NatureAndDisciplineCollection()
{
  // nothing to do
}

void CS_NatureAndDisciplineCollection::addNature( MY_SMPO<const CS_NatureDeclaration> &n)
{
  // add the nature only when they are not present
  if ( ! hasNature(n->getName()) ) {
      natures_.insert( std::pair< std::string, MY_SMPO<const CS_NatureDeclaration> >
                     (n->getName(), n));
  }
}

void CS_NatureAndDisciplineCollection::addDiscipline( MY_SMPO<const CS_DisciplineDeclaration> &d)
{
  // add the discipline only when they are not present
  if ( ! hasDiscipline( d->getDisciplineName() ) ) {
      disciplines_.insert(
          std::pair< std::string, MY_SMPO<const CS_DisciplineDeclaration> >
                     ( d->getDisciplineName(), d));
  }
}

bool CS_NatureAndDisciplineCollection::hasNature(std::string natureName) const
{
  return ( natures_.find(natureName) != natures_.end() );
}

bool CS_NatureAndDisciplineCollection::hasDiscipline(std::string discpName) const
{
  return ( disciplines_.find(discpName) != disciplines_.end() );
}

MY_SMPO<const CS_NatureDeclaration>
CS_NatureAndDisciplineCollection::getNature(std::string natureName)
{
  if (hasNature(natureName))
    return (natures_.find(natureName))->second;
  else
    return MY_SMPO<const CS_NatureDeclaration>();
}

MY_SMPO<const CS_DisciplineDeclaration>
CS_NatureAndDisciplineCollection::getDiscipline(std::string disciplineName)
{
  if ( hasDiscipline(disciplineName) )
    return (disciplines_.find(disciplineName))->second;
  else
    return MY_SMPO<const CS_DisciplineDeclaration>();

}

MY_SMPO<CS_NatureAndDisciplineCollection>& CS_NatureAndDisciplineCollection::getDiscAndNatures()
{
  return staticCollection_;
}

void CS_NatureAndDisciplineCollection::addElectricalIfNonExistent()
{

  if ( !staticCollection_->hasNature("Current")  ) {
      CS_NatureDeclaration* current = new CS_NatureDeclaration("Current", "none", 1, 1);
      current->addAttribute("units","A");
      current->addAttribute("access","I");
      current->addAttribute("abstol","1e-12");
      // create Current nature and add it
      MY_SMPO<const CS_NatureDeclaration> curentNature = MY_SMPO<const CS_NatureDeclaration>(current);
      staticCollection_->addNature(curentNature);
      VERILOG_WARNING("Nature \"Current\" not present, adding a default one.",0)
  }
  if ( !staticCollection_->hasNature("Voltage") )  {
      CS_NatureDeclaration* voltage = new CS_NatureDeclaration("Voltage", "none", 1, 1);
      voltage->addAttribute("units","V");
      voltage->addAttribute("access","V");
      voltage->addAttribute("abstol","1e-6");
      // create voltage nature and add it
      MY_SMPO<const CS_NatureDeclaration> voltNature = MY_SMPO<const CS_NatureDeclaration>(voltage);
      staticCollection_->addNature(voltNature);
      VERILOG_WARNING("Nature \"Voltage\" not present, adding a default one.",0)
  }

  // these two mut be existing now!
  MY_SMPO<const CS_NatureDeclaration> curentNature = staticCollection_->getNature("Current");
  MY_SMPO<const CS_NatureDeclaration> voltNature = staticCollection_->getNature("Voltage");

  // now add the 3 disciplines if not present!
  if ( !staticCollection_->hasDiscipline("electrical") ) {
      CS_DisciplineDeclaration *elec = new CS_DisciplineDeclaration("electrical", "none", 1, 1);
      elec->setFlowNature( curentNature );
      elec->setPotentialNature( voltNature );
      MY_SMPO<const CS_DisciplineDeclaration> elecD = MY_SMPO<const CS_DisciplineDeclaration>(elec);
      staticCollection_->addDiscipline( elecD );
      VERILOG_WARNING("Discipline \"electrical\" not present, adding a default one.",0)
  }
  if ( !staticCollection_->hasDiscipline("voltage") ) {
      CS_DisciplineDeclaration *elec = new CS_DisciplineDeclaration("voltage", "none", 1, 1);
      elec->setPotentialNature( voltNature );
      MY_SMPO<const CS_DisciplineDeclaration> elecD = MY_SMPO<const CS_DisciplineDeclaration>(elec);
      staticCollection_->addDiscipline( elecD );
      VERILOG_WARNING("Discipline \"voltage\" not present, adding a default one.",0)
  }
  if ( !staticCollection_->hasDiscipline("current") ) {
      CS_DisciplineDeclaration *elec = new CS_DisciplineDeclaration("current", "none", 1, 1);
      elec->setFlowNature( curentNature );
      MY_SMPO<const CS_DisciplineDeclaration> elecD = MY_SMPO<const CS_DisciplineDeclaration>(elec);
      staticCollection_->addDiscipline( elecD );
      VERILOG_WARNING("Discipline \"current\" not present, adding a default one.",0)
  }
}
