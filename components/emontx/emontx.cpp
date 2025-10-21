
#include "emontx.h"
#include "esphome/core/log.h"

static const char *const TAG = "power_meter";
namespace emontx {

EmontxSensor::EmontxSensor() {
	ESP_LOGI(TAG, "PowerMeter START");
}

void EmontxSensor::update() {
    while(available()) {
      uint8_t c = read();
      parse_byte(c);  // Your decoding logic
    }
}

void EmontxSensor::parse_byte(uint8_t c)  {
    static std::string buffer;

    if (c == '\n') {
        EmonMessage msg;
        if( msg.parse(buffer) ) {
            ESP_LOGI(TAG, "PowerMeter parse success");
        }


        buffer.clear();
    } else {
        buffer.push_back((char)c);
    }
}

/// Message
bool EmonMessage::parse(const std::string& message) {
    if (!message.starts_with("MSG:"))
		return false;

    ESP_LOGI(TAG, "PowerMeter parse %s", message.c_str());
	

	int l1 = message.length();
	int l2 = strlen(message.c_str());

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