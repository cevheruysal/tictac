
#include "SimpleDiode.hpp"
#include "AN_Analysis.hpp"
#include "AN_SimulationControllers.hpp"
#include "AN_DTController.hpp"

#include <cmath>


SimpleDiode::SimpleDiode(std::vector<MYINT> & pins,
                         std::vector<MYREAL> &params,
                         std::string &        instName,
                         MYREAL               mFact)
    : Device(pins, instName),  i_s_(params[0]), v_t_(params[1]), v_t_inv_(1. / v_t_)
{
  this->setMFactor(mFact);
}


void
    SimpleDiode::getAnalysisStamp(std::vector<MYINT> &          stampColumn,
                                  std::vector<MYINT> &          stampRow,
                                  std::vector<MatrixEntryType> &entryType,
                                  std::vector<MYREAL> &         potentialEntryValues,
                                  MYINT &                       nrStampsElements,
                                  [[maybe_unused]] const AN_Analysis *           analysis) const
{

  nrStampsElements = 4;
  // resize the vectors corectly
  if (nrStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrStampsElements, -1);
  if (nrStampsElements > (MYINT)stampRow.size()) stampRow.resize(nrStampsElements, -1);
  if (nrStampsElements > (MYINT)entryType.size()) entryType.resize(nrStampsElements);
  if (nrStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrStampsElements, 0.0);

 /* === HERE STARTS THE CODE OF ASSIGNMENT: 3 ==== */ 
 
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 3 ==== */ 
}

void
    SimpleDiode::getAnalysisStampRHS(std::vector<MYINT> &          stampColumn,
                                     std::vector<MatrixEntryType> &entryType,
                                     std::vector<MYREAL> &         potentialEntryValues,
                                     MYINT &                       nrRHSStampsElements,
                                     [[maybe_unused]] const AN_Analysis *           analysis) const
{

 /* === HERE STARTS THE CODE OF ASSIGNMENT: 3 ==== */ 

 /* === HERE ENDS THE CODE OF ASSIGNMENT: 3 ==== */ 
}

void
    SimpleDiode::evalDevice([[maybe_unused]] const AN_Analysis *       analysis,
                            [[maybe_unused]] AN_SimulationControllers *simulationController,
                            std::vector<MYREAL> &     localVars,
                            [[maybe_unused]] MYINT                     partiID,
                            MYINT &                   nrMatrixStamp,
                            MYINT &                   nrRHSStamp,
                            std::vector<MYREAL> &     matrixValues,
                            std::vector<MYREAL> &     rhsValues,
                            [[maybe_unused]] MYINT &                   flags)
{

 /* === HERE STARTS THE CODE OF ASSIGNMENT: 3 ==== */ 
 
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 3 ==== */ 
}


MYREAL
SimpleDiode::getCurrent(MYREAL vDiff) { return i_s_ * (exp(vDiff * v_t_inv_) - 1.0); }
