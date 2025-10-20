#ifndef __EMONTX_H__
#define __EMONTX_H__

#include "esphome.h"


class Emontx : public PollingComponent, public UARTDevice {
   public:
   Emontx(UArTComponent* parent) : UARTDevice(paremt) {}

   void setup() override {

   }

   void update() override;

   private:
    void parse_byte(uint8+t c);

}

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

#endif