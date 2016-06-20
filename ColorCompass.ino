/*******************************************************************************
  A program to display compass heading angle as a color on an RGB LED.

  Set up the LED so that when the color pin is low, the LED is off.

  andy.oliver at raritet dot co
*******************************************************************************/

// INCLUDES
#include <Wire.h>
// Get these from the Adafruit website
#include <Adafruit_LSM303.h>
#include <Adafruit_Sensor.h>

// Assign a unique ID to this sensor.
Adafruit_LSM303_Mag mag = Adafruit_LSM303_Mag(12345);

// CONSTANT DECLARATIONS
// What pin is connected to what color in the RGB LED?
const int RED_PIN = 11;
const int GREEN_PIN = 10;
const int BLUE_PIN = 9;
// Set MAX_PWM to 255 for common anode RGB LEDs and 0 for common cathode.
const int MAX_PWM = 0;
// Set MIN_PWM to 0 for common anode RGB LEDs and 255 for common cathode.
const int MIN_PWM = 255;
// Smoothing factor 0 < ALPHA < 1 (larger APLHA is less smooth).
const float ALPHA = 0.0625;

// VARIABLE DECLARATIONS
// The previously retrieved heading X and Y values (used for smoothing).
float oldHeadingX = 0.0;
float oldHeadingY = 0.0;
// The newly retrieved heading X and Y values (used for smoothing).
float newHeadingX = 0.0;
float newHeadingY = 0.0;
// The heading angle calculated from the heading X and Y values.
float headingAngle = 0.0;

// CLASS DECLARATIONS

// SETUP FUNCTION
void setup(void) {
  // Start serial communication to monitor on the terminal for troubleshooting.
  Serial.begin(9600);

  // Initialize the sensor.
  if(!mag.begin()) {
    // And if it doesn't initialize, there was a problem.
    Serial.println("No LSM303 detected. Check your wiring.");
    while(1);
  }

  // Even though it's not necessary, it seems cleaner to set the pins as output.
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Get a new sensor event.
  sensors_event_t event;
  mag.getEvent(&event);
  // Load up old heading components with the first measurement.
  oldHeadingX = event.magnetic.x;
  oldHeadingY = -event.magnetic.y;
}

// MAIN PROGRAM LOOP
void loop(void) {
  // Get the heading X and Y readings, smooth, and calculate the heading.
  // Get a new sensor event.
  sensors_event_t event;
  mag.getEvent(&event);
  // Load up new heading components with smoothed values.
  newHeadingX = ALPHA*event.magnetic.x + (1-ALPHA)*oldHeadingX;
  newHeadingY = -ALPHA*event.magnetic.y + (1-ALPHA)*oldHeadingY;
  // Store the new headings under the old headings for future smoothing.
  oldHeadingX = newHeadingX;
  oldHeadingY = newHeadingY;
  // Calculate the heading angle in degrees.
  headingAngle = (atan2(newHeadingX, newHeadingY) * 180) / PI;
  // Normalize it to 0 to 360.
  if(headingAngle < 0) {
    headingAngle = 360 + headingAngle;
  }
  // Send the heading to the console for monitoring.
  // Serial.println(headingAngle);
  // delay(100);
  // Display the heading angle as a color on the RGB LED.
  // Note that the function expects an integer.
  displayColor(HIGH, int(headingAngle));
}

// FUNCTIONS
// Function to display the color, passed as an angle in degrees, on the RGB LED.
void displayColor(boolean enable, int colorAngle) {
  // Initialize local color intensity variables.
  int red, green, blue;
  // Check if we have enabled the RGB LED.
  if(enable) {
    // Follow the color ramps in six sections as shown here:
    // https://en.wikipedia.org/wiki/File:HSV-RGB-comparison.svg
    if((colorAngle >= 0) && (colorAngle < 60)) {
      red = MAX_PWM;
      green = map(colorAngle, 0, 60, MIN_PWM, MAX_PWM);
      blue = MIN_PWM;
    }
    if((colorAngle >= 60) && (colorAngle < 120)) {
      red = map(colorAngle, 60, 120, MAX_PWM, MIN_PWM);
      green = MAX_PWM;
      blue = MIN_PWM;
    }
    if((colorAngle >= 120) && (colorAngle < 180)) {
      red = MIN_PWM;
      green = MAX_PWM;
      blue = map(colorAngle, 120, 180, MIN_PWM, MAX_PWM);
    }
    if((colorAngle >= 180) && (colorAngle < 240)) {
      red = MIN_PWM;
      green = map(colorAngle, 180, 240, MAX_PWM, MIN_PWM);
      blue = MAX_PWM;
    }
    if((colorAngle >= 240) && (colorAngle < 300)) {
      red = map(colorAngle, 240, 300, MIN_PWM, MAX_PWM);
      green = MIN_PWM;
      blue = MAX_PWM;
    }
    if((colorAngle >= 300) && (colorAngle < 360)) {
      red = MAX_PWM;
      green = MIN_PWM;
      blue = map(colorAngle, 300, 360, MAX_PWM, MIN_PWM);
    }
    // Output the colors by sending the color intensity to the LEDs using
    // analogWrite().
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
  } else {
    // Turn off the LED by sending MIN_PWM for all color intensities.
    analogWrite(RED_PIN, MIN_PWM);
    analogWrite(GREEN_PIN, MIN_PWM);
    analogWrite(BLUE_PIN, MIN_PWM);
  }
}
