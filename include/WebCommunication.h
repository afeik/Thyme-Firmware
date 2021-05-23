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





//Certificate for SSL Communication

const char* root_ca="-----BEGIN CERTIFICATE-----\n"
"MIIFFjCCAv6gAwIBAgIRAJErCErPDBinU/bWLiWnX1owDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTA0MDAwMDAw\n"
"WhcNMjUwOTE1MTYwMDAwWjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n"
"RW5jcnlwdDELMAkGA1UEAxMCUjMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
"AoIBAQC7AhUozPaglNMPEuyNVZLD+ILxmaZ6QoinXSaqtSu5xUyxr45r+XXIo9cP\n"
"R5QUVTVXjJ6oojkZ9YI8QqlObvU7wy7bjcCwXPNZOOftz2nwWgsbvsCUJCWH+jdx\n"
"sxPnHKzhm+/b5DtFUkWWqcFTzjTIUu61ru2P3mBw4qVUq7ZtDpelQDRrK9O8Zutm\n"
"NHz6a4uPVymZ+DAXXbpyb/uBxa3Shlg9F8fnCbvxK/eG3MHacV3URuPMrSXBiLxg\n"
"Z3Vms/EY96Jc5lP/Ooi2R6X/ExjqmAl3P51T+c8B5fWmcBcUr2Ok/5mzk53cU6cG\n"
"/kiFHaFpriV1uxPMUgP17VGhi9sVAgMBAAGjggEIMIIBBDAOBgNVHQ8BAf8EBAMC\n"
"AYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMBIGA1UdEwEB/wQIMAYB\n"
"Af8CAQAwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYfr52LFMLGMB8GA1UdIwQYMBaA\n"
"FHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEBBCYwJDAiBggrBgEFBQcw\n"
"AoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzAnBgNVHR8EIDAeMBygGqAYhhZodHRw\n"
"Oi8veDEuYy5sZW5jci5vcmcvMCIGA1UdIAQbMBkwCAYGZ4EMAQIBMA0GCysGAQQB\n"
"gt8TAQEBMA0GCSqGSIb3DQEBCwUAA4ICAQCFyk5HPqP3hUSFvNVneLKYY611TR6W\n"
"PTNlclQtgaDqw+34IL9fzLdwALduO/ZelN7kIJ+m74uyA+eitRY8kc607TkC53wl\n"
"ikfmZW4/RvTZ8M6UK+5UzhK8jCdLuMGYL6KvzXGRSgi3yLgjewQtCPkIVz6D2QQz\n"
"CkcheAmCJ8MqyJu5zlzyZMjAvnnAT45tRAxekrsu94sQ4egdRCnbWSDtY7kh+BIm\n"
"lJNXoB1lBMEKIq4QDUOXoRgffuDghje1WrG9ML+Hbisq/yFOGwXD9RiX8F6sw6W4\n"
"avAuvDszue5L3sz85K+EC4Y/wFVDNvZo4TYXao6Z0f+lQKc0t8DQYzk1OXVu8rp2\n"
"yJMC6alLbBfODALZvYH7n7do1AZls4I9d1P4jnkDrQoxB3UqQ9hVl3LEKQ73xF1O\n"
"yK5GhDDX8oVfGKF5u+decIsH4YaTw7mP3GFxJSqv3+0lUFJoi5Lc5da149p90Ids\n"
"hCExroL1+7mryIkXPeFM5TgO9r0rvZaBFOvV2z0gp35Z0+L4WPlbuEjN/lxPFin+\n"
"HlUjr8gRsI3qfJOQFy/9rKIJR0Y/8Omwt/8oTWgy1mdeHmmjk7j1nYsvC9JSQ6Zv\n"
"MldlTTKB3zhThV1+XWYp6rjd5JW1zbVWEkLNxE7GJThEUG3szgBVGP7pSWTUTsqX\n"
"nLRbwHOoq7hHwg==\n"
"-----END CERTIFICATE-----";

