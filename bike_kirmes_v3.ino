#include <Adafruit_NeoPixel.h>

#define NUM_SEGMENTS 2
#define SEGMENT_NUM_LEDS 70

// LED Definition
#define LED_LEFT_PIN 1
Adafruit_NeoPixel led_left = Adafruit_NeoPixel(SEGMENT_NUM_LEDS, LED_LEFT_PIN, NEO_GRB + NEO_KHZ800);

#define LED_RIGHT_PIN 12
Adafruit_NeoPixel led_right = Adafruit_NeoPixel(SEGMENT_NUM_LEDS, LED_RIGHT_PIN, NEO_GRB + NEO_KHZ800);


// Input Definitions
#define BUTTON 14
#define INDICATOR_LEFT 4
#define INDICATOR_RIGHT 5
#define SWITCH 13

#define SOFTWARE_DEBOUNCE_MS 50
#define BUTTON_LONG_PRESS_THRESHHOLD 400

// Effect Definitions
#define FRAMETIME_MS 10
#define LOOP_LENGTH 100




volatile bool buttonPressed = false;
volatile bool switchActive = false;
volatile bool indicatorLeftActive = false;
volatile bool indicatorRightActive = false;
unsigned long lastDebounceTime = 0;
unsigned long buttonPressTime = 0;
volatile bool longPressActive = false;


Adafruit_NeoPixel segments[] = {led_left, led_right};

int red[] = {255,10,0};
int white[] = {150,150,150};
int amber[] = {255,60,0};
int blue[] = {20,40,200};


#define NUM_MODES 5 //Anzahl Effekte, entsprechend anpassen
typedef void (*FunctionPtr)(int);
FunctionPtr modes[NUM_MODES];

uint mode = 0;

#define NUM_EFFECTS 2
FunctionPtr effects[NUM_EFFECTS];

uint global_frame_counter = 0;

void loadFunctionMap(){
  modes[0] = [](int frame) { rainbow_wave(frame, 45); };
  modes[1] = [](int frame) { rainbow_wave(frame, 200); };
  modes[2] = [](int frame) { single_color(red); };
  modes[3] = [](int frame) { color_fade(frame, 100); };
  modes[4] = [](int frame) { police(frame); };

  effects[0] = [](int frame) { strobe(frame); };
  effects[1] = [](int frame) { brake_light(frame); };

}

void initialize_inputs(){
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(INDICATOR_LEFT, INPUT_PULLUP);
  pinMode(INDICATOR_RIGHT, INPUT_PULLUP);
  pinMode(SWITCH, INPUT_PULLUP);
}

void initialize_outputs(){
  led_left.begin();
  led_right.begin();
}

void setup() {
  initialize_outputs();
  initialize_inputs();
  loadFunctionMap();
}

void input_polling() {
  button_handler();
  indicator_left_handler();
  indicator_right_handler();
  switch_handler();
}

void loop(){
  for(int frame = 0; frame < LOOP_LENGTH; frame++){
    global_frame_counter++;
    input_polling();
    if(longPressActive && buttonPressed){
      if(switchActive){
        effects[0](frame);
        Serial.println("switchActive");
      } else {
        effects[1](frame);
        Serial.println("!switchActive");
      }
    } else if( indicatorLeftActive){
      indicatorLeft(frame);
      Serial.println("indicatorLeftActive");
    } else if( indicatorRightActive) {
      indicatorRight(frame);
      Serial.println("indicatorRightActive");
    } else {
      Serial.println("else");

      modes[mode](frame);
    }
    Serial.println("delay");

    delay(FRAMETIME_MS);
  }
  Serial.println("loop");

}

int single_color(int color[]){
  for (int i = 0; i < NUM_SEGMENTS; i++){
    for (int j = 0; j < SEGMENT_NUM_LEDS; j++){
      segments[i].setPixelColor(j, segments[i].Color(color[0],color[1],color[2]));
    }
    segments[i].show();
  }
  return 0;
}

int rainbow_wave(int frame, int brightness){
  int hueOffset = (frame * 65535) / LOOP_LENGTH;  // Maps frame number to hue 0-65535
  int spread = 2000; // the higher the number, the fewer leds are between repeating colors
  for (int segment = 0; segment < NUM_SEGMENTS; segment++) {
    for (int i = 0; i < SEGMENT_NUM_LEDS; i++) {
      int hue = (i * spread) - hueOffset;  // Adjust hue based on position and frame
      segments[segment].setPixelColor(i, segments[segment].ColorHSV(hue, 255, brightness));
    }
    segments[segment].show();
  }
  return 0;
}

void color_fade(int frame, int brightness){
  float speed_modifier = 0.5;
  int hueStep = 65535 / LOOP_LENGTH * speed_modifier;
    for (int segment = 0; segment < NUM_SEGMENTS; segment++) {
      for (int i = 0; i < SEGMENT_NUM_LEDS; i++) {
        int hue = global_frame_counter * hueStep;
        segments[segment].setPixelColor(i, segments[segment].ColorHSV(hue, 255, brightness));
      }
    segments[segment].show();
  }
}

