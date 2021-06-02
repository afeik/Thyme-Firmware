/*
 * stomata - ESP32 Firmware for the Thyme project
 * Copyright (C) 2021 afeik
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <WebServer.h>
#include <time.h>
#include <AutoConnect.h>
#include <base64.h>
#include <string.h>
#include <analogWrite.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TaskScheduler.h>
#include <LITTLEFS.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>
#include <ArduinoWebsockets.h>
#include <CircularBuffer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>


//Station Name / ID
  int station_id_int=1234567808;
  String station_id = String(station_id_int);
  String station_name="TestStation08";
  String station_state="idle";

//Authentification Declarations
  String token= ""; 
  String auth = base64::encode(station_id + ":" + token);


//token and config are stored here
  const String FILE_PATH =  "/stationdata.txt";
  const String TOKEN_PATH = "/token.txt";
 

//Station configuration
  static DynamicJsonDocument stationconfig(1024);

//ESP32_Ports
  int PUMP = 27;
  int MOISTURE=32;
  int TEMPERATURE=14;
  int TANK=34;
  int VENTIL=15;
  int BUTTON=36;
  
//Sensor-Value-Variables
  int moisture_value=0;
  bool tank_empty=false;
  float temperature_value=0;
  float humidity_value=0;

//Moisture - Sensor Calibration - Values 
  int moisture_wet_value=1321; //in water
  int moisture_dry_value=3422; //in air 
  //Buffer to store moisture values
  CircularBuffer<int, 30> moisture_buffer;

// RGB LED 
  int Led_Red = 0;
  int Led_Green = 2;
  int Led_Blue = 4;

//DHT 11 Temperature and Humidity Sensor
  #define DHTTYPE DHT11 
  DHT dht(TEMPERATURE, DHTTYPE);

//Task Scheduler 
Scheduler runner;
//watering process
bool currently_watering=false;

//including additional functions for handling Wifi/Web-Requests
  #include "WifiConnection.h"
  #include "WebCommunication.h"
  
//Periodic Tasks
  Task PingWebsocket(30000,TASK_FOREVER,&ws_ping_callback,&runner,true);
  Task Write_MoistureValQueue(20000,TASK_FOREVER,&moisture_queue_callback,&runner,true);
  Task SendSensorData(600000,TASK_FOREVER,&send_sensordata_callback,&runner,true);
  Task ReconnectWechsocket(10000,TASK_FOREVER,&ws_reconnect_callback,&runner,true);

void setup() {

  //RGB LED turns blue
  analogWrite(Led_Blue, 255);
 
  //Assign Pin-Modes
  //Sensors
  pinMode(MOISTURE,INPUT);
  pinMode(TEMPERATURE,INPUT);
  pinMode(TANK,INPUT);
  pinMode(BUTTON,INPUT);
  
  //Status-LED and Pump
  pinMode(VENTIL,OUTPUT);
  pinMode(PUMP,OUTPUT);
  pinMode(Led_Red, OUTPUT); 
  pinMode(Led_Green, OUTPUT); 
  pinMode(Led_Blue, OUTPUT); 
  
  //Set initial Values
  digitalWrite(VENTIL,HIGH);
  digitalWrite(PUMP,HIGH);
  moisture_buffer.push(mapVal(analogRead(MOISTURE),moisture_dry_value,moisture_wet_value,0,1000));

  //Serial Connection
  Serial.begin(115200);

  //Temperature Sensor begin  
  dht.begin();

  Config.apid = "THYME-Station";
  Config.title = "THYME-Station";
  Config.autoReconnect = true;
  Portal.config(Config);

  Server.on("/", rootPage);

  // Establish a connection with an autoReconnect option.
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }

  //WebSocket-Init
  // run callback when messages are received
   wsclient.onMessage(onMessageCallback);
    
  // run callback when events are occuring
   wsclient.onEvent(onEventsCallback);
    
  // Set SSL Certificate
  wsclient.setCACert(root_ca);
 
  // Connect to server
  wsclient.connect(websockets_server_host);
    

//Register Station or restore previous token

/* //Do not enable for testing

if(token==""){
    if(LITTLEFS.begin(true)){
      //LITTLEFS.remove(FILE_PATH); // --> only for testing purposes
      //if opening the file was successful
      if(LITTLEFS.exists(TOKEN_PATH)){
        File token_file = LITTLEFS.open(TOKEN_PATH);
        while(token_file.available()){
            //read the token from the file 
            token=token+char(token_file.read());
         }
      }
      else{ 
        File token_file = LITTLEFS.open(TOKEN_PATH,"w");
        Serial.println("Station needs to be registered.");
        
        //register the Station via API and write Auth-Token to a file
        token = registerStation(); 
          
          if(!token_file){
            Serial.println("There was an error opening the token file for writing");
          }else{
            if(token_file.print(token)){
              Serial.println("File was written");
            } else {
              Serial.println("File write failed");
            }
          }

          token_file.close();
      }       
  }
}   */


  //check for configuration update
  stationconfig=get_station_config();

  //Finalize HTTP Authentification 
  auth = base64::encode(station_id + ":" + token);  

  //Set Interrupt for control button
  //attachInterrupt(BUTTON,detected_button_pressed,RISING);

  
  //Register Websocket Connection
  DynamicJsonDocument wsregdoc(1024);
  wsregdoc["id"] = station_id_int;
  wsregdoc["token"]= token;

  // Serialize JSON document
  String wssrequestdata="";
  serializeJson(wsregdoc, wssrequestdata);
      
  //Serial.println("WSS-RequestData:\n"+wssrequestdata);
  wsclient.send(wssrequestdata);

  //Start Scheduler Instance
  runner.startNow();

  //Turn LED from Blue to Green (Setup completed)
  analogWrite(Led_Blue,0);
  analogWrite(Led_Green,255);
}


void loop() {
  //Execute Scheduler
  runner.execute();
  
  //Wifi-Connection Handler
  Portal.handleClient();
  //Serial.println(WiFi.localIP());

  //WebSocket - Handling 
  wsclient.poll();

Serial.println(digitalRead(TANK));
  //water level detection:
  if(digitalRead(TANK)==0){
    delay(500);
    if(digitalRead(TANK)==1){
      Serial.println("Tank is empty!");
      sendSensorData();
      update_station_status("empty");
    }
  }
  if((station_state=="empty")&&digitalRead(TANK)==0){
    update_station_status("idle");
  }

  if(digitalRead(BUTTON)==HIGH){
      activate_pump();
  }

  delay(300);
}