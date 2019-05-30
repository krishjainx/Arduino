
#include <Arduino.h>
#include <U8g2lib.h>


U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI disp1(U8G2_R0, /* cs=*/ 6, /* dc=*/ 5, /* reset=*/ 4);
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI disp2(U8G2_R0, /* cs=*/ 9, /* dc=*/ 8, /* reset=*/ 7);

void setup() {
  disp1.begin();
  disp2.begin();
  pinMode(10,OUTPUT);
  pinMode(3,OUTPUT);
  digitalWrite(10,HIGH);
}

int y = 0;

void loop() {
  analogWrite(3,millis()/30 % 255);
  disp1.clearBuffer();
  disp1.setFont(u8g2_font_logisoso32_tr);
  disp1.drawStr(y - 256,32,"BIG PEE PEE");
  disp1.sendBuffer();
  disp1.clearBuffer();
  
  disp2.setFont(u8g2_font_logisoso32_tr);
  disp1.drawStr(y - 256,32,"BIG PEE PEE");
  disp2.sendBuffer();
  
  y = (y + 5) % 1024;
}
