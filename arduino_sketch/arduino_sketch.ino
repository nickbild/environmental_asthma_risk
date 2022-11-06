/* Edge Impulse ingestion SDK
 * Copyright (c) 2022 EdgeImpulse Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Modified by Nick Bild
 * November 2022
 * Note:
 * In the Arduino_BHY2 library, this requires changhing the WORK_BUFFER_SIZE in the file "BoschSensortech.h" like so:
 * #define WORK_BUFFER_SIZE    64
 * Otherwise, the Nicla Sense ME will run out of memory and crash.
 */

#include <asthma_risk_inferencing.h>
#include "Arduino_BHY2.h"
#include <ArduinoBLE.h>

Sensor temperature(SENSOR_ID_TEMP);
Sensor humidity(SENSOR_ID_HUM);
float temperatureValue = 0;
float humidityValue = 0;

// temperature, humidity
float features[2] = {};

BLEService asthmaRiskService("150D");
BLEBoolCharacteristic asthmaRiskChar("2AE2", BLERead | BLENotify);

/**
 * @brief      Copy raw feature data in out_ptr
 *             Function called by inference library
 *
 * @param[in]  offset   The offset
 * @param[in]  length   The length
 * @param      out_ptr  The out pointer
 *
 * @return     0
 */
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

/**
 * @brief      Arduino setup function
 */
void setup()
{
//    Serial.begin(115200);
//    while (!Serial);

    BHY2.begin();
    temperature.begin();
    humidity.begin();

    if (!BLE.begin()) {
//      Serial.println("starting BLE failed!");
      while (1);
    }
    BLE.setLocalName("AsthmaRiskMonitor");
    BLE.setAdvertisedService(asthmaRiskService); // add the service UUID
    asthmaRiskService.addCharacteristic(asthmaRiskChar); // add the level characteristic
    BLE.addService(asthmaRiskService); // Add the service
    asthmaRiskChar.writeValue(0); // set initial value for this characteristic
    BLE.advertise();
}

/**
 * @brief      Arduino main function
 */
void loop()
{
    BLEDevice central = BLE.central();
    if (central) {
       while (central.connected()) {
        BHY2.update();
        temperatureValue = temperature.value();
        humidityValue = humidity.value();
        features[0] = temperatureValue;
        features[1] = humidityValue;
    
        ei_impulse_result_t result = { 0 };
    
        // the features are stored into flash, and we don't want to load everything into RAM
        signal_t features_signal;
        features_signal.total_length = sizeof(features) / sizeof(features[0]);
        features_signal.get_data = &raw_feature_get_data;
    
        // invoke the impulse
        EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
        if (res != 0) return;
    
//        Serial.println(temperatureValue);
//        Serial.println(humidityValue);
//        Serial.println(result.classification[0].value);
//        Serial.println("----");

        // Write asthma risk level to BLE packet.
        if (result.classification[0].value > 5) {
          asthmaRiskChar.setValue(1);
        } else {
          asthmaRiskChar.setValue(0);
        }
       }
    }
    
    delay(3000);
}
