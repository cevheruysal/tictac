/*
 * D_SimpleMOSFET.cpp
 *
 *  Created on: May 13, 2015
 *      Author: benk
 */

#include "D_SimpleMOSFET.hpp"
#include "AN_Analysis.hpp"
#include "AN_SimulationControllers.hpp"
#include "AN_DTController.hpp"

D_SimpleMOSFET::D_SimpleMOSFET(std::vector<MYINT> &pins,
    std::vector<MYREAL> &params,
    std::string &instName,
    MYREAL mFact) : Device(pins, instName)
{
  this->setMFactor(mFact);
  this->setNrDDTs(1); // we have one DDT

  //std::cout << " NUMMOSFETPIN=" << numPins_ << "\n";

  // setting the parameters
  N_       = params[0];
  W_       = params[1];
  L_       = params[2];
  KP_      = params[3];
  VTO_     = params[4];
  LAMBDA_  = params[5];
  CGB_     = params[6];
  GMIN_    = params[7];
}

void D_SimpleMOSFET::getAnalysisStamp(
     std::vector<MYINT>& stampColumn,
     std::vector<MYINT>& stampRow,
     std::vector<MatrixEntryType>& entryType,
     std::vector<MYREAL>& potentialEntryValues,
     MYINT &nrStampsElements,
     const AN_Analysis *analysis) const
{
  // return the local element stamp
  // for DC we have less contribution
  if (analysis->getAnalysisType() == DC_TYPE)
    nrStampsElements = 11;
  else
    nrStampsElements = 11+4;

}

void D_SimpleMOSFET::getAnalysisStampRHS(
      std::vector<MYINT>& stampColumn,
      std::vector<MatrixEntryType>& entryType,
      std::vector<MYREAL>& potentialEntryValues,
      MYINT &nrRHSStampsElements,
      const AN_Analysis *analysis) const
{
  // for DC we have no contribution
  if (analysis->getAnalysisType() == DC_TYPE)
    nrRHSStampsElements = 3;
  else
    nrRHSStampsElements = 3+2;


}

void D_SimpleMOSFET::evalDevice(
     // ---------- INPUTS --------
     const AN_Analysis *analysis,
     AN_SimulationControllers* simulationController,
     std::vector<MYREAL> &localVars,
     MYINT partiID,
     // ----------- OUTPUTS ----------
     MYINT &nrMatrixStamp,
     MYINT &nrRHSStamp,
     std::vector<MYREAL> &matrixValues,
     std::vector<MYREAL> &rhsValues,
     [[maybe_unused]] MYINT &flags )
{
//
}
