#include "ir_light.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ir_light {

static const char *TAG = "ir_light.light";

void IrLightOutput::setup() {}

light::LightTraits IrLightOutput::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supported_color_modes({light::ColorMode::BRIGHTNESS});
  return traits;
}

void IrLightOutput::write_state(light::LightState *state) {}

void IrLightOutput::dump_config() { ESP_LOGCONFIG(TAG, "Empty custom light"); }

}  // namespace ir_light
}  // namespace esphome