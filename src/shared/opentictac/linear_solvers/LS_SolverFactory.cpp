/*
 * LS_SolverFactory.cpp
 *
 *  Created on: Oct 2, 2013
 *      Author: benk
 */

#include "LS_SolverFactory.hpp"
#include "LA_CRSMatrix.hpp"
#include "LA_MatrixNetlistMap.hpp"
#include "LS_LUSolver_STUDENT.hpp"

LS_SolverFactory::LS_SolverFactory(): UT_SIMVerboseClass("SolverFactory")
{
  nrMaxMatrixStampSize_ = 0;

  // Actual nothing to do
  //this->setVerb(10);
}

LS_SolverFactory::~LS_SolverFactory()
{
  // Actual nothing to do
}

LS_SolverBase<MYREAL>* LS_SolverFactory::factorSolverReal(
    const AN_Analysis* analysis, const Netlist* netlist,
    [[maybe_unused]] const IN_SimOptions* simoptions)
{
  // actual we will produce only CRS solvers
  SIM_PRINT_L3(verb()," LS_SolverFactory::factorSolverReal ");

  [[maybe_unused]] Analysis_type analType = analysis->getAnalysisType();
  MYINT nrRows, nrCols, nnz, nrES = 0, i = 0, j = 0;
  MYINT partiID = netlist->getPariID();
  std::vector< std::set<MYINT> > rows;
  std::vector< std::set<MYINT> > entriesNotToUseForDiagonal;
  std::vector<MYINT> unknownMap, rhsMap; // input is the conventional index output is the new map to ensure diagonals
  std::vector<MYINT> rowPtr, colPtr, ESCol, ESRow;
  std::vector<MYINT> outrowPtr, outcolPtr;
  std::vector<MatrixEntryType> entryType;
  std::vector<MYREAL> potentialEntryValues;
  std::set<MYINT>::iterator it, it1;

  // needed for the Matrix-Netlist map
  std::vector<MYINT> nodeToUnknown;
  std::vector<MYINT> currentsToUnknown;
  std::vector<MNAVarType> vartypes;

  // set to zero the actual maximal
  nrMaxMatrixStampSize_ = 0;

  // this is the total number of unknowns
  nrRows = nrCols = netlist->getNrNetlistVariables();
  rows.resize(nrRows); entriesNotToUseForDiagonal.resize(nrRows);
  // build the first simple map
  unknownMap.resize(nrRows,-1);
  rhsMap.resize(nrRows,-1);
  for (i = 0; i < nrRows; i++)
    rhsMap[i] = unknownMap[i] = i;

  // now iterate over each element and build information for each row
  for (i = 0; i < netlist->getNrDevices(); i++)
    {
      // call the device's function to get the local stamp
      netlist->getDevice(i)->getAnalysisStamp(ESCol, ESRow,
          entryType, potentialEntryValues, nrES, analysis);
      // build the maximum to get the maximal matrix stamp
      nrMaxMatrixStampSize_ = SIMMAX(nrMaxMatrixStampSize_, nrES);
      // for each row stamp add the column index
      for (j = 0; j < nrES; j++)
        {
          // we need to map the local variables to global IDs (global to the actual system)
          MYINT globRow = netlist->getDevice(i)->getMapLocalVarsToGlobalVar(ESRow[j], partiID, analysis);
          MYINT globCol = netlist->getDevice(i)->getMapLocalVarsToGlobalVar(ESCol[j], partiID, analysis);
          SIM_PRINT_L3(verb()," globRow = " << globRow << " globCol = " << globCol
                           << " lr=" << ESRow[j] << " lc=" << ESCol[j]);

          // consider the GROUND pin ... this should NOT be in the matrix
          if ((globRow < 0) || (globCol < 0)) continue;

          it = rows[globRow].find(globCol);
          it1 = entriesNotToUseForDiagonal[globRow].find(globCol);
          // IF the absolute value is too low then this should not be used for diagonal
          if (    (potentialEntryValues[j] < SIM_ABS_DIAG_LIMIT)
               && (  ((      it == rows[globRow].end())
                        &&  (it1 == entriesNotToUseForDiagonal[globRow].end())
                     ) ||
                     ((      it != rows[globRow].end())
                        &&  (it1 != entriesNotToUseForDiagonal[globRow].end())
                     )
                  ))
            entriesNotToUseForDiagonal[globRow].insert(globCol);
          else
            entriesNotToUseForDiagonal[globRow].erase(globCol);
          // adds to the row
          rows[globRow].insert(globCol);
        }
    }
  // to ensure diagonal entries make now only row swapping
  MYINT inverseSwap = 0; // variable to monitor if there is an infinite loop
                           // in the diagonal entry search
  for (i = 0; i < nrRows; i++)
    { // loop through each row and ensure that there is a diagonal entry in each row
      MYINT rowSwap = i;
      std::set<MYINT> &tmpSet = rows[i];
      it = tmpSet.find(i);
      if (it == tmpSet.end())
        { // NO diagonal entry
          SIM_PRINT_L3(verb()," NO DIAGONAL ENTRY IN ROW " << i);
          bool foundRow = false;
          // find a row where there is a stable diagonal entry
          for (j = nrRows-1; j >= 0; j--) {
            it = rows[j].find(i);
            it1 = entriesNotToUseForDiagonal[j].find(i);
            if ((it != rows[j].end()) && (it1 == entriesNotToUseForDiagonal[j].end()))
              {// make a row swap, so we only swap the rhs
                MYINT tmp = rhsMap[i];
                rhsMap[i] = rhsMap[j]; rhsMap[j] = tmp;
                std::set<MYINT> tmpSwapSet = rows[i];
                rows[i] = rows[j];
                rows[j] = tmpSwapSet;
                foundRow = true;
                if (j < i) // might here be an infinite loop, we test it
                  {
                    SIM_PRINT_L3(verb(),"Swap i= " << i << " j= " << j);
                    inverseSwap++;
                    SIM_ERROR_COND_STOP( inverseSwap > nrRows, " Internal error while looking for diagonal entries"
                        << " for diagonal " << i);
                  }
                // reset the counter of the for loop
                i = SIMMIN(i,j);
                break;
              }
          }
          // if not found then we have a problem
          SIM_ERROR_COND_STOP( (foundRow == false), "No row swaping found (missing diagonal element) "
              "for row " << rowSwap << ", Matrix is probably singular row=" << i);
        }
    } // end of finding a diagonal entry


  //generate: nrRows, nrCols, nnz, rowPtr, colPtr, such that there is always a diagonal entry
  nnz = 0;
  // count all the elements
  for (i = 0; i < nrRows; i++)
    // measure the number of nonzeros
    { nnz = nnz + rows[i].size(); }
  colPtr.resize(nnz,-1);
  rowPtr.resize(nrRows+1,-1);
  MYINT poscol = 0;
  for (i = 0; i < nrRows; i++)
    {
      MYINT len = rows[i].size();
      rowPtr[i] = poscol;
      rowPtr[i+1] = poscol + len;
      std::set<MYINT>::iterator it = rows[i].begin();
      // in each row copy the column
      for (; it != rows[i].end(); ++it)
        colPtr[poscol++] = *it;
      // sort the row indices, such that we get incremental representation
      // required by CRS!!
      sim_sort(rowPtr[i], rowPtr[i+1], colPtr);

      if (verb() > 4){
          std::string row = sim_printvector(rowPtr[i], rowPtr[i+1], colPtr);
          SIM_PRINT_L3(verb(),"Row i=" << i << " " << row );
      }
    }

  // the last element must always be nnz
  rowPtr[nrRows] = nnz;
  // now the rowPtr and colPtr are filled

  // print the mappings
  if (verb() > 4){
      std::string tmp = "";
      for (i = 0; i < nrRows; i++)
        tmp += sim_integerToString(unknownMap[i]) + ", ";
      SIM_PRINT_L3(verb(),"unknownMap = {" << tmp << "};" );
      tmp = "";
      for (i = 0; i < nrRows; i++)
        tmp += sim_integerToString(rhsMap[i]) + ", ";
      SIM_PRINT_L3(verb(),"rhsMap = {" << tmp << "};" );
  }

  MYINT nnzOut = 0;
  // TODO: the Markowitz might not always be needed ... e.g. for GMRES
  // create the Markovitz matrix HERE
  this->makeMarkowitz(rowPtr, colPtr, nrRows, nnz,
                      unknownMap, rhsMap,
                      outrowPtr, outcolPtr, nnzOut);


  // Here we set the maps from the netlist to the Matrix
  // set the Rhs and the unknown maps !!!
  nodeToUnknown.resize( netlist->getNrNodesNoGnd(), -1);
  currentsToUnknown.resize(netlist->getNrCurrents(), -1 );
  vartypes.resize( netlist->getNrNetlistVariables());

  for (i = 0; i < netlist->getNrExternalNodes()-1; i++) { //-1 due to GND
      nodeToUnknown[i] = unknownMap[i];
      vartypes[unknownMap[i]] = EXT_VOLTAGE;
  }
  for (i = netlist->getNrExternalNodes()-1; i < netlist->getNrNodesNoGnd(); i++) {
      nodeToUnknown[i] = unknownMap[i];
      vartypes[unknownMap[i]] = INT_VOLTAGE;
  }
  for (i = 0; i < netlist->getNrCurrents(); i++) {
      currentsToUnknown[i] = unknownMap[i + netlist->getNrNodesNoGnd()];
      vartypes[unknownMap[i + netlist->getNrNodesNoGnd()]] = INT_CURRENT;
  }

  // create the map between the matrix (after row and column swapping )
  LA_MatrixNetlistMap *map = new LA_MatrixNetlistMap(netlist, nrRows, vartypes,
                               nodeToUnknown, currentsToUnknown, unknownMap, rhsMap);
  // matrix with fill-ins
  LA_CRSMatrix<MYREAL> *matrix = new LA_CRSMatrix<MYREAL>(nrRows, nrCols, nnzOut, outrowPtr, outcolPtr, map);

  // print the mappings
  if (verb() > 4){
      std::string tmp = "";
      for (i = 0; i < nrRows; i++)
        tmp += sim_integerToString(unknownMap[i]) + ", ";
      SIM_PRINT_L3(verb(),"unknownMap = {" << tmp << "};" );
      tmp = "";
      for (i = 0; i < nrRows; i++)
        tmp += sim_integerToString(rhsMap[i]) + ", ";
      SIM_PRINT_L3(verb(),"rhsMap = {" << tmp << "};" );
  }
  if (verb() > 2){
      // print the matrix for testing purposes
      std::string filename = "dummy.m";
      matrix->printMatrix(filename);
  }

  LS_LUSolver_STUDENT<MYREAL> *solver = new LS_LUSolver_STUDENT<MYREAL>(matrix);

  // return the created solver
  return solver;
}


