/*
  DIGITAL CLOCK
  by Vito Tomažin

  DS3231 wiring:
  *GND->GND
  *VCC->5V
  *SDA->A4
  *SCL->A5
  
  16x2 LCD wiring:
  *VSS->GND
  *VDD->5V
  *V0->Potenciometer output (contrast)
  *RS->7
  *RW->GND
  *E->8
  *D0-D3->not connected
  *D4->9
  *D5->10
  *D6->11
  *D7->12
  *A->5V (220ohm resistor)
  *K->GND
  
  DHT22 wiring:
  *Pin1->5V
  *Pin2->2
  *Pin4->GND
 */
#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <dht.h>

// Main loop interval (miliseconds)
unsigned long prev, interval = 100;

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// Init LCD
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Init DHT
dht DHT;

// Init a Time-data structure
Time  t;

const int dhtPin = 2;
const int button1Pin = 3;
const int button2Pin = 4;
const int button3Pin = 5;
const int lcdBacklightPin = 6;

int button1State = 0;
int button2State = 0;
int button3State = 0;
int lastButton1State = 0;
int lastButton2State = 0;
int lastButton3State = 0;
int buttonMode = 0; //hour, min, sec, dow, day, month, year

void setup(){
    // Start I2C
    Wire.begin();
    
    // Initialize the rtc object
    rtc.begin();
    
    // The following lines can be uncommented to set the date and time
    //rtc.setDOW(1);     // Set Day-of-Week 1-7
    //rtc.setTime(20, 00, 10);     // Set the time h-m-s
    //rtc.setDate(10, 11, 2017);   // Set the date d-m-y
    
    // Setup LCD
    lcd.begin(16, 2);
    lcd.clear();
   
    // LCD brightness control
    pinMode(lcdBacklightPin, OUTPUT);
    analogWrite(lcdBacklightPin, LOW); //off
    
    // Turn off 'L' onboard LED
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    
    // Buttons setup
    pinMode(button1Pin, INPUT);
    pinMode(button2Pin, INPUT);
    pinMode(button3Pin, INPUT);
}

void loop(){
    unsigned long now = millis();

    if (now - prev > interval) {

        // Get data from RTC3231 & DHT22
        t = rtc.getTime();
        int chk = DHT.read22(dhtPin);
        
        // LCD brightness
        int light = analogRead(0);
        int brightness = 0;
        if (light > 100){brightness = light/4;}
        analogWrite(lcdBacklightPin, brightness);
         
        // DISPLAY
        lcd.clear();
        // time
        lcd.setCursor(0,0);
        lcd.print(String(t.hour) + ":");
        if(t.min<10){lcd.print("0");}
        lcd.print(String(t.min) + ":");
        if(t.sec<10){lcd.print("0");}
        lcd.print(t.sec);
        // temp
        lcd.setCursor(13,0);
        lcd.print(DHT.temperature, 0); lcd.print((char)223);
        // date
        lcd.setCursor(0,1);
        lcd.print(getDay(t.dow) + " " + String(t.date) + "." + String(t.mon));
        // humidity
        lcd.setCursor(13,1);
        lcd.print(DHT.humidity, 0); lcd.print("%");
        
        // BUTTONS
        // read state
        button1State = digitalRead(button1Pin);
        button2State = digitalRead(button2Pin);
        button3State = digitalRead(button3Pin);

        // button1 changed from low to high -> change buttonMode
        if ((button1State != lastButton1State) && button1State == HIGH) {
          buttonMode = (buttonMode + 1) % 7;
        }     
        
        // button2 changed from low to high -> increase
        if ((button2State != lastButton2State) && button2State == HIGH) {
          if (buttonMode == 0) {rtc.setTime((t.hour + 1), t.min, t.sec);}
          else if (buttonMode == 1) {rtc.setTime(t.hour, (t.min + 1), t.sec);}
          else if (buttonMode == 2) {rtc.setTime(t.hour, t.min, 0);}
          else if (buttonMode == 3) {rtc.setDOW((t.dow + 1) % 8);}
          else if (buttonMode == 4) {rtc.setDate((t.date + 1), t.mon, t.year);}
          else if (buttonMode == 5) {rtc.setDate(t.date, (t.mon + 1), t.year);}
          else if (buttonMode == 6) {rtc.setDate(t.date, t.mon, (t.year + 1));}
        }     
        
        // button3 changed from low to high -> decrease
        if ((button3State != lastButton3State) && button3State == HIGH) {
          if (buttonMode == 0) {rtc.setTime((t.hour - 1), t.min, t.sec);}
          else if (buttonMode == 1) {rtc.setTime(t.hour, (t.min - 1), t.sec);}
          else if (buttonMode == 2) {rtc.setTime(t.hour, t.min, 0);}
          else if (buttonMode == 3) {rtc.setDOW((t.dow - 1) % 8);}
          else if (buttonMode == 4) {rtc.setDate((t.date - 1), t.mon, t.year);}
          else if (buttonMode == 5) {rtc.setDate(t.date, (t.mon - 1), t.year);}
          else if (buttonMode == 6) {rtc.setDate(t.date, t.mon, (t.year - 1));}
        }
        // save the current state as the last state, for next time through the loop
        lastButton1State = button1State;
        lastButton2State = button2State;
        lastButton3State = button3State;

        // save time of last loop execution       
        prev = now;
    }
}
String getDay(int day){
  switch(day){
    case 1: return "Pon";
    case 2: return "Tor";
    case 3: return "Sre";
    case 4: return "Cet";
    case 5: return "Pet";
    case 6: return "Sob";
    case 7: return "Ned";
    default: return "Err";
  } 
}
