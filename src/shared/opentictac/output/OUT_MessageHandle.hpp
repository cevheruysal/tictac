/*
 * OUT_MessageHandle.hpp
 *
 *  Created on: Oct 25, 2013
 *      Author: benk
 */

#ifndef OUT_MESSAGEHANDLE_HPP_
#define OUT_MESSAGEHANDLE_HPP_

#include "sim_utils.hpp"
#include "LS_SolverBase.hpp"


class Netlist;
class AN_Analysis;

/** class to handle all the messages*/
class OUT_MessageHandle
{
public:

  /** empty Ctor since this class will operate with static methods */
  OUT_MessageHandle();

  /** empty Dtor */
  virtual ~OUT_MessageHandle() {;}

  /** writes out a protocol message */
  static void writeProtocol(
      Analysis_type analyType,
      Output_type outputType,
      std::string output,
      MYINT levelOut = 3,
      MYINT thread = 0);

  /** writes out an error and actually terminates the program*/
  static void writeProtocolError(
      Analysis_type analyType,
      Output_type outputType,
      std::string output,
      MYINT thread = 0);

  /** writes out a warning */
  static void writeProtocolWarning(
      Analysis_type analyType,
      Output_type outputType,
      std::string output,
      MYINT thread = 0);

  /** print one DC state */
  static void printDCState(
      const AN_Analysis *analysis,
      const LS_SolverBase<MYREAL>* dcLinearSolver,
      const Netlist *netlist,
      const std::vector<MYREAL> &actualState);

private:

  // add here the fields which are necessary to write out into files etc
};

#endif /* OUT_MESSAGEHANDLE_HPP_ */
