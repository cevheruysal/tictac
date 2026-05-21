#include "IP_D_Buffer.hpp"

TICTAC_SPTR<IP_D_BaseElement>
    IP_D_Buffer::promote_clone(std::string prefix)
{
  std::string in = promote_node_name(prefix, this->in()->node_name());
  std::string out = promote_node_name(prefix, this->out()->node_name());

  return create_d_buffer(prefix + this->name_space(), this->model_name(), in, out);
}

std::tuple<std::string, std::string, std::string>
    IP_D_Buffer::get_channel_info(IP_NetworkNode* nn)
{
  if (in_.get() == nn)
    {
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
      param_multiply(input_load, 1);
      param_multiply(sink_current, 1);
      param_multiply(source_current, 1);
      return std::make_tuple(input_load, sink_current, source_current);
    }
  else
    SIM_ERROR_STOP("Node is not connected to this element!");
}


TICTAC_SPTR<IP_D_Buffer>
    create_d_buffer(std::string name, std::string model_name, std::string in, std::string out)
{
  auto port_in = std::make_shared<IP_NetworkNode>(IP_NetworkNode(in));
  port_in->is_analog(false);

  auto port_out = std::make_shared<IP_NetworkNode>(IP_NetworkNode(out));
  port_out->is_analog(false);

  auto d_buffer = std::make_shared<IP_D_Buffer>(IP_D_Buffer(name, model_name));

  d_buffer->in(port_in);
  port_in->digital_ins(d_buffer);
  d_buffer->out(port_out);
  port_out->digital_outs(d_buffer);
  return d_buffer;
}
