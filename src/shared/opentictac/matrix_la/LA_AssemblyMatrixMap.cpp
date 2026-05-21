/*
 * LA_AssemblyMatrixMap.cpp
 *
 *  Created on: Jan 31, 2014
 *      Author: benk
 */

#include "LA_AssemblyMatrixMap.hpp"
#include "AN_Analysis.hpp"
#include "Netlist.hpp"
#include "LA_MatrixBasis.hpp"
#include "LA_MatrixNetlistMap.hpp"

LA_AssemblyMatrixMap::LA_AssemblyMatrixMap(AN_Analysis *analysis, Netlist *netlist):
UT_SIMVerboseClass(std::string("AssemblyMatrixMap")), analysis_(analysis), netlist_(netlist)
{
  // nothing do yet since there is no Matrix or solver
  isInitialized_ = false;
}


void LA_AssemblyMatrixMap::generateMap()
{
  // if already initialized then exit
  if (isInitialized_) return;

  MYINT netlistIndex = netlist_->getPariID();
  LA_MatrixBasis<MYREAL>* analysisMatrix = analysis_->getRealSolver(netlistIndex)->getSolverMatrix();
  const LA_MatrixNetlistMap* matrixMap = analysisMatrix->getMatrixMap();
  std::vector<MYINT> stampColumn;
  std::vector<MYINT> stampRow;
  std::vector<MatrixEntryType> entryType;
  std::vector<MYREAL> potentialEntryValues;
  MYINT totalNrLocalVars = 0, toralNrLocalMatrixEntry = 0, totalNrRHS = 0, indx = 0, indx1 = 0;
  MYINT nrStampsElements, i, j;

  nrLocalVars_.resize(netlist_->getNrDevices());
  mapLocalVarsStartIndex_.resize(netlist_->getNrDevices() + 1);
  mapLocalMatrixStartIndex_.resize(netlist_->getNrDevices() + 1);
  mapLocalRHSStartIndex_.resize(netlist_->getNrDevices() + 1);

  // here only build the information about
  for (i = 0; i < netlist_->getNrDevices(); i++) {
      nrLocalVars_[i] = netlist_->getDevice(i)->getNrVariables(analysis_);
      netlist_->getDevice(i)->getAnalysisStamp(stampColumn, stampRow, entryType,
            potentialEntryValues, nrStampsElements, analysis_);
      mapLocalVarsStartIndex_[i] = totalNrLocalVars;
      mapLocalMatrixStartIndex_[i] = toralNrLocalMatrixEntry;
      mapLocalRHSStartIndex_[i] = totalNrRHS;
      totalNrLocalVars += nrLocalVars_[i];
      toralNrLocalMatrixEntry += nrStampsElements;
      netlist_->getDevice(i)->getAnalysisStampRHS( stampRow, entryType,
            potentialEntryValues, nrStampsElements, analysis_);
      totalNrRHS += nrStampsElements;
      mapLocalVarsStartIndex_[i+1] = totalNrLocalVars;
      mapLocalMatrixStartIndex_[i+1] = toralNrLocalMatrixEntry;
      mapLocalRHSStartIndex_[i+1] = totalNrRHS;
    }

  // print debug informations
  SIM_PRINT_L3(verb(),"Ctor AN_DCFunction, nrElems=" << netlist_->getNrDevices()
      << ", totalNrLocalVars=" << totalNrLocalVars
      << ", toralNrLocalMatrixEntry=" << toralNrLocalMatrixEntry
      << ", totalNrRHS=" << totalNrRHS);

  mapLocalVarsToGlobalVector_.resize(totalNrLocalVars);
  mapLocalVarsToGlobalMatrix_.resize(toralNrLocalMatrixEntry);
  mapLocalVarsToGlobalRHS_.resize(totalNrRHS);

  // now map all the local indices to unknown vectors and matrix indices
  for (i = 0; i < netlist_->getNrDevices(); i++) {
      Device* device = netlist_->getDevice(i);
      // to provide the local variables
      for (j = 0; j < nrLocalVars_[i]; j++) {
          MYINT tmp = device->getMapLocalVarsToGlobalVar( j, netlist_->getPariID(), analysis_);
          indx  = ( (tmp >= 0) ? matrixMap->getColIndexForUnkID(tmp) : -1 );
          mapLocalVarsToGlobalVector_[mapLocalVarsStartIndex_[i] + j] = indx;
          SIM_PRINT_L4(verb(), "Element i=" << i << " local var=" << j << " col indx=" << indx);
      }
      // to provide the RHS index
      // here we call again getAnalysisStampRHS, such that we see which entry is is bound to which row
      netlist_->getDevice(i)->getAnalysisStampRHS( stampRow, entryType,
            potentialEntryValues, nrStampsElements, analysis_);
      // loop again over each RHS entry
      for (j = 0; j < mapLocalRHSStartIndex_[i+1] - mapLocalRHSStartIndex_[i]; j++) {
          MYINT tmp = device->getMapLocalVarsToGlobalVar( stampRow[j], netlist_->getPariID(), analysis_);
          indx  = ( (tmp >= 0) ? matrixMap->getRowIndexForUnkID(tmp) : -1 );
          mapLocalVarsToGlobalRHS_[mapLocalRHSStartIndex_[i] + j] = indx;
          SIM_PRINT_L4(verb(), "Element i=" << i << " local var=" << j << " RHS row indx=" << indx);
      }
      // now use the matrix
      device->getAnalysisStamp(stampColumn, stampRow, entryType,
                  potentialEntryValues, nrStampsElements, analysis_);
      for (j = 0; j < nrStampsElements; j++)
        {
          MYINT tmp  = device->getMapLocalVarsToGlobalVar( stampColumn[j], netlist_->getPariID(), analysis_);
          MYINT tmp1 = device->getMapLocalVarsToGlobalVar( stampRow[j]   , netlist_->getPariID(), analysis_);
          MYINT linearInd = analysisMatrix->getDummyLinearIndex();
          if ( (tmp >= 0) && (tmp1 >= 0))
            { // only when GND is not implied
              indx  = matrixMap->getColIndexForUnkID(tmp);
              indx1 = matrixMap->getRowIndexForUnkID(tmp1);
              // get the matrix linear index
              linearInd = analysisMatrix->getLinearIndex(indx1, indx);
            }
          //store the matrix index
          mapLocalVarsToGlobalMatrix_[mapLocalMatrixStartIndex_[i] + j] = linearInd;
          SIM_PRINT_L4(verb(), "Element i=" << i << " local stamp=" << j
              << " [" << stampColumn[j] << "," <<  stampRow[j]
              << "] globalVarInd[" << tmp << "," << tmp1 << "] matrixIndex[" << indx << "," << indx1
              << "] linearIndex = " << linearInd);
        }
  }

  // object was already initialized
  isInitialized_ = true;
}

