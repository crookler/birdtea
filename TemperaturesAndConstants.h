#include <Arduino.h>

#define SPEAKER 12
#define ONE_WIRE_BUS 6
#define STEPS_PER_REVOLUTION 2048
//#define STEPS_PER_REVOLUTION 4096
#define START_BUTTON 7
#define SELECT_BUTTON 8
#define RED 11 
#define GREEN 10
#define BLUE 9

// Hex codes for color
typedef struct {
  int red_code;
  int green_code;
  int blue_code;
} color;

// Define steep and steep time (minutes) and the hex codes for the colors
typedef struct {
  int steep_time;
  color hex_codes;
} steep;

// Define temperature to notify user (degrees fahrenheit) and colors
typedef struct {
  float cool_temp;
  color hex_codes;
} temperature;

// Array of tea steeping times and accompnaying index
const steep steeping_presets[] = {
  {1, {0, 0, 255}}, // Deep blue
  {3, {255, 0, 255}}, // Purple
  {5, {0, 255, 0}}, // Green
  {10, {255, 0, 0}} // Red
};

int selected_steep = 0; // Index for above array tracking selected steep time

// Array of structs TODO: fine tune temperatures
const temperature cooling_presets[] = {
  {80.0, {255, 80, 0}}, // Bright Orange
  {70.0, {200, 200, 0}}, // Cool Yellow
  {60.0, {0, 100, 255}}  // Cool Blue
};

int selected_cool = 0; // Index for above array tracking selected temperature

// Function for index over one of the above presets (T is a struct and N is the number of members in the strut)
// Need to declare template right after structs for Arduino compiler for some reason
template<typename T, size_t N>
bool await_select(int &index, T (&presets)[N]) { // Read only presets
  // Advance index and wrap if at end of array
  if (digitalRead(SELECT_BUTTON)) {
    index = (index + 1) % N;
    delay(100); // Debounce
  }

  Serial.print("Select Index: ");
  Serial.println(index);

  // Write to RGB LED with color presets
  analogWrite(RED, presets[index].hex_codes.red_code);
  analogWrite(GREEN, presets[index].hex_codes.green_code);
  analogWrite(BLUE, presets[index].hex_codes.blue_code);

  // Check if start button is hit
  bool start = digitalRead(START_BUTTON);
  delay(100); // Debounce
  
  return start;
}