void police(int frame) {
  int PATTERN_FLASHES = 3;
  int SHORT_PAUSE = 6;
  int LONG_PAUSE = 12;
  int FLASH_DURATION = 2; // Number of frames LEDs stay on for each flash

  int patternLength = (PATTERN_FLASHES * (FLASH_DURATION + SHORT_PAUSE)) + LONG_PAUSE; // Total pattern cycle length

  // Determine if the current frame is within a flash or pause interval
  int cycleFrame = frame % patternLength; // Frame within the current cycle

  // Calculate if LEDs should be on (flash) or off based on the cycle frame
  bool isFlashOn = false;
  for (int flashIndex = 0; flashIndex < PATTERN_FLASHES; flashIndex++) {
    int flashStart = flashIndex * (FLASH_DURATION + SHORT_PAUSE);
    if (cycleFrame >= flashStart && cycleFrame < flashStart + FLASH_DURATION) {
      isFlashOn = true; // Turn on LEDs during the flash duration
      break;
    }
  }

  // Set LED colors based on whether the current frame is in an "on" or "off" state
  for (int segment = 0; segment < NUM_SEGMENTS; segment++) {
    for (int i = 0; i < SEGMENT_NUM_LEDS; i++) {
      if (isFlashOn) {
        segments[segment].setPixelColor(i, segments[segment].Color(0, 30, 255)); // Set LED to blue
      } else {
        segments[segment].setPixelColor(i, segments[segment].Color(0, 0, 0)); // Turn off LED
      }
    }
    segments[segment].show();
  }
}
void brake_light(int frame){
  int cycleLength = 18;                
  int cycleFrame = frame % cycleLength; 

  bool isOn = (cycleFrame < cycleLength/3); 

  for (int segment = 0; segment < NUM_SEGMENTS; segment++) {
      for (int i = 0; i < SEGMENT_NUM_LEDS; i++) {
        if(isOn){
          segments[segment].setPixelColor(i, segments[segment].Color(255,10,00));
        } else {
          segments[segment].setPixelColor(i, segments[segment].Color(0,0,0));
        }
      }
      segments[segment].show();
  }
}

void strobe(int frame){
  int cycleLength = 6;                
  int cycleFrame = frame % cycleLength; 

  bool isOn = (cycleFrame < cycleLength/2); 

  for (int segment = 0; segment < NUM_SEGMENTS; segment++) {
      for (int i = 0; i < SEGMENT_NUM_LEDS; i++) {
        if(isOn){
          segments[segment].setPixelColor(i, segments[segment].Color(255,255,255));
        } else {
          segments[segment].setPixelColor(i, segments[segment].Color(0,0,0));
        }
      }
      segments[segment].show();
  }
}

void blank(){
  for (int segment = 0; segment < NUM_SEGMENTS; segment++) {
      for (int i = 0; i < SEGMENT_NUM_LEDS; i++) {
        segments[segment].setPixelColor(i, segments[segment].Color(0,0,0));
      }
      segments[segment].show();
  }
}

void button_handler() {
  unsigned long currentTime = millis(); // Get the current time in milliseconds

  // Check the current state of the button
  bool currentButtonState = digitalRead(BUTTON) == LOW;

  // Debounce logic: ignore changes that happen too quickly
  if (currentButtonState != buttonPressed && (currentTime - lastDebounceTime) > SOFTWARE_DEBOUNCE_MS) {
    lastDebounceTime = currentTime;

    if (currentButtonState) {
      // Button is pressed
      buttonPressed = true;
      buttonPressTime = currentTime; // Record the time when the button was pressed
      longPressActive = false;       // Reset long press active state
    } else {
      // Button is released
      buttonPressed = false;
      longPressActive = false;       // Reset long press active state when released
      unsigned long pressDuration = currentTime - buttonPressTime;

      if (pressDuration < BUTTON_LONG_PRESS_THRESHHOLD) {
        button_handle_short_press(); // Handle short press
      } 
    }
  }

   // Handle continuous long-press while held
  if (buttonPressed && !longPressActive && (currentTime - buttonPressTime >= BUTTON_LONG_PRESS_THRESHHOLD)) {
    longPressActive = true;          // Set long press active
  }


}

void indicatorLeft(int frame){
  int cycleLength = 20;                
  int cycleFrame = frame % cycleLength; 

  bool isOn = (cycleFrame < cycleLength/2); 

  for (int i = 0; i < SEGMENT_NUM_LEDS; i++) {
    if(isOn){
      segments[0].setPixelColor(i, segments[0].Color(255,50,00));
      segments[1].setPixelColor(i, segments[1].Color(0,0,0));
    } else {
      segments[0].setPixelColor(i, segments[0].Color(0,0,0));
    }
  }
  segments[0].show();
  segments[1].show();
}

void indicatorRight(int frame){
 int cycleLength = 20;                
  int cycleFrame = frame % cycleLength; 

  bool isOn = (cycleFrame < cycleLength/2); 

  for (int i = 0; i < SEGMENT_NUM_LEDS; i++) {
    if(isOn){
      segments[1].setPixelColor(i, segments[0].Color(255,50,00));
      segments[0].setPixelColor(i, segments[1].Color(0,0,0));
    } else {
      segments[1].setPixelColor(i, segments[0].Color(0,0,0));
    }
  }
  segments[0].show();
  segments[1].show();
}

void button_handle_short_press() {
  mode++;
  mode %= NUM_MODES;
} 

void indicator_left_handler(){
  indicatorLeftActive = digitalRead(INDICATOR_LEFT) == LOW;
}
void indicator_right_handler(){
  indicatorRightActive = digitalRead(INDICATOR_RIGHT) == LOW;
}
void switch_handler(){
  switchActive = digitalRead(SWITCH) == LOW;
}
