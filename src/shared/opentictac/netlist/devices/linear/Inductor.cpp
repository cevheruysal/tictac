/*
 * Inductor.cpp
 *
 *  Created on: Sep 12, 2013
 *      Author: benk
 */

#include "Inductor.hpp"
#include "AN_Analysis.hpp"
#include "AN_SimulationControllers.hpp"
#include "AN_DTController.hpp"

Inductor::Inductor(MYINT p1, MYINT p2, MYREAL inductance,
    MYREAL mFact, std::string name):
Device(p1,p2,name)
{
  this->setMFactor(mFact);
  inductance_ = inductance;
  currentID_.resize(1,-1);
  this->setNrDDTs(1); // we have one DDT
}

/** see base class */
void Inductor::getAnalysisStamp(
    std::vector<MYINT>& stampColumn,
    std::vector<MYINT>& stampRow,
    std::vector<MatrixEntryType>& entryType,
    std::vector<MYREAL>& potentialEntryValues,
    MYINT &nrStampsElements,
    const AN_Analysis *analysis) const
{

 
 /* === HERE STARTS THE CODE OF ASSIGNMENT: 2 ==== */ 
	if (analysis->getAnalysisType() == DC_TYPE) {
		nrStampsElements = 4;
		if (nrStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrStampsElements, -1);
		if (nrStampsElements > (MYINT)stampRow.size()) stampRow.resize(nrStampsElements, -1);
		if (nrStampsElements > (MYINT)entryType.size()) entryType.resize(nrStampsElements);
		if (nrStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrStampsElements, 0.0);

		stampColumn[0] = 2; stampRow[0] = 0; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = 1.0;
		stampColumn[1] = 2; stampRow[1] = 1; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = 1.0;
		stampColumn[2] = 0; stampRow[2] = 2; entryType[2] = STATIC_ONLY; potentialEntryValues[2] = 1.0;
		stampColumn[3] = 1; stampRow[3] = 2; entryType[3] = STATIC_ONLY; potentialEntryValues[3] = 1.0;
		return;
	} 

	nrStampsElements = 5;
	if (nrStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrStampsElements, -1);
	if (nrStampsElements > (MYINT)stampRow.size()) stampRow.resize(nrStampsElements, -1);
	if (nrStampsElements > (MYINT)entryType.size()) entryType.resize(nrStampsElements);
	if (nrStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrStampsElements, 0.0);

	stampColumn[0] = 2; stampRow[0] = 0; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = 1.0;
	stampColumn[1] = 2; stampRow[1] = 1; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = 1.0;
	stampColumn[2] = 0; stampRow[2] = 2; entryType[2] = STATIC_ONLY; potentialEntryValues[2] = 1.0;
	stampColumn[3] = 1; stampRow[3] = 2; entryType[3] = STATIC_ONLY; potentialEntryValues[3] = 1.0;
	stampColumn[4] = 2; stampRow[4] = 2; entryType[4] = DYNAMIC_ONLY; potentialEntryValues[4] = 1.0;
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 2 ==== */ 
}


void Inductor::getAnalysisStampRHS(
     std::vector<MYINT>& stampColumn,
     std::vector<MatrixEntryType>& entryType,
     std::vector<MYREAL>& potentialEntryValues,
     MYINT &nrRHSStampsElements,
     const AN_Analysis *analysis) const
{

 
 /* === HERE STARTS THE CODE OF ASSIGNMENT: 2 ==== */ 
	nrRHSStampsElements = 3;
	if (nrRHSStampsElements > (MYINT)stampColumn.size()) stampColumn.resize(nrRHSStampsElements, -1);
	if (nrRHSStampsElements > (MYINT)entryType.size()) entryType.resize(nrRHSStampsElements);
	if (nrRHSStampsElements > (MYINT)potentialEntryValues.size()) potentialEntryValues.resize(nrRHSStampsElements, 0.0);

	stampColumn[0] = 0; entryType[0] = STATIC_ONLY; potentialEntryValues[0] = 1.0;
	stampColumn[1] = 1; entryType[1] = STATIC_ONLY; potentialEntryValues[1] = 1.0;
	stampColumn[2] = 2; entryType[2] = DYNAMIC_ONLY; potentialEntryValues[2] = 1.0;
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 2 ==== */ 
}


void Inductor::evalDevice(
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
    [[maybe_unused]] MYINT &flags
    )
{

 
 /* === HERE STARTS THE CODE OF ASSIGNMENT: 2 ==== */ 
	MYREAL mfact = (MYREAL)this->getMFactor();
    
	if (analysis->getAnalysisType() == DC_TYPE) {
		nrMatrixStamp = 4;
		nrRHSStamp = 3;
		matrixValues[0] =  1.0 * mfact;
		matrixValues[1] = -1.0 * mfact;
		matrixValues[2] =  1.0 * mfact;
		matrixValues[3] = -1.0 * mfact;

		rhsValues[0] = 0.0;
		rhsValues[1] = 0.0;
		rhsValues[2] = 0.0;
		return;
	}  
	
	if (analysis->getAnalysisType() == TRAN_TYPE) {
		nrMatrixStamp = 5;
		nrRHSStamp = 3;
		MYINT ddtIndex = this->getDDTIndex(0, partiID);
		simulationController->getDDTController()->setDDTX(ddtIndex, inductance_ * localVars[2]);
		MYREAL alpha = simulationController->getDDTController()->getAlpha(ddtIndex);
		MYREAL beta  = simulationController->getDDTController()->getBeta(ddtIndex);

		matrixValues[0] =  1.0 * mfact;
		matrixValues[1] = -1.0 * mfact;
		matrixValues[2] =  1.0 * mfact;
		matrixValues[3] = -1.0 * mfact;
		matrixValues[4] = -inductance_ * alpha * mfact;

		rhsValues[0] = 0.0;
		rhsValues[1] = 0.0;
		rhsValues[2] = beta * mfact;
		return;
	}
 /* === HERE ENDS THE CODE OF ASSIGNMENT: 2 ==== */ 
}
