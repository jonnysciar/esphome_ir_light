#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/remote_transmitter/remote_transmitter.h"
#include "esphome/components/remote_base/lg_protocol.h"

namespace esphome
{
  namespace ir_light
  {

    class IrLightOutput : public light::LightOutput, public Component
    {
    public:
      void setup() override;
      light::LightTraits get_traits() override;
      void set_output(remote_transmitter::RemoteTransmitterComponent *transmitter) { transmitter_ = transmitter; }
      void setup_state(light::LightState *state) override;
      void write_state(light::LightState *state) override;
      void dump_config() override;

    private:
      void send(uint32_t data);

    protected:
      remote_transmitter::RemoteTransmitterComponent *transmitter_;
      remote_base::LGProtocol protocol_;
      float brightness_;
    };

  } // namespace ir_light
} // namespace esphome