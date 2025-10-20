import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL

CONF_UART_ID = "uart_id"

emontx_ns = cg.esphome_ns.namespace("emontx")
EmontxSensor = emontx_ns.class_("EmontxSensor", cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = (
    sensor.sensor_schema(unit_of_measurement="V", icon="mdi:flash")
    .extend(
        {
            cv.GenerateID(): cv.declare_id(EmontxSensor),
            cv.Optional(CONF_UART_ID): cv.use_id(uart.UARTComponent),
            cv.Optional(CONF_UPDATE_INTERVAL, default="1s"): cv.update_interval,
        }
    )
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)