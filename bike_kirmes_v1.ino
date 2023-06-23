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

void setup() {
  pinMode(7, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(7), button, FALLING); 
  attachInterrupt(digitalPinToInterrupt(1), clear_leds, FALLING);
  attachInterrupt(digitalPinToInterrupt(0), indicator_left, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), indicator_right, FALLING);
  pixels.begin();
  Serial.begin(19200);
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

void button(){
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();    
  if (interrupt_time - last_interrupt_time > 200) {
    terminate = true;
    if(digitalRead(0) == HIGH && digitalRead(3) == HIGH && digitalRead(1) == LOW) {
      mode++;
      if(mode>9){
        mode = 0;
      }
      prevmode = mode;
    }
  }
  last_interrupt_time = interrupt_time;
}

void play(){
  if(digitalRead(1) == LOW){
    switch(mode){
      case 0:
        prevmode = mode;
        rainbow_wave(50);
        break;
      case 1:
        prevmode = mode;
        rainbow_wave(100);
        break; 
      case 2:
        prevmode = mode;
        rainbow_wave(150);
        break;
      case 3:
        prevmode = mode;
        color_fade();
        break;
      case 4:
        prevmode = mode;
        single_color(50, 0, 0);
        break;
      case 5:
        prevmode = mode;
        single_color(100,100,100);
        break;
      case 6:
        prevmode = mode;
        police();
        break;
      case 7:
        prevmode = mode;
        strobo();
        break;
      case 8:
        prevmode = mode;
        strobo2();
        break;
      case 9:
        prevmode = mode;
        indicator_hard(0,NUM);
        break;
      case 10: 
        indicator_hard(0,segment_1);
        break;
      case 11:
        indicator_hard(segment_1 + 1,NUM);
        break;
    }
  } else {
    terminate = true;
    clear_leds();
  }
}

void clear_leds(){
  terminate = true;
  for (int i = 0; i < NUM; i++){
      pixels.setPixelColor(i, pixels.Color(0,0,0));
    }
  pixels.show();
}

void indicator_right(){
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();    
  if (interrupt_time - last_interrupt_time > 400) {
    indicator = true;
    clear_leds();
    mode = 10;
  }
}

void indicator_left(){
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();    
  if (interrupt_time - last_interrupt_time > 400) {
    indicator = true;
    clear_leds();
    mode = 11;
  }
}

void restore_mode(){
  mode = prevmode;
}

void strobo(){
  for (int i = 0; i < NUM; i++){
    pixels.setPixelColor(i, pixels.Color(strobo_color[0],strobo_color[1],strobo_color[2]));
  }
  pixels.show();
  delay(strobo_delay);
  for (int i = 0; i < NUM; i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();
  delay(strobo_delay);
}

void strobo2(){
  for (int i = 0; i < NUM; i++){
    pixels.setPixelColor(i, pixels.Color(strobo_color[0],strobo_color[1],strobo_color[2]));
  }
  pixels.show();
  delay(strobo2_on);
  for (int i = 0; i < NUM; i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();
  delay(strobo2_off);
  for (int i = 0; i < NUM; i++){
    pixels.setPixelColor(i, pixels.Color(strobo_color[0],strobo_color[1],strobo_color[2]));
  }
  pixels.show();
  delay(strobo2_on);
  for (int i = 0; i < NUM; i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  pixels.show();
  delay(strobo2_off);
}

void police(){
  for( int j = 0; j < police_blink; j++){
    for (int i = 0; i < NUM; i++){
      pixels.setPixelColor(i, pixels.Color(police_color[0],police_color[1],police_color[2]));
    }
    pixels.show();
    delay(police_delay_1);
    for (int i = 0; i < NUM; i++){
      pixels.setPixelColor(i, pixels.Color(0,0,0));
    }
    pixels.show();
    delay(police_delay_1);
  }
  delay(police_delay_2);
}

void indicator_hard(int start_led, int end_led){
  for(int i = start_led; i < end_led; i++){
    pixels.setPixelColor(i, pixels.Color(indicator_color[0],indicator_color[1],indicator_color[2]));
  }
  delay(indicator_hard_delay);
  pixels.show();
  for (int i = start_led; i < end_led; i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0));
  }
  delay(indicator_hard_delay);
  pixels.show();
}

void single_color(int r, int g, int b){
  for (int i = 0; i < NUM; i++){
    pixels.setPixelColor(i, pixels.Color(r,g,b));
  }
  pixels.show();
  
}

void color_fade(){
  for (int i = 0; i < 255; i++){
    if(terminate){break;}
    for (int j = 0; j < NUM; j++){
      pixels.setPixelColor(j, pixels.ColorHSV(i*(65535/255), 255, 100));
    }
    pixels.show();
    delay(cf_delay);
  }
}

void rainbow_wave(int brightness){
  for (int hue = 0; hue <= 65535; hue++){
    if(terminate){break;}
    for (int i = 0; i < NUM; i++){
      pixels.setPixelColor(i, pixels.ColorHSV(((i - hue) * 1900), 255, brightness));
    }
    pixels.show();
    delay(r_delay);

  }
}

void indicator_smooth(){
  for (int i = 0; i < NUM + i_length; i++){
    if( i <= i_pos && i >= i_pos - i_length ){
      pixels.setPixelColor(i, pixels.Color(indicator_color[0],indicator_color[1],indicator_color[2]));
    }  else {
      pixels.setPixelColor(i, pixels.Color(0,0,0));
    }
  }
  pixels.show();
  i_pos++;
  delay(i_delay);
  if(i_pos > NUM + i_length){
    i_pos = 0;
  }
}
