import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor
from esphome.const import (
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_WIND_SPEED,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    UNIT_DEGREES,
    UNIT_KILOMETER_PER_HOUR,
)
from . import W132

DEPENDENCIES = ["w132"]

CONF_W132_ID = 'w132_id'
CONF_XWIND_DIRECTION = 'xwind_direction'
CONF_XWIND_SPEED = 'xwind_speed'
CONF_XWIND_GUST = 'xwind_gust'
CONF_XWIND_TEMPERATURE = 'xwind_temperature'
CONF_XWIND_HUMIDITY = 'xwind_humidity'

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(CONF_W132_ID): cv.use_id(W132),
            cv.Optional(CONF_XWIND_DIRECTION): sensor.sensor_schema(
                unit_of_measurement=UNIT_DEGREES,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_WIND_SPEED,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_XWIND_SPEED): sensor.sensor_schema(
                unit_of_measurement=UNIT_KILOMETER_PER_HOUR,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_WIND_SPEED,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_XWIND_GUST): sensor.sensor_schema(
                unit_of_measurement=UNIT_KILOMETER_PER_HOUR,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_WIND_SPEED,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_XWIND_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_XWIND_HUMIDITY): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_W132_ID])
    if direction_config := config.get(CONF_XWIND_DIRECTION):
        sens = await sensor.new_sensor(direction_config)
        cg.add(parent.set_direction_sensor(sens))
    if speed_config := config.get(CONF_XWIND_SPEED):
        sens = await sensor.new_sensor(speed_config)
        cg.add(parent.set_speed_sensor(sens))
    if gust_config := config.get(CONF_XWIND_GUST):
        sens = await sensor.new_sensor(gust_config)
        cg.add(parent.set_gust_sensor(sens))
    if temperature_config := config.get(CONF_XWIND_TEMPERATURE):
        sens = await sensor.new_sensor(temperature_config)
        cg.add(parent.set_temperature_sensor(sens))
    if humidity_config := config.get(CONF_XWIND_HUMIDITY):
        sens = await sensor.new_sensor(humidity_config)
        cg.add(parent.set_humidity_sensor(sens))
