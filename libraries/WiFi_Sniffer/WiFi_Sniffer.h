/*
  Copyright 2017 Andreas Spiess

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  This software is based on the work of Andreas Spiess, https://github.com/SensorsIot/Wi-Fi-Sniffer-as-a-Human-detector/blob/master/WiFi_Sniffer/WiFi_Sniffer.ino
  and Ray Burnette: https://www.hackster.io/rayburne/esp8266-mini-sniff-f6b93a
  
*/
// This-->tab == "functions.h"

// Expose Espressif SDK functionality
extern "C" {
#include "user_interface.h"
  typedef void (*freedom_outside_cb_t)(uint8 status);
  int  wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
  void wifi_unregister_send_pkt_freedom_cb(void);
  int  wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);
}

#include <ESP8266WiFi.h>
#include "./structures.h"

#define MAX_APS_TRACKED 100
#define MAX_CLIENTS_TRACKED 200

beaconinfo aps_known[MAX_APS_TRACKED];                    // Array to save MACs of known APs
int aps_known_count = 0;                                  // Number of known APs
int nothing_new = 0;
clientinfo clients_known[MAX_CLIENTS_TRACKED];            // Array to save MACs of known CLIENTs
int clients_known_count = 0;                              // Number of known CLIENTs
int Sniffer_erstemal = 1;


String formatMac1(uint8_t mac[ETH_MAC_LEN]) {
  String hi = "";
  for (int i = 0; i < ETH_MAC_LEN; i++) {
    if (mac[i] < 16) hi = hi + "0" + String(mac[i], HEX);
    else hi = hi + String(mac[i], HEX);
    if (i < 5) hi = hi + ":";
  }
  return hi;
}

int register_beacon(beaconinfo beacon)
{
  int known = 0;   // Clear known flag
  for (int u = 0; u < aps_known_count; u++)
  {
    if (! memcmp(aps_known[u].bssid, beacon.bssid, ETH_MAC_LEN)) {
      aps_known[u].lastDiscoveredTime = millis();
      aps_known[u].rssi = beacon.rssi;
      known = 1;
      break;
    }   // AP known => Set known flag
  }
  if (! known && (beacon.err == 0))  // AP is NEW, copy MAC to array and return it
  {
    beacon.lastDiscoveredTime = millis();
    memcpy(&aps_known[aps_known_count], &beacon, sizeof(beacon));
	/*
        Serial.print("Register Beacon ");
        Serial.print(formatMac1(beacon.bssid));
        Serial.print(" Channel ");
        Serial.print(aps_known[aps_known_count].channel);
        Serial.print(" RSSI ");
        Serial.println(aps_known[aps_known_count].rssi);
    */
    aps_known_count++;

    if ((unsigned int) aps_known_count >=
        sizeof (aps_known) / sizeof (aps_known[0]) ) {
      Serial.printf("exceeded max aps_known\n");
      aps_known_count = 0;
    }
  }
  return known;
}

int register_client(clientinfo &ci,int isdata) {
  int known = 0;   // Clear known flag
  
  /*
  if (isdata) Serial.print("\nD "); else Serial.print("\nP");
  Serial.print(" St: ");
  for (int i = 0; i < 3; i++) Serial.printf("%02x", ci.station[i]);
  Serial.print(" bssid: ");
  for (int i = 0; i < 3; i++) Serial.printf("%02x", ci.bssid[i]);
  Serial.print(" ch:");
  Serial.print(ci.channel);
  */
 
  for (int u = 0; u < clients_known_count; u++)  {
    if (! memcmp(clients_known[u].station, ci.station, ETH_MAC_LEN)) { // Knoten ist bereits bekannt
	  if (isdata) {                                                    // und sendet Daten
    	  clients_known[u].lastDiscoveredTime = millis();                    
		  for (int ua = 0; ua < aps_known_count; ua++) {               // Wir schauen, auf welchem Kanal
            if (! memcmp(aps_known[ua].bssid, ci.bssid, ETH_MAC_LEN)) {
                clients_known[u].channel = aps_known[ua].channel;
		        //Serial.printf(" alt%8s", aps_known[ua].ssid); 
                break;
            }
         }
	  }
	  if ((!isdata) && (clients_known[u].channel < 0)) clients_known[u].lastDiscoveredTime = millis(); // Bekannt, schon bisher ohne Verbindung 
      clients_known[u].rssi = ci.rssi;
      known = 1;
      break;
    } // unbekannt
  }

  //Uncomment the line below to disable collection of probe requests from randomised MAC's
  //if (ci.channel == -2) known = 1; // This will disable collection of probe requests from randomised MAC's
  
  if (! known) {
    ci.lastDiscoveredTime = millis();
    // search for Assigned AP
	if (isdata) { // Knoten sendet Daten, also suchen wir den dazugehörigen AP
      for (int u = 0; u < aps_known_count; u++) {
       if (! memcmp(aps_known[u].bssid, ci.bssid, ETH_MAC_LEN)) {
        ci.channel = aps_known[u].channel;
        //Serial.printf("neu%8s", aps_known[u].ssid);
        break;
       }
      }
	}
    if (ci.channel != 0) {
      memcpy(&clients_known[clients_known_count], &ci, sizeof(ci));
	  /*
         Serial.println();
         Serial.print("Register Client ");
         Serial.print(formatMac1(ci.station));
         Serial.print(" Channel ");
         Serial.print(ci.channel);
         Serial.print(" RSSI ");
         Serial.println(ci.rssi);
		*/ 
      clients_known_count++;
    }

    if ((unsigned int) clients_known_count >=
        sizeof (clients_known) / sizeof (clients_known[0]) ) {
      Serial.printf("exceeded max clients_known\n");
      clients_known_count = 0;
    }
  }
  return known;
}


