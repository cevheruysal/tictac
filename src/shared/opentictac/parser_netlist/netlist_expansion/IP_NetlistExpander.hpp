/*
 * IP_NetlistExpander.hpp
 *
 *  Created on: Feb 23, 2015
 *      Author: benk
 */

#ifndef IP_NETLISTEXPANDER_HPP_
#define IP_NETLISTEXPANDER_HPP_

#include "sim_utils.hpp"
#include "Netlist.hpp"
#include "IP_SubcirDef.hpp"
#include "IP_NetlistVisitBase.hpp"

#include "TicTacComputer.hpp"
#include "CS_ExpressionBasis.hpp"
#include "CS_Parameter.hpp"

#include "IP_ExpansionStack.hpp"
#include "IP_ExprConstEvaluator.hpp"
#include "IP_InstanceableDeviceList.hpp"

/** this class is responsible for the netlist expansion. This class has to consider many
 * things during the expansion
 * At the end it n*/
class IP_NetlistExpander : public IP_NetlistVisitBase
{
public:

  /** */
  IP_NetlistExpander();

  /** empty Dtor */
  virtual  ~IP_NetlistExpander();

  /** method for the netlist expansion
   * The input is a definition tree, which has not been parsed */
  void doNetlistExpansion( TICTAC_SPTR<IP_SubcirDef> toplevel);

  /// after expansion returns the tictac computer
  TICTAC_SPTR<TicTacComputer> getTicTacComputer() { return tictacComputer_; }

  // ========== the visitor method for the expansion
  virtual void   Visit(IP_NetlistStructBase &node);
  virtual void   Visit(IP_NetlistStructSequence &node);
  virtual void   Visit(IP_ControlBase &node);
  virtual void   Visit(IP_SubcirDef &node);
  virtual void   Visit(IP_Element &node);
  virtual void   Visit(IP_Function &node);
  virtual void   Visit(IP_Model &node);
  virtual void   Visit(IP_Param &node);
  virtual void   Visit(IP_SubcirSettings &node);
  virtual void   Visit(IP_XCall &node);
  virtual void   Visit(IP_ConditionedNetlist &node);
  virtual void   Visit(IP_GlobalSettings &node);
  virtual void   Visit(IP_Option &node);
  virtual void   Visit(IP_Output &node);
  virtual void   Visit(IP_Measurement &node);
  virtual void   Visit(IP_Assertion &node);
  virtual void   Visit(IP_AnaysisCard &node);


  /** tries to evaluate an expression which is only in a token */
  ConstExpressionValue evalConstExpression(const IP_Token *token,
      const ExpansionStack* stack,  bool ignoreActualLevel = false);
  /** parses an expression token and returns an expression smart pointer */
  TICTAC_SPTR<CS_ExpressionBasis> parseExpression(const IP_Token *token,
      const ExpansionStack* stack,  bool ignoreActualLevel = false);
  /** evaluates an expression directly on the expansion stack */
  ConstExpressionValue evalExpression(TICTAC_SPTR<CS_ExpressionBasis>& expr,
      const ExpansionStack* stack,  bool ignoreActualLevel = false);

  bool getGLobalErrorFlag() const { return hadError_; }
  const std::string getGLobalErrorMessage() const { return allErrors_; }
  bool getExprError() const { return hadExprError_; }
  const std::string& getExprErrorMessage() const { return  exprProcessError_; }

private:

  /// this is the visiting method
  void  visitSubcircuit(IP_SubcirDef* toplevelToVisit);

  /** the is the main method to expand all the items on the current level,
   * which was gathererd on the stack
   * @param subcircuitToExpand  subcircuit to expand on the actual level*/
  void treatActualLevel(IP_SubcirDef* subcircuitToExpand);

  /// expand all the elements on the current level
  void expandActualLevelDevices();

  /// method to expand all X-calls at the actual level
  void expandXCalls();

  /// method to expand an X-call
  void expandXCall(IP_XCall *node);

