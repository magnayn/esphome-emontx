
#include "emontx.h"
#include "esphome/core/log.h"

static const char *const TAG = "emontx";
namespace emontx {

EmontxSensor::EmontxSensor() {
	
}

void EmontxSensor::setup() {
    
}

void EmontxSensor::update() {
	ESP_LOGI(TAG, "PowerMeter UPDATE");
    while(available()) {
      uint8_t c = read();
      parse_byte(c);  // Your decoding logic
    }
}

void EmontxSensor::parse_byte(uint8_t c)  {
    static std::string buffer;
	//ESP_LOGW(TAG, "CHAR %d", c);

	//ESP_LOGW(TAG, "BUF: %s", buffer.c_str());
	if( c == '\r') {
		// ignore
	}
    else if (c == '\n') {
		// Null terminate?
		//buffer.push_back(0);

		ESP_LOGW(TAG, "BUF: %s", buffer.c_str());

        EmonMessage msg;
        if( msg.parse(buffer) ) {
            ESP_LOGI(TAG, "PowerMeter parse success");
			

			publish(msg);

        }
		else {
			ESP_LOGI(TAG, "PowerMeter parse rejected");
		}


        buffer.clear();
    } else {
        buffer.push_back((char)c);
    }
}

void EmontxSensor::publish(const EmonMessage& msg) {
	ESP_LOGD(TAG, "SEQ %d", msg.msg_seq);
	ESP_LOGD(TAG, "V %f", msg.v);

	for( int i=0; i<4; i++ ) {
		ESP_LOGD(TAG, "%d: %d %d %d", i, msg.power[i], msg.energy_in[i], msg.energy_out[i]);
	}

	channel1.publish(msg.power[0], msg.energy_in[0], msg.energy_out[0]);
	channel2.publish(msg.power[1], msg.energy_in[1], msg.energy_out[1]);
	channel3.publish(msg.power[2], msg.energy_in[2], msg.energy_out[2]);
	channel4.publish(msg.power[3], msg.energy_in[3], msg.energy_out[3]);

	if (voltage_sensor_ != nullptr) {
      voltage_sensor_->publish_state(msg.v);
    }

}


/// Message
bool EmonMessage::parse(const std::string& message) {
    if (!message.starts_with("MSG:"))
		return false;

    ESP_LOGI(TAG, "PowerMeter parse '%s'.", message.c_str());
	

	int l1 = message.length();
	int l2 = strlen(message.c_str());

	ESP_LOGI(TAG, "L1 %d L2 %d", l1, l2);
	

//	Serial.printf("L1 %d\n L2 %d\n", l1, l2);

	char *cstr = new char[message.length() + 1];
	strcpy(cstr, message.c_str());
	char *saveptr;
	char *token = strtok_r(cstr, ",", &saveptr);

	while (token != NULL)
	{
//		Debugging.println("TOKEN! ");
		if (strncmp("MSG:", token, 4) == 0)
		{
			msg_seq = atoi(token + 4);
		}
		else if (strncmp("Vrms:", token, 5) == 0)
		{
			v = atof(token + 5);
		}
		else if (token[0] == 'P')
		{
			int idx = (int)(token[1] - '1');
			power[idx] = atoi(token + 3);
		}
		else if (token[0] == 'E')
		{
			int idx = (int)(token[1] - '1'); // P1 to P4, E1 to E4 so '1' indexed.

			energy_in[idx] = atoi(token + 3);
			char *eout = strchr(token + 3, ' ');
			if (eout != NULL)
			{
				energy_out[idx] = atoi(eout + 1);
			}
		}

		token = strtok_r(NULL, ",", &saveptr);
	}

	delete[] cstr;
//	Debugging.println(".. done");
	// Make sure V > 200V as a test.
	if (v < 200 || msg_seq == 0)
		return false;
  //  Debugging.println(".. ok");
	return true;
}
}