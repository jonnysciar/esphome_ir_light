import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light, remote_transmitter
from esphome.const import CONF_OUTPUT_ID, CONF_OUTPUT

ir_light_ns = cg.esphome_ns.namespace("ir_light")
IrLightOutput = ir_light_ns.class_("IrLightOutput", light.LightOutput)

CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(IrLightOutput),
        cv.Required(CONF_OUTPUT): cv.use_id(remote_transmitter.RemoteTransmitterComponent),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    await light.register_light(var, config)

    out = await cg.get_variable(config[CONF_OUTPUT])
    cg.add(var.set_output(out))