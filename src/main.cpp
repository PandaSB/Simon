#include <Arduino.h>

#include <TFT_eSPI.h> 
#include <SPI.h>
#include "WiFi.h"
#include <Wire.h>
#include <Button2.h>
#include "esp_adc_cal.h"

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

#define ADC_EN          14
#define ADC_PIN         34
#define BUTTON_1        35
#define BUTTON_2        0

#define LED_BLUE        2
#define LED_RED         26
#define LED_YELLOW      13
#define LED_GREEN       32
#define BUZZER          12

#define BUTTON_BLUE     17
#define BUTTON_YELLOW   15
#define BUTTON_RED      27
#define BUTTON_GREEN    33

#define DO              262
#define RE              294
#define MI              330
#define FA              349
#define SOL             392
#define LA              440
#define SI              494

typedef enum {
  eModeInit,
  eModeBasic,
  eMode1,
  eMode2,
  eModeExpert
}tMode ; 
tMode Mode ;

typedef enum {
  eKeyYellow ,
  eKeyRed,
  eKeyBlue,
  eKeyGreen,
  eKeyButton1,
  eKeyButton2
}tKey ;

boolean KeyPressed = false ;
tKey Key ; 

TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

void IRAM_ATTR isr_button1() {
  //KeyPressed = true ; 
  //Key = eKeyButton1 ; 
  //Serial.println("button Button 1 clicked");
}

void IRAM_ATTR isr_button2() {
  //KeyPressed = true ; 
  //Key = eKeyButton2 ; 
  //Serial.println("button Button 2 clicked");
}

void IRAM_ATTR isr_red() {
  KeyPressed = true ; 
  Key = eKeyRed ; 
  //Serial.println("button red clicked");
}

void IRAM_ATTR isr_green() {
  KeyPressed = true ; 
  Key = eKeyGreen ; 
  //Serial.println("button green clicked");
}

void IRAM_ATTR isr_blue() {
  KeyPressed = true ; 
  Key = eKeyBlue ; 
  //Serial.println("button blue clicked");
}

void IRAM_ATTR isr_yellow() {
  KeyPressed = true ; 
  Key = eKeyYellow ; 
  //Serial.println("button yellow clicked");
}

void done() {
	Serial.println("Done!");
}

void setup() {
  Serial.begin(115200);

  ledcSetup(0, 1000, 8);
  ledcAttachPin(BUZZER, 0);
  ledcWrite(0, 0);
  ledcWriteTone (0,0) ; 

  pinMode (LED_BLUE,OUTPUT); 
  pinMode (BUTTON_BLUE,INPUT_PULLUP); 
  digitalWrite (LED_BLUE,1) ; 
  attachInterrupt(BUTTON_BLUE, isr_blue, FALLING);

  pinMode (LED_YELLOW,OUTPUT); 
  pinMode (BUTTON_YELLOW,INPUT_PULLUP); 
  digitalWrite (LED_YELLOW,1) ; 
  attachInterrupt(BUTTON_YELLOW, isr_yellow, FALLING);

  pinMode (LED_RED,OUTPUT); 
  pinMode (BUTTON_RED,INPUT_PULLUP); 
  digitalWrite (LED_RED,1) ; 
  attachInterrupt(BUTTON_RED, isr_red, FALLING);

  pinMode (LED_GREEN,OUTPUT); 
  pinMode (BUTTON_GREEN,INPUT_PULLUP); 
  digitalWrite (LED_GREEN,1) ; 
  attachInterrupt(BUTTON_GREEN, isr_green, FALLING);

  pinMode (BUTTON_1,INPUT_PULLUP); 
  attachInterrupt(BUTTON_1, isr_button1, FALLING);

  pinMode (BUTTON_2,INPUT_PULLUP); 
  attachInterrupt(BUTTON_2, isr_button2, FALLING);


  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);

  if (TFT_BL > 0) { // TFT_BL has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
        pinMode(TFT_BL, OUTPUT); // Set backlight pin to output mode
        digitalWrite(TFT_BL, TFT_BACKLIGHT_ON); // Turn backlight on. TFT_BACKLIGHT_ON has been set in the TFT_eSPI library in the User Setup file TTGO_T_Display.h
  }

  tft.setSwapBytes(true);
  tft.setTextSize(5);
  tft.drawString("SIMON",  tft.width() / 2, tft.height() / 2 );

  ledcWriteTone (0,DO) ; 
  sleep (1) ; 
  ledcWriteTone (0,0) ;
  Serial.println("Start");
  Serial.print ("Running on core : ") ; 
  Serial.println(xPortGetCoreID());

  digitalWrite (LED_BLUE,0) ; 
  digitalWrite (LED_RED,0) ; 
  digitalWrite (LED_YELLOW,0) ; 
  digitalWrite (LED_GREEN,0) ; 
  sleep (1) ; 

}

