#include "IP_D_DAC.hpp"

void
    IP_D_DAC::add_param(TICTAC_SPTR<IP_Param> p)
{
  if (x_dac_ == nullptr) SIM_ERROR_STOP("D_ADC: no pointer to x_adc provided.")

  std::string parameter_name = p->getName();
  if (parameter_name == "out_low" || parameter_name == "out_high"
      || parameter_name == "g_resistive" || parameter_name == "g_pullup"
      || parameter_name == "g_pullup" || parameter_name == "g_pullup"
      || parameter_name == "g_pulldown" || parameter_name == "g_hiz" || parameter_name == "t_rise"
      || parameter_name == "t_fall" || parameter_name == "knee_high"
      || parameter_name == "knee_low" || parameter_name == "v_smooth")
    {
      x_dac_->add_param(p);
    }
  else
    IP_D_BaseElement::add_param(p);
}

void
    IP_D_DAC::copy_params_dac(const IP_D_DAC& from)
{
  for (const auto& param : from.x_dac()->getCallParams())
    this->x_dac_->add_param(std::make_shared<IP_Param>(IP_Param(*(param))));

  IP_D_BaseElement::copy_params(from);
}

TICTAC_SPTR<IP_D_BaseElement>
    IP_D_DAC::promote_clone(std::string prefix)
{
  auto [x_dac, d_dac, nn] = promote_clone_dac(prefix);
  return d_dac;
}

std::tuple<TICTAC_SPTR<IP_XCall>, TICTAC_SPTR<IP_D_DAC>, TICTAC_SPTR<IP_NetworkNode>>
    IP_D_DAC::promote_clone_dac(std::string prefix)
{
  std::string in = promote_node_name(prefix, this->in()->node_name());
  std::string out = promote_node_name(prefix, this->out()->node_name());
  std::string analog_out = promote_node_name(prefix, this->analog_out()->node_name());
  std::vector<IP_Token> tokens = this->x_dac()->getTokens();
  return create_dac_bridge(
      tokens, prefix + this->name_space(), this->model_name(), in, out, analog_out);
}

std::tuple<TICTAC_SPTR<IP_XCall>, TICTAC_SPTR<IP_D_DAC>, TICTAC_SPTR<IP_NetworkNode>>
    create_dac_bridge(std::vector<IP_Token>& tokens,
                      std::string model_name,
                      TICTAC_SPTR<IP_NetworkNode> in,
                      TICTAC_SPTR<IP_NetworkNode> out)
{
  if (in->is_analog()) SIM_ERROR_STOP("Input node must not be analog! Cannot generate dac.");

  // We need to generate an auxiliary node since the dac bridge is split into a SystemC and a
  // Verilog-A part

  std::string name_aux_node = tokens[1].getValue() + "__AUX__" + tokens[2].getValue();
  std::regex e("[.]");
  name_aux_node = std::regex_replace(name_aux_node, e, "_DOT_");
  auto aux = std::make_shared<IP_NetworkNode>(IP_NetworkNode(name_aux_node));
  aux->is_sc_out(true);

  auto d_dac = std::make_shared<IP_D_DAC>(IP_D_DAC(tokens[0].getValue(), model_name));

  std::vector<TICTAC_SPTR<IP_Param>> xparams(0);
  std::vector<std::string> pinNameBinding(0); // name binding is empty for TITAN
  tokens[0].setValue("X" + tokens[0].getValue());
  tokens[1].setValue(aux->node_name());
  auto x_dac =
      TICTAC_SPTR<IP_XCall>(new IP_XCall(tokens, 2, "dac_bridge", xparams, pinNameBinding));
  x_dac->is_resolvable(false);
  d_dac->x_dac(x_dac);
  d_dac->analog_out(out);

  // Wire the SystemC part of the dac bridge (output node is the aux node)
  d_dac->in(in);
  in->digital_ins(d_dac); // Add the new bridge as a digital in
  d_dac->out(aux);
  d_dac->analog_out(out);

  // Wire the analog part of the dac bridge (first node is the auxiliary one)
  x_dac->add_network_node(aux);
  x_dac->add_network_node(out);

  return std::make_tuple(x_dac, d_dac, aux);
}

std::tuple<TICTAC_SPTR<IP_XCall>, TICTAC_SPTR<IP_D_DAC>, TICTAC_SPTR<IP_NetworkNode>>
    create_dac_bridge(std::vector<IP_Token>& tokens,
                      std::string name_space,
                      std::string model_name,
                      std::string in,
                      std::string out,
                      std::string analog_out)
{

  // Create in node (exists only in digital (vulgo SystemC) world)
  auto port_in = std::make_shared<IP_NetworkNode>(IP_NetworkNode(in));
  port_in->is_analog(false);

  // We need to generate an auxiliary node since the dac bridge is split into a SystemC and a
  // Verilog-A part
  auto port_out = std::make_shared<IP_NetworkNode>(IP_NetworkNode(out));
  port_out->is_sc_out(true);

  // Create out node that is purely analog
  auto nn_analog_out = std::make_shared<IP_NetworkNode>(IP_NetworkNode(analog_out));

  // Create analog part of the DAC
  std::vector<TICTAC_SPTR<IP_Param>> xparams(0);
  std::vector<std::string> pinNameBinding(0); // name binding is empty for TITAN
  tokens[0].setValue("X" + name_space);
  tokens[1].setValue(port_out->node_name());
  tokens[2].setValue(analog_out);
  auto x_dac =
      TICTAC_SPTR<IP_XCall>(new IP_XCall(tokens, 2, "dac_bridge", xparams, pinNameBinding));
  x_dac->is_resolvable(false);


  // Create digital part of the DAC
  auto d_dac = std::make_shared<IP_D_DAC>(IP_D_DAC(name_space, model_name));
  d_dac->x_dac(x_dac);
  d_dac->analog_out(nn_analog_out);

  // Wire the SystemC part of the dac bridge (output node is the aux node)
  d_dac->in(port_in);
  port_in->digital_ins(d_dac); // Add the new bridge as a digital in
  d_dac->out(port_out);

  // Wire the analog part of the dac bridge (first node is the auxiliary one)
  x_dac->add_network_node(port_out);
  x_dac->add_network_node(nn_analog_out);

  return std::make_tuple(x_dac, d_dac, port_out);
}