long mapVal(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

String registerStation(){
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
  
      // Your Domain name with URL path or IP address with path
      http.begin("https://stomata.undertheprinter.com/v1/stations",root_ca);

      //POST REQUEST 
      // Specify content-type header
      http.addHeader("Content-Type", "application/json");
      // Data to send with HTTP POST
      // Prepare JSON document
      DynamicJsonDocument doc(2048);
      doc["id"] = station_id_int;
      doc["name"]= station_name;

      // Serialize JSON document
      String httpRequestData;
      serializeJson(doc, httpRequestData);
      Serial.println(httpRequestData);
      // Send HTTP POST
      int httpResponseCode = http.POST(httpRequestData);     
  
     if (httpResponseCode==200) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, http.getStream());
        String payload = http.getString();
        Serial.println(payload);
        return doc["token"];
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        return "";
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
      return "";
    }

}

DynamicJsonDocument get_station_config(){

  static DynamicJsonDocument current_config(1024);
  DynamicJsonDocument new_config(1024);
  DynamicJsonDocument current_config_buf(1024);
  String current_config_string="";  
  
  current_config_buf["watering_duration"]=3;
  current_config_buf["moisture_threshold"]=50;
  current_config_buf["moisture_sensor_wet"]=1321;
  current_config_buf["moisture_sensor_dry"]=3422;
  current_config_buf["station_count"]=1;

  serializeJson(current_config_buf,current_config_string);
  current_config_buf.clear();

  current_config["name"]=station_name;
  current_config["conf"]=current_config_string;

  current_config_string="";  
  serializeJson(current_config,current_config_string);
  bool config_request_valid=false;
  String new_config_string="";
        
  //Do the Web Request
  //HTTP GET - Get up-to-Date Config from Web-Server
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;

    // Config Domain 
    http.begin("https://stomata.undertheprinter.com/v1/stations/"+station_id,root_ca);

    //GET REQUEST 
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Basic " + auth);

    int httpResponseCode = http.GET();

    config_request_valid=false;
    //Response Handling
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      new_config_string = http.getString();
      Serial.println("New Config:"+new_config_string); 
      if(httpResponseCode==200||httpResponseCode==201){
        config_request_valid=true;
      }
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }

  new_config.clear();
  deserializeJson(new_config,new_config_string);

    //in case config is empty, initialize server with standard config
    String config_empty=new_config["conf"];
    if(config_empty==""){
      if(WiFi.status()== WL_CONNECTED){
        HTTPClient http;

        // Config Domain 
        http.begin("https://stomata.undertheprinter.com/v1/stations/"+station_id,root_ca);

        //GET REQUEST 
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "Basic " + auth);

        int httpResponseCode = http.PUT(current_config_string);

        //Response Handling
        if (httpResponseCode>0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
        }
          // Free resources
          http.end();
      }
      else {
        Serial.println("WiFi Disconnected");
      } 
    }


  if(LITTLEFS.begin(true)&&config_request_valid){
      //LITTLEFS.remove(FILE_PATH); //--> only for testing purposes
      //if opening the file was successful
      
      if(LITTLEFS.exists(FILE_PATH)){
          File stationdata1 = LITTLEFS.open(FILE_PATH,"r");
          //Serial.println(stationdata1.readString());
          current_config_string="";
          while(stationdata1.available()){
            current_config_string+=char(stationdata1.read());
          }

          stationdata1.close();
          deserializeJson(current_config,current_config_string);
          Serial.println("Current Config: "+current_config_string+"\n");
          

          //update File if needed
          if(current_config_string!=new_config_string){
            current_config=new_config;
            //Serial.println("Old Config removed? - "+bool(LITTLEFS.remove(FILE_PATH)));

            File stationdata = LITTLEFS.open(FILE_PATH, "w");  
            if(!stationdata){
              Serial.println("There was an error opening the file for writing");
            }else{
              if(stationdata.print(new_config_string)){
                Serial.println("Old config removed - new config written to flash memory.\n");;
              } else {
                Serial.println("Writing new Config Failed 1");
              }
            }
            stationdata.close();
          }

      }else{
          File stationdata = LITTLEFS.open(FILE_PATH, "w");   
          if(!stationdata){
            Serial.println("There was an error opening the file for writing");
          }else{
            if(stationdata.print(new_config_string)){
              Serial.println("New Config was written to Flash Memory.\n");;
            } else {
              Serial.println("Writing new Config Failed");
            }  

          stationdata.close();      
          } 
      }
  }
  deserializeJson(current_config,current_config["conf"]);
  return current_config;
}


