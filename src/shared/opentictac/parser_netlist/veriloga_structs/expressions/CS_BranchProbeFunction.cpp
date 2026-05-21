/*
 * CS_BranchProbeFunction.cpp
 *
 *  Created on: Dec 18, 2012
 *      Author: benk
 */

#include "CS_BranchProbeFunction.hpp"
#include "CS_Number.hpp"
#include "CS_BinaryOperator.hpp"
#include "CS_UnaryOperator.hpp"
#include "sim_utils.hpp"

CS_BranchProbeFunction::CS_BranchProbeFunction(const CS_ModuleDeclaration* module, int startLine, int startColumn,
    BranchFunctionType bt, std::string branchName,
    CS_AnalogPortReference* p1, CS_AnalogPortReference* p2):
    CS_ExpressionBasis(module, startLine, startColumn),
    bt_(bt), branchName_(branchName), p1_(p1), p2_(p2), signInstance_(1)
{
  // make checks, that the expressions are OK
  VERILOG_ERROR_COND_STOP( (bt_==VERILOG_BRANCH_I_EXT_PROBE) && (p2_!=0) ,
      "Port Probe Function Call is allowed to have only one argument e.g., I(<p>)", this);

  // test also if the discipline is continous
  VERILOG_ERROR_COND_STOP( !p1->getPortObject()->getDiscipline()->isDisciplineContinous(),
      "Discipline \"" << p1->getPortObject()->getDiscipline()->getDisciplineName()
      << "\" of Net \"" << p1->getPortObject()->getName() << "\" must have continuous binding!",
      this);

  // test if both have the same discipline, by simply comparing the name!
  if (p2 != 0) {
    VERILOG_ERROR_COND_STOP(
        p1->getPortObject()->getDiscipline()->getDisciplineName()
        != p2->getPortObject()->getDiscipline()->getDisciplineName(),
        "In a Branch probe both nets must have the same discipline!", this);
    VERILOG_ERROR_COND_STOP( !p2->getPortObject()->getDiscipline()->isDisciplineContinous(),
        "Discipline \"" << p2->getPortObject()->getDiscipline()->getDisciplineName()
        << "\" of Net \"" << p2->getPortObject()->getName() << "\" must have continuous binding!",
        this);
  }
  //this->setVerb(6);
}

CS_AKDependency CS_BranchProbeFunction::propagateDependencies(){
  // for branch probes the Collect system Variable visitor should set the dependencies
  CS_AKDependency ret_tmp = this->getActVarDependency();
  // return the set (it is already stored in this node )
  return ret_tmp;
}


int CS_BranchProbeFunction::getPossibleBranches(
    std::vector<int>& arrayP1, std::vector<int>& arrayP2, std::vector<std::string>& brNames)
{
  int brI1 = -1;
  int brI2 = -1;
  int loopRange_br1 = 1;
  int loopRange_br2 = 1;
  int ret_Size = -1;

  // P1
  loopRange_br1 = p1_->getPortObject()->getAkIndex() + p1_->getPortObject()->getSize();
  brI1 = p1_->getPortObject()->getAkIndex();
  // test if p1 this is indexed
  if (p1_->getIndexerExpr()){
      int indexType = p1_->getIndexerExpr()->getClasses();
      // if we have a constant indexing then use this index
      if ( indexType <= CONSTANT )
        { // the expression is a constant so it can be evaluated
          int evalVal = p1_->getPortObject()->getCorrectArrayIndex_int(
                                 (int)p1_->getIndexerExpr()->evalConst() );
          VERILOG_PRINT_L3( verb(), " S,O,I: " << p1_->getPortObject()->getSize()
                     << " , " << p1_->getPortObject()->getOffset()
                     << " , " <<  p1_->getIndexerExpr()->evalConst() );

          brI1 = brI1 + evalVal;
          loopRange_br1 = brI1 + 1;
          VERILOG_PRINT_L3( verb(), " p1 CONST :" << evalVal );
        }
  }

  // P2 ... test if the second branch is null
  if (p2_ == 0)
    { // the second branch is ground, therefore test only the first branch
      brI2 = -1;
      loopRange_br2 = 0;
    }
  else
    {// the second port is NOT ground
      brI2 = p2_->getPortObject()->getAkIndex();
      loopRange_br2 = p2_->getPortObject()->getAkIndex() + p2_->getPortObject()->getSize();
      // test if P2 is indexed
      if (p2_->getIndexerExpr())
        {
          int indexType = p2_->getIndexerExpr()->getClasses();
          // if we have a constant indexing then use this index
          if ( indexType <= CONSTANT )
            { // the expression is a constant so it can be evaluated
              int evalVal = p2_->getPortObject()->getCorrectArrayIndex_int(
                                    (int)p2_->getIndexerExpr()->evalConst() );
              brI2 = brI2 + evalVal;
              loopRange_br2 = brI2 + 1;
              VERILOG_PRINT_L3( verb(), " p2 CONST : " << evalVal );
            }
        }
    }

  // compute the size of the matrix
  ret_Size = (brI1 - loopRange_br1) * (brI2 - loopRange_br2);
  //VERILOG_PRINT_L3( verb()," ret_Size :" << ret_Size );
  arrayP1.resize(ret_Size);
  arrayP2.resize(ret_Size);
  brNames.resize(ret_Size);
  ret_Size = 0;
  // for all branch combinations, (if it is indexed and the index is not constant)
  int i = 0;
  for ( /*brI1 = p1->getPortObject()->getAkIndex()*/ ; brI1 < loopRange_br1 ; brI1++)
    for ( /*brI2 = p2->getPortObject()->getAkIndex()*/ ; brI2 < loopRange_br2 ; brI2++)
      {
        // brI1, and brI2 are not allowed to be the same
        if (brI1 != brI2){
            arrayP1[i] = brI1;
            arrayP2[i] = brI2;
            brNames[i]   = branchName_;
            i++;
            ret_Size++;
        }
      }
  // returns the size of the array
  return ret_Size;
}

