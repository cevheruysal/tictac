/*
 * AN_Analysis.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: benk
 */

#include "AN_Analysis.hpp"
#include "LS_SolverBase.hpp"
#include "LA_MatrixBasis.hpp"
#include "LA_MatrixNetlistMap.hpp"
#include "Netlist.hpp"


void AN_Analysis::copyOneStateToOther(
    const AN_Analysis *source, AN_Analysis* dest,
    [[maybe_unused]] Netlist *netlist
    )
{
  const std::vector<MYREAL> &srcV = source->getState();
  std::vector<MYREAL>       &destV = dest->getState();
  // we use the global "0" solver ...
  const LA_MatrixNetlistMap *srcM = source->getRealSolver(0)->getSolverMatrix()->getMatrixMap();
  const LA_MatrixNetlistMap *destM = dest->getRealSolver(0)->getSolverMatrix()->getMatrixMap();
  std::vector<MYINT> unknowTOunknown;
  MYINT i;

  unknowTOunknown.resize(destM->getNrColAndRow(),-1);
  for (i = 0; i < destM->getNrColAndRow(); i++){
      // TODO: make this better, the mapping from unknown to unknown
      unknowTOunknown[i] = i;
  }

  // sizes should match, or not ... ?
  SIM_ERROR_COND_STOP(boost::numeric_cast<decltype(destV.size())>(destM->getNrColAndRow()) != destV.size(), "AN_Analysis::copyOneStateToOther size do not match " <<
      destM->getNrColAndRow() << " != " << destV.size());
  // here just map from unknown index of one analysis to the unknown index of the other analysis
  for (i = 0; i < destM->getNrColAndRow(); i++){
      MYINT sourceColInd = srcM->getColIndexForUnkID(unknowTOunknown[i]);
      MYINT destolInd    = destM->getColIndexForUnkID(i);
      destV[destolInd]     = srcV[sourceColInd];
  }
}
