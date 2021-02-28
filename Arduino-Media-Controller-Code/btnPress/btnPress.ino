#include "HID-Project.h"
#define VOLUP_PIN 5
#define VOLDWN_PIN 6
#define BTN_PIN 7
#define LED_PIN 4

boolean A,a,B,b;

int prevBtnVal = LOW;
int btnVal;
int aState;
int aLastState;
int t;
int buttonPressCount = -1;

unsigned long prevDebounceTime = 0;
unsigned long bounceDelay = 50;
unsigned long multiplePressDelay = 400;
unsigned long multiplePressTime = 0;

void setup() {
  Serial.begin(38400);
  Consumer.begin();
  BootKeyboard.begin();
  
  pinMode(VOLUP_PIN, INPUT_PULLUP);
  pinMode(VOLDWN_PIN, INPUT_PULLUP);
  pinMode(BTN_PIN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);

  aLastState = digitalRead(VOLUP_PIN);
  t = 0;
  a = false;
  b = false;
}

void loop() {
  //Code for handling knob turns.
  aState = digitalRead(VOLUP_PIN);

  //Debounce the input from the radial encoder.
  //Code adapted from: https://www.instructables.com/USB-Volume-Control-and-Caps-Lock-LED-Simple-Cheap-/
  if(millis() + 5 > t){
    A = digitalRead(VOLUP_PIN) == LOW;
    B = digitalRead(VOLDWN_PIN) == LOW;
    if (A && !a){
      if(B){
        Consumer.write(MEDIA_VOL_UP);
      }
      else{
        Consumer.write(MEDIA_VOL_DOWN);
      }
    }
    a = A;
  }

  //Code for handling button presses.
  int btnInput = digitalRead(BTN_PIN);
  if(btnInput != prevBtnVal){
    prevDebounceTime = millis();
  }

  if((millis() - prevDebounceTime) > bounceDelay){
    if(btnInput != btnVal){
      btnVal = btnInput;
      if(btnVal == HIGH){
        //Do the button action.
        multiplePressTime = millis() + multiplePressDelay;
        buttonPressCount++;
      }
    }
  }

  //Check to see if the button has been pressed at least once, and if the delay for completing a multiple press action is complete.
  if(buttonPressCount != 0 && (millis() > multiplePressTime)){
    //Do an action based on how many times the button has been pressed.
    switch(buttonPressCount){
      case 1:
        Consumer.write(MEDIA_PLAY_PAUSE);
        break;
      case 2:
        Consumer.write(MEDIA_NEXT);
        break;
      case 3:
        Consumer.write(MEDIA_PREVIOUS);
        break;
      case 4:
        Consumer.write(MEDIA_VOLUME_MUTE);
        break;
    }
    //Reset the count of the button presses.
    buttonPressCount = 0;
  }

  prevBtnVal = btnInput;
}