String print_beacon(beaconinfo beacon)
{
  String hi = "";
  if (beacon.err != 0) {
    //Serial.printf("BEACON ERR: (%d)  ", beacon.err);
  } else {
    Serial.printf(" BEACON: <=============== [%32s]  ", beacon.ssid);
    Serial.print(formatMac1(beacon.bssid));
    Serial.printf("   %2d", beacon.channel);
    Serial.printf("   %4d\r\n", beacon.rssi);
  }
  return hi;
}

String print_client(clientinfo ci)
{
  String hi = "";
  int u = 0;
  int known = 0;   // Clear known flag
  if (ci.err != 0) {
    // nothing
  } else {
    Serial.printf("CLIENT: ");
    Serial.print(formatMac1(ci.station));  //Mac of device
    Serial.printf(" ==> ");

    for (u = 0; u < aps_known_count; u++)
    {
      if (! memcmp(aps_known[u].bssid, ci.bssid, ETH_MAC_LEN)) {
               Serial.print("   ");
               Serial.printf("[%32s]", aps_known[u].ssid);   // Name of connected AP
        known = 1;     // AP known => Set known flag
        break;
      }
    }

	/*
    if (! known)  {
      Serial.printf("   Unknown/Malformed packet \r\n");
      for (int i = 0; i < 6; i++) Serial.printf("%02x", ci.bssid[i]);
    } else {
      //    Serial.printf("%2s", " ");
      */
      Serial.print(formatMac1(ci.ap));   // Mac of connected AP
      Serial.printf("  % 3d", ci.channel);  //used channel
      Serial.printf("   % 4d\r\n", ci.rssi);
    //}
  }
  return hi;
}

void promisc_cb(uint8_t *buf, uint16_t len)
{
  int i = 0;
  uint16_t seq_n_new = 0;
  if (len == 12) {
    struct RxControl *sniffer = (struct RxControl*) buf;
  } else if (len == 128) {
    struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) buf;
    if ((sniffer->buf[0] == 0x80)) {
      struct beaconinfo beacon = parse_beacon(sniffer->buf, 112, sniffer->rx_ctrl.rssi);
      if (register_beacon(beacon) == 0) {
        //print_beacon(beacon);
        nothing_new = 0;
      }
    } else if ((sniffer->buf[0] == 0x40)) {
      struct clientinfo ci = parse_probe(sniffer->buf, 36, sniffer->rx_ctrl.rssi);
      if (memcmp(ci.bssid, ci.station, ETH_MAC_LEN)) {
        if (register_client(ci,0) == 0) {
      //    print_client(ci);
          nothing_new = 0;
        }
      }
    }
  } else {
    struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
    //Is data or QOS?
    if ((sniffer->buf[0] == 0x08) || (sniffer->buf[0] == 0x88)) {
      struct clientinfo ci = parse_data(sniffer->buf, 36, sniffer->rx_ctrl.rssi, sniffer->rx_ctrl.channel);
      if (memcmp(ci.bssid, ci.station, ETH_MAC_LEN)) {
        if (register_client(ci,1) == 0) {
      //    print_client(ci);
          nothing_new = 0;
        }
      }
    }
  }
}


