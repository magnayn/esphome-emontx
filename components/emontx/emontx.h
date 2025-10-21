#ifndef __EMONTX_H__
#define __EMONTX_H__

#include "esphome.h"
#include "esphome/components/uart/uart.h"

namespace emontx {

class EmonMessage
{
public:
    bool parse(const std::string &str);

    int msg_seq = 0;
    float v = 0;
    int power[4] = {0, 0, 0, 0};
    int energy_in[4] = {0, 0, 0, 0};
    int energy_out[4] = {0, 0, 0, 0};
};

class EmontxChannel {
public:
    EmontxChannel() {}

    void set_power_sensor(esphome::sensor::Sensor *s) { power_sensor_ = s; }
    void set_energy_in_sensor(esphome::sensor::Sensor *s) { energy_in_sensor_ = s; }
    void set_energy_out_sensor(esphome::sensor::Sensor *s) { energy_out_sensor_ = s; }

    void publish(int watt, int in, int out) {
        if (power_sensor_) power_sensor_->publish_state(watt);
        if (energy_in_sensor_) energy_in_sensor_->publish_state(in);
        if (energy_out_sensor_) energy_out_sensor_->publish_state(out);
    }

private:

    esphome::sensor::Sensor *power_sensor_{nullptr};
    esphome::sensor::Sensor *energy_in_sensor_{nullptr};
    esphome::sensor::Sensor *energy_out_sensor_{nullptr};
};

class EmontxSensor : public esphome::PollingComponent, public esphome::uart::UARTDevice {
   public:

    EmontxChannel channels[4];
    
   EmontxSensor();

   void setup() override;

   void update() override;


    // Setters
    // Setter for voltage sensor
    void set_voltage_sensor(esphome::sensor::Sensor *sensor) { voltage_sensor_ = sensor; }


   private:
    void parse_byte(uint8_t c);
    void publish(const EmonMessage& message);


    esphome::sensor::Sensor *voltage_sensor_ = nullptr;

};


}
#endif