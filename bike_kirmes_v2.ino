#include <Adafruit_NeoPixel.h>
#define PIN 5
#define NUM 141
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM, PIN, NEO_GRB + NEO_KHZ800);

// Segment Lengths
int segment_1 = 70;
int segment_2 = 71;


// indicator smooth segment lenght
int i_length = 20;

// indicator smooth frame tim / speed
int i_delay = 12;

// indicator smooth segment position 
int i_pos = 0;

// rainbow frame time / speed
int r_delay = 40;

// color fade frame time / speed
int cf_delay = 10;

// indicator hard frame time / speed
int indicator_hard_delay = 400;

// indicator color 
int indicator_color[3] {150,35,0};

// Blaulicht
int police_delay_1 = 80;
int police_delay_2 = 200;
int police_blink = 2;
// x_x___x_x___

int police_color[3] {20, 40, 150};

// Strobo
int strobo_color[3] {255,255, 255};
int strobo_delay = 20;

// Strobo 2
int strobo2_on = 30;
int strobo2_off = 120;


int mode = 0;
int prevmode = 0;
bool terminate = false;
bool indicator = false;


// FARBEN RGB [0-255]
int red[] = {255,20,0};
int strobo_c[] = {150,150,150};
int blinker_c[] = {255,60,0};
int blau[] = {20,40,200};

//MISC CONFIG
int button_debounce = 200;



typedef void (*FunctionPtr)();

FunctionPtr modes[6];
FunctionPtr blinkerMap[2];

int size = 6; //Anzahl Effekte (Anzahl Elemente in modes[])

void loadFunctions(){
  modes[0] = []() { rainbow_wave(255, 20); };
  modes[1] = []() { rainbow_wave(30, 20); };
  modes[2] = []() { color_fade(255, 50); };
  modes[3] = []() { police(blau, 3, 60, 150); };
  modes[4] = []() { strobo(strobo_c, 50, 30); };
  modes[5] = []() { blinker(blinker_c, 400, 400, 0, NUM); };
  

  blinkerMap[1] = []() { blinker(blinker_c, 400, 400, 0, segment_1); };
  blinkerMap[0] = []() { blinker(blinker_c, 400, 400, segment_1 + 1, NUM); };
}

void setup() {
  pinMode(7, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(7), button, FALLING); 
  attachInterrupt(digitalPinToInterrupt(1), clear_leds, FALLING);
  attachInterrupt(digitalPinToInterrupt(0), indicator_left, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), indicator_right, FALLING);
//  attachInterrupt(digitalPinToInterrupt(0), play, RISING); //TODO: Debounce
//  attachInterrupt(digitalPinToInterrupt(3), play, RISING);
  pixels.begin();
  Serial.begin(19200);
  loadFunctions();
}


void loop() {
  if(digitalRead(0) == HIGH && digitalRead(3) == HIGH) {
    if(indicator == true) {
      mode = prevmode;
      indicator = false;
    }
  }
  play();
  terminate = false; 
}

void play(){
  if(digitalRead(1) == LOW){
    modes[mode]();
  } else {
    terminate = true;
    clear_leds();
  }
}

void button(){
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();    
  if (interrupt_time - last_interrupt_time > button_debounce) {
    terminate = true;
    if(digitalRead(0) == HIGH && digitalRead(3) == HIGH && digitalRead(1) == LOW) {
      mode++;
      mode %= size;
      Serial.println("Mode: " + String(mode));
      Serial.println("Size: " + String(size));
      prevmode = mode;
    }
  }
  last_interrupt_time = interrupt_time;
}



void rainbow_wave(int brightness, int delay_){
  for (int hue = 0; hue <= 65535; hue++){
    if(terminate){break;}
    for (int i = 0; i < NUM; i++){
      pixels.setPixelColor(i, pixels.ColorHSV(((i - hue) * 1900), 255, brightness));
    }
    pixels.show();
    delay(delay_);
  }
}

void color_fade(int brightness, int delay_){
  for (int i = 0; i < 255; i++){
    if(terminate){break;}
    for (int j = 0; j < NUM; j++){
      pixels.setPixelColor(j, pixels.ColorHSV(i*(65535/255), 255, brightness));
    }
    pixels.show();
    delay(delay_);
  }
}

void police(int color[], int blinks, int delay1, int delay2){
  for( int j = 0; j < blinks; j++){
    for (int i = 0; i < NUM; i++){
      pixels.setPixelColor(i, pixels.Color(color[0],color[1],color[2]));
    }
    pixels.show();
    delay(delay1);
    for (int i = 0; i < NUM; i++){
      pixels.setPixelColor(i, pixels.Color(0,0,0));
    }
    pixels.show();
    delay(delay1);
  }
  delay(delay2);
}

void strobo(int color[], int on_duration, int off_duration){
  for (int i = 0; i < NUM; i++){
    pixels.setPixelColor(i, pixels.Color(color[0],color[1],color[2]));
  }
  pixels.show();
  delay(on_duration);
  for (int i = 0; i < NUM; i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();
  delay(off_duration);
}

void blinker(int color[], int on_duration, int off_duration, int start_led, int end_led){
  for(int i = start_led; i < end_led; i++){
    pixels.setPixelColor(i, pixels.Color(color[0],color[1],color[2]));
  }
  pixels.show();
  delay(on_duration);
  for (int i = start_led; i < end_led; i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();
  delay(off_duration);
}

void single_color(int color[]){
  for (int i = 0; i < NUM; i++){
    pixels.setPixelColor(i, pixels.Color(color[0],color[1],color[2]));
  }
  pixels.show();
}

void indicator_right(){
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();    
  if (interrupt_time - last_interrupt_time > 400) {
    indicator = true;
    clear_leds();
    while(digitalRead(3) == LOW){
      blinkerMap[1]();
    } 
  }
}

void indicator_left(){
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();    
  if (interrupt_time - last_interrupt_time > 400) {
    indicator = true;
    clear_leds();
    while(digitalRead(0) == LOW){
      blinkerMap[0]();
    } 
  }
}

void clear_leds(){
  terminate = true;
  for (int i = 0; i < NUM; i++){
      pixels.setPixelColor(i, pixels.Color(0,0,0));
    }
  pixels.show();
}

void restore(){
  mode = prevmode;
  } 

