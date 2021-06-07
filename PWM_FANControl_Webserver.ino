




#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>


////////////////////////////////////////////////////////
// Webserver
////////////////////////////////////////////////////////


WiFiServer server(80);                  //Port 80

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 20000;


////////////////////////////////////////////////////////
// WiFi connect
////////////////////////////////////////////////////////

//Enter your SSID and PASSWORD
const char* ssid = "********";
const char* password = "*******";






////////////////////////////////////////////////////////
// Fan modes
////////////////////////////////////////////////////////

// Output Status
String luefter1State = "auto";
String luefter2State = "auto";
String luefter3State = "auto";
String luefter4State = "auto";

// definition of PWM pins:
const int luefter1 = 16;  // 16 corresponds to GPIO16, sets fan1(luefter1) to GPIO16
const int luefter2 = 17;  // 17 corresponds to GPIO17, sets fan2(luefter2) to GPIO17
const int luefter3 = 18;  // 18 corresponds to GPIO18, sets fan3(luefter3) to GPIO18
const int luefter4 = 19;  // 19 corresponds to GPIO19, sets fan4(luefter4) to GPIO19


////////////////////////////////////////////////////////
// Temperatur Sensor 
////////////////////////////////////////////////////////

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

////////////////////////////////////////////////////////
// PWM Werte setzen
////////////////////////////////////////////////////////

// setting PWM properties
const int freq1 = 200;                //frequency fan1(luefter1)
const int freq2 = 200;                //frequency fan2(luefter2)
const int freq3 = 200;                //frequency fan3(luefter3)
const int freq4 = 200;                //frequency fan4(luefter4)
const int resolution = 8;             //bit-resolution
const int pwmChannel1 = 0;            //inner PWM-Channels
const int pwmChannel2 = 1;            //inner PWM-Channels
const int pwmChannel3 = 2;            //inner PWM-Channels
const int pwmChannel4 = 3;            //inner PWM-Channels

//FanSpeed Variablen
int fanSpeed = 0;                     //basic fanspeed(set to 0)
int tempMin = 22;                     //definition of minimum temperature
int tempMax = 28;                     //definition of maximum temperature


////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
// Setup
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

void setup() {
    Serial.begin(115200);                               //baud rate
    Serial.println("Ready");
    // configure PWM functionalitites
    ledcSetup(pwmChannel1, freq1, resolution);          
    ledcSetup(pwmChannel2, freq2, resolution);
    ledcSetup(pwmChannel3, freq3, resolution);
    ledcSetup(pwmChannel4, freq4, resolution);


    // channelvariable for GPIO-control
    ledcAttachPin(luefter1, pwmChannel1);
    ledcAttachPin(luefter2, pwmChannel2);
    ledcAttachPin(luefter3, pwmChannel3);
    ledcAttachPin(luefter4, pwmChannel4);


    // temperaturesensor start
    sensors.begin();


    // initializing Output variables
    pinMode(luefter1, OUTPUT);
    pinMode(luefter2, OUTPUT);
    pinMode(luefter3, OUTPUT);
    pinMode(luefter4, OUTPUT);
    // Set outputs to LOW
    pinMode(luefter1, LOW);
    pinMode(luefter2, LOW);
    pinMode(luefter3, LOW);
    pinMode(luefter4, LOW);

    // connection to Network, for example WLAN 
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // Output of the connection options
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // start web server
    server.begin();

}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
// loop
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