  /** once the netlist is expanded we process all toplevel option,output, analysis, ...
   * stuff such that at the end we get a TicTac "cumputer" which is ready to simulate */
  void processTopLevelStuff_And_CreateTictac();

  /** @returns the options value on the top level if this option is given
   * @param optionName the name of the option in small letters
   * @param defaultValue if the option is not given than use this default value*/
  MYREAL getOptionRealValue(std::string optionName, MYREAL defaultValue);

  /** @returns always the ID in the ntlist of the local subcircuit, if this pinn
   * is the moment not found in the netlist than this method ADDS this pinn
   * considering the actual expansion hierarchy */
  MYINT getLocalPinn(const std::string& localPinnName);

  /** return the ID belonging to a pinn global name, -1 if not found  */
  MYINT getPinn(const std::string& pinnName);

  /** return the ID belonging to a device expanded name,-1 if not found  */
  MYINT getDevice(const std::string& devName);

  /** one pinn might have different instantiation forms e.g. X1 n p sub
   * where .subckt a b sub ... than "n" or "X1.a" should be eqvivalent.
   * For this reason a pinn name might be added with the same name during X-call
   * instantiation */
  void addLocalPinn(const std::string& localPinnName, MYINT pinID);

  /** this method test whether this elem record matches the given device matching rule
   * if yes returns the device otherwise NULL. */
  Device* matchAndCreateDevice(const IP_Element* elem,
      const IP_DeviceMatchinRule* rule);

  /** internal method to compare strings */
  bool compTwoString(const std::string &s1, const std::string &s2);

  /** returns true if the next token starting from the index is "="
   * otherwise false. */
  bool isNextTokenEqual(const IP_Element* elem, MYINT index);

  /** returns pairs of "name=value" of "value" */
  void getTokenPairs(const IP_Element* elem, MYINT index, bool &isPair,
      std::string &localName, std::string &localValue, bool &error);

  /// method to treat errors and collect error messages
  void reportError(bool cond, std::string errorString);

  /// test if a global option is set (does not tests the value), -1 if it is not existent
  MYINT isOptionSet(std::string errorString);

  /** the created netlist */
  TICTAC_SPTR<Netlist> netlist_;
  /** this is the ready build tictac computer */
  TICTAC_SPTR<TicTacComputer> tictacComputer_;

  /** toplevel to start the expansion with */
  TICTAC_SPTR<IP_SubcirDef> toplevel_;

  /** the file where stuff should be saved */
  std::string saveFile_;

  /** flag to show if we are within the "header of one subcircuit */
  bool withinSubCirDef_;

  /** here we store the names of all pinn */
  std::unordered_map< std::string , MYINT > allPins_;
  /** list of all expanded devices */
  std::unordered_map< std::string , MYINT > allDevices_;
  /** list of X-calls in the expanded netlist */
  std::unordered_map< std::string , MYINT > allXCalls_;

  /** very important! this is the expansion stack */
  ExpansionStack expansionStack_;
  /// object which holds the list of all matchable/instantiable devices
  IP_InstanceableDeviceList devMatchList_;
  /// internal flag to show if there was an error during expansion
  bool hadError_;
  /// string to collect all error messaged
  std::string allErrors_;

  /// internal flag to show if an expression  parsing process went OK
  bool hadExprError_;
  /// if an expression parsing process was NOT OK here we store the error message
  std::string exprProcessError_;

  /// here we gather all the global options
  std::vector<IP_GlobalSettings*> globalSettings_;
  /// here we gather all anaylisis
  std::vector<IP_AnaysisCard*>    analysis_;
  /// in this vector we gather all the options on the toplevel
  std::vector<IP_Option*>         globalOptions_;
  /// list of all outputs gathered
  std::vector<IP_Output*>         outputs_;
  /// here we collect all the ICs from the toplevel
  std::vector<IP_SubcirSettings*> globalICs_;
};



#endif /* IP_NETLISTEXPANDER_HPP_ */
