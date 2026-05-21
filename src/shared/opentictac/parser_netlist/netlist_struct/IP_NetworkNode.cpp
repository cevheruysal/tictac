//
// Created by adrian on 7/15/19.
//
#include "IP_NetworkNode.hpp"
#include "IP_D_BaseElement.hpp"

void
    IP_NetworkNode::update(IP_NetworkNode network_node)
{
  this->is_analog(this->is_analog() || network_node.is_analog());
  this->is_sc_in(this->is_sc_in() || network_node.is_sc_in());
  this->is_sc_out(this->is_sc_out() || network_node.is_sc_out());

  for (auto in : network_node.digital_ins_) digital_ins(in.second);

  for (auto out : network_node.digital_outs_) digital_outs(out.second);
}

TICTAC_SPTR<IP_D_BaseElement>
    IP_NetworkNode::digital_ins(TICTAC_SPTR<IP_D_BaseElement> element)
{
  auto elem = digital_ins_.find(element->instance_name());
  if (elem == digital_ins_.end())
    {
      // Add digital in
      digital_ins_.emplace(std::make_pair(element->instance_name(), element));
      return digital_ins_.find(element->instance_name())->second;
    }
  else
    {
      // Network element is already registered at being attached to this node as a digital in
      if (elem->second != element)
        SIM_ERROR_STOP("Pointers are different, that is not supposed to happen");
      return elem->second;
    }
}

TICTAC_SPTR<IP_D_BaseElement>
    IP_NetworkNode::digital_outs(TICTAC_SPTR<IP_D_BaseElement> element)
{
  auto elem = digital_outs_.find(element->instance_name());
  if (elem == digital_outs_.end())
    {
      // Network element is already registered at being attached to this node as a digital out
      digital_outs_.emplace(std::make_pair(element->instance_name(), element));
      return digital_outs_.find(element->instance_name())->second;
    }
  else
    {
      // Network element is registered
      if (elem->second != element)
        SIM_ERROR_STOP("Pointers are different, that is not supposed to happen");
      return elem->second;
    }
}

void
    IP_NetworkNode::exchange_digital_ins(TICTAC_SPTR<IP_NetworkNode> nn)
{
  for (auto d_elem : digital_ins_)
    {
      nn->digital_ins(d_elem.second); // Move to new node
      d_elem.second->exchange_digital_in(this, nn);
    }
  digital_ins_.clear();
}

void
    IP_NetworkNode::exchange_digital_outs(TICTAC_SPTR<IP_NetworkNode> nn)
{
  for (auto d_elem : digital_outs_)
    {
      nn->digital_outs(d_elem.second); // Move to new node
      d_elem.second->exchange_digital_out(this, nn);
    }
  digital_outs_.clear();
}

std::tuple<std::string, std::string, std::string>
    IP_NetworkNode::get_channel_info()
{
  // std::cout << "Number of digital ins: " << digital_ins_.size() << std::endl;
  std::string load = "0";
  std::string sink_current = "0";
  std::string source_current = "0";

  for (const auto& elem : digital_ins_)
    {
      std::tuple<std::string, std::string, std::string> channel_info =
          elem.second->get_channel_info(this);
      if (std::get<0>(channel_info) != "0") load += " + " + std::get<0>(channel_info);

      if (std::get<1>(channel_info) != "0") sink_current += " + " + std::get<1>(channel_info);

      if (std::get<2>(channel_info) != "0") source_current += " + " + std::get<2>(channel_info);
    }
  return std::make_tuple(load, sink_current, source_current);
}

void
    IP_NetworkNode::set_cxx_node_name(
        std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>>& list_of_names)
{
  std::regex e("[.]"); // matches words beginning by "sub"

  // using string/c-string (3) version:
  std::string cxx_node_name = std::regex_replace(node_name(), e, "_DOT_");
  cxx_node_name_ = cxx_node_name;
  if (node_name() == cxx_node_name_) return;

  for (int i = 1;; i++)
    {
      auto nn = list_of_names.find(cxx_node_name);
      if (nn != list_of_names.end())
        cxx_node_name = cxx_node_name_ + "_" + std::to_string(i);
      else
        {
          cxx_node_name_ = cxx_node_name;
          break;
        }
    }
}

TICTAC_SPTR<IP_NetworkNode>
    IP_NetworkNode::get_prefixed_quasiclone(
        const std::string& prefix,
        TICTAC_SPTR<std::unordered_map<std::string, std::string>> xcall_pin_pairs)
{
  std::set<std::string> xcall_instance_names;

  for (const auto& nn : node_names_)
    {
      auto xcall_pin = xcall_pin_pairs->find(nn);
      if (xcall_pin != xcall_pin_pairs->end()) xcall_instance_names.insert(xcall_pin->second);
    }

  if (prefix == "") SIM_ERROR_STOP("Prefix must not be an empty string!");
  auto clone = std::make_shared<IP_NetworkNode>(*this);
  clone->digital_ins_.clear();
  clone->digital_outs_.clear();
  clone->is_sc_in_ = false;
  clone->is_sc_out_ = false;
  // Now add the prefix
  auto clone_node_names =
      clone->node_names_; // This could potentially be optimized by avoiding the copy operation...
  for (const auto& elem : clone_node_names)
    {
      if (elem != "0") // Do not modify the ground node
        {
          // Extract the node (it is a set, so we cannot directly manipulate)
          auto node = clone->node_names_.extract(elem);
          // Update it ;
          node.value() = prefix + node.value();
          // Reinsert it
          clone->node_names_.insert(std::move(node));
        }
    }

  // Now add the pins from the higher level
  clone->node_names_.insert(xcall_instance_names.begin(), xcall_instance_names.end());
  return clone;
}