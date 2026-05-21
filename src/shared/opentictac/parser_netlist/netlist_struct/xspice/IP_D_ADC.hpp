//
// Created by adrian on 7/18/19.
//

#ifndef TICTAC_IP_D_ADC_HPP
#define TICTAC_IP_D_ADC_HPP

#include "IP_D_BaseElement.hpp"
#include "IP_Element.hpp"
#include "IP_NetworkNode.hpp"
#include "IP_XCall.hpp"
#include "sim_utils.hpp"

class IP_D_ADC : public IP_D_BaseElement
{
 public:
  IP_D_ADC(std::string name_space, std::string model_name)
      : IP_D_BaseElement(name_space, model_name)
  {
    ;
  }
  /// empty Dtor
  virtual ~IP_D_ADC() { ; }
  virtual void Accept(IP_NetlistVisitBase& visitor) { visitor.Visit(*this); }

  virtual void add_param(TICTAC_SPTR<IP_Param> p);
  virtual void add_param(std::string param_name, std::string param_value)
  {
    IP_D_BaseElement::add_param(param_name, param_value);
  };
  virtual void add_param(const std::vector<TICTAC_SPTR<IP_Param>>& params)
  {
    IP_D_BaseElement::add_param(params);
  };

  void copy_params_adc(const IP_D_ADC& from);

  TICTAC_SPTR<IP_NetworkNode> in() { return in_; };
  IP_D_ADC& in(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    in_ = network_node;
    return *this;
  };

  TICTAC_SPTR<IP_NetworkNode> out() { return out_; };
  IP_D_ADC& out(TICTAC_SPTR<IP_NetworkNode> network_node)
  {
    out_ = network_node;
    return *this;
  };

  virtual void unify_nodes(IP_NetlistStructSequence& seq)
  {
    SIM_PRINT_L5(verb(), "We are in the d_adc!!!");
    in_ = seq.add_flattened_network_node(in_);
    out_ = seq.add_flattened_network_node(out_);
  }

  TICTAC_SPTR<IP_XCall> x_adc() const { return x_adc_; }
  IP_D_ADC& x_adc(TICTAC_SPTR<IP_XCall> x_adc)
  {
    x_adc_ = std::move(x_adc);
    return *this;
  }

  virtual void exchange_digital_in(const IP_NetworkNode* old_node,
                                   TICTAC_SPTR<IP_NetworkNode> new_node)
  {
    if (in_.get() == old_node)
      in_ = new_node;
    else
      SIM_ERROR_STOP("Cannot exchange nodes!")
  }
  virtual void exchange_digital_out(const IP_NetworkNode* old_node,
                                    TICTAC_SPTR<IP_NetworkNode> new_node)
  {
    if (out_.get() == old_node)
      out_ = new_node;
    else
      SIM_ERROR_STOP("Cannot exchange nodes!")
  }

  virtual TICTAC_SPTR<IP_D_BaseElement> promote_clone(std::string prefix);
  std::tuple<TICTAC_SPTR<IP_XCall>, TICTAC_SPTR<IP_D_ADC>> promote_clone_adc(std::string prefix);

  virtual std::tuple<std::string, std::string, std::string> get_channel_info(IP_NetworkNode* nn)
  {
    if (in_.get() == nn)
      {
        return std::make_tuple("0", "0", "0");
      }
    else
      SIM_ERROR_STOP("Node is not connected to this element!");
  }

 private:
  TICTAC_SPTR<IP_NetworkNode> in_ = nullptr;
  TICTAC_SPTR<IP_NetworkNode> out_ = nullptr;
  TICTAC_SPTR<IP_XCall> x_adc_ = nullptr;

  std::vector<IP_Token> tokens;
};

std::tuple<TICTAC_SPTR<IP_XCall>, TICTAC_SPTR<IP_D_ADC>>
    create_adc_bridge(std::vector<IP_Token>& tokens,
                      std::string model_name,
                      TICTAC_SPTR<IP_NetworkNode> in,
                      TICTAC_SPTR<IP_NetworkNode> out);

std::tuple<TICTAC_SPTR<IP_XCall>, TICTAC_SPTR<IP_D_ADC>>
    create_adc_bridge(std::vector<IP_Token>& tokens,
                      std::string name_space,
                      std::string model_name,
                      std::string in,
                      std::string out);

TICTAC_SPTR<IP_D_ADC>
    create_d_adc_bridge(std::string name,
                        std::string model_name,
                        TICTAC_SPTR<IP_XCall> x_adc,
                        std::string in,
                        std::string out);
#endif // TICTAC_IP_D_ADC_HPP