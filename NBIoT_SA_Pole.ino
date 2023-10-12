#include "SriLinkDevBoard.h"
#include <String>

#include <SoftwareSerial.h>

SriLinkDevBoard devboard;

Broker TB_Broker;
bool connected;

//Broker data
#define mqtt_server "fazenda.nbiot.sltdigitallab.lk" //MQTT Broker IP address
#define mqtt_port 1883   //MQTT Broker Port

#define device_ID "033"
// MQTT pubish topic 
#define MQTT_topic "slt_agro21/data/id_0033/lc_033"

//Modbus data
#define PIN_RX 32 //RO
#define PIN_TX 14 //DI
#define DE 33         
SoftwareSerial mod(PIN_RX,PIN_TX);

const byte test[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x07, 0x04, 0x08}; 

byte readings[20];
float avgReading[7];

struct SoilData {
  String t;
  String m;
  String c;
  String ph;
  String n;
  String p;
  String k;
  bool w;
};
volatile bool timer_flag=false;
//Timer Interrupt ISR
void IRAM_ATTR datasendtimer(){
   timer_flag=true;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initModBus();
  // Interrupt time interval in seconds 
  config_Timer(600);
  Serial.println("Start...");


}

void loop() {


  if(timer_flag){
    // Collect soil data 
    SoilData d = getAvgReadings_struct();
    if(!d.w){
      Serial.println("Faild");
       
    }
    else{
      String Data = "{\'T\':26.5,\'H\':70.0,\'ST\':"+d.t+",\'SM\':"+d.m+",\'SEC\':"+d.c+",\'SPH\':"+d.ph+",\'SN\':"+d.n+",\'SP\':"+d.p+",\'SK\':"+d.k +",\'R\':200,\'B\':7.5,\'SI\':2}";
      
      if(devboard.Init(115200)){
        Serial.println("initialization suceessful...");
        Serial.println("waiting for connection");
        long starttime = millis();
        do{
          Serial.print(".");
          delay(5);
        }while((devboard.IS_ATTACH()==false) && ((millis()-starttime)<60000));
        if((millis()-starttime)<60000){
          Serial.println(" ");
          Serial.println("connection succesful");
          //if(devboard.GET_IP())
          delay(100);
          if(devboard.MQTT_SETUP(&TB_Broker, "mqtt.thingsboard.cloud","1883"))
          {
            delay(200);
            if(devboard.MQTT_CONNECT(&TB_Broker, "device_ID"))
            {
              Serial.println("MQTT Connect successful");
              
              uint8_t val = devboard.MQTT_PUB(&TB_Broker, MQTT_topic, Data, Data.length(), 0, false, false, false);
              Serial.print(val);


            }else{
              Serial.println("MQTT Login fail");
            }
          }else{
            Serial.println("New Connection Fail");
          }
        }else{
          Serial.println("Connection time out");
        }
      }else{
        Serial.println("initializartion fail...");
      }
    }
  }
}