void update_station_status(String stat){
  station_state=stat;
  //HTTP PUT - Update Station Status after watering is complete
  if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      // Your Domain name with URL path or IP address with path
      http.begin("https://stomata.undertheprinter.com/v1/stations/"+station_id+"/state",root_ca);

      //PUT REQUEST 
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Authorization", "Basic " + auth);

      DynamicJsonDocument doc(2048);
      doc["state"] = stat;
      
      // Serialize JSON document
      String httpRequestData;
      serializeJson(doc, httpRequestData);
      Serial.println(httpRequestData);

      int httpResponseCode = http.PUT(httpRequestData);

     //Response Handling
     if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        /*String payload = http.getString();
        Serial.println(payload); */
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}


//initates the watering process (unit -> int seconds )
void activate_pump(){
  //Watering Process

  if(currently_watering==false){
    currently_watering=true;
    update_station_status("water");

    Serial.println("Watering started!");
    digitalWrite(VENTIL,LOW);
    delay(200);
    digitalWrite(PUMP,LOW);
    delay(int(stationconfig["watering_duration"])*1000);
    //delay(10000);
    digitalWrite(PUMP,HIGH);
    delay(100);
    digitalWrite(VENTIL,HIGH); 

    update_station_status("idle");
    currently_watering=false;
  }
    
}


//Websocket - Connection 
const char* websockets_server_host = "wss://stomata-ws.undertheprinter.com/"; 

using namespace websockets;


//Callback for received Websocket Messages
void onMessageCallback(WebsocketsMessage msg) {

      if(msg.isText()){
      Serial.println("Got Message: " + msg.data());

      DynamicJsonDocument wsmessagedata(1024);

      deserializeJson(wsmessagedata, msg.data());
      
      String wsmessagedata_string="";

      deserializeJson(wsmessagedata, msg.data());
      if((wsmessagedata.containsKey("state")||wsmessagedata.containsKey("conf"))&&!msg.isPong()&&!msg.isPing()){
        //Serial.println(wsmessagedata["state"]);
        if(wsmessagedata["state"]=="water"){
          //wsmessagedata.remove("state");
          activate_pump();
        }
        if(wsmessagedata.containsKey("conf")){
          stationconfig=get_station_config();
        }
        //wsmessagedata_string=""; 
        wsmessagedata.clear();
      }
      wsmessagedata.clear();
    }
  
} 

//Create Websocket Instance
WebsocketsClient wsclient;


