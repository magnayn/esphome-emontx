import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL

DEPENDENCIES = ["uart"]

CONF_UART_ID = "uart_id"
CONF_VOLTAGE_SENSOR = "voltage_sensor"

CONF_CHANNELS = "channels"
CONF_WATT = "power"
CONF_ENERGY_IN = "energy_in"
CONF_ENERGY_OUT = "energy_out"

emontx_ns = cg.esphome_ns.namespace("emontx")
EmontxSensor = emontx_ns.class_("EmontxSensor", cg.PollingComponent, uart.UARTDevice)

channel_schema = cv.Schema({
    cv.Optional(CONF_WATT): sensor.sensor_schema(unit_of_measurement="W"),
    cv.Optional(CONF_ENERGY_IN): sensor.sensor_schema(unit_of_measurement="Wh"),
    cv.Optional(CONF_ENERGY_OUT): sensor.sensor_schema(unit_of_measurement="Wh"),
})


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(EmontxSensor),
            cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
            cv.Optional(CONF_UPDATE_INTERVAL, default="1s"): cv.update_interval,
            cv.Optional(CONF_VOLTAGE_SENSOR): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_CHANNELS): cv.Schema({
        cv.Optional("channel1"): channel_schema,
        cv.Optional("channel2"): channel_schema,
        cv.Optional("channel3"): channel_schema,
        cv.Optional("channel4"): channel_schema,
    })
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

    for i, channel_name in enumerate(["channel1", "channel2", "channel3", "channel4"]):
        channel_conf = config[CONF_CHANNELS][channel_name]
        if CONF_WATT in channel_conf:
            sens = await cg.get_variable(channel_conf[CONF_WATT])
            cg.add(var.channels[i].set_power_sensor(sens))
        if CONF_ENERGY_IN in channel_conf:
            sens = await cg.get_variable(channel_conf[CONF_ENERGY_IN])
            cg.add(var.channels[i].set_energy_in_sensor(sens))
        if CONF_ENERGY_OUT in channel_conf:
            sens = await cg.get_variable(channel_conf[CONF_ENERGY_OUT])
            cg.add(var.channels[i].set_energy_out_sensor(sens))