#include <Adafruit_NeoPixel.h>

#define PIN 5
#define NUM 141
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM, PIN, NEO_GRB + NEO_KHZ800);

// Segmente
int segment_1 = 70;
int segment_2 = 71;

int mode = 0; //initial Mode
int prevmode = 0;
bool terminate = false;
bool indicator = false;
unsigned long button_pressed_time = 0;
unsigned long button_released_time = 0;
unsigned long lastPressed = 0;
bool button_pressed_flipflop = false;

// FARBEN RGB [0-255]
// int name[] = {0-255, 0-255, 0-255}
int rot[] = {200,20,0};
int strobo_c[] = {200,200,200};
int blinker_c[] = {255,60,0};
int blau[] = {20,40,200};

//MISC CONFIG
int button_debounce = 50;
int hold_thresh = 350;



/*
Verfügbare Modi:

rainbow_wave(helligkeit[0-255], frametime) //ToDo test 100 vs 255
color_fade(helligkeit[0-255], frametime)
single_color(Farb-Array)
police(Farb-Array, AnzalBlitze, Delay1, Delay2)
strobo(Farb-Array, An-Dauer, Aus-Dauer)
blinker(Farb-Array, An-Dauer, Aus-Dauer, Start-Position, End-Position)

*/
const int size = 7; //Anzahl Effekte, entsprechend anpassen
typedef void (*FunctionPtr)();

FunctionPtr modes[size];
FunctionPtr blinkerMap[2];

void loadFunctions(){
  modes[0] = []() { rainbow_wave(200, 20); };
  modes[1] = []() { rainbow_wave(30, 20); };
  modes[2] = []() { single_color(rot); };
  modes[3] = []() { color_fade(100, 50); };
  modes[4] = []() { police(blau, 3, 60, 150); };
  modes[5] = []() { strobo(strobo_c, 50, 30); };
  modes[6] = []() { blinker(blinker_c, 400, 400, 0, NUM); };
  

  blinkerMap[1] = []() { blinker(blinker_c, 400, 400, 0, segment_1); };
  blinkerMap[0] = []() { blinker(blinker_c, 400, 400, segment_1 + 1, NUM); };
}

void setup() {
  pinMode(7, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(7), button_change, CHANGE);
  attachInterrupt(digitalPinToInterrupt(1), clear_leds, FALLING);
  attachInterrupt(digitalPinToInterrupt(0), indicator_left, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), indicator_right, FALLING);
//  attachInterrupt(digitalPinToInterrupt(0), play, RISING); //TODO: Debounce
//  attachInterrupt(digitalPinToInterrupt(3), play, RISING);
  pixels.begin();
  loadFunctions();
  Serial.begin(9600);
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
    pixels.show();
  } else {
    terminate = true;
    clear_leds();
  }
}



void interrupt_check(){
  if(button_pressed_time > button_released_time){
    Serial.println("Strobo");
  }
  while(button_pressed_time > button_released_time){
    //Serial.println(millis() - button_pressed_time);
    if(millis() - button_pressed_time > hold_thresh){
      modes[5]();
    }
    
  } 
}

void button_change(){
  bool valid = millis() - lastPressed > button_debounce;

  if(digitalRead(7)==LOW && valid && !button_pressed_flipflop && millis() - lastPressed > 0){
    button_pressed_flipflop = true;
    button_pressed_time = millis();
    lastPressed = millis();
  }

  if(digitalRead(7)==HIGH && valid && button_pressed_flipflop && millis() - lastPressed > 0){
    button_pressed_flipflop = false;
    button_released_time = millis();
    lastPressed = millis();
  }

  if(button_pressed_time < button_released_time && millis() - button_pressed_time < hold_thresh){
    terminate = true;
    if(digitalRead(0) == HIGH && digitalRead(3) == HIGH && digitalRead(1) == LOW) {
      Serial.println("Mode");
      mode++;
      mode %= size;
      prevmode = mode;
    }
  }
  
}


void rainbow_wave(int brightness, int delay_){
  for (int hue = 0; hue <= 65535; hue++){
    if(terminate){break;}
    interrupt_check();
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
    interrupt_check();
    for (int j = 0; j < NUM; j++){
      pixels.setPixelColor(j, pixels.ColorHSV(i*(65535/255), 255, brightness));
    }
    pixels.show();
    delay(delay_);
  }
}

void police(int color[], int blinks, int delay1, int delay2){
  for( int j = 0; j < blinks; j++){
    interrupt_check();
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
    interrupt_check();
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
  interrupt_check();
  for (int i = 0; i < NUM; i++){
    pixels.setPixelColor(i, pixels.Color(color[0],color[1],color[2]));
  }
  pixels.show();
  delay(50);
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
}
