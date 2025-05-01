#include "TemperaturesAndConstants.h"
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Stepper.h>

OneWire oneWire(ONE_WIRE_BUS); // One wire bus
DallasTemperature sensors(&oneWire); // Pass pointer to sensors
Stepper stepper = Stepper(STEPS_PER_REVOLUTION, 2, 4, 3, 5); // Attach stepper to pins 2-5 (IN1 - IN4) - delcared IN1, IN3, IN2, IN4

void setup(void)
{
  // Start serial communication and temperature sensor
  Serial.begin(9600);
  sensors.begin();

  // Define digital read pins for starting 
  pinMode(START_BUTTON, INPUT);
  pinMode(SELECT_BUTTON, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(SPEAKER, OUTPUT);

  while (!await_select(selected_steep, steeping_presets)) {};
  delay(200); // Slight debounce delay (so that second select isn't skipped)
  while (!await_select(selected_cool, cooling_presets)) {};

  analogWrite(RED, 0); // Reset LED
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 0);

  // Lower stepper motor (neck control) and start waiting for the selected steep time
  stepper.setSpeed(5);
  stepper.step(STEPS_PER_REVOLUTION/4);

  // Wait for the user selected amount of time
  waitForSteep(steeping_presets[selected_steep].steep_time);

  // Raise neck and begin waiting for temperature (main loop)
  stepper.step(-STEPS_PER_REVOLUTION/4);
}

// Use loop mainly for temperature sensing
void loop(void) { 
  sensors.requestTemperatures(); 
  float f_temp = sensors.getTempFByIndex(0);
  
  Serial.print("Fahrenheit Temperature: ");
  Serial.println(f_temp);

  if (f_temp < cooling_presets[selected_cool].cool_temp) {
    // Notify user that tea is at desired temperature
    while (!signalDrinkTime()) {};

    // Enter end trap Todo: maybe a goto can be used to restart instead of hardware reset button
    while (true) {};
  }

  delay(1000);
}

// Note: Arduino internal clock does not have great accuracy (external timer would be better)
void waitForSteep(int minutes) {
  unsigned long start_time = millis();
  unsigned long wait_duration = minutes * 60000UL; // Convert to ms (unsigned long has lower risk of overflow)

  while (millis() - start_time < wait_duration) {
    // Flash LED while waiting
    analogWrite(RED, 255);
    analogWrite(GREEN, 255);
    analogWrite(BLUE, 255);
    delay(1000);

    analogWrite(RED, 0);
    analogWrite(GREEN, 0);
    analogWrite(BLUE, 0);
    delay(1000);

    // Give updates to serial
    Serial.print("Estimated Time Remaining: ");
    Serial.print((wait_duration - (millis() - start_time)));
    Serial.println(" millis");
  }
}

bool signalDrinkTime() {
  // Flash LED fast until user arrives
  analogWrite(RED, rand() % 256);
  analogWrite(GREEN, rand() % 256);
  analogWrite(BLUE, rand() % 256);

  // Turn speaker on for random tone
  int frequency = rand() % (3951-1047) + 1047;
  Serial.print("Frequency: ");
  Serial.println(frequency);
  tone(SPEAKER, frequency);
  delay(100);

  // Flash LED off
  analogWrite(RED, rand() % 256);
  analogWrite(GREEN, rand() % 256);
  analogWrite(BLUE, rand() % 256);
  noTone(SPEAKER);
  delay(100);

  // Check if start button is hit (similar to alarm clock in this case)
  bool end = digitalRead(START_BUTTON);
  delay(100); // Debounce
  return end;
}




