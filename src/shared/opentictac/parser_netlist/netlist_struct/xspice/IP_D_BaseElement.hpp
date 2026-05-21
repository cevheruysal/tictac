//
// Created by adrian on 7/18/19.
//

#ifndef TICTAC_IP_D_BASEELEMENT_HPP
#define TICTAC_IP_D_BASEELEMENT_HPP

#include "IP_NetlistStructSequence.hpp"
#include "IP_Param.hpp"
#include "sim_utils.hpp"

inline std::string
    promote_node_name(const std::string& prefix, const std::string& node_name)
{
  if (node_name != "0" && prefix != "")
    return prefix + "." + node_name;
  else
    return node_name;
}

class IP_NetworkNode;

void
    param_multiply(std::string& param, int factor);

void
    param_addition(std::string& param, const std::string& in_param);


const std::vector<std::string> digital_models{"adc_bridge",
                                              "dac_bridge",
                                              "d_and",
                                              "d_buffer",
                                              "d_dff",
                                              "d_dlatch",
                                              "d_inverter",
                                              "d_jkff",
                                              "d_nand",
                                              "d_nor",
                                              "d_or",
                                              "d_srff",
                                              "d_srlatch",
                                              "d_tff",
                                              "d_xnor",
                                              "d_xor"};

/** the base class for ALL components which can appear in the netlist */
class IP_D_BaseElement : public UT_SIMVerboseClass
{
 public:
  IP_D_BaseElement(std::string name_space, std::string model_name)
      : UT_SIMVerboseClass(std::string("IP_D_BaseElement"))
      , name_space_(std::move(name_space))
      , model_name_(std::move(model_name))
  {
    setVerb(0);
  }
  /// empty Dtor
  virtual ~IP_D_BaseElement() { ; }
  virtual void Accept(IP_NetlistVisitBase& visitor) { visitor.Visit(*this); }

  virtual std::string name_space() { return name_space_; }
  virtual std::string model_name() { return model_name_; }
  virtual std::string instance_name() { return name_space_ + "__" + model_name_; }

  /** get the number of model parameters */
  auto get_nr_params() const { return model_params_.size(); }

  TICTAC_SPTR<IP_Param>& get_param(int i) { return model_params_[i]; }
  const TICTAC_SPTR<IP_Param>& get_param(int i) const { return model_params_[i]; }

  /** looks for a parameter , returns NULL if not found */
  const IP_Param* get_param(std::string param_name, bool case_sens = false) const;

  const std::vector<TICTAC_SPTR<IP_Param>>& get_params() { return model_params_; }

  virtual void copy_params(const IP_D_BaseElement& from);
  /// deletes a parameter if this exists in this model
  // void delete_param(std::string param_name);

  /// deletes a parameter if this exists in this model
  // void replace_param_name(std::string param_name, std::string new_param_name);

  /// adds a new parameter to the model
  virtual void add_param(TICTAC_SPTR<IP_Param> p);
  virtual void add_param(std::string param_name, std::string param_value);
  virtual void add_param(const std::vector<TICTAC_SPTR<IP_Param>>& params);

  bool has_resolved_params() { return has_resolved_params_; }
  void has_resolved_params(const bool& has_resolved_params)
  {
    has_resolved_params_ = has_resolved_params;
  };

  void family(const std::string& family) { string_parameters_["family"] = family; }
  std::string family() { return string_parameters_["family"]; }

  void in_family(const std::string& in_family) { string_parameters_["in_family"] = in_family; }
  std::string in_family()
  {
    if (string_parameters_["in_family"] == "")
      return string_parameters_["family"];
    else
      return string_parameters_["in_family"];
  }

  void out_family(const std::string& out_family) { string_parameters_["out_family"] = out_family; }
  std::string out_family()
  {
    if (string_parameters_["out_family"] == "")
      return string_parameters_["family"];
    else
      return string_parameters_["out_family"];
  }

  // Maybe replace with iterators. But a bit more complicated...
  virtual void unify_nodes(IP_NetlistStructSequence& seq) = 0;
  virtual void exchange_digital_in(const IP_NetworkNode* old_node,
                                   TICTAC_SPTR<IP_NetworkNode> new_node) = 0;
  virtual void exchange_digital_out(const IP_NetworkNode* old_node,
                                    TICTAC_SPTR<IP_NetworkNode> new_node) = 0;
  virtual TICTAC_SPTR<IP_D_BaseElement> promote_clone(std::string prefix) = 0;

  virtual std::tuple<std::string, std::string, std::string>
      get_channel_info(IP_NetworkNode* nn) = 0;

  virtual bool is_string_param(TICTAC_SPTR<IP_Param> p);

 private:
  bool has_resolved_params_ = false;
  std::unordered_map<std::string, std::string> string_parameters_{{"family", "UNIV"},
                                                                  {"in_family", ""},
                                                                  {"out_family", ""}};

  std::string name_space_;
  std::string model_name_;
  std::vector<TICTAC_SPTR<IP_Param>> model_params_;
};

#endif // TICTAC_IP_D_BASEELEMENT_HPP