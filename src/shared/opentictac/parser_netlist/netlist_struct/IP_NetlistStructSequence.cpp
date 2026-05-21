/*
 * IP_NetlistStructSequence.cpp
 *
 *  Created on: Jan 29, 2015
 *      Author: benk
 */

#include "IP_NetlistStructSequence.hpp"

IP_NetlistStructSequence::IP_NetlistStructSequence(std::vector<IP_Token>& tokens)
    : IP_NetlistStructBase(tokens)
{
  //
  sequence_.clear();
}


void
    IP_NetlistStructSequence::flatten_out_subckt_network_nodes(
        IP_NetlistStructSequence& seq,
        std::string prefix,
        TICTAC_SPTR<std::unordered_map<std::string, std::string>> xcall_pin_pairs)
{
  for (auto& elem : seq.flattened_network_nodes_)
    {
      // Aren't we losing here all the properties such as whether it is an analog node or not?
      auto moved_node = elem.second->get_prefixed_quasiclone(prefix, xcall_pin_pairs);

      auto nn = add_flattened_network_node(moved_node);
      if (!(nn == elem.second))
        {
          // Node did exist already
          // std::cout << "Node exists: redirect shared_ptr\n";
          elem.second = nn;
        }
    }
}