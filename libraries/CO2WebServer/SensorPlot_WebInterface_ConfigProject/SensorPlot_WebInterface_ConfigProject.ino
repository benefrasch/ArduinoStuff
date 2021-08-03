/* This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details. */
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//------------------------- SensorPlot WebInterface Reference -------------------------//
#include <sensorplot_webinterface.h>
//------------------------- SensorPlot WebInterface Reference -------------------------//

String matrixausgabe_text  = " "; // Ausgabetext als globale Variable

volatile int matrixausgabe_index = 0;// aktuelle Position in Matrix

IPAddress myOwnIP; // ownIP for mDNS 


//---------------------- SensorPlot WebInterface Module 1/3 START ----------------------//
ESP8266WebServer server(80);
String WebInterfaceTitle = "IoT-Werkstatt CO2";// <- set by Ardublocks
SensorPlot_WebInterface webInterface = SensorPlot_WebInterface();

String calibrationPassword = "password";    // <- set by Ardublocks
void calibration() {
  // Kalibrierfunktion | <- set by Ardublocks
}
int calibrateCallback(String password) {
  if (password == calibrationPassword) {
    calibration();
    return 1;
  }
  return 0;
}

void sensorReading(float *measurements, int *measurementsCount, int maxMeasurements, int *measurementsTimestamp, float sensorInput) {
  if (*measurementsCount < maxMeasurements) {
    *measurementsCount += 1;
    measurements[(*measurementsCount - 1)] = sensorInput;
  } else {
    *measurementsCount = maxMeasurements;
    for(int i = 0; i < (*measurementsCount - 1); i++) {
      measurements[i] = measurements[i + 1];
    }
    measurements[(*measurementsCount - 1)] = sensorInput;
  }
  *measurementsTimestamp = millis();
}

int measurementsCount1 = 0;
float measurements1[128] = {};
int measurementsTimestamp1 = millis();
int cycleDuration1 = 60;  // <- set by Ardublocks
void sensorReading1() {
  float sensorInput = (float) 0;    // <- sensor reading for first input | set by Ardublocks
  sensorReading(measurements1, &measurementsCount1, 128, &measurementsTimestamp1, sensorInput);
}

int measurementsCount2 = 0;
float measurements2[128] = {};
int measurementsTimestamp2 = millis();
int cycleDuration2 = 60;  
void sensorReading2() {
  float sensorInput = (float) 0;    // <- sensor reading for second input | set by Ardublocks
  sensorReading(measurements2, &measurementsCount2, 128, &measurementsTimestamp2, sensorInput);
}

int measurementsCount3 = 0;
float measurements3[128] = {};
int measurementsTimestamp3 = millis();
int cycleDuration3 = 60;  // <- set by Ardublocks
void sensorReading3() {
  float sensorInput = (float) 0;    // <- sensor reading for third input | set by Ardublocks
  sensorReading(measurements3, &measurementsCount3, 128, &measurementsTimestamp3, sensorInput);
}

void configWebInterface() {
  // Graph 1
  String name1 = "CO2 Konzentration"; // <- set by Ardublocks
  String unit1 = "ppm";               // <- set by Ardublocks
  int good1 = 1200;                   // <- set by Ardublocks
  int bad1 = 2000;                    // <- set by Ardublocks
  int min1 = 400;                       // <- set by Ardublocks
  int max1 = 3000;                    // <- set by Ardublocks
  int stepsize1 = 200;                // <- set by Ardublocks
  int cycleStepsize1 = 600;           // <- set by Ardublocks
  if (name1 != "") {
    webInterface.addPlot(name1, unit1, cycleDuration1, good1, bad1, min1, max1, stepsize1, cycleDuration1, cycleStepsize1, &measurementsCount1, measurements1, &measurementsTimestamp1);
  }
  
  // Graph 2
  String name2 = "";          // <- set by Ardublocks
  String unit2 = "";          // <- set by Ardublocks
  int good2 = 30;             // <- set by Ardublocks
  int bad2 = 50;              // <- set by Ardublocks
  int min2 = -10;;            // <- set by Ardublocks
  int max2 = 40;              // <- set by Ardublocks
  int stepsize2 = 5;;         // <- set by Ardublocks
  int cycleStepsize2 = 600;   // <- set by Ardublocks
  if (name2 != "") {
    webInterface.addPlot(name2, unit2, cycleDuration2, good2, bad2, min2, max2, stepsize2, cycleDuration2, cycleStepsize2, &measurementsCount2, measurements2, &measurementsTimestamp2);
  }
  
  // Graph 3
  String name3 = "";          // <- set by Ardublocks
  String unit3 = "";          // <- set by Ardublocks
  int good3 = 1200;           // <- set by Ardublocks
  int bad3 = 2000;            // <- set by Ardublocks
  int min3 = 0;               // <- set by Ardublocks
  int max3 = 3000;            // <- set by Ardublocks
  int stepsize3 = 200;        // <- set by Ardublocks
  int cycleStepsize3 = 600;   // <- set by Ardublocks
  if (name3 != "") {
    webInterface.addPlot(name3, unit3, cycleDuration3, good3, bad3, min3, max3, stepsize3, cycleDuration3, cycleStepsize3, &measurementsCount3, measurements3, &measurementsTimestamp3);
  }
}
//----------------------- SensorPlot WebInterface Module 1/3 END -----------------------//


void setup(){ // Einmalige Initialisierung
Serial.begin(115200);

//---------------------- SensorPlot WebInterface Module 2/3 START ----------------------//
  configWebInterface();
  webInterface.interfaceConfig(WebInterfaceTitle, "Passwort", "Kalibrieren");
  webInterface.serverResponseSetup(&server, &calibrateCallback);
  server.begin();// Server starten

  sensorReading1();
  sensorReading2();
  sensorReading3();
//----------------------- SensorPlot WebInterface Module 2/3 END -----------------------//


//------------ eigenen WLAN - Accespoint aufbauen
WiFi.softAP("WiFi SSID","WiFi Password");
Serial.print("\nAccessPoint SSID:"); Serial.print("WiFi SSID");
Serial.println ("  IP:"+ WiFi.softAPIP().toString());
myOwnIP = WiFi.softAPIP();
matrixausgabe_text = String("Mein Netz:") + String("WiFi SSID") + String( " IP:") + WiFi.softAPIP().toString();
matrixausgabe_index=0;

}

void loop() { // Kontinuierliche Wiederholung 
	delay(100);
  //---------------------- SensorPlot WebInterface Module 3/3 START ----------------------//
  if ((millis() - measurementsTimestamp1) > (cycleDuration1 * 1000)) {
    sensorReading1();
  }
  if ((millis() - measurementsTimestamp2) > (cycleDuration2 * 1000)) {
    sensorReading2();
  }
  if ((millis() - measurementsTimestamp3) > (cycleDuration3 * 1000)) {
    sensorReading3();
  }
  server.handleClient();
  //----------------------- SensorPlot WebInterface Module 3/3 END -----------------------//
  
  delay(1);
}
