import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL

DEPENDENCIES = ["uart"]

CONF_UART_ID = "uart_id"
CONF_VOLTAGE_SENSOR = "voltage_sensor"

CONF_CHANNEL1_WATT = "channel1_power"
CONF_CHANNEL1_IN = "channel1_in"
CONF_CHANNEL1_OUT = "channel1_out"

emontx_ns = cg.esphome_ns.namespace("emontx")
EmontxSensor = emontx_ns.class_("EmontxSensor", cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(EmontxSensor),
            cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
            cv.Optional(CONF_UPDATE_INTERVAL, default="1s"): cv.update_interval,
            cv.Optional(CONF_VOLTAGE_SENSOR): cv.use_id(sensor.Sensor),
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_VOLTAGE_SENSOR in config:
        sens = await cg.get_variable(config[CONF_VOLTAGE_SENSOR])
        cg.add(var.set_voltage_sensor(sens))

    if CONF_CHANNEL1_WATT in config:
        sens = await cg.get_variable(config[CONF_CHANNEL1_WATT])
        cg.add(var.channel1.set_watt_sensor(sens))

    if CONF_CHANNEL1_IN in config:
        sens = await cg.get_variable(config[CONF_CHANNEL1_IN])
        cg.add(var.channel1.set_energy_in_sensor(sens))

    if CONF_CHANNEL1_OUT in config:
        sens = await cg.get_variable(config[CONF_CHANNEL1_OUT])
        cg.add(var.channel1.set_energy_out_sensor(sens))