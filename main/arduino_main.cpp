// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 Ricardo Quesada
// http://retro.moe/unijoysticle2

#include "sdkconfig.h"
#include <Arduino.h>
#include <Bluepad32.h>
#include <uni.h>
#include "controller_callbacks.h"
#include <QTRSensors.h>
#include <ESP32SharpIR.h>
#include <Wire.h>
#include <Arduino_APDS9960.h>
#include <bits/stdc++.h>
#include <array>
#include <ESP32Servo.h>

// CONTROLLER
extern ControllerPtr myControllers[BP32_MAX_GAMEPADS]; // BP32 library allows for up to 4 concurrent controller connections, but we only need 1

// MOTORS
#define IN1  16  // Control pin 1
#define IN2  17  // Control pin 2
#define IN3  18  // Control pin 1
#define IN4  19  // Control pin 2

void moveForward() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void turnRight() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void turnLeft() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void moveBack() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void stop() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

// LINE SENSOR
QTRSensors qtr;
uint16_t sensors[4];

void lineSensor() {
    qtr.readLineBlack(sensors); // Get calibrated sensor values returned into sensors[]
    Serial.printf("S1: %d S2: %d S3: %d S4: %d\n", sensors[0], sensors[1], sensors[2], sensors[3]);
    //point between black and white
    //while left outer sensor reads black, turn left
    int boundary = 500;
    while (sensors[0] < boundary)
    {
        turnLeft();
    }
    //while right outer sensor reads black, turn right
    while(sensors[3] < boundary)
    {
        turnRight();
    }
    delay(250);
}

// IR SENSOR
ESP32SharpIR IRLeft(ESP32SharpIR::GP2Y0A21YK0F, 36); 
ESP32SharpIR IRRight(ESP32SharpIR::GP2Y0A21YK0F, 12);
ESP32SharpIR IRFront(ESP32SharpIR::GP2Y0A21YK0F, 39);

// COLOR SENSOR
#define APDS9960_INT 0
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ 100000

TwoWire I2C_0 = TwoWire(0);
APDS9960 apds = APDS9960(I2C_0, APDS9960_INT);

extern ControllerPtr myControllers[BP32_MAX_GAMEPADS];

int color = -1;
std::array<int, 3> initialColors = {};
bool changed = false;

void colorSensor(int index){

    if (color == -1){ // if uninitialized, read color
        color = index;
    }
}

Servo myServo1;  // create servo object to control a servo
Servo myServo2;  // create another servo object to control a second servo

// void dumpGamepad(ControllerPtr ctl) {
//     Console.printf(
//         "DPAD: %d A: %d B: %d X: %d Y: %d LX: %d LY: %d RX: %d RY: %d L1: %d R1: %d L2: %d R2: %d\n",
//         ctl->dpad(),        // D-pad
//         ctl->a(),           // Letter buttons
//         ctl->b(),
//         ctl->x(),
//         ctl->y(),
//         ctl->axisX(),        // (-511 - 512) left X Axis
//         ctl->axisY(),        // (-511 - 512) left Y axis
//         ctl->axisRX(),       // (-511 - 512) right X axis
//         ctl->axisRY(),       // (-511 - 512) right Y axis
//         ctl->l1(),           // Bumpers
//         ctl->r1(),
//         ctl->l2(),
//         ctl->r2()
//     );
// }

void setup() {
    // CONTROLLER
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    delay(250);
    esp_log_level_set("gpio", ESP_LOG_ERROR); // Suppress info log spam from gpio_isr_service
    uni_bt_allowlist_set_enabled(true);

    // MOTORS
    Serial.begin(115200); //9600
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    // // LINE SENSOR
    // // set up Serial Communication and sensor pins

    // Serial.begin(115200);
    // qtr.setTypeAnalog(); // or setTypeAnalog()
    // qtr.setSensorPins((const uint8_t[]) {32, 33, 35, 34}, 4); // pin numbers go in the curly brackets {}, and number of sensors in use goes after

    // // calibration sequence
    // for (uint8_t i = 0; i < 250; i++) { 
    //     Console.printf("calibrating %d/250\n", i); // 250 is the number of calibrations recommended by manufacturer
    //     qtr.calibrate(); 
    //     delay(20);
    // }
    
    // IR SENSOR
    IRLeft.setFilterRate(1.0f);
    IRRight.setFilterRate(1.0f);
    IRFront.setFilterRate(1.0f);

    // COLOR SENSOR
    //sets up I2C protocol
    I2C_0.begin(I2C_SDA, I2C_SCL, I2C_FREQ);

    //sets up color sensor
    apds.setInterruptPin(APDS9960_INT);
    apds.begin();

    // SERVO
    myServo1.attach(25);  // attaches the servo on pin to the servo object
    myServo2.attach(26); // attaches the servo on pin to the servo object

    myServo1.write(180);
    myServo2.write(90);
}

