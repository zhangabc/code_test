/**
 * @file TEST_PSA.ino
 * @author  huang xianming (email:<xianming.huang@itead.cc>)
 * @date    2015/10/8
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 8
#define ENABLE_2_PWM_CHANNEL_TEST
SSD1306 display(OLED_RESET);
#define PSASerial Serial1

#if 0
#define PIN_1  26
#define PIN_2  25
#define PIN_3  28
#define PIN_4  27
#define PIN_5  23
#endif 

#define ESP8266_GPIO4     41
#define ESP8266_GPIO5     40
#define ESP8266_GPIO12    24
#define ESP8266_GPIO14    25
#define ESP8266_GPIO15    35
#define ESP8266_FW        36

String buffer; 
char serial[10] = "--"; 
char pin[10] = "--"; 
char wifi[10] = "- --"; 

int test_log = 1;

int serial_ok = 0;
int PIN_LOW_ok = 0;
int PIN_HIGH_ok = 0;
int wifi_ok = 0;
int wifi_satisfy = 0;
int PIN_LOW_LOG = 0;
int PIN_HIGH_LOG = 0;


void serialTest(void)
{
    test_log = 0;
    unsigned long nowtime = millis();
    unsigned long settime = nowtime + 200;
    PSASerial.print("AT+START\r\n");
    while ((nowtime = millis()) < settime && serial_ok == 0)
    {
        buffer = "\0";
        PSASerial.setTimeout(200);
        buffer= PSASerial.readStringUntil('\n');
        buffer += '\n';
        //PSASerial.readBytesUntil('\r', buffer, 10);  
        if (0 == strcmp(buffer.c_str(),"OK\r\n"))
        {
            strcpy(serial,"OK");
            screenFlush(serial, pin, wifi);
            serial_ok = 1;
        }
    }
    if (serial_ok == 0)
    {
        strcpy(serial,"Fail");
        screenFlush(serial, pin, wifi);    
    }
    while(PSASerial.available() > 0)
    {
       PSASerial.read();
    }
}

void gpioTest(void)
{
    int io1=-1, io2=-1, io3=-1, io4=-1, io5=-1;
  
    unsigned long nowtime = millis();
    unsigned long settime = nowtime + 200;
    PSASerial.print("AT+GPIO_LOW\r\n"); 
    while ((nowtime = millis()) < settime && PIN_LOW_ok == 0)
    {
        buffer="\0";
        //PSASerial.setTimeout(100);
        buffer= PSASerial.readStringUntil('\n');
        buffer += '\n';
        //PSASerial.readBytesUntil('\r', buffer, 10);
        if (0 == strcmp(buffer.c_str(),"OK\r\n"))
        {
            PIN_LOW_ok = 1;
            io1 = digitalRead(ESP8266_GPIO4);
            io2 = digitalRead(ESP8266_GPIO5);
            io3 = digitalRead(ESP8266_GPIO12);
            io4 = digitalRead(ESP8266_GPIO14);
            io5 = digitalRead(ESP8266_GPIO15);
      #ifdef ENABLE_2_PWM_CHANNEL_TEST
          if (io3 == LOW && io4 == LOW)
        {
          PIN_LOW_LOG++;    
        }
      #else 
        if (io1 == LOW && io2 == LOW && io3 == LOW && io4 == LOW && io5 == LOW)
        {
          PIN_LOW_LOG++;    
        }
        #endif      
        }
    }
    while(PSASerial.available() > 0)
    {
        PSASerial.read();
    }

    nowtime = millis();
    settime = nowtime + 200;
    PSASerial.print("AT+GPIO_HIGH\r\n"); 
    while ((nowtime = millis()) < settime && PIN_HIGH_ok == 0)
    {
        
        buffer="\0";
        PSASerial.setTimeout(200);
        buffer= PSASerial.readStringUntil('\n'); 
        //Serial.print(buffer.c_str());
         buffer += '\n';
        if (0 == strcmp(buffer.c_str(),"OK\r\n"))
        {
            PIN_HIGH_ok = 1;
            io1 = digitalRead(ESP8266_GPIO4);
            io2 = digitalRead(ESP8266_GPIO5);
            io3 = digitalRead(ESP8266_GPIO12);
            io4 = digitalRead(ESP8266_GPIO14);
            io5 = digitalRead(ESP8266_GPIO15);
      #ifdef ENABLE_2_PWM_CHANNEL_TEST
        if (io3 == HIGH && io4 == HIGH)
        {
          PIN_HIGH_LOG++;    
        }
      #else 
        if (io1 == HIGH && io2 == HIGH && io3 == HIGH && io4 == HIGH && io5 == HIGH)
        {
          PIN_HIGH_LOG++;    
        }
        #endif  
        }
    }
    while(PSASerial.available() > 0)
    {
        PSASerial.read();
    }
    
    if (PIN_LOW_LOG == 1 && PIN_HIGH_LOG == 1)
    {
        strcpy(pin,"OK");
        screenFlush(serial, pin, wifi);
    }
    else
    {
        strcpy(pin,"Fail");
        screenFlush(serial, pin, wifi);
    }
    
}

void wifiTest(void)
{
    int i, wifi_data = 0, eq,aq;
    char wifi_value[10] = {0};
    char wifi_buffer[10] = {0};
    unsigned long nowtime = millis();
    unsigned long settime = nowtime + 20000;
    String temp;
    PSASerial.print("AT+WIFI\r\n");  
    while ((nowtime = millis()) < settime && wifi_ok == 0)
    {
        buffer="\0";
        buffer= PSASerial.readStringUntil('\n');
        buffer += '\n';
        //PSASerial.readBytesUntil('=', buffer, 10); 
        temp = buffer.substring(0, 9);
        if (0 == strcmp(temp.c_str(),"AT+SIGNAL"))
        {
            wifi_ok = 1;
            eq = buffer.indexOf("=");
            aq = buffer.indexOf("\r");
            temp = buffer.substring(eq+1,aq);
            strcpy(wifi_value,temp.c_str());
            if (wifi_value[0] == '-')
            {
                for (i = 1; wifi_value[i]!=0; i++)
                {
                    wifi_data = wifi_data *10 + wifi_value[i];
                }
                wifi_data *= -1;
            }
            else
            {
                for (i = 0; wifi_value[i]!=0; i++)
                {
                    wifi_data = wifi_data *10 + wifi_value[i];
                } 
            } 
            memset(wifi_buffer, '\0', sizeof(wifi));
            if(wifi_data < 0 && wifi_data > -50)
            {
                wifi_satisfy = 1;
                strcpy(wifi,"v ");
            }
            else
            {
                 strcpy(wifi,"X ");    
            }
            memset(wifi_buffer, '\0', sizeof(wifi));
            itoa(wifi_data, wifi_buffer, 10);
            strcat(wifi,wifi_buffer);
            screenFlush(serial, pin, wifi);    
        }
    }
    if (wifi_ok == 0)
    {
        strcpy(wifi,"X 0");           
         screenFlush(serial, pin, wifi);
    }
}

void screenFlush(char serial[], char pin[], char wifi[])
{
    display.clearDisplay(); 
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0); 
    display.print("Ser:"); 
    display.print(serial);
    display.setCursor(0,20); 
    display.print("Pin:"); 
    display.print(pin); 
    display.setCursor(0,40);
    display.print("Wifi:"); 
    display.print(wifi); 
    display.display();  
    display.stopscroll();
}


void setup(void)
{
    unsigned long detect_flash_data_timeout;
    display.begin(SSD1306_SWITCHCAPVCC, 0x3c); 
    #if 0
  pinMode(PIN_1, INPUT);
    pinMode(PIN_2, INPUT);
    pinMode(PIN_3, INPUT);
    pinMode(PIN_4, INPUT);
    pinMode(PIN_5, INPUT);
    digitalWrite(PIN_1,HIGH);
    digitalWrite(PIN_2,HIGH);
    digitalWrite(PIN_3,HIGH);
    digitalWrite(PIN_4,HIGH);
    digitalWrite(PIN_5,HIGH);
  #endif 
  pinMode(ESP8266_GPIO4, INPUT);
  pinMode(ESP8266_GPIO5, INPUT);
  pinMode(ESP8266_GPIO12, INPUT);
  pinMode(ESP8266_GPIO14, INPUT);
  pinMode(ESP8266_GPIO15, INPUT);
  
  digitalWrite(ESP8266_GPIO4,HIGH);
  digitalWrite(ESP8266_GPIO5,HIGH);
  digitalWrite(ESP8266_GPIO12,HIGH);
  digitalWrite(ESP8266_GPIO14,HIGH);
  digitalWrite(ESP8266_GPIO15,HIGH);
  
    screenFlush(serial, pin, wifi);
  #if 0
    detect_flash_data_timeout = millis();
    while ((millis() - detect_flash_data_timeout) <= 1000)
    {
        if (digitalRead(ESP8266_FW) == LOW)
        {
            display.clearDisplay(); 
            display.setTextSize(2);
            display.setTextColor(WHITE);
            display.setCursor(0,0); 
            display.print("Firmware    Mode"); 
            display.display();
            while(true) {
                delay(500);
            }
        }
    }  
  #endif 
    PSASerial.begin(19200);
}


void loop(void)
{
    if (test_log == 1)
    {
        serialTest();
        if (serial_ok == 1)
        {
            gpioTest();
            wifiTest();
            PSASerial.print("AT+END\r\n");
            if(serial_ok == 1 && wifi_satisfy == 1 && PIN_LOW_LOG == 1 && PIN_HIGH_LOG == 1)
            {
                PSASerial.end();
            }
        }
        else
        {
            screenFlush(serial, pin, wifi);
        }
    }
}

