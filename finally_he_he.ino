#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

const char* ssid = "Dialog 4G 609"; 
const char* password = "H3RM2MEHF9L"; 

WiFiServer server(80);
String header;

String output5State = "NOT SELECT";
String output4State = "NOT SELECT";
String output18State = "NOT SELECT";
String selected_colour = "";
String sensor_colour = "";
int selected_count = 0;
const int output5 = 5;
const int output4 = 4;
const int output18 = 18;
const int output19 = 19;

void setup() {
  Serial.begin(115200);

  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  pinMode(output18, OUTPUT);
  pinMode(output19, OUTPUT);

  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);
  digitalWrite(output18, LOW);
  digitalWrite(output19, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();

  if (client) {                            
    Serial.println("New Client.");       
    String currentLine = "";               
    while (client.connected()) {           
      if (client.available()) {            
        char c = client.read();           
        Serial.write(c);                
        header += c;
        if (c == '\n') {      

          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            if (header.indexOf("GET /5/SELECT") >= 0) {
              Serial.println("RED IS SELECTED");
              output5State = "IS_SELECT";
              selected_colour = "RED";
              digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /5/NOT_SELECT") >= 0) {
              Serial.println("RED IS NOT SELECT");
              output5State = "NOT_SELECT";
              digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /4/SELECT") >= 0) {
              Serial.println("BLUE IS SELECTED");
              output4State = "IS_SELECT";
              selected_colour = "BLUE";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/NOT_SELECT") >= 0) {
              Serial.println("BLUE IS NOT SELECTED");
              output4State = "NOT_SELECT";
              digitalWrite(output4, LOW);
            } else if (header.indexOf("GET /18/SELECT") >= 0) {
              Serial.println("GREEN SELECTED");
              output18State = "IS_SELECT";
              selected_colour = "GREEN";
              digitalWrite(output18, HIGH);
            } else if (header.indexOf("GET /18/NOT_SELECT") >= 0) {
              Serial.println("GREEN IS NOT SELECTED");
              output18State = "NOT_SELECT";
              digitalWrite(output18, LOW);
            }

            selected_count = selected_colour.length();
             
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
//            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
 //           client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
 //           client.println(".button2 {background-color: #555555;}</style></head>");
            client.println(".button-blue {background-color: #3498db; color: #fff; border: none; padding: 20px 50px; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button-red {background-color: #e74c3c; color: #fff; border: none; padding: 20px 50px; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button-green {background-color: #27ae60; color: #fff; border: none; padding: 20px 50px; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println("</style></head>");
            
            client.println("<body><h1>What is you need?...</h1>"); 

            client.println("<p>RED " + output5State + "</p>");
            if (output5State=="IS_SELECT") {
              client.println("<p><a href=\"/5/NOT_SELECT\"><button class=\"button-red\">SELECTED</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/SELECT\"><button class=\"button-red\">CLICK HERE</button></a></p>");
            } 
  
            client.println("<p>BLUE " + output4State + "</p>");    
            if (output4State=="IS_SELECT") {
              client.println("<p><a href=\"/4/NOT_SELECT\"><button class=\"button-blue\">SELECTED</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/SELECT\"><button class=\"button-blue\">CLICK HERE</button></a></p>");
            }

            client.println("<p>GREEN " + output18State + "</p>");    
            if (output18State=="IS_SELECT") {
              client.println("<p><a href=\"/18/NOT_SELECT\"><button class=\"button-green\">SELECTED</button></a></p>");
            } else {
              client.println("<p><a href=\"/18/SELECT\"><button class=\"button-green\">CLICK HERE</button></a></p>");
            }

            client.println("</body></html>");

            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");

  }

    if (tcs.begin()) {
    Serial.println("Found TCS3472 sensor");
  } else {
    Serial.println("No TCS3472 sensor found");
    while (1);
  }
  
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  //Serial.print("R: "); Serial.print(r);  
  //Serial.print(" G: "); Serial.print(g);  
  //Serial.print(" B: "); Serial.print(b);  
  //Serial.print(" C: "); Serial.println(c);
  delay(500);

  if (r<200 && b<200 && g<200){
    //digitalWrite(blue, LOW);
    //digitalWrite(red, LOW);
    //digitalWrite(green, LOW);
    sensor_colour = "";
  }

  else if (r > b && g < r) {
    //digitalWrite(red, HIGH);
    sensor_colour = "RED";
  } 

  else if (b > g && r < b) {
    //digitalWrite(blue, HIGH);
    sensor_colour = "BLUE";
  } 
  else if (g>r && g>b){
   // digitalWrite(green, HIGH);
    sensor_colour = "GREEN";
  }

  Serial.println("Sensor colour is " + sensor_colour);
  Serial.println("Selected colour is " + selected_colour);
  
  if (sensor_colour==selected_colour){

    digitalWrite(output19, HIGH);
  }
  else{
    digitalWrite(output19, LOW);
  }


}

  

