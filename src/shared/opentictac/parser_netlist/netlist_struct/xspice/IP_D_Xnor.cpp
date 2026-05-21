#include "IP_D_Xnor.hpp"

TICTAC_SPTR<IP_D_BaseElement>
    IP_D_Xnor::promote_clone(std::string prefix)
{
  std::vector<std::string> ins;
  for (const auto& in : ins_) ins.push_back(promote_node_name(prefix, in->node_name()));

  std::string out = promote_node_name(prefix, this->out()->node_name());

  return create_d_xnor(prefix + this->name_space(), this->model_name(), ins, out);
}

void
    IP_D_Xnor::exchange_digital_in(const IP_NetworkNode* old_node,
                                   TICTAC_SPTR<IP_NetworkNode> new_node)
{
  for (auto& in : ins_)
    {
      if (in.get() == old_node) in = new_node;
    }
}

std::tuple<std::string, std::string, std::string>
    IP_D_Xnor::get_channel_info(IP_NetworkNode* nn)
{
  // Figure out, how a node is connected (could be attached at several slots)
  // Note that when get_channel_info is called, we have resolved the nodes, so it suffices to
  // compare pointers
  int number_of_attachments = 0;
  for (const auto& in : ins_)
    {
      if (in.get() == nn) number_of_attachments++;
    }

  // The node is attached:
  if (number_of_attachments > 0)
    {
      // Default values
      std::string input_load = "1p";
      std::string sink_current = "0";
      std::string source_current = "0";

      const IP_Param* param = NULL;
      if (get_nr_params() > 0)
        {
          param = get_param("input_load");
          if (param != NULL) input_load = get_param("input_load")->getInitExprToken().getValue();

          param = get_param("sink_current");
          if (param != NULL)
            sink_current = get_param("sink_current")->getInitExprToken().getValue();

          param = get_param("source_current");
          if (param != NULL)
            source_current = get_param("source_current")->getInitExprToken().getValue();
        }
      param_multiply(input_load, number_of_attachments);
      param_multiply(sink_current, number_of_attachments);
      param_multiply(source_current, number_of_attachments);
      return std::make_tuple(input_load, sink_current, source_current);
    }
  else
    SIM_ERROR_STOP("Node is not connected to this element!");
}


TICTAC_SPTR<IP_D_Xnor>
    create_d_xnor(std::string name,
                  std::string model_name,
                  const std::vector<std::string>& ins,
                  std::string out)
{
  std::vector<TICTAC_SPTR<IP_NetworkNode>> port_ins;
  for (const auto& in : ins)
    {
      auto port_in = std::make_shared<IP_NetworkNode>(IP_NetworkNode(in));
      port_in->is_analog(false);
      port_ins.push_back(port_in);
    }

  auto port_out = std::make_shared<IP_NetworkNode>(IP_NetworkNode(out));
  port_out->is_analog(false);

  auto d_xnor = std::make_shared<IP_D_Xnor>(IP_D_Xnor(name, model_name));

  d_xnor->ins(port_ins);
  for (auto& port_in : port_ins) port_in->digital_ins(d_xnor);
  d_xnor->out(port_out);
  port_out->digital_outs(d_xnor);
  return d_xnor;
}
