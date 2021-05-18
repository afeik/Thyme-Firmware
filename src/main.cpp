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
  

//Station Name / ID
  int station_id_int=1234567804;
  String station_id = String(station_id_int);
  String station_name="TestStation123";

//Authentification Declarations
  String token= "Token for TestStation"; 
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

//Moisture - Sensor Calibration - Values 
  int moisture_wet_value=1321; //in water
  int moisture_dry_value=3422; //in air 
  //Buffer to store moisture values
  CircularBuffer<int, 30> moisture_buffer;

// RGB LED 
  int Led_Red = 4;
  int Led_Green = 2;
  int Led_Blue = 0;

//DS18B20 - Temperature Sensor
  //Setup a oneWire instance to communicate with any OneWire devices
  OneWire oneWire(TEMPERATURE);

  //Pass our oneWire reference to DS18B20 Temperature sensor 
  DallasTemperature sensors(&oneWire);

//Task Scheduler 
Scheduler runner;
//watering process
bool currently_watering=false;

//including additional functions for handling Wifi/Web-Requests
  #include "WifiConnection.h"
  #include "WebCommunication.h"
  
//Periodic Tasks
  Task PingWebsocket(10000,TASK_FOREVER,&ws_ping_callback,&runner,true);
  Task Write_MoistureValQueue(30000,TASK_FOREVER,&moisture_queue_callback,&runner,true);
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
  
  //Status-LED and Pump
  pinMode(VENTIL,OUTPUT);
  pinMode(PUMP,OUTPUT);
  pinMode(Led_Red, OUTPUT); 
  pinMode(Led_Green, OUTPUT); 
  pinMode(Led_Blue, OUTPUT); 
  
  //Set initial Values
  digitalWrite(VENTIL,LOW);
  digitalWrite(PUMP,LOW);
  moisture_buffer.push(mapVal(analogRead(MOISTURE),moisture_dry_value,moisture_wet_value,0,1024));

  //Serial Connection
  Serial.begin(115200);

  //Temperature Sensor begin  
  sensors.begin();
  sensors.requestTemperatures();
  // Enable saved past credential by autoReconnect option,
  // even once it is disconnected.

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

//Do not enable for testing

/* if(token==""){
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
        File token_file = LITTLEFS.open(TOKEN_PATH);
        Serial.println("Station needs to be registered.");
        
        //register the Station via API and write Auth-Token to a file
        token = registerStation();
        token_file = LITTLEFS.open(FILE_PATH, "w");   
          
          if(!token_file){
            Serial.println("There was an error opening the file for writing");
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
} */  


  //check for configuration update
  stationconfig=get_station_config();

  //Finalize HTTP Authentification 
  auth = base64::encode(station_id + ":" + token);  

  //Set Interrupt for Hall Sensor and control button
  attachInterrupt(TANK, detected_tank_empty, FALLING);
  attachInterrupt(BUTTON,detected_button_pressed,RISING);
  
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

  //watering process
  /*   if(moisture_value<stationconfig["moisture_threshold"]){
    activate_pump();
  }  */
  delay(100);
}