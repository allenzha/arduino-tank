/*
 * Copyright (C) 2016 Brian Masney
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <Wire.h>
#include <Servo.h> 
#include <LiquidCrystal.h> 

#define LCD_RS_PIN  8
#define LCD_EN_PIN  9  
#define LCD_D4_PIN 10
#define LCD_D5_PIN 11
#define LCD_D6_PIN 12
#define LCD_D7_PIN 13

#define RIGHT_SENSOR_TRIG_PIN 6
#define RIGHT_SENSOR_ECHO_PIN 7

#define LEFT_SENSOR_TRIG_PIN 4
#define LEFT_SENSOR_ECHO_PIN 5

#define LEFT_SERVO_PIN  3
#define RIGHT_SERVO_PIN 2

#define CLOSEST_DISTANCE_CM 50

Servo leftServo;
Servo rightServo;

LiquidCrystal lcd(LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

void setup() {
  lcd.begin(16, 2);

  Serial.begin(9600);

  leftServo.attach(LEFT_SERVO_PIN);
  rightServo.attach(RIGHT_SERVO_PIN);

  pinMode(RIGHT_SENSOR_TRIG_PIN, OUTPUT);
  pinMode(RIGHT_SENSOR_ECHO_PIN, INPUT);

  pinMode(LEFT_SENSOR_TRIG_PIN, OUTPUT);
  pinMode(LEFT_SENSOR_ECHO_PIN, INPUT);
}

long measureUltrasonicDistance(int trigPin, int echoPin) {
  long microsecs;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  microsecs = pulseIn(echoPin, HIGH);

  return microsecs / 29 / 2;
}

void loop()
{
  char buf[25];

  int leftServoPosition = 90;
  int rightServoPosition = 90;

  long rightDistanceCm = measureUltrasonicDistance(RIGHT_SENSOR_TRIG_PIN, RIGHT_SENSOR_ECHO_PIN);
  long leftDistanceCm = measureUltrasonicDistance(LEFT_SENSOR_TRIG_PIN, LEFT_SENSOR_ECHO_PIN);

  int ifBlock;
  if (leftDistanceCm < CLOSEST_DISTANCE_CM and rightDistanceCm < CLOSEST_DISTANCE_CM) {
    if(leftDistanceCm < rightDistanceCm) {
      ifBlock = 1;
      leftServoPosition = 0;
      rightServoPosition = 0;
    }
    else {
      ifBlock = 2;
      leftServoPosition = 180;
      rightServoPosition = 180;
    }
  }
  else if (leftDistanceCm < CLOSEST_DISTANCE_CM) {
    ifBlock = 3;
    leftServoPosition = 180;
    rightServoPosition = 180;
  }
  else if (rightDistanceCm < CLOSEST_DISTANCE_CM) {
    ifBlock = 4;
    leftServoPosition = 0;
    rightServoPosition = 0;
  }
  else {
    ifBlock = 5;
    leftServoPosition = 180;
    rightServoPosition = 0;
  }

  leftServo.write(leftServoPosition);
  rightServo.write(rightServoPosition);

  snprintf(buf, sizeof(buf), "%d: L:%ld R:%ld        ", ifBlock, leftDistanceCm, rightDistanceCm);
  Serial.print(buf);
  Serial.print(" ");
  lcd.setCursor(0, 0);
  lcd.print(buf);

  snprintf(buf, sizeof(buf), "S: L:%d R:%d        ", leftServoPosition, rightServoPosition);
  Serial.println(buf);
  lcd.setCursor(0, 1);
  lcd.print(buf);

  delay(50);
}