void loop(void) {

    // read temperature sensor
    sensors.requestTemperatures();                         //Request to temperature sensor
    float temperatureC = sensors.getTempCByIndex(0);       //sets temperatureC to input value
    String StemperatureC;                                  //StemperatureC for Webserver, (actually Value)
    StemperatureC = String(temperatureC);                  //cast temperature variable too String Casten for output an the Webserver
    Serial.print(temperatureC);
    Serial.println("ºC");
    delay(100);

    //------------------------------------------------------------------------
   WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

                        // turns Luefter status   auto -> on ->  off -> auto...
                        if (header.indexOf("GET /1/on") >= 0) {                   //fan1, Url-extension
                            Serial.println("L\x81fter 1 on");                     //print "fan1 on" to output
                            luefter1State = "on";                                 //sets fan1state "on"
                        } 
                          else if (header.indexOf("GET /1/off") >= 0) {           //fan1
                            Serial.println("L\x81fter 1 off");
                            luefter1State = "off";
                        } else if (header.indexOf("GET /1/auto") >= 0) {          //fan1
                            Serial.println("L\x81fter 1 auto");
                            luefter1State = "auto";
                        } 
                        else if (header.indexOf("GET /2/on") >= 0) {               //fan2 
                            Serial.println("L\x81fter 2 on");
                            luefter2State = "on";
                        } else if (header.indexOf("GET /2/off") >= 0) {           //fan2
                            Serial.println("L\x81fter 2 off");
                            luefter2State = "off";
                        } else if (header.indexOf("GET /2/auto") >= 0) {          //fan2
                            Serial.println("L\x81fter 2 auto");
                            luefter2State = "auto";
                        }
                        else if (header.indexOf("GET /3/on") >= 0) {              //fan3
                            Serial.println("L\x81fter 3 on");
                            luefter3State = "on";
                        } else if (header.indexOf("GET /3/off") >= 0) {           //fan3
                            Serial.println("L\x81fter 3 off");
                            luefter3State = "off";
                        } else if (header.indexOf("GET /3/auto") >= 0) {          //fan3
                            Serial.println("L\x81fter 3 auto");
                            luefter3State = "auto";
                        }
                        else if (header.indexOf("GET /4/on") >= 0) {              //fan4
                            Serial.println("L\x81fter 4 on");
                            luefter4State = "on";
                        } else if (header.indexOf("GET /4/off") >= 0) {           //fan4
                            Serial.println("L\x81fter 4 off");
                            luefter4State = "off";
                        } else if (header.indexOf("GET /4/auto") >= 0) {          //fan4
                            Serial.println("L\x81fter 4 auto");
                            luefter4State = "auto";
                        }

                        // Display the HTML web page
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" http-equiv=\"refresh\" content=\"2\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");
                        // CSS to style the on/off buttons 
                        // Feel free to change the background-color and font-size attributes to fit your preferences
                        // Three Button colors for the three modes "on","off","auto"
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".button2 {background-color: #555555; border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".button3 {background-color: #D8D800;border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");

                        // Web Page Heading
                        client.println("<body><h1>ESP32 Web Server</h1>");
                        // current temperature
                        client.println("<h1>Temperatur: " + StemperatureC + "&degC</h1>");
                        // Display current state, and ON/OFF/AUTO buttons for fan1(luefter1)  
                        client.println("<p>L&uumlfter1 - Status " + luefter1State + "</p>");

                        // If the luefter1State is off, it displays the auto button       
                        if (luefter1State == "off") {
                            client.println("<p><a href=\"/1/auto\"><button class=\"button button3\">AUTO</button></a></p>");
                            luefter1State = "off";
                        }
                        // If the luefter1State is auto, it displays the on button
                        else if (luefter1State == "auto") {
                            client.println("<p><a href=\"/1/on\"><button class=\"button \">ON</button></a></p>");
                            luefter1State = "auto";
                        }
                        // If the luefter1State is on, it displays the off button
                        else {
                            client.println("<p><a href=\"/1/off\"><button class=\"button button2\">OFF</button></a></p>");
                            luefter1State = "on";
                        }


                        // If the luefter2State is off, it displays the auto button
                        client.println("<p>L&uumlfter2 - Status " + luefter2State + "</p>");
                        if (luefter2State == "off") {
                            client.println("<p><a href=\"/2/auto\"><button class=\"button button3\">AUTO</button></a></p>");
                            luefter2State = "off";
                        }
                        // If the luefter2State is auto, it displays the on button
                        else if (luefter2State == "auto") {
                            client.println("<p><a href=\"/2/on\"><button class=\"button \">ON</button></a></p>");
                            luefter2State = "auto";
                        }
                        // If the luefter2State is on, it displays the off button
                        else {
                            client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
                            luefter2State = "on";
                        }

                        // If the luefter3State is off, it displays the auto button
                        client.println("<p>L&uumlfter3 - Status " + luefter3State + "</p>");
                        if (luefter3State == "off") {
                            client.println("<p><a href=\"/3/auto\"><button class=\"button button3\">AUTO</button></a></p>");
                            luefter3State = "off";
                        }
                        // If the luefter3State is auto, it displays the on button
                        else if (luefter3State == "auto") {
                            client.println("<p><a href=\"/3/on\"><button class=\"button \">ON</button></a></p>");
                            luefter3State = "auto";
                        }
                        // If the luefter3State is on, it displays the off button
                        else {
                            client.println("<p><a href=\"/3/off\"><button class=\"button button2\">OFF</button></a></p>");
                            luefter3State = "on";
                        }

                        // If the luefter4State is off, it displays the auto button
                        client.println("<p>L&uumlfter4 - Status " + luefter4State + "</p>");
                        if (luefter4State == "off") {
                            client.println("<p><a href=\"/4/auto\"><button class=\"button button3\">AUTO</button></a></p>");
                            luefter4State = "off";
                        }
                        // If the luefter4State is auto, it displays the on button
                        else if (luefter4State == "auto") {
                            client.println("<p><a href=\"/4/on\"><button class=\"button \">ON</button></a></p>");
                            luefter4State = "auto";
                        }
                        // If the luefter4State is on, it displays the off button
                        else {
                            client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
                            luefter4State = "on";
                        }
                        client.println("</body></html>");

                        // The HTTP response ends with another blank line
                        client.println();
                        // Break out of the while loop
                        break;
                    } else { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }

    //-------------------------------------------------  
      // fan1(Luefter 1) state requesting
    if (luefter1State == "auto")                         // auto = automatical PWM regulation
    {
        if (temperatureC < tempMin)
        {
            //Temperature below tempMin fan1 off
            fanSpeed = 0;
            Serial.println("L1 = auto, Temp < " + String(tempMin) + ", fanSpeed= " + String(fanSpeed));                                     //print in serial interface the temperature,the current terms and the fan speed
            ledcWrite(pwmChannel1, fanSpeed);
        }
        else if (temperatureC >= tempMin && temperatureC < tempMax)
        {
            //Temperature from tempMin fan1 20% -80%
            //0...255 = 256 values because of 8-bit
            // 32 means 20% of 256 values and 80% means 216
            //fanSpeed = map(tempC, tempMin, tempMax, 32, 255);
            fanSpeed = map(temperatureC, tempMin, tempMax, 32, 216);
            Serial.println("L1 = auto, Temp > " + String(tempMin) + " + Temp < " + String(tempMax) + ", fanSpeed= " + String(fanSpeed));    //print in serial interface the temperature,the current terms and the fan speed
            ledcWrite(pwmChannel1, fanSpeed);
        }
        else if (temperatureC >= tempMax)
        {
            //Temperature from temMax fan1 100%
            fanSpeed = 255;                                                                                                                 //fanSpeed 100% means 255
            Serial.println("L1 = auto, Temp > " + String(tempMax) + ", fanSpeed= " + String(fanSpeed));                                     //print in serial interface the temperature,the current terms and the fan speed
            ledcWrite(pwmChannel1, fanSpeed);
        }
    }
    else if (luefter1State == "off")  // no terms -> fan 1 off
    {
        fanSpeed = 0;
        Serial.println("L1 = off , fanSpeed= " + String(fanSpeed));
        ledcWrite(pwmChannel1, fanSpeed);

    }
    else if (luefter1State == "on")  // no terms -> fan 1 on
    {
        fanSpeed = 255;
        Serial.println("L1 = on , fanSpeed= " + String(fanSpeed));
        ledcWrite(pwmChannel1, fanSpeed);
    }

    // fan2(Luefter 2) state requesting
    if (luefter2State == "auto")                    // auto = automatical PWM regulation
    {
        if (temperatureC < tempMin)
        {
            //Temperature below tempMin fan2 off
            fanSpeed = 0;
            Serial.println("L2 = auto, Temp < " + String(tempMin) + ", fanSpeed= " + String(fanSpeed));     //print in serial interface the temperature,the current terms and the fan speed
            ledcWrite(pwmChannel2, fanSpeed);
        }
        else if (temperatureC >= tempMin && temperatureC < tempMax)
        {
            //Temperature from tempMin fan2 20% -80%
            //0...255 = 256 values because of 8-bit
            // 32 means 20% of 256 values and 80% means 216
            //fanSpeed = map(tempC, tempMin, tempMax, 32, 255)
            fanSpeed = map(temperatureC, tempMin, tempMax, 32, 216);
            Serial.println("L2 = auto, Temp > " + String(tempMin) + " + Temp < " + String(tempMax) + ", fanSpeed= " + String(fanSpeed));    //print in serial interface the temperature,the current terms and the fan speed
            ledcWrite(pwmChannel2, fanSpeed);
        }
        else if (temperatureC >= tempMax)
        {
            //Temperature from temMax fan2 100%
            fanSpeed = 255;                                                                                                         //fanSpeed 100% means 255
            Serial.println("L2 = auto, Temp > " + String(tempMax) + ", fanSpeed= " + String(fanSpeed));                             //print in serial interface the temperature,the current terms and the fan speed
            ledcWrite(pwmChannel2, fanSpeed);
        }
    }
    else if (luefter2State == "off")  // no terms -> fan 2 off
    {
        fanSpeed = 0;
        Serial.println("L2 = off , fanSpeed= " + String(fanSpeed));
        ledcWrite(pwmChannel2, fanSpeed);
    }
    else if (luefter2State == "on")  // no terms -> fan 2 on
    {    fanSpeed = 255;
        Serial.println("L2 = on , fanSpeed= " + String(fanSpeed));
        ledcWrite(pwmChannel2, fanSpeed);
    }


    // fan3(Luefter 3) state requesting
    if (luefter3State == "auto")                         // auto = automatical PWM regulation
    {
        if (temperatureC < tempMin)
        {
            //Temperature below tempMin fan3 off
            fanSpeed = 0;
            Serial.println("L3 = auto, Temp < " + String(tempMin) + ", fanSpeed= " + String(fanSpeed));     //print in serial interface the temperature,the current terms and the fan speed
            ledcWrite(pwmChannel3, fanSpeed);
        }
        else if (temperatureC >= tempMin && temperatureC < tempMax)
        {
            //Temperature from tempMin fan3 20% -80%
            //0...255 = 256 values because of 8-bit
            // 32 means 20% of 256 values and 80% means 216
            //fanSpeed = map(tempC, tempMin, tempMax, 32, 255);
            fanSpeed = map(temperatureC, tempMin, tempMax, 32, 216);
            Serial.println("L3 = auto, Temp > " + String(tempMin) + " + Temp < " + String(tempMax) + ", fanSpeed= " + String(fanSpeed));    //print in serial interface the temperature,the current terms and the fan speed
            ledcWrite(pwmChannel3, fanSpeed);
        }
        else if (temperatureC >= tempMax)
        {
            //Temperature from tempMax fan3 100%
            fanSpeed = 255;                                                                                                         //fanSpeed 100% means 255
            Serial.println("L3 = auto, Temp > " + String(tempMax) + ", fanSpeed= " + String(fanSpeed));                             //print in serial interface the temperature,the current terms and the fan speed
            ledcWrite(pwmChannel3, fanSpeed);
        }
    }
    else if (luefter3State == "off")  // no terms -> fan 3 off
    {
        fanSpeed = 0;
        Serial.println("L3 = off , fanSpeed= " + String(fanSpeed));
        ledcWrite(pwmChannel3, fanSpeed);
    }
    else if (luefter3State == "on")  // no terms -> fan 3 on
    {
        fanSpeed = 255;
        Serial.println("L3 = on , fanSpeed= " + String(fanSpeed));
        ledcWrite(pwmChannel3, fanSpeed);
    }


    // fan4(Luefter 4) state requesting
    if (luefter4State == "auto")                         // auto = automatical PWM regulation
    {
        if (temperatureC < tempMin)
        {
            //Temperature below tempMin fan4 off
            fanSpeed = 0;
            Serial.println("L4 = auto, Temp < " + String(tempMin) + ", fanSpeed= " + String(fanSpeed));     //print in serial interface the temperature,the current terms and the fan speed
            ledcWrite(pwmChannel4, fanSpeed);
        }
        else if (temperatureC >= tempMin && temperatureC < tempMax)
        {
            //Temperature from tempMin fan4 20% -80%
            //0...255 = 256 values because of 8-bit
            // 32 means 20% of 256 values and 80% means 216
            //fanSpeed = map(tempC, tempMin, tempMax, 32, 255);
            fanSpeed = map(temperatureC, tempMin, tempMax, 32, 216);
            Serial.println("L4 = auto, Temp > " + String(tempMin) + " + Temp < " + String(tempMax) + ", fanSpeed= " + String(fanSpeed));    //print in serial interface the temperature,the current terms and the fan speed
            ledcWrite(pwmChannel4, fanSpeed);
        }
        else if (temperatureC >= tempMax)
        {
            //Temperature from tempMax fan4 100%
            fanSpeed = 255;                                                                                                         //fanSpeed 100% means 255
            Serial.println("L4 = auto, Temp > " + String(tempMax) + ", fanSpeed= " + String(fanSpeed));                             //print in serial interface the temperature,the current terms and the fan speed
            ledcWrite(pwmChannel4, fanSpeed);
        }
    }
    else if (luefter4State=="off")  // no terms -> fan 4 off
    {
    fanSpeed = 0;
    Serial.println("L4 = off , fanSpeed= "+String(fanSpeed));
    ledcWrite(pwmChannel4, fanSpeed);

  }
  else if (luefter4State=="on")  // no terms -> fan 4 on
  {
    fanSpeed = 255;
    Serial.println("L4 = on , fanSpeed= "+String(fanSpeed));
    ledcWrite(pwmChannel4, fanSpeed);
  }




  
}
