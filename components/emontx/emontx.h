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

class EmontxSensor : public esphome::PollingComponent, public esphome::uart::UARTDevice {
   public:
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