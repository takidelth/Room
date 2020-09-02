//使用Arduino UNO的连线方式
#define DIN 11
#define CS  10
#define CLK 13
//使用ESP8266的连线方式
// #define DIN 13 //D7
// #define CS  15 //D8
// #define CLK 14 //D5

#define uchar unsigned char
#include <LedControl.h>
#include <SimpleDHT.h>

int DHTpin = 9;
SimpleDHT11 dht11(DHTpin);    // 初始化DHT引脚
int led = 3;

LedControl lc=LedControl(DIN,CLK,CS,4); //4个MAX7219数码管模块级联
unsigned long delaytime=500;
uchar led_rev[10] = {0x7e, 0x06, 0x6d, 0x4f, 0x17, 0x5b, 0x7b, 0x0e, 0x7f, 0x5f}; //反向共阴七段数码管码表
uchar C_rev = 0x78; //反向字母码表
uchar P_rev = 0x3D;
uchar U_rev = 0x76;
uchar G_rev = 0x7B;
uchar S_rev = 0x5B;
uchar D_rev = 0x7E;
uchar F_rev = 0x39;
uchar A_rev = 0x3F;
uchar N_rev = 0x3E;
uchar Cel_rev = 0xF8; //摄氏度符号

void setRevNum(int addr, int digit, char num)
{ //用于反向显示数字(第几个数码管模块, 第几位, 显示什么数字)
  if(num!=' ')
    lc.setRow(addr, digit, led_rev[num-48]);
  else
    lc.setRow(addr, digit, 0x00);
}

void setup() {
  pinMode(2, OUTPUT);
  
  
  Serial.begin(9600);
  // while(!Serial)
  //   continue;
  Serial.println("Serial Port Ready");
  int devices=lc.getDeviceCount();
  for(int address=0;address<devices;address++) {
    /*The MAX72XX is in power-saving mode on startup*/
    lc.shutdown(address,false);
    /* 数码管亮度调至最低，防止功率过高开发板过热，亮度过高会有安全隐患 */
    /* 注意：实测亮度调至15，数码管全亮时，滤波电容温度会高于100度 */
    lc.setIntensity(address,1);
    /* and clear the display */
    lc.clearDisplay(address);
  }
//  while(Serial.available()==0)
//  {
//    ledConnectionTest();
//  }

  attachInterrupt(2, interruptRequest, CHANGE);   // 2 引脚设置为通信中断标志

}

// char frame[40] = {}; //用于存储AIDA64从串口发来的信息
// byte inByte;
// int temperature,humidity
// char temp[3];

byte temperature = 0;
byte humidity = 0;

void refreshled()
{
  char a0,a1,a2,a3,b0,b1,b2,b3;
  a3 = temperature/1000+48;
  a2 = (temperature%1000)/100+48;
  a1 = ((temperature%1000)%100)/10+48;
  a0 = (((temperature%1000)%100)%10)+48;
  if(temperature<1000) a3=' ';
  if(temperature<100)  a2=' ';
  if(temperature<10)   a1=' ';

//注释掉的内容用于正向使用数码管
/*lc.setChar(0,0,a0,false);
  lc.setChar(0,1,a1,false);
  lc.setChar(0,2,a2,false);
  lc.setChar(0,3,a3,false);
  lc.setRow(0,7,B01001110);
  lc.setRow(0,6,B01100111);
  lc.setRow(0,5,B00111110);*/
   
  lc.setRow(0,0,C_rev);
  lc.setChar(0,3,' ',false);
  setRevNum(0,4,a2);
  setRevNum(0,5,a1);
  setRevNum(0,6,a0);
  lc.setRow(0,7,Cel_rev);
  
  b3 = humidity/1000+48;
  b2 = (humidity%1000)/100+48;
  b1 = ((humidity%1000)%100)/10+48;
  b0 = (((humidity%1000)%100)%10)+48;
  if(humidity<1000) b3=' ';
  if(humidity<100)  b2=' ';
  if(humidity<10)   b1=' ';
/*lc.setChar(1,0,b0,false);
  lc.setChar(1,1,b1,false);
  lc.setChar(1,2,b2,false);
  lc.setChar(1,3,b3,false);
  lc.setRow(1,7,B01011011);
  lc.setRow(1,6,B01011011);
  lc.setRow(1,5,B01111110);*/  
  lc.setRow(1,0,S_rev);
  lc.setChar(1,3,' ',false);
  setRevNum(1,5,b2);
  setRevNum(1,6,b1);
  setRevNum(1,7,b0);
  // lc.setRow(1,7,Cel_rev);
   
}

void ledConnectionTest()
{
  for(int i = 0; i < 4; i++){
    for(int j = 7; j >= 0; j--){
      lc.setRow(i, j, 0x01);
      delay(50);
      lc.setRow(i, j, 0x00);
    }  
  }
}

void interruptRequest() {

  char comData[43] = "";
  char param[20] = "";
  char order[20] = "";
  char temp, *flag;

  // Save data
  while (Serial.available()) {
      
    temp = Serial.read();
    // start for '^'
    if (temp == '^') {
      // data save to comData
      for (int i=1; Serial.available() > 0 && i <= 42; i++) {
        
        comData[i] = Serial.read();

        if (comData[i] == '#') {
          comData[i] = '\0';
          flag = &comData[i+1];
        }

        if (comData[i] == '$') {
          comData[i] = '\0';
          break;
        }

      }

      // run command
      strcpy(order, &comData[1]);
      strcpy(param, flag);
      runFunction(order, param);
    
    }
  
  }
    
}

void runFunction(String order, String param) {
  if (order == "led") {

    if (param == "get") {
      bool ledStatus = digitalRead(led);
      Serial.print("^#"); Serial.print(ledStatus); Serial.println("$");
    } else {
      digitalWrite(led, ! digitalRead(led));
    }

  } else if (order == "dht") {
    
    Serial.print("^temperature#");  Serial.print(temperature);  Serial.println("$");
    Serial.print("^humidity#");     Serial.print(humidity);     Serial.println("$");

  } else if (order == "all") {
    
    bool ledStatus = digitalRead(led);
    Serial.print("^led#");          Serial.print(ledStatus);    Serial.println("$");
    Serial.print("^temperature#");  Serial.print(temperature);  Serial.println("$");
    Serial.print("^humidity#");     Serial.print(humidity);     Serial.println("$");

  } else {
    
    Serial.println("^ERR#$");  
  
  }
}
void loop() {
  if (dht11.read(&temperature, &humidity, NULL) != 0){
    refreshled();
    delay(500);
  }
  interruptRequest();
}