// ====================== MARKOWITZ =========================

void LS_SolverFactory::makeMarkowitz(
    const std::vector<MYINT>& inRowPtr,
    const std::vector<MYINT>& inColPtr,
    MYINT nrRows,
    [[maybe_unused]] MYINT nnz,
    // ------ inout -----
    std::vector<MYINT>& unknownMap,
    std::vector<MYINT>& rhsMap,
    // ------ output ----------
    std::vector<MYINT>& outRowPtr,
    std::vector<MYINT>& outColPtr,
    MYINT &nnzOut     )
{

  std::vector< std::set<MYINT> > rows(nrRows); // vector of maps containing the NNZ elements
  MYINT i, j;
  std::set<MYINT>::iterator it, it1;
  std::vector<MYINT> unkSwap(nrRows,-1);

  // we assume that at the diagonal there are stable/dominant elements ...

  SIM_PRINT_L3(verb(), "Start Markowitz");
  for (i = 0; i < nrRows; i++)
    { // build the set of each row that represents the nonzero elements
      for (j = inRowPtr[i]; j < inRowPtr[i+1]; j++)
        rows[i].insert(inColPtr[j]);
      unkSwap[i] = i;
    }

  // here we start the loop for each row
  for (i = 0; i < nrRows; i++)
    {
      // this row must have a diagonal entry
      it = rows[i].find(i);
      SIM_ERROR_COND_STOP( it == rows[i].end(), "makeMarkowitz row i=" << i << " has no diagonal entry");

      //first get the rows which are affected by the actual row "i"
      std::vector<MYINT> affectedRows(0);
      MYINT nrAffectedRows = 0;
      SIM_PRINT_L3(verb(), " Take row:" << i);
      // we start searching from the i=th row and column
      for (j = i; j < nrRows; j++)
        { // see which rows are affected by this
          it = rows[j].find(i);
          if (it != rows[j].end()) {
            affectedRows.push_back(j);
            nrAffectedRows++;
            SIM_PRINT_L3(verb(), "for row " << i << " affected row " << j << " , " << nrAffectedRows);
          }
        }


      // === now choose among the affected rows which would produce the least fill-ins ====

      // the indices for the row and column swapping
      MYINT oldInd = i, newInd = -1, actualFillInNumber = SIMMAX(nrRows*nrRows+2,100000000);

      // loop through each affected rows, and which are the candidates for swap
      for (j = 0; j < nrAffectedRows; j++)
        { // for each affected row look how much fill-ins would be produced
          // IMPORTANT:through the column swap we need to look in the actual row
          MYINT rowIndTest = affectedRows[j], rowI, nrFillIns = 0;
          // for one row from the affected rows look how many fill-ins this would produce
          for (rowI = i; rowI < nrRows; rowI++)
            {
              it = rows[rowI].find(rowIndTest);
              // look if row "rowI" would be affected by "rowIndTest" if we would make the row replacement
              if (it != rows[rowI].end()) {
                // now count the number of fill-ins
                // TODO: this loop cold be done more efficiently
                // TODO: if we would used sorted arrays then we could achieve more speedup
                for(it = rows[rowIndTest].begin(); it != rows[rowIndTest].end(); it++)
                  // only for elements greater than i, other are already dealt with
                  if (*it >= i) {
                    it1 = rows[rowI].find(*it);
                    SIM_PRINT_L5(verb(), "look for " << *it << " in row " << rowI
                        << " res=" << (it1 != rows[rowI].end()));
                    // here we check if this row would produce any additional fill-ins,
                    // if yes we count it
                    // if the element of "rowIndTest" is not in row "rowI" then we have one fill-in
                    nrFillIns = nrFillIns + ((it1 != rows[rowI].end()) ? 0 : 1);
                  }
              }
              SIM_PRINT_L6(verb(), "row " << rowIndTest << " in row " << rowI << " produced fill-ins=" << nrFillIns);
            }
          // if this is the actual "winner" row then store the index and
          if (actualFillInNumber > nrFillIns) {
              actualFillInNumber = nrFillIns;
              newInd = rowIndTest;
              SIM_PRINT_L5(verb(), "actual winner = " << rowIndTest);
          }
        } // end of loop over the affected rows

      // ===== make the row and column swapping, only when there are different indices
      SIM_PRINT_L4(verb(), " Make swap " << oldInd << " <--> " << newInd);
      if (oldInd != newInd)
        {
          // swap the permutation vector for unknown
          MYINT tmpI = unkSwap[oldInd];
          unkSwap[oldInd] = unkSwap[newInd]; unkSwap[newInd] = tmpI;
          tmpI = unknownMap[oldInd]; unknownMap[oldInd] = unknownMap[newInd]; unknownMap[newInd] = tmpI;
          // swap the rhs permutations
          tmpI = rhsMap[oldInd]; rhsMap[oldInd] = rhsMap[newInd]; rhsMap[newInd] = tmpI;
          // swap the sets, ROW SWAPING
          std::set<MYINT> tmpSet = rows[oldInd];
          rows[oldInd] = rows[newInd]; rows[newInd] = tmpSet;
          // COLUMN swaping
          for (j = 0; j < nrRows; j++)
            { // in each row make the column swap
              it = rows[j].find(oldInd);
              it1 = rows[j].find(newInd);
              // make insert and erase only when this is require, when both of these indices
              // are in the row of none of them then there is nothing to do
              if (it == rows[j].end() && it1 != rows[j].end())
                {
                  SIM_PRINT_L4(verb(), "1 oldInd=" << oldInd << " <--> " << newInd);
                  rows[j].erase(newInd); rows[j].insert(oldInd);
                }
              if (it != rows[j].end() && it1 == rows[j].end())
                {
                  SIM_PRINT_L4(verb(), "2 oldInd=" << oldInd << " <--> " << newInd);
                  rows[j].erase(oldInd); rows[j].insert(newInd);
                }
            }
          // if we had a swap than recreate the list of affected rows (since we also had column swap)
          nrAffectedRows = 0;
          affectedRows.clear();
          for (j = i; j < nrRows; j++)
            { // see which rows are affected by this
              it = rows[j].find(i);
              if (it != rows[j].end()) {
                affectedRows.push_back(j);
                nrAffectedRows++;
                SIM_PRINT_L3(verb(), "recreate for row " << i << " affected row " << j << " , " << nrAffectedRows);
              }
            }
        }

      // ==== make the fill-in generation in all rows > i and columns > i
      for (j = 0; j < nrAffectedRows; ++j)
        {
          MYINT rowI = affectedRows[j];
          std::set<MYINT> &rowtarget = rows[rowI];
          SIM_PRINT_L4(verb(), " i=" << i << " j=" << j << " rowI=" << rowI << " nrAffectedRows=" << nrAffectedRows);
          // this row has "i" column element so insert the rest
          for (it = rows[oldInd].begin(); it != rows[oldInd].end(); ++it)
            { // for each element of row "i" if they are grater than i add to the
              // lower rows
              if (*it > i) {
                  SIM_PRINT_L4(verb(), " fill-in in row rowI=" << rowI << " it=" << (*it) << " oldInd=" << oldInd << " <--> " << newInd);
                  rowtarget.insert(*it);
              }
            }
        }
    }

  // ==== make a fill-in test and report eventual errors ====
  // ===== this test is inactive for large matrixes
  for (i = 0; (i < nrRows) && (nrRows < 20001); i++)
    {
      // make the test for this
      std::set<MYINT> &rowElimin = rows[i];
      std::set<MYINT>::iterator iterTmp;
      for (j = i+1; j < nrRows; j++)
        {
          std::set<MYINT> &rowTarget = rows[j];
          it = rowElimin.begin();
          it1 = rowTarget.find(i);
          // if this does not have column "i" no elimination is needed and jump to the next
          if (it1 == rowTarget.end())
            continue;
          for (;it != rowElimin.end(); ++it)
            if (*it >= i)
              {
                // now it is important that (*it) is in "rowTarget"
                iterTmp = rowTarget.find(*it);
                if (iterTmp == rowTarget.end())
                  { // --- THIS IS THE ERROR: no fills where there should be one ----
                    // print out the two rows
                    std::string outTmp = "";
                    for (iterTmp = rowElimin.begin(); iterTmp!=rowElimin.end(); ++iterTmp)
                      outTmp += sim_integerToString(*iterTmp) + ",";
                    SIM_PRINT_L0(2,"Elim Row = {" << outTmp << "}");
                    outTmp = "";
                    for (iterTmp = rowTarget.begin(); iterTmp!=rowTarget.end(); ++iterTmp)
                      outTmp += sim_integerToString(*iterTmp) + ",";
                    SIM_PRINT_L0(2,"Target Row = {" << outTmp << "}");
                    SIM_ERROR_STOP( "makeMarkowitz error column(fill-in)=" << *it
                        << " should be in row=" << j << " triggered by row=" << i);
                  }
              }
        }
    }

  // ===== create CRS structure including the fill-ins
  nnzOut = 0;
  // measure the number of nonzeros
  for (i = 0; i < nrRows; i++)
    { nnzOut = nnzOut + rows[i].size(); }

   outColPtr.resize(nnzOut,-1);
   outRowPtr.resize(nrRows+1,-1);
   MYINT poscol = 0;
   for (i = 0; i < nrRows; i++)
     {
       MYINT len = rows[i].size();
       outRowPtr[i] = poscol;
       outRowPtr[i+1] = poscol + len;
       std::set<MYINT>::iterator it = rows[i].begin();
       // in each row copy the column
       for (; it != rows[i].end(); ++it)
         outColPtr[poscol++] = *it;
       // sort the row indices, such that we get incremental representation
       // required by CRS!!
       sim_sort(outRowPtr[i], outRowPtr[i+1], outColPtr);
     }

   // the last element must always be nnz
   outRowPtr[nrRows] = nnzOut;

   SIM_PRINT_L3(verb(), "End Markowitz");
}
