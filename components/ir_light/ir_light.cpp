#include "ir_light.h"
#include "esphome/core/log.h"
#include <unordered_map>
#include <limits>

namespace esphome
{
  namespace ir_light
  {

    struct Color
    {
      uint8_t red;
      uint8_t green;
      uint8_t blue;

      bool operator==(const Color &rhs) const
      {
        return red == rhs.red && green == rhs.green && blue == rhs.blue;
      }

      uint32_t distance_to(const Color &rhs) const
      {
        return std::sqrt(std::pow(red - rhs.red, 2) + std::pow(green - rhs.green, 2) + std::pow(blue - rhs.blue, 2));
      }
    };

    struct ColorHasher
    {
      std::size_t operator()(const Color &c) const
      {
        uint32_t hex_value = (((c.red << 8) ^ (c.green)) << 8) ^ c.blue;
        return std::hash<uint32_t>{}(hex_value);
      }
    };

    static const char *TAG = "ir_light.light";
    static constexpr uint8_t NBITS = 32;
    static constexpr uint32_t ON = 0x00FFA25D;
    static constexpr uint32_t OFF = 0x00FFE21D;
    static constexpr uint32_t BR_DOWN = 0x00FF906F;
    static constexpr uint32_t BR_UP = 0x00FFA857;

    static const std::unordered_map<Color, uint32_t, ColorHasher> SUPPORTED_COLORS = {
        {{255, 255, 255}, 0x00FF52AD}, // WHITE
        {{255, 0, 0}, 0x00FF6897},     // RED
        {{0, 255, 0}, 0x00FF9867},     // GREEN
        {{0, 0, 255}, 0x00FFB04F},     // BLUE
        {{255, 255, 0}, 0x00FF4AB5},   // YELLOW
        {{255, 160, 0}, 0x00FF30CF},   // ORANGE
        {{237, 130, 237}, 0x00FF10EF}, // PURPLE
        {{239, 255, 255}, 0x00FF5AA5}, // SKY BLUE
        {{173, 216, 229}, 0x00FF7A85}, // LIGHT BLUE
        {{255, 213, 0}, 0x00FF38C7},   // GOLD
        {{159, 128, 255}, 0x00FF42BD}, // VIOLET
        {{135, 206, 234}, 0x00FF18E7}  // TURQUOISE
    };

    void IrLightOutput::setup() {}

    light::LightTraits IrLightOutput::get_traits()
    {
      auto traits = light::LightTraits();
      traits.set_supported_color_modes({light::ColorMode::BRIGHTNESS, light::ColorMode::RGB});
      return traits;
    }

    void IrLightOutput::setup_state(light::LightState *state)
    {
      brightness_ = state->current_values.get_brightness();
    }

    void IrLightOutput::write_state(light::LightState *state)
    {
      auto values = state->current_values;

      if (!values.is_on())
      {
        send(OFF);
        ESP_LOGCONFIG(TAG, "set state to OFF");
        return;
      }

      send(ON);
      ESP_LOGCONFIG(TAG, "set state to ON");

      if (brightness_ != values.get_brightness())
      {
        for (int i = 0; i < 4; i++)
        {
          send(BR_DOWN);
        }
        for (int i = 0; i < (int)(values.get_brightness() * 4); i++)
        {
          send(BR_UP);
        }
        ESP_LOGCONFIG(TAG, "set brightness to %d", values.get_brightness() * 100);
        brightness_ = values.get_brightness();
      }

      Color c{static_cast<uint8_t>(values.get_red() * 255),
              static_cast<uint8_t>(values.get_green() * 255),
              static_cast<uint8_t>(values.get_blue() * 255)};
      uint32_t min_distance = std::numeric_limits<uint32_t>::max();
      uint32_t color_data = 0;
      for (auto &pair : SUPPORTED_COLORS)
      {
        auto distance = c.distance_to(pair.first);
        ESP_LOGCONFIG(TAG, "The distance between %d %d %d and %d %d %d is %d",
                      c.red, c.green, c.blue,
                      pair.first.red, pair.first.green, pair.first.blue,
                      distance);
        if (distance < min_distance)
        {
          min_distance = distance;
          color_data = pair.second;
        }
      }

      if (color_data > 0)
      {
        send(color_data);
      }
    }

    void IrLightOutput::dump_config() { ESP_LOGCONFIG(TAG, "IR light"); }

    void IrLightOutput::send(uint32_t data)
    {
      auto transmit_call = transmitter_->transmit();
      remote_base::LGData lg_data{
          .data = data,
          .nbits = NBITS};

      protocol_.encode(transmit_call.get_data(), lg_data);
      transmit_call.perform();
      delay(200);
    }

  } // namespace ir_light
} // namespace esphome
