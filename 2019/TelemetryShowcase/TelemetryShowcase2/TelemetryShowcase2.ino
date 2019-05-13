
#include <Arduino.h>
#include <U8g2lib.h>


U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI disp3(U8G2_R0, /* cs=*/ 4, /* dc=*/ 3, /* reset=*/ 2);
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI disp4(U8G2_R0, /* cs=*/ 7, /* dc=*/ 6, /* reset=*/ 5);
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI disp5(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

void setup() {
  disp3.begin();
  disp4.begin();
  disp5.begin();
}

int y = 0;

void loop() {
  disp3.clearBuffer();
  disp3.setFont(u8g2_font_logisoso32_tr);
  disp3.drawStr(y - 256,32,"BIG PEE PEE ");
  disp3.sendBuffer();
  disp3.clearBuffer();

  
  disp4.clearBuffer();
  disp4.setFont(u8g2_font_logisoso32_tr);
  disp4.drawStr(y - 256,32,"BIG PEE PEE");
  disp4.sendBuffer();
  disp4.clearBuffer();

  
  disp5.clearBuffer();
  disp5.setFont(u8g2_font_logisoso32_tr);
  disp5.drawStr(y - 256,32,"BIG PEE PEE");
  disp5.sendBuffer();
  disp5.clearBuffer();
  
  y = (y + 5) % 1024;
}
