/*
 * IP_NetlistStructBase.cpp
 *
 *  Created on: Jan 29, 2015
 *      Author: benk
 */

#include "IP_NetlistStructBase.hpp"

IP_NetlistStructBase::IP_NetlistStructBase(const std::vector<IP_Token>& tokens) : tokens_(tokens)
{
  // set the tokens
}

TICTAC_SPTR<IP_NetworkNode>
    add_network_node2(std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>>& network_nodes_,
                     TICTAC_SPTR<IP_NetworkNode> network_node)
{
  // First: let's see whether network_node exists under one of it names, then
  // 1) update it
  // 2) register the ones that are not there
  // Second: add the names that have not been found

  TICTAC_SPTR<IP_NetworkNode> found_nn = network_node; // This is overwritten, if we have a hit
  std::vector<std::string> unfound_node_names;

  bool had_hit = false;
  for (const auto& node_name : network_node->node_names())
    {
      auto nn = network_nodes_.find(node_name);
      if (nn != network_nodes_.end())
        {
          if(!had_hit)
            {
              had_hit = true; // It suffices to update only one of them;
              found_nn = nn->second;
              if (found_nn != network_node)
                found_nn->update(*network_node); // TODO Maybe by constant reference??
            }
        }
      else
        {
          unfound_node_names.push_back(node_name);
        }
    }

  for (const auto& unfound_node_name : unfound_node_names)
    {
      network_nodes_.emplace(std::make_pair(unfound_node_name, found_nn));
    }

  return found_nn;
}


TICTAC_SPTR<IP_NetworkNode>
    IP_NetlistStructBase::add_network_node(TICTAC_SPTR<IP_NetworkNode> network_node)
{
   return add_network_node2(network_nodes_, network_node);
}

void
    IP_NetlistStructBase::add_network_node(
        const std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>>& network_node_map)
{
  for (const auto& network_node : network_node_map) add_network_node(network_node.second);
}


TICTAC_SPTR<IP_NetworkNode>
IP_NetlistStructBase::add_flattened_network_node(TICTAC_SPTR<IP_NetworkNode> network_node)
{
  return add_network_node2(flattened_network_nodes_, network_node);
}

void
IP_NetlistStructBase::add_flattened_network_node(
    const std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>>& network_node_map)
{
  for (const auto& network_node : network_node_map) add_flattened_network_node(network_node.second);
}


void
    IP_NetlistStructBase::merge_network_nodes(IP_NetlistStructBase& base_struct)
{
  for (auto& elem : base_struct.network_nodes_)
    {
      auto nn = add_network_node(elem.second);
      if (!(nn == elem.second))
        {
          // Node did exist already
          // std::cout << "Node exists: redirect shared_ptr\n";
          elem.second = nn;
        }
    }
}