void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {

        Serial.println("Connnection Closed");
        //Try to Re-Register 
        //wsclient.connect(websockets_server_host);

        //Register Websocket Connection
        DynamicJsonDocument wsregdoc(1024);
        wsregdoc["id"] = station_id_int;
        //wsregdoc["name"]= station_name;
        wsregdoc["token"]= token;
        // Serialize JSON document
        String wssrequestdata="";
        serializeJson(wsregdoc, wssrequestdata);
        //wsclient.send(wssrequestdata);

    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void ws_reconnect_callback(){
    if(!wsclient.available()){
        //Try to Re-Register 
        wsclient.connect(websockets_server_host);
        //Register Websocket Connection
        DynamicJsonDocument wsregdoc(1024);
        wsregdoc["id"] = station_id_int;
        //wsregdoc["name"]= station_name;
        wsregdoc["token"]= token;
        // Serialize JSON document
        String wssrequestdata="";
        serializeJson(wsregdoc, wssrequestdata);
        wsclient.send(wssrequestdata);
    }

}

void ws_ping_callback(){
  wsclient.ping();
}


void moisture_queue_callback(){
  if(moisture_buffer.isFull()){
    moisture_buffer.shift();
  }
  int buf=analogRead(MOISTURE);
  buf= mapVal(buf,moisture_dry_value,moisture_wet_value,0,1000);
  if(buf>1000){
    buf=1000;
  }else if(buf<0){
    buf=0;
  }
  moisture_buffer.push(buf); 
}

void printBuffer() {
	if (moisture_buffer.isEmpty()) {
		Serial.println("empty");
	} else {
		Serial.print("[");
		for (decltype(moisture_buffer)::index_t i = 0; i < moisture_buffer.size() - 1; i++) {
			Serial.print(moisture_buffer[i]);
			Serial.print(",");
		}
		Serial.print(moisture_buffer[moisture_buffer.size() - 1]);
		Serial.print("] (");

		Serial.print(moisture_buffer.size());
		Serial.print("/");
		Serial.print(moisture_buffer.size() + moisture_buffer.available());
		if (moisture_buffer.isFull()) {
			Serial.print(" full");
		}

		Serial.println(")");
	}
}
//sends a JSON Document with the Sensor Data via a POST request request to the central database 
bool sendSensorData(){

//reading sensor values 
  //get the mean value of all the buffer elements
  moisture_value=0;
  using index_t = decltype(moisture_buffer)::index_t;
		for (index_t i = 0; i < moisture_buffer.size(); i++) {
			moisture_value += moisture_buffer[i];
		}
  moisture_value=moisture_value/moisture_buffer.size();


  printBuffer();
  tank_empty=!digitalRead(TANK);
  Serial.println(moisture_value);
  
  Serial.println(moisture_value);
  Serial.println(temperature_value);
  Serial.println(tank_empty);
  Serial.println(token);

  sensors.requestTemperatures(); 
  temperature_value = sensors.getTempCByIndex(0);


  //Sensor-JSON-Data
  DynamicJsonDocument doc(1024);
    doc["moisture"] = moisture_value;
    doc["temperature"] = temperature_value;
    doc["tank_empty"] = tank_empty;

 //Requests and Data Processing
    String json="";
    serializeJson(doc, json);
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      // Your Domain name with URL path or IP address with path
      http.begin("https://stomata.undertheprinter.com/v1/stations/"+station_id+"/data",root_ca);

      //POST REQUEST 
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Authorization", "Basic " + auth);

      int httpResponseCode = http.POST(json); 

     //Response Handling
     if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        /*String payload = http.getString();
        Serial.println(payload); */
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
        if(httpResponseCode==201||httpResponseCode==200){
          return 1;  
        }
        else{
          return 0;
        }
    }
    else {
      Serial.println("WiFi Disconnected");
      return 0;
    }

}

/* static bool interrupt_triggered=false;

void detected_tank_empty(){
    delay(200);
    if(interrupt_triggered==false){
      interrupt_triggered=true;
      Serial.println("Tank ist empty!");
      delay(2000);
      sendSensorData();
      update_station_status("empty");
      interrupt_triggered=false;
    }
        
} */


void detected_button_pressed(){
    digitalWrite(VENTIL,HIGH);
    delay(100);
    while(digitalRead(BUTTON)==1){
      digitalWrite(PUMP,HIGH);
    }
    digitalWrite(PUMP,LOW);
    delay(100);
    digitalWrite(VENTIL,LOW);
}

void send_sensordata_callback(){
  sendSensorData();
}
