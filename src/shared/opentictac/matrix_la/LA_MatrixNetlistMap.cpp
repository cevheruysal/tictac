/*
 * LA_MatrixNetlistMap.cpp
 *
 *  Created on: Oct 2, 2013
 *      Author: benk
 */

#include "LA_MatrixNetlistMap.hpp"
#include "Netlist.hpp"
#include "LA_MatrixBasis.hpp"

LA_MatrixNetlistMap::LA_MatrixNetlistMap(
    const Netlist *netlist,
    MYINT rowNr,
    std::vector<MNAVarType>& vartypes,
    std::vector<MYINT>& nodeToUnknown,
    std::vector<MYINT>& currentsToUnknown,
    std::vector<MYINT>& colMap,
    std::vector<MYINT>& rowsMap):
    UT_SIMVerboseClass("MatrixNetlistMap"), netlist_(netlist),
    rowAndColNr_(rowNr), vartypes_(vartypes)
{
  // total number of variables
  nrTotalVars_ = rowAndColNr_;
  // all nodes but not the pin
  nrNodesNoGnd_ = netlist_->getNrNodesNoGnd();
  // test if the nrnodes > nr. Varibles
  SIM_ERROR_COND_STOP( nrTotalVars_ < netlist_->getNrNetlistVariables() , "Ctor LA_MatrixNetlistMap nrTotalVars="
      << nrTotalVars_ << " < nrNodes=" << netlist_->getNrNetlistVariables());

  // count first the voltages and the currents
  MYINT nrVoltageUnkn = 0, nrCurrentUnkn = 0, i;
  MYINT nrUnknowns = rowAndColNr_;
  for (i = 0; i < (MYINT)vartypes_.size(); i++)
    {
      if ((vartypes_[i] == EXT_VOLTAGE) || (vartypes_[i] == INT_VOLTAGE))
        nrVoltageUnkn++;
      if (vartypes_[i] == INT_CURRENT)
        nrCurrentUnkn++;
    }
  // test if input is consistent
  SIM_ERROR_COND_STOP(  (nrCurrentUnkn != (MYINT)currentsToUnknown.size()) ,
      "Ctor LA_MatrixNetlistMap, nrCurrentUnkn " << nrCurrentUnkn << "!=" << currentsToUnknown.size() );
  SIM_ERROR_COND_STOP( (nrVoltageUnkn != (MYINT)nodeToUnknown.size()),
      "Ctor LA_MatrixNetlistMap, nrVoltageUnkn " << nrVoltageUnkn << "!=" << nodeToUnknown.size());
  SIM_ERROR_COND_STOP( (nrUnknowns != (MYINT)rowsMap.size()),
      "Ctor LA_MatrixNetlistMap, nrUnknowns " << nrUnknowns << "!=" << rowsMap.size());
  SIM_ERROR_COND_STOP( (nrUnknowns != (MYINT)colMap.size()),
      "Ctor LA_MatrixNetlistMap, nrUnknowns " << nrUnknowns << "!=" << colMap.size());

  // copy the two vectors
  pinID_To_UnknID_ = nodeToUnknown;
  currentID_To_UnknID_ = currentsToUnknown;
  row_To_unknID_ = rowsMap;
  col_To_unknID_ = colMap;
  // Build the reverse map from the unknown ID to current and voltage unknown IDs
  //
  unknID_To_PinID_.resize(nrUnknowns,-1);
  unknID_To_currentID_.resize(nrUnknowns,-1);
  unknID_To_row_ .resize(nrUnknowns,-1);
  unknID_To_col_.resize(nrUnknowns,-1);
  for (i = 0; i < netlist_->getNrNodesNoGnd(); i++)
    unknID_To_PinID_[pinID_To_UnknID_[i]] = i;
  for (i = 0; i < netlist_->getNrCurrents(); i++)
    unknID_To_currentID_[currentID_To_UnknID_[i]] = i;
  for (i = 0; i < nrUnknowns; i++) {
      unknID_To_row_[row_To_unknID_[i]] = i;
      unknID_To_col_[col_To_unknID_[i]] = i;
  }
}

LA_MatrixNetlistMap::~LA_MatrixNetlistMap()
{
  // memory free is automatic
}

