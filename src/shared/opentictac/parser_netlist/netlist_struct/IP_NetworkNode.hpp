//
// Created by adrian on 7/15/19.
//

#ifndef IP_NETWORKNODE_HPP
#define IP_NETWORKNODE_HPP

#include <sim_utils.hpp>
#include <regex>

class IP_D_BaseElement;

class IP_NetworkNode
{
 public:
  IP_NetworkNode(const std::string& node_name) { node_names_.insert(node_name); };
  IP_NetworkNode(const std::string& node_name, bool is_analog)
      : is_analog_(is_analog)
  {
    node_names_.insert(node_name);
  };

  // Quasiclone: the digital ports are erased since they will be regenerated on the
  // next instance level.
  TICTAC_SPTR<IP_NetworkNode> get_prefixed_quasiclone(
      const std::string& prefix,
      TICTAC_SPTR<std::unordered_map<std::string, std::string>> xcall_pin_pairs);

  bool is_analog() const { return is_analog_; };
  IP_NetworkNode& is_analog(bool is_analog)
  {
    is_analog_ = is_analog;
    return *this;
  };

  void has_added(bool has_added) {has_added_ = has_added;};
  bool has_added() {return has_added_;};
  bool is_digital() const
  {
    return (!digital_ins_.empty() || !digital_outs_.empty()) ? true : false;
  };

  bool is_sc_in() const { return is_sc_in_; };
  IP_NetworkNode& is_sc_in(bool is_sc_in)
  {
    if (is_sc_in)
      {
        if (!is_analog_) SIM_ERROR_STOP("An sc input port must be analog!");
      }
    is_sc_in_ = is_sc_in;
    return *this;
  };

  bool is_sc_out() const { return is_sc_out_; };
  IP_NetworkNode& is_sc_out(bool is_sc_out)
  {
    if (is_sc_out)
      {
        if (!is_analog_) SIM_ERROR_STOP("An sc output port must be analog!");
      }
    is_sc_out_ = is_sc_out;
    return *this;
  };

  std::string node_name() const { return *node_names_.cbegin(); };
  const std::set<std::string>& node_names() const {return node_names_;};

  // This function can be used for debugging purposes (therefore it might currently not be used
  // anywhere in the code)
  std::string node_names_list() const {
    std::string node_names_list;
    for(const auto& elem : node_names_)
      node_names_list = (node_names_list == "") ? elem : node_names_list + ", " + elem;
    return node_names_list;
  };

  std::string cxx_node_name() const { return cxx_node_name_; };

  void update(IP_NetworkNode nn);

  TICTAC_SPTR<IP_D_BaseElement> digital_ins(TICTAC_SPTR<IP_D_BaseElement> element);
  bool has_digital_ins()
  {
    if (digital_ins_.size() > 0) return true;
    return false;
  }

  /// This function is used when we automatically insert missing adc bridges
  void exchange_digital_ins(TICTAC_SPTR<IP_NetworkNode> nn);

  /// This function is used when we automatically insert missing dac bridges
  void exchange_digital_outs(TICTAC_SPTR<IP_NetworkNode> nn);
  void delete_digital_links()
  {
    digital_ins_.clear();
    digital_outs_.clear();
  }

  TICTAC_SPTR<IP_D_BaseElement> digital_outs(TICTAC_SPTR<IP_D_BaseElement> element);
  bool has_digital_outs()
  {
    if (digital_outs_.size() > 0) return true;
    return false;
  }

  std::tuple<std::string, std::string, std::string> get_channel_info();
  void set_cxx_node_name(std::unordered_map<std::string, TICTAC_SPTR<IP_NetworkNode>> &list_of_names);

 private:
  //std::string node_name_;
  std::set<std::string> node_names_; // A node can go by many names
  std::string cxx_node_name_;
  bool is_analog_ = true;
  // Ports refer here to those node that connect the digital SystemC world with
  // analog TITAN
  bool is_sc_in_ = false;
  bool is_sc_out_ = false;
  bool has_added_ = false;
  std::unordered_map<std::string, TICTAC_SPTR<IP_D_BaseElement>> digital_ins_;
  std::unordered_map<std::string, TICTAC_SPTR<IP_D_BaseElement>> digital_outs_;
};


#endif // IP_NETWORKNODE_HPP
