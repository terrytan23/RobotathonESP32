// // // // SPDX-License-Identifier: Apache-2.0
// // // // Copyright 2021 Ricardo Quesada
// // // // http://retro.moe/unijoysticle2

// // #include "sdkconfig.h"
// // #include <Arduino.h>
// // #include <Bluepad32.h>
// // #include <uni.h>
// // #include "controller_callbacks.h"

// // extern ControllerPtr myControllers[BP32_MAX_GAMEPADS]; // BP32 library allows for up to 4 concurrent controller connections, but we only need 1

// // void dumpGamepad(ControllerPtr ctl) {
// //     Console.printf(
// //         "DPAD: %d A: %d B: %d X: %d Y: %d LX: %d LY: %d RX: %d RY: %d L1: %d R1: %d L2: %d R2: %d\n",
// //         ctl->dpad(),        // D-pad
// //         ctl->a(),           // Letter buttons
// //         ctl->b(),
// //         ctl->x(),
// //         ctl->y(),
// //         ctl->axisX(),        // (-511 - 512) left X Axis
// //         ctl->axisY(),        // (-511 - 512) left Y axis
// //         ctl->axisRX(),       // (-511 - 512) right X axis
// //         ctl->axisRY(),       // (-511 - 512) right Y axis
// //         ctl->l1(),           // Bumpers
// //         ctl->r1(),
// //         ctl->l2(),
// //         ctl->r2()
// //     );
// // }

// // void setup() {
// //     BP32.setup(&onConnectedController, &onDisconnectedController);
// //     BP32.forgetBluetoothKeys(); 
// //     esp_log_level_set("gpio", ESP_LOG_ERROR); // Suppress info log spam from gpio_isr_service
// //     uni_bt_allowlist_set_enabled(true);
// // }

// // void loop() {
// //     vTaskDelay(1); // Ensures WDT does not get triggered when no controller is connected
// //     BP32.update(); 
// //     for (auto myController : myControllers) { // Only execute code when controller is connected
// //         if (myController && myController->isConnected() && myController->hasData()) {        
          
// //             /*
// //             ====================
// //             Your code goes here!
// //             ====================
// //             */

// //             dumpGamepad(myController); // Prints the gamepad state, delete or comment if don't need
// //         }
// //     }
// // }



// // // #include "sdkconfig.h"
// // // #include <Arduino.h>

// // // #define IN1  16  // Control pin 1
// // // #define IN2  17  // Control pin 2

// // // void setup() {
// // //   Serial.begin(115200);
// // //   pinMode(IN1, OUTPUT);
// // //   pinMode(IN2, OUTPUT);
// // // }

// // // void loop() {
// // //   // Spin motor
// // //   analogWrite(IN1, 255);  // PWM signal
// // //   digitalWrite(IN2, LOW); // Direction control

// // //   delay(1000);  // Run for 1 second

// // //   // Stop motor
// // //   digitalWrite(IN1, LOW);
// // //   digitalWrite(IN2, LOW);

// // //   delay(1000); // Stop for 1 second
// // // }

#include "sdkconfig.h"
#include <Arduino.h>

#define IN1  16  // Control pin 1
#define IN2  17  // Control pin 2
#define IN3  18  // Control pin 1
#define IN4  19  // Control pin 2

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  // Spin motor
  digitalWrite(IN1, HIGH);  // PWM signal
  digitalWrite(IN2, LOW); // Direction control

  digitalWrite(IN3, LOW);  // PWM signal
  digitalWrite(IN4, HIGH); // Direction control

  delay(1000);  // Run for 1 second

  // Stop motor
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  delay(1000); // Stop for 1 second

  vTaskDelay(1); // Yield CPU to not starve other ESP32 processes and cause WDT reset
}

// #include "sdkconfig.h"
// #include <Arduino.h>

// #include <QTRSensors.h>

// QTRSensors qtr;
// uint16_t sensors[2];

// void setup() {
//     // set up Serial Communication and sensor pins
//     Serial.begin(115200);
//     qtr.setTypeAnalog(); // or setTypeAnalog()
//     qtr.setSensorPins((const uint8_t[]) {32, 33}, 2); // pin numbers go in the curly brackets {}, and number of sensors in use goes after

//     // calibration sequence
//     for (uint8_t i = 0; i < 250; i++) { 
//         Serial.printf("calibrating %d/250\n", i); // 250 is the number of calibrations recommended by manufacturer
//         qtr.calibrate(); 
//         delay(20);
//     }
// }

// void loop() {
//     qtr.readLineBlack(sensors); // Get calibrated sensor values returned into sensors[]
//     Serial.printf("S1: %d S2: %d\n", sensors[0], sensors[1]);
//     delay(250);
// }