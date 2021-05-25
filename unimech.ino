#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
const char* ssid = "TRAM:864/1944";
String web="www.unimech.tram.001.com";
const byte DNS_PORT = 53;
bool NOTI_Flag = 0;
IPAddress apIP(192, 168, 1, 1);
String head = "<html lang=\"en\"><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"><meta name=\"format-detection\" content=\"telephone=no\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"><title>UV_Remote</title><script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;p = l.nextElementSibling.classList.contains('l');document.getElementById('p').disabled = !p;if(p)document.getElementById('p').focus();}</script><style>.c,body{text-align:center;font-family:verdana}div,input{padding:5px;font-size:1em;margin:5px 0;box-sizing:border-box;}input,button,.msg{border-radius:.3rem;width: 100%}button,input[type='button'],input[type='submit']{cursor:pointer;border:0;line-height:2.4rem;font-size:1.2rem;width:100%}input[type='file']{border:1px solid #1fa3ec}.wrap {text-align:left;display:inline-block;min-width:260px;max-width:500px}a{color:#000;font-weight:700;text-decoration:none}a:hover{color:#1fa3ec;text-decoration:underline}.q{height:16px;margin:0;padding:0 5px;text-align:right;min-width:38px;float:right}.q.q-0:after{background-position-x:0}.q.q-1:after{background-position-x:-16px}.q.q-2:after{background-position-x:-32px}.q.q-3:after{background-position-x:-48px}.q.q-4:after{background-position-x:-64px}.q.l:before{background-position-x:-80px;padding-right:5px}.ql .q{float:left}.q:after,.q:before{content:'';width:16px;height:16px;display:inline-block;background-repeat:no-repeat;background-position: 16px 0;}@media (-webkit-min-device-pixel-ratio: 2),(min-resolution: 192dpi){.q:before,.q:after {background-size: 95px 16px;}}.msg{padding:20px;margin:20px 0;border:1px solid #eee;border-left-width:5px;border-left-color:#777}.msg h4{margin-top:0;margin-bottom:5px}.msg.P{border-left-color:#1fa3ec}.msg.P h4{color:#1fa3ec}.msg.D{border-left-color:#dc3630}.msg.D h4{color:#dc3630}.msg.S{border-left-color: #5cb85c}.msg.S h4{color: #5cb85c}dt{font-weight:bold}dd{margin:0;padding:0 0 0.5em 0;min-height:12px}td{vertical-align: top;}.h{display:none}button.D{background-color:#dc3630}body.invert,body.invert a,body.invert h1 {background-color:#060606;color:#fff;}body.invert.msg{color:#fff;background-color:#282828;border-top:1px solid #555;border-right:1px solid #555;border-bottom:1px solid #555;}body.invert .q[role=img]{-webkit-filter:invert(1);filter:invert(1);}input:disabled {opacity: 0.5;}</style></head>";
String body_1="<body method=\"get\" data-gr-c-s-loaded=\"true\"><div class=\"wrap\" style=\"border-top-width: 100px;padding-top: 150px;padding-bottom: 100px;\"><h1>WiFi_Daa</h1><h3>UV Remote!...</h3>";
String message="<h4 style=\"center\">Please move away from disinfector</h4><br><br><a href=\"ButtonOn\"> <button style=\"background-color:#1fa3ec;color:#fff;\" name=\"btn\" value=\"on\">ON</button></a><br></div><svg id=\"zcomponents__svg\" style=\"display: none;\"></svg></body></html>";
ESP8266WebServer server(80);
DNSServer dnsServer;
void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(5, OUTPUT);//for controller_2 to power
  pinMode(4,INPUT);//FOR Ballast listen
  pinMode(A0,INPUT);//for temperature
  // pinMode(2, OUTPUT);
  digitalWrite(LED_BUILTIN , HIGH);
  digitalWrite(5, LOW);
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid);//, password);

  server.on("/", handle_root);
  server.on("/ButtonOn", handle_L0);
  server.on("/ButtonOff", handle_L2);
  server.begin();
  // Serial.println("HTTP server started");
  dnsServer.setTTL(300);
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(DNS_PORT, web, apIP);
  // Serial.println("USP Server started");
}

void loop() {
  server.handleClient();
  dnsServer.processNextRequest();
  check_loop();
}

void handle_root() {
  // Serial.println("Page served");
  String toSend = head + body_1 + message;
  server.send(200, "text/html", toSend);
  delay(100);
}

void handle_L0() {
  PLAY();
}
void handle_L2() {
  NOTI_Flag= 0;
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(5,HIGH);
  message = "<h4 style=\"center\">Please move away from disinfector</h4><br><br> <a href=\"ButtonOn\"> <button style=\"background-color:#1fa3ec;color:#fff;\" name=\"btn\" value=\"on\">ON</button></a><br></div><svg id=\"zcomponents__svg\" style=\"display: none;\"></svg></body></html>";
  handle_root();
}
void PLAY(){
  message = "<h4 style=\"center\">UV is ON </h4><br><br> <a href=\"ButtonOff\"> <button style=\"background-color:#ec1f1f;color:#fff;\" name=\"btn\" value=\"Off\">OFF</button></a><br></div><svg id=\"zcomponents__svg\" style=\"display: none;\"></svg></body></html>";
  handle_root();
  digitalWrite(5, HIGH);
  digitalWrite(LED_BUILTIN,HIGH);
}
void  check_loop()
{
  if(NOTI_Flag == 1){
  tempo();
  ballast();
  }
}
int tempo(){
  uint16_t temp = analogRead(A0);    //Read the analog pin
temp = temp * 0.48828125;   // convert output (mv) to readable celcius
Serial.print("Temperature: ");
  Serial.print(temp);
  if(temp>=30){
     digitalWrite(LED_BUILTIN,LOW);
     digitalWrite(5,LOW);
     message ="<h3 style=\"color:red;\"> Device is temporarily off  </h4><h5 style=\"color:red;\">Due to an internal temperature.</h5><h5 style=\"color:red;\">please turn off device</h5></div><svg id=\"zcomponents__svg\" style=\"display: none;\"></svg></body></html>";
     handle_root();
   while(1);{delay(10000);}
  }
}

int ballast(){
uint16_t condition= analogRead(4);
  if(condition>=500){
     digitalWrite(LED_BUILTIN,LOW);
     digitalWrite(5,LOW);
     message = "<h3 style=\"color:red;\"> Something went wrong :-( </h4><h5 style=\"color:red;\">Please contact customer care.</h3></div><svg id=\"zcomponents__svg\" style=\"display: none;\"></svg></body></html>";
     handle_root();
     while(1);{delay(10000);}
  }
}
