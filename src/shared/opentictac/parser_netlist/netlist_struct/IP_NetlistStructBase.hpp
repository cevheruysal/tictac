/*
 * IP_NetlistStructBase.hpp
 *
 *  Created on: Jan 29, 2015
 *      Author: benk
 */

#ifndef IP_NETLISTSTRUCTBASE_HPP_
#define IP_NETLISTSTRUCTBASE_HPP_

#include "IP_NetlistVisitBase.hpp"
#include "IP_Token.hpp"
#include "sim_utils.hpp"

#include "IP_NetlistStructInfo.hpp"
#include "IP_NetworkNode.hpp"

/** the base class for ALL components which can appear in the netlist */
class IP_NetlistStructBase
{
 public:
  /** the basic class Ctor with a list of tokens */
  IP_NetlistStructBase(const std::vector<IP_Token>& tokens);

  /** empty Dtor */
  virtual ~IP_NetlistStructBase() { ; }

  /** method for the data structure traversal*/
  virtual void Accept(IP_NetlistVisitBase& visitor)
  {
    visitor.Visit((IP_NetlistStructBase&)(*this));
  }

  /** returns the number of tokens */
  MYINT getNrTokens() const { return (MYINT)tokens_.size(); }

  /** returns the token */
  IP_Token& getToken(int i) { return tokens_[i]; }
  const IP_Token& getToken(int i) const { return tokens_[i]; }

  /** returns reference to the general information object */
  IP_NetlistStructInfo& getInfo() { return info_; }
  const IP_NetlistStructInfo& getInfo() const { return info_; }

  /** returns the tokens which build this structure */
  const std::vector<IP_Token>& getTokens() const { return tokens_; }
  std::vector<IP_Token>& getTokens() { return tokens_; }

  TICTAC_SPTR<std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>>> network_nodes()
  {
    return std::make_shared<std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>>>(
        network_nodes_);
  }

  TICTAC_SPTR<std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>>> flattened_network_nodes()
  {
    return std::make_shared<std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>>>(
        flattened_network_nodes_);
  }

  TICTAC_SPTR<IP_NetworkNode> add_network_node(TICTAC_SPTR<IP_NetworkNode> network_node);
  void add_network_node(
      const std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>>& network_node_map);

  TICTAC_SPTR<IP_NetworkNode> add_flattened_network_node(TICTAC_SPTR<IP_NetworkNode> network_node);
  void add_flattened_network_node(
      const std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>>& network_node_map);

  void merge_network_nodes(IP_NetlistStructBase& base_struct);

  void initialize_flattened_network_nodes()
  {
    flattened_network_nodes_.clear();
    for (auto elem : network_nodes_)
      {
        flattened_network_nodes_[elem.first] = std::make_shared<IP_NetworkNode>(*elem.second);
        flattened_network_nodes_[elem.first]->delete_digital_links();
      }
  }

 protected:
  /** the tokens representing the statement, or at least the first record (e.g. .subckt)*/
  std::vector<IP_Token> tokens_;
  /// each node will hold this class to store general information in each node
  IP_NetlistStructInfo info_;

  // Not all IP_* classes need this entry, but we need it for IP_NetlistStructSequence and
  // IP_Element at least.
  std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>> network_nodes_;
  // The following container will be overwritten for every instance.
  std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>> flattened_network_nodes_;
};

#endif /* IP_NETLISTSTRUCTBASE_HPP_ */
