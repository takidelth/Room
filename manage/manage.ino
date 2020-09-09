#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <string.h>



ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);



IPAddress local_IP(192, 168, 31, 116);
IPAddress gateway(192, 168, 31, 0);
IPAddress subnet(255, 255, 255, 0);



// 中断允许标志位
#define interruptPin D3
/*
  串口通信约定格式:
    ^Key#Word$  一组数据
    !         为单次通信结束标记

    例:
        ^relay#abcd$  ^asdas#assdfs$ !
*/


// page not found callback this function
void handleUserRequest() {
  String reqResource = server.uri();  // get user requests Resource name
//   Serial.print("reqResource:"); Serial.println(reqResource);

  if (! handleFileRead(reqResource)) {
    server.send(404, "text/plain", "404 Not Found!!");
  }
}

void allInfoJson() {

  // D3 set HIGH
  digitalWrite(interruptPin, HIGH);

  // send command
  Serial.println("^all#$");


  server.send(200, "application/json", returnDataToJsonString());
  // clear
  while (Serial.available() > 0) {
    Serial.read();
  }
}

void relay_control() {
  Serial.print("^relay#$!");
  server.send(200, "application/json", returnDataToJsonString());

  // clear
  while (Serial.available() > 0) {
    Serial.read();
  }
}

bool handleFileRead(String resouce) {
  if (resouce.endsWith("/")) {
    resouce = "/index.html";
  }

  String contentType = getContentType(resouce);

  if (SPIFFS.exists(resouce)) {
    File file = SPIFFS.open(resouce, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

String getContentType(String filename) {
  if      (filename.endsWith(".htm"))   return "text/html";
  else if (filename.endsWith(".html"))  return "text/html";
  else if (filename.endsWith(".css"))   return "text/css";
  else if (filename.endsWith(".js"))    return "application/javascript";
  else if (filename.endsWith(".png"))   return "image/png";
  else if (filename.endsWith(".gif"))   return "image/gif";
  else if (filename.endsWith(".jpg"))   return "image/jpeg";
  else if (filename.endsWith(".ico"))   return "image/x-icon";
  else if (filename.endsWith(".xml"))   return "text/xml";
  else if (filename.endsWith(".pdf"))   return "application/x-pdf";
  else if (filename.endsWith(".zip"))   return "application/x-zip";
  else if (filename.endsWith(".gz"))    return "application/x-gzip";
  return "text/plain";
}







/********************   json   ********************
 *****************************************************/
String comData;
char temp, flag;
bool serToString(String& Key, String& Word) {
		
	while (Serial.available() > 0) {
			
		temp = Serial.read();
		
		if (temp == '^' || temp == ' ') continue;
		if (temp == '$') {
			flag = comData.indexOf("#");
			Key = comData.substring(0, (int)flag);
			Word = comData.substring((int)flag+1);
			comData = "";
			return false;
		}
			
		if (temp == '!') {
			return true;
		}
			
		comData += temp;
		
	}
	

}

String jsonData, k, w;
// 将串口即将返回的数据转 json 字符串
String returnDataToJsonString() {
  jsonData = "";
  delay(5);   // 115200频率下 每秒大约传输74字节数据
  if (Serial.available() > 0) {
    
    while (! serToString(k, w)) {
      jsonData += "\"" + k + "\":\"" + w + "\","; 
    }
    jsonData[jsonData.length() - 1] = ' ';
    jsonData = "{ " + jsonData + "}";

    return jsonData;
  }
}
/****************************************************
 *****************************************************/





void setup() {
  // put your setup code here, to run once:
  pinMode(interruptPin, OUTPUT);

  Serial.begin(115200);

  wifiMulti.addAP("Xiaomi_782C", "wanghai1015");    // add wifi
  WiFi.config(local_IP, gateway, subnet);   // set WiFi 

  // wifi connection test
  Serial.print("Connecting");
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  // output base info
  Serial.print("Connected to"); Serial.println(WiFi.SSID());
  Serial.print("IP address: "); Serial.println(WiFi.localIP());

  // start flash system
  if (SPIFFS.begin()) {
    Serial.println("SPIFFS Started.");
  } else {
    Serial.println("SPIFFS Failed to Start");
  }



  // web server program run start
  server.onNotFound(handleUserRequest);
  server.on("/info", allInfoJson);
  server.on("/relay_control", relay_control);
  server.begin();
  

}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
