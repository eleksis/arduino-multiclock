/*
  DIGITAL CLOCK
  
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

unsigned long prev, interval = 100;
DS3231  rtc(SDA, SCL);
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
dht DHT;
Time  t;

void setup()
{
    Wire.begin();
    rtc.begin();
    
    //Setup LCD
    lcd.begin(16, 2);
    lcd.clear();    
    //LCD brightness control
    pinMode(6, OUTPUT); //lcd backlight
    analogWrite(6, 0); //off
    
    //Turn off 'L' onboard LED
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
    
    //LED strip
    pinMode(3, OUTPUT);
    digitalWrite(3, LOW);
}

void loop()
{
    unsigned long now = millis();

    if (now - prev > interval) {
        //Read data
        t = rtc.getTime();
        int chk = DHT.read22(2);
        
        //LCD brightness
        int light = analogRead(0);
        int brightness = 0;
        if (light > 100){
          brightness = light/4;
        }
        analogWrite(6, brightness);
        
        //DISPLAY
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(t.hour);
        lcd.print(":");
        if(t.min<10){lcd.print("0");}
        lcd.print(t.min);
        lcd.print(":");
        if(t.sec<10){lcd.print("0");}
        lcd.print(t.sec);
        //temp
        lcd.setCursor(12,0);
        lcd.print(DHT.temperature, 0);
        lcd.print((char)223);
        lcd.print("C");
        //date
        lcd.setCursor(0,1);
        printDay(t.dow);
        lcd.print(' ');
        lcd.print(t.date);
        lcd.print('.');
        lcd.print(t.mon);
        //hum
        lcd.setCursor(13,1);
        lcd.print(DHT.humidity, 0);
        lcd.print('%');
        
        prev = now;
    }
}
void printDay(int day)
{
  switch(day)
  {
    case 1: lcd.print("Pon");break;
    case 2: lcd.print("Tor");break;
    case 3: lcd.print("Sre");break;
    case 4: lcd.print("Cet");break;
    case 5: lcd.print("Pet");break;
    case 6: lcd.print("Sob");break;
    case 7: lcd.print("Ned");break;
    default: lcd.print("Err");break;
  } 
}