void SetLed ( int led , boolean state)
{
  if (state)
  {
    digitalWrite (led,1) ;
    switch (led)
    {
      case LED_YELLOW :  ledcWriteTone (0,DO) ; break ; 
      case LED_RED    :  ledcWriteTone (0,RE) ; break ; 
      case LED_BLUE   :  ledcWriteTone (0,MI) ; break ; 
      case LED_GREEN  :  ledcWriteTone (0,FA) ; break ; 
    }   
  }
  else
  {
    ledcWriteTone (0,0) ;
    digitalWrite (led,0) ;    

  }
  usleep (250000) ; 
}


void ModeBasic(void)
{
  int Level = 0 ;
  String Sequence = "" ;
  boolean pass = true ; 
  String  StrLevel = "" ; 
  tft.fillScreen(TFT_BLACK) ; 
  tft.fillRoundRect (25 , 25, tft.width()  - 50 , tft.height() - 50 , 5 , TFT_YELLOW) ;
  tft.setTextColor (TFT_BLACK) ; 
  tft.setTextSize(4);
  tft.drawString("Basic",  tft.width() / 2, tft.height() /2);
  
  usleep (500000) ;

  do {
    Level ++ ; 
    StrLevel = "Level " + String (Level) ;
    Sequence += String (random(0, 4)) ; 
    
    tft.fillScreen(TFT_BLACK) ; 
    tft.setTextColor (TFT_WHITE) ; 
    tft.setTextSize(3);
    tft.drawString("Listen",  tft.width() / 2, tft.height() * 1 /3);
    tft.drawString(StrLevel,  tft.width() / 2, tft.height() * 2 /3);

    Serial.print ("(") ;
    Serial.print (Sequence.length()) ;
    Serial.print (")") ; 
    Serial.println (Sequence) ; 

    
    for ( int i = 0 ;  i < Sequence.length() ; i ++)
    {
      char dis =  Sequence.charAt (i) - '0' ; 
      switch (dis) {
        case 0 : 
          SetLed (LED_YELLOW, true) ; 
          SetLed (LED_YELLOW, false) ;
          break ;    
        case 1 : 
          SetLed (LED_RED, true) ; 
          SetLed (LED_RED, false) ;
          break ;    
        case 2 : 
          SetLed (LED_BLUE, true) ; 
          SetLed (LED_BLUE, false) ;
          break ;    
        case 3 :
          SetLed (LED_GREEN, true) ; 
          SetLed (LED_GREEN, false) ;
          break ;    
      }
    }    

    tft.fillScreen(TFT_BLACK) ; 
    tft.setTextColor (TFT_WHITE) ; 
    tft.setTextSize(3);
    tft.drawString("Play",  tft.width() / 2, tft.height() * 1 /3);
    tft.drawString(StrLevel,  tft.width() / 2, tft.height() * 2 /3);

    for ( int i = 0 ;  (i < Sequence.length() && (pass)); i ++)
    {
      char received = -1; 
      char dis =  Sequence.charAt (i) - '0' ;  
      while (!KeyPressed ) {usleep (1000) ; }
      received = -1 ; 
      switch (Key) 
      {
        case eKeyYellow : received = 0 ; break ; 
        case eKeyRed    : received = 1 ; break ;
        case eKeyBlue   : received = 2 ; break ; 
        case eKeyGreen  : received = 3 ; break ;
        default : received = -1 ;  
      }
      Serial.printf ("%d : %d \r\n", dis , received) ; 
      switch (received) 
      {
        case 0 : 
          SetLed (LED_YELLOW, true) ; 
          SetLed (LED_YELLOW, false) ;
          break ;    
        case 1 : 
          SetLed (LED_RED, true) ; 
          SetLed (LED_RED, false) ;
          break ;    
        case 2 : 
          SetLed (LED_BLUE, true) ; 
          SetLed (LED_BLUE, false) ;
          break ;    
        case 3 :
          SetLed (LED_GREEN, true) ; 
          SetLed (LED_GREEN, false) ;
          break ;    
      }
      KeyPressed = false ;
      if ( dis != received) {
        pass = false ; 
        tft.fillScreen(TFT_BLACK) ; 
        tft.fillRoundRect (25 , 25, tft.width()  - 50 , tft.height() - 50 , 5 , TFT_RED );
        tft.setTextColor (TFT_WHITE) ; 
        tft.setTextSize(5);
        tft.drawString("LOST",  tft.width() / 2, tft.height() /2);
        ledcWriteTone (0,SI) ;
        usleep (1500000) ; 
        ledcWriteTone (0,0) ;
      } 
    }
  } while (pass) ; 

  Mode = eModeInit ; 
}