void loop() {
    vTaskDelay(1); // Ensures WDT does not get triggered when no controller is connected
    BP32.update(); 
    for (auto myController : myControllers) { // Only execute code when controller is connected
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->axisY() != 0) { // MANUAL
                if (myController->axisY() < 0 || myController->r2()) {
                    // Spin motor
                    Console.println("Spin motors");
                    moveForward();
                } else if (myController->axisX() < 0) {
                    Console.println("Turn left");
                    turnLeft();
                } else if (myController->axisX() > 0) {
                    Console.println("Turn right");
                    turnRight();
                }  else if (myController->axisY() > 0) {
                // Stop motor
                    Console.println("Reverse motors");
                    moveBack();
                }
            } else if (myController->b()) { // LINE SENSOR
                moveForward();
                delay(75);
                stop();
                qtr.readLineBlack(sensors); // Get calibrated sensor values returned into sensors[]
                Console.printf("S1: %d S2: %d S3: %d S4: %d\n", sensors[0], sensors[1], sensors[2], sensors[3]);
                //point between black and white
                //while left outer sensor reads black, turn left
                int boundary = 500;
                if (sensors[0] < boundary)
                {
                    stop();
                    delay(5000);
                    turnLeft();
                    delay(5000);
                } else if (sensors[3] < boundary) //while right outer sensor reads black, turn right
                {
                    stop();
                    delay(5000);
                    turnRight();
                    delay(5000);
                }
                delay(250);
            } else if (myController->a()) { // IR SENSOR
                moveForward();
                float leftdistance = IRLeft.getDistanceFloat(); 
                float rightdistance = IRRight.getDistanceFloat(); 
                float frontdistance = IRFront.getDistanceFloat(); 
                Console.println(leftdistance);
                Console.println(rightdistance);
                Console.println(frontdistance);
                delay(100); 

                int threshold = 20;
                //default forward
                // if(leftdistance < threshold && rightdistance < threshold){
                //     Console.println("hello");
                //     moveForward();
                //     delay(20);
                // }

                //turn right
                //check when wall is close to front and left sensors
                if(frontdistance < threshold && leftdistance < threshold){
                    Console.println("hi");
                    stop();
                    delay(1000);
                    turnRight();
                    delay(600);
                }
                //turn left
                //check when wall is close to front and right sensors
                if(frontdistance < threshold && rightdistance < threshold){
                    Console.println("bi");
                    stop();
                    delay(1000);
                    turnLeft();
                    delay(600);
                }
                Console.println(myController->a());
            } else if (myController->x()) { // COLOR SENSOR
                // LED
                pinMode(2, OUTPUT);
                digitalWrite(2, HIGH);
                int r, g, b, a;

                while (!apds.colorAvailable()) { delay(5); } // Wait until color is read from the sensor 
                apds.readColor(r, g, b, a);
                    int colors[] = {r, g, b};
                    int largestColorIndex = 0; // detected color
                    if (colors[1] > colors[largestColorIndex]) largestColorIndex = 1;
                    if (colors[2] > colors[largestColorIndex]) largestColorIndex = 2;

                    colorSensor(largestColorIndex); // set reference color

                    // do {
                    //     moveForward();
                    // } while (color == largestColorIndex) // exit after off first color

                    // moveForward()
                    if ((color == largestColorIndex) && !changed) {
                        Console.println("forward");
                        moveForward();
                        delay(500);
                        stop();
                    } else if ((color == largestColorIndex) && changed) {
                        Console.println("stop");
                        stop();
                    } else { 
                        Console.println("changed");
                        changed = true;
                        moveForward();
                        delay(500);
                        stop();
                    }
                    
                    // do {
                    //     moveForward();
                    // } while (color != largestColorIndex) // exit after same color is found

                delay(1000); // if it doesnt work try vTaskDelay(1);
                Serial.printf("RED: %d GREEN: %d BLUE: %d AMBIENT: %d\n", r, g, b, a);
                vTaskDelay(1); // Yield CPU to not starve other ESP32 processes and cause WDT reset
            } else if (myController->y()) { // SERVO
                myServo1.write(0);
                delay(2000);
                myServo2.write(0);
                delay(1000);
                myServo1.write(180);
                delay(1000);
                myServo2.write(90);
                delay(1000);
            } else if (myController->axisY() == 0 || myController->l2()) { // STOP
                stop();
            }
        }
    }
}