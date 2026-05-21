/*
 * IP_AnaysisCard.hpp
 *
 *  Created on: Jan 29, 2015
 *      Author: benk
 */

#ifndef IP_ANAYSISCARD_HPP_
#define IP_ANAYSISCARD_HPP_

#include "IP_ControlBase.hpp"

/** the various analysis card types which can occure on the netlist
 * (later this could be extended) */
typedef enum {
  IP_ANALYSIS_DC = 0,   ///< DC anaysis
  IP_ANALYSIS_OP = 1,   ///< OP anaysis
  IP_ANALYSIS_TR = 2,   ///< TRAN anaysis
  IP_ANALYSIS_AC = 3,   ///< AC anaysis
  IP_ANALYSIS_ERR = 4     ///< error signaling enum type
} AnalysisCardType;

/** class represents various analysis cards */
class IP_AnaysisCard : public IP_ControlBase
{
public:

  /** the Ctor with the array of strings */
  IP_AnaysisCard(std::vector<IP_Token> &tokens, AnalysisCardType aType);

  /** empty Dtor */
  virtual  ~IP_AnaysisCard() {;}

  /** returns the analysis type */
  AnalysisCardType getAnalysisType() const { return analysisType_; }

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase &visitor) { visitor.Visit( (IP_AnaysisCard &)(*this)); }

private:

  /** the analysis type */
  AnalysisCardType analysisType_;

};

#endif /* IP_ANAYSISCARD_HPP_ */
