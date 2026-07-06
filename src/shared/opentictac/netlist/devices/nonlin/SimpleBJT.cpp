#include "SimpleBJT.hpp"

#include <cmath>
// #include "AN_Analysis.hpp"
// #include "AN_SimulationControllers.hpp"
// #include "AN_DTController.hpp"

SimpleBJT::SimpleBJT(std::vector<MYINT> & pins,
                     std::vector<MYREAL> &params,
                     std::string &        instName,
                     MYREAL               mFact)
    : Device(pins, instName),
      alpha_f_(params[0]),
      alpha_r_(params[1]),
      I_es_(params[2]),
      I_cs_(I_es_ * alpha_f_ / alpha_r_),
      v_t_(params[3]),
      v_t_inv_(1. / v_t_)
{
  this->setMFactor(mFact);
}



void
    SimpleBJT::getAnalysisStamp(std::vector<MYINT> &          stampColumn,
                                std::vector<MYINT> &          stampRow,
                                std::vector<MatrixEntryType> &entryType,
                                std::vector<MYREAL> &         potentialEntryValues,
                                MYINT &                       nrStampsElements,
                                [[maybe_unused]] const AN_Analysis *           analysis) const
{

  nrStampsElements = 9;
  // resize the vectors corectly
  if (nrStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrStampsElements, -1);
  if (nrStampsElements > (MYINT)stampRow.size()) stampRow.resize(nrStampsElements, -1);
  if (nrStampsElements > (MYINT)entryType.size()) entryType.resize(nrStampsElements);
  if (nrStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrStampsElements, 0.0);

 /* === HERE STARTS THE CODE OF ASSIGNMENT: 3 ==== */ 
  stampColumn[0] = 0; stampRow[0] = 0; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = 0.0;
  stampColumn[1] = 1; stampRow[1] = 0; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = 0.0;
  stampColumn[2] = 2; stampRow[2] = 0; entryType[2] = STATIC_ONLY; potentialEntryValues[2] = 0.0;
  stampColumn[3] = 0; stampRow[3] = 1; entryType[3] = STATIC_ONLY; potentialEntryValues[3] = 0.0;
  stampColumn[4] = 1; stampRow[4] = 1; entryType[4] = STATIC_ONLY; potentialEntryValues[4] = 0.0;
  stampColumn[5] = 2; stampRow[5] = 1; entryType[5] = STATIC_ONLY; potentialEntryValues[5] = 0.0;
  stampColumn[6] = 0; stampRow[6] = 2; entryType[6] = STATIC_ONLY; potentialEntryValues[6] = 0.0;
  stampColumn[7] = 1; stampRow[7] = 2; entryType[7] = STATIC_ONLY; potentialEntryValues[7] = 0.0;
  stampColumn[8] = 2; stampRow[8] = 2; entryType[8] = STATIC_ONLY; potentialEntryValues[8] = 0.0;
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 3 ==== */ 
}

void
    SimpleBJT::getAnalysisStampRHS(std::vector<MYINT> &          stampColumn,
                                   std::vector<MatrixEntryType> &entryType,
                                   std::vector<MYREAL> &         potentialEntryValues,
                                   MYINT &                       nrRHSStampsElements,
                                   [[maybe_unused]] const AN_Analysis *           analysis) const
{

 /* === HERE STARTS THE CODE OF ASSIGNMENT: 3 ==== */ 
  nrRHSStampsElements = 3;
  if (nrRHSStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrRHSStampsElements, -1);
  if (nrRHSStampsElements > (MYINT)entryType.size()) entryType.resize(nrRHSStampsElements);
  if (nrRHSStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrRHSStampsElements, 0.0);
  
  stampColumn[0] = 0; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = 0.0;
  stampColumn[1] = 1; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = 0.0;
  stampColumn[2] = 2; entryType[2] = STATIC_ONLY; potentialEntryValues[2] = 0.0;
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 3 ==== */ 
}



void
    SimpleBJT::evalDevice([[maybe_unused]] const AN_Analysis *       analysis,
                          [[maybe_unused]] AN_SimulationControllers *simulationController,
                          std::vector<MYREAL> &     localVars,
                          [[maybe_unused]] MYINT    partiID,
                          MYINT &                   nrMatrixStamp,
                          MYINT &                   nrRHSStamp,
                          std::vector<MYREAL> &     matrixValues,
                          std::vector<MYREAL> &     rhsValues,
                          [[maybe_unused]] MYINT &                   flags)
{

 /* === HERE STARTS THE CODE OF ASSIGNMENT: 3 ==== */ 
  MYREAL mfact = (MYREAL)this->getMFactor();
  nrRHSStamp = 3;
  nrMatrixStamp = 9;

  MYREAL vC = localVars[0];
  MYREAL vB = localVars[1];
  MYREAL vE = localVars[2];
  
  MYREAL expF = exp((vB - vE) * v_t_inv_);
  MYREAL expR = exp((vB - vC) * v_t_inv_);
  
  MYREAL IF = I_es_ * (expF - 1.0);
  MYREAL IR = I_cs_ * (expR - 1.0);
  
  MYREAL dIF_dvB = I_es_ * v_t_inv_ * expF;
  MYREAL dIF_dvE = -dIF_dvB;
  MYREAL dIF_dvC = 0.0;

  MYREAL dIR_dvB = I_cs_ * v_t_inv_ * expR;
  MYREAL dIR_dvC = -dIR_dvB;
  MYREAL dIR_dvE = 0.0;
  
  // Row 0: C -> residual +IC = alpha_f*IF - IR
  matrixValues[0] = (-dIR_dvC) * mfact;
  matrixValues[1] = (alpha_f_ * dIF_dvB - dIR_dvB) * mfact;
  matrixValues[2] = (alpha_f_ * dIF_dvE) * mfact;

  // Row 1: B -> residual +IB = (1-alpha_f)*IF + (1-alpha_r)*IR
  matrixValues[3] = ((1.0 - alpha_r_) * dIR_dvC) * mfact;
  matrixValues[4] = ((1.0 - alpha_f_) * dIF_dvB + (1.0 - alpha_r_) * dIR_dvB) * mfact;
  matrixValues[5] = ((1.0 - alpha_f_) * dIF_dvE) * mfact;

  // Row 2: E -> residual -IE = -(IF - alpha_r*IR)
  matrixValues[6] = (alpha_r_ * dIR_dvC) * mfact;
  matrixValues[7] = (alpha_r_ * dIR_dvB - dIF_dvB) * mfact;
  matrixValues[8] = (-dIF_dvE) * mfact;

  rhsValues[0] = (alpha_f_ * IF - IR) * mfact;
  rhsValues[1] = (IF * (1.0 - alpha_f_) + IR * (1.0 - alpha_r_)) * mfact;
  rhsValues[2] = (alpha_r_ * IR - IF) * mfact;
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 3 ==== */ 
}