void SnifferPurgeDevice(int timeout) {
  long lastseen; 	
  for (int u = 0; u < clients_known_count; u++) {
	// Serial.println(((millis() - clients_known[u].lastDiscoveredTime)/1000));
//    if (((millis() - clients_known[u].lastDiscoveredTime)/1000) > timeout) {
	lastseen = ((millis() - clients_known[u].lastDiscoveredTime)/1000);
	
    if ((lastseen > timeout) || ((clients_known[u].channel==-2) && (lastseen>1))) {  
      //Serial.print("purge Client" );
      //Serial.print(u);
      for (int i = u; i < clients_known_count; i++) memcpy(&clients_known[i], &clients_known[i + 1], sizeof(clients_known[i]));
      clients_known_count--;
      //break;
    }
  }
  for (int u = 0; u < aps_known_count; u++) {
	lastseen = ((millis() - aps_known[u].lastDiscoveredTime)/1000);
    if (lastseen > timeout) {
      //Serial.print("purge Bacon" );
      //Serial.println(u);
      for (int i = u; i < aps_known_count; i++) memcpy(&aps_known[i], &aps_known[i + 1], sizeof(aps_known[i]));
      aps_known_count--;
      break;
    }
  }
  
  
  
}

int SnifferCountDevices(int MinRSSI,int timeout, String myMAC, int randMAC, int display)
 {
  int mycount = 0;	 
  if (Sniffer_erstemal) {
    Serial.printf("\n\nSDK version:%s\n\r", system_get_sdk_version());
    Serial.println(F("Human detector by Andreas Spiess. ESP8266 mini-sniff by Ray Burnette http://www.hackster.io/rayburne/projects"));
    Serial.println(F("Based on the work of Ray Burnette http://www.hackster.io/rayburne/projects"));
	Sniffer_erstemal = 0;
  }  
  SnifferPurgeDevice(timeout); // entferne stumme Teilnehmer ( die den Raum verlassen haben)
  for (int u = 0; u < clients_known_count; u++) {
    if (clients_known[u].rssi >= MinRSSI) { 
      if (myMAC.length() >= 17) { // MAC Adresse vorgegeben 
        if (myMAC == formatMac1(clients_known[u].station)) mycount = 1;
      } 
      else { // sonst alles sniffen
	    if (clients_known[u].channel >= 0) { // special randomised MAC
           mycount++; // echte immer zaehlen
		} else {
			if (randMAC <= clients_known[u].channel) {
               mycount++; // auch randomized zaehlen
			}
		}
      }
    }
  } 
  
  if (display >= 1) {
    Serial.println("");
    Serial.print("Detected WiFi-CLIENTS: ");
    Serial.println(mycount);
    long lastseen;
	// show Clients
   for (int u = 0; u < clients_known_count; u++) {
     Serial.printf("%4d ",u); // Show client number
	 String isMAC;
	 isMAC = formatMac1(clients_known[u].station);
	 if (display > 1) {
         Serial.print(formatMac1(clients_known[u].station));
	 } else {
	     Serial.print(isMAC.substring(0,8)+String(":xx:xx:xx"));
	 }	 

     Serial.print(" RSSI ");
 	 Serial.print(clients_known[u].rssi);
	 if (clients_known[u].rssi < MinRSSI) Serial.print(" (to weak) "); else Serial.print("           ");
     Serial.print(" channel ");
     Serial.print(clients_known[u].channel);
	 lastseen = ((millis() - clients_known[u].lastDiscoveredTime)/1000);
     Serial.print(" Tout ");
	 Serial.println(lastseen);
   }
   if (display>2) {
		 // show Beacons
		  Serial.println("----------- BEACONS"); 
		  for (int u = 0; u < aps_known_count; u++) {
			Serial.printf( "%4d ",u); // Show beacon number
			Serial.print(formatMac1(aps_known[u].bssid));
			Serial.print(" RSSI ");
			Serial.print(aps_known[u].rssi);
			Serial.print(" channel ");
			Serial.print(aps_known[u].channel);
			Serial.printf(" %-15s \n",aps_known[u].ssid);
		  }
    }
  }
  return mycount;
}