void Mode1 (void)
{
  Mode = eModeInit ; 
}

void Mode2 (void)
{
  Mode = eModeInit ; 
}

void ModeExpert (void)
{
  Mode = eModeInit ; 
}


void ModeSelect(void)
{
  for ( int i = 0 ; ((i < 4) && !KeyPressed) ; i++)
  {
    tft.fillScreen(TFT_BLACK) ; 
    tft.setTextSize(3);
    tft.setTextColor (TFT_WHITE) ; 
    tft.drawString("Select Mode",  tft.width() / 2, tft.height() / 3 );
    switch (i) {
      case 0 : 
        tft.fillRoundRect (25 , (tft.height()  * 2 / 3) - 10 , tft.width() - 50 , 40 , 5 , TFT_YELLOW) ;
        tft.setTextColor (TFT_BLACK) ; 
        tft.setTextSize(3);
        tft.drawString("Basic",  tft.width() / 2, (tft.height() *2 / 3) +10  );
      break ;   
      case 1 : 
        tft.fillRoundRect (25 , (tft.height()  * 2 / 3) - 10 , tft.width() - 50 , 40 , 5 , TFT_RED) ;
        tft.setTextColor (TFT_BLACK) ; 
        tft.setTextSize(3);
        tft.drawString("Mode 1",  tft.width() / 2, (tft.height() *2 / 3) +10  );
      break ; 
      case 2 : 
        tft.fillRoundRect (25 , (tft.height()  * 2 / 3) - 10 , tft.width() - 50 , 40 , 5 , TFT_BLUE) ;
        tft.setTextColor (TFT_BLACK) ; 
        tft.setTextSize(3);
        tft.drawString("Mode 2",  tft.width() / 2, (tft.height() *2 / 3) +10  );
      break ; 
      case 3 : 
        tft.fillRoundRect (25 , (tft.height()  * 2 / 3) - 10 , tft.width() - 50 , 40 , 5 , TFT_GREEN) ;
        tft.setTextColor (TFT_BLACK) ; 
        tft.setTextSize(3);
        tft.drawString("Expert",  tft.width() / 2, (tft.height() *2 / 3) +10  );
      break ; 
    }
      for (int j = 0 ; ((j < (i+1)) && !KeyPressed) ; j++)
      {
        int led ;
        switch (i) {
          case 0 : led = LED_YELLOW ; break;
          case 1 : led = LED_RED ; break ;
          case 2 : led = LED_BLUE ; break ;
          case 3 : led = LED_GREEN ; break ;
        }
        digitalWrite (led,1) ;
        usleep (500000); 
        digitalWrite (led,0) ;
        usleep (500000); 
      }
  }
  if (KeyPressed) {
    int led ; 
    switch (Key) {
          case eKeyBlue   : led = LED_BLUE ; break;
          case eKeyGreen  : led = LED_GREEN ; break ;
          case eKeyYellow : led = LED_YELLOW ; break ;
          case eKeyRed    : led = LED_RED ; break ;
    }
    SetLed (led, true ) ; 
    SetLed (led, false ) ; 
    SetLed (led, true ) ; 
    SetLed (led, false ) ; 
    
    KeyPressed = false ; 

    switch (Key) {
      case eKeyYellow : Mode = eModeBasic ; break ; 
      case eKeyRed    : Mode = eMode1 ; break ; 
      case eKeyBlue   : Mode = eMode2 ; break ; 
      case eKeyGreen  : Mode = eModeExpert ; break ; 
      default : eModeInit ; 

    }
  }
}

void ManageMode(void)
{
  switch (Mode) {
    case eModeInit   : ModeSelect(); break ; 
    case eModeBasic  : ModeBasic(); break ; 
    case eMode1      : Mode1() ; break ;
    case eMode2      : Mode2() ; break;
    case eModeExpert : ModeExpert(); break ; 
    default : Mode = eModeInit ; break ; 
  } 
}

void loop() {

  ManageMode ();
}