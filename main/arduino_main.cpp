// // // // SPDX-License-Identifier: Apache-2.0
// // // // Copyright 2021 Ricardo Quesada
// // // // http://retro.moe/unijoysticle2

#include "sdkconfig.h"
#include <Arduino.h>
#include <Bluepad32.h>
#include <uni.h>
#include "controller_callbacks.h"

#define IN1  16  // Control pin 1
#define IN2  17  // Control pin 2
#define IN3  18  // Control pin 1
#define IN4  19  // Control pin 2

extern ControllerPtr myControllers[BP32_MAX_GAMEPADS]; // BP32 library allows for up to 4 concurrent controller connections, but we only need 1

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

void setup() {
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys(); 
    esp_log_level_set("gpio", ESP_LOG_ERROR); // Suppress info log spam from gpio_isr_service
    uni_bt_allowlist_set_enabled(true);

    // Serial.begin(115200);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}

void loop() {
    vTaskDelay(1); // Ensures WDT does not get triggered when no controller is connected
    BP32.update(); 
    for (auto myController : myControllers) { // Only execute code when controller is connected
        if (myController && myController->isConnected() && myController->hasData()) {        
            if (myController->axisY() < 0) {
              // Spin motor
              Console.println("Spin motors");
              digitalWrite(IN1, HIGH);  // PWM signal
              digitalWrite(IN2, LOW); // Direction control

              digitalWrite(IN3, LOW);  // PWM signal
              digitalWrite(IN4, HIGH); // Direction control
            } else {
              // Stop motor
              Console.println("Stop motors");
              digitalWrite(IN1, LOW);
              digitalWrite(IN2, LOW);

              digitalWrite(IN3, LOW);
              digitalWrite(IN4, LOW);
            }

            
            

            // delay(1000);  // Run for 1 second

           

            // delay(1000); // Stop for 1 second

            vTaskDelay(1); // Yield CPU to not starve other ESP32 processes and cause WDT reset
        }
    }
}