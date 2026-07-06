
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
  stampColumn[0] = 0; stampRow[0] = 0; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = 0.0;
  stampColumn[1] = 1; stampRow[1] = 0; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = 0.0;
  stampColumn[2] = 0; stampRow[2] = 1; entryType[2] = STATIC_ONLY; potentialEntryValues[2] = 0.0;
  stampColumn[3] = 1; stampRow[3] = 1; entryType[3] = STATIC_ONLY; potentialEntryValues[3] = 0.0;
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
  nrRHSStampsElements = 2;
  if (nrRHSStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrRHSStampsElements, -1);
  if (nrRHSStampsElements > (MYINT)entryType.size()) entryType.resize(nrRHSStampsElements);
  if (nrRHSStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrRHSStampsElements, 0.0);
  
  stampColumn[0] = 0; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = 0.0;
  stampColumn[1] = 1; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = 0.0;
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
  MYREAL mfact = (MYREAL)this->getMFactor();
  nrRHSStamp = 2;
  nrMatrixStamp = 4;
  
  MYREAL vp = localVars[0];
  MYREAL vn = localVars[1];
  MYREAL vDiff = vp - vn;
  
  MYREAL ID = getCurrent(vDiff);
  MYREAL dID = i_s_ * v_t_inv_ * exp(vDiff * v_t_inv_);
  
  matrixValues[0] = dID * mfact;
  matrixValues[1] = -dID * mfact;
  matrixValues[2] = -dID * mfact;
  matrixValues[3] = dID * mfact;
  
  rhsValues[0] = ID * mfact;
  rhsValues[1] = -ID * mfact;
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 3 ==== */ 
}


MYREAL
SimpleDiode::getCurrent(MYREAL vDiff) { return i_s_ * (exp(vDiff * v_t_inv_) - 1.0); }
