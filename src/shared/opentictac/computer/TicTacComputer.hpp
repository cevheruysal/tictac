/*
 * TicTacComputer.hpp
 *
 *  Created on: Sep 30, 2013
 *      Author: benk
 */

#ifndef TICTACCOMPUTER_HPP_
#define TICTACCOMPUTER_HPP_

#include "AN_Analysis.hpp"
#include "Netlist.hpp"
#include "IN_SimOptions.hpp"

/** main class of the execution, this holds all top level information
 * and steers the whole execution of the programm */
class TicTacComputer: public UT_SIMVerboseClass
{
public:

  /** Ctor
   * @param IPNetlist the netlist read from the IP */
  TicTacComputer(TICTAC_SPTR<Netlist> IPNetlist,
      TICTAC_SPTR<IN_SimOptions> globalOptions);

  /** Dtor that deletes all the allocated objects */
  virtual ~TicTacComputer();

  /** function to be called to execute all the required simulation s*/
  void executeRequiredAnalyses();

  /** method used by the external setup rountines to add analysis to the
   * simulation */
  void addAnlysis(TICTAC_SPTR<AN_Analysis> newAnalysis);

private:

  /** list of consecutive analysis */
  std::vector< TICTAC_SPTR<AN_Analysis> > analysis_;

  /** number of required analysis*/
  MYINT nr_analysis_;

  /** the global netlist read from the input processor */
  TICTAC_SPTR<Netlist> IPNetlist_;

  /** global simulation options */
  TICTAC_SPTR<IN_SimOptions> globalOptions_;
};

#endif /* TICTACCOMPUTER_HPP_ */
