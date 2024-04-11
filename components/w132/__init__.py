import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import time
from esphome.const import (
    CONF_ID,
    CONF_TIME_ID,
)

CODEOWNERS = ["@swoboda1337"]

CONF_PIN = 'pin'

w132_ns = cg.esphome_ns.namespace("w132")
W132 = w132_ns.class_("W132", cg.PollingComponent)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(W132),
            cv.Required(CONF_PIN): pins.internal_gpio_input_pin_schema,
        }
    )
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    pin = await cg.gpio_pin_expression(config.get(CONF_PIN))
    cg.add(var.set_pin(pin))