CS_ExpressionBasis* CS_BranchProbeFunction::doDifferentiate(int sysVarIndex, [[maybe_unused]] int *flag)
{
  // test if this dependent on the chosen system variable
  if (this->getActVarDependency().isSysVarIndexPresent(sysVarIndex) < 0)
    return 0;

  CS_ExpressionBasis *retExpr = 0;
  CS_ExpressionBasis *one = new CS_Number(1.0, getModule(), getStartLine(), getStartColumn());

  VERILOG_PRINT_L3( verb() ,"doDifferentiate , sysVarIndex=" << sysVarIndex
      << " Dep:" << this->getActVarDependency().toString() << " type=" << bt_);

  // depending on the type of this probe function
  switch (bt_)
  {
    case VERILOG_BRANCH_V_TWO:
      {
        //int v1 = (p1_->getPortObject()->getPortType() != (int)PORT_GROUND) ? p1_->getPortObject()->getAkIndex() : -1;
        //int v2 = (p2_->getPortObject()->getPortType() != (int)PORT_GROUND) ? p2_->getPortObject()->getAkIndex() : -1;
        // these port objects can be indexed objects, and these indices depend not just on the index,
        // but also on the way how the port is defined e.g, [0:3] or [3:0], therefore
        // when this index is computed it NEEDS to be stored in the p1_ and p2_ objects
        int v1 = (p1_->getPortObject()->getPortType() != (int)PORT_GROUND) ? p1_->getAkIndex() : -1;
        int v2 = ((p2_ != NULL) && (p2_->getPortObject()->getPortType() != (int)PORT_GROUND)) ? p2_->getAkIndex() : -1;
        if ( v1 == sysVarIndex ) {
            VERILOG_PRINT_L3( verb() ,"1 v1=" << v1 << " , v2=" << v2 << " , sysVarIndex=" << sysVarIndex
                << " t=" << p1_->getPortObject()->getPortType() << " p1=" << p1_->getAkIndex() );
            retExpr = one;
        }
        else{
            VERILOG_PRINT_L3( verb() ,"2 v1=" << v1 << " , v2=" << v2 << " , sysVarIndex=" << sysVarIndex
                << " t=" << p2_->getPortObject()->getPortType() << " p2=" << p2_->getAkIndex() << " n=");
            retExpr = new CS_UnaryOperator( VERILOG_UNAR_MIN, one, getModule(), getStartLine(), getStartColumn());
        }
        break;
      }
    case VERILOG_BRANCH_V_ONE:
      { // this must be only one, the derivative is just +1.0;
        retExpr = one;
        break;
      }
    case VERILOG_BRANCH_I_ONE:
    case VERILOG_BRANCH_I_TWO:
    case VERILOG_BRANCH_I_EXT_PROBE:
      {
        // since this is a current probe there must be already a system variable
        if ( this->getInstanceSign() > 0 )
          retExpr = one;
        else
          // same branch but with different order so derivative is -1
          retExpr = new CS_UnaryOperator( VERILOG_UNAR_MIN, one, getModule(), getStartLine(), getStartColumn());
        break;
      }
    /*case VERILOG_BRANCH_I_EXT_PROBE:
      {
        // there are no derivatives, this is a simple function call without derivatives
        break;
      }*/
  }
  // we should not return null pointer
  if (retExpr == 0)
    retExpr = new CS_Number(0.0, getModule(), getStartLine(), getStartColumn());

  // return the expression
  return retExpr;
}


std::string CS_BranchProbeFunction::getAkTypeString() const
{
  std::string retStr = "";
  switch (bt_){
     case VERILOG_BRANCH_V_TWO:
     case VERILOG_BRANCH_V_ONE:
       {
         retStr = "VAMSEVAL_CONTRIB_V";
         break;
       }
     case VERILOG_BRANCH_I_ONE:
     case VERILOG_BRANCH_I_TWO:
       {
         retStr = "VAMSEVAL_CONTRIB_I";
         break;
       }
    default:
      SIM_WARNING("Please check if we should do something in this case!");
   }
  // return the generated string
  return retStr;
}
