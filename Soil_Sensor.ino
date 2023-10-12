void initModBus(){
 mod.begin(9600);
 pinMode(DE, OUTPUT);
 delay(1000);
 readSensor();
 delay(1000);
 readSensor();
 delay(1000);
 readSensor();
 delay(1000);  
}

boolean readSensor(){
  digitalWrite(DE,HIGH);
  delay(100);
 // Serial.print("k1");

  if(mod.write(test,sizeof(test))==8){
    digitalWrite(DE,LOW);
    unsigned long timeoutStartTime = millis();
    byte bytesRead = 0;
    //Serial.print("k2");
    while ((bytesRead < 19)){//} && ((millis() - timeoutStartTime) < 000)) {
      
      if (mod.available()){
        //Serial.print("k");
        readings[bytesRead] = mod.read(); // Read data byte by byte
        
        bytesRead++;
      }
    }
  }else{
    //Serial.print("write error");
    return false;
  }
  if((readings[0]==1)&&(readings[1]==3)&&(readings[2]==14)){
   // Serial.print("true ");
    return true;
    }
  else{
   // Serial.print("fail ");
    return false;
  }
}

void printSensorDataHex(){
   for(byte i=0;i<19;i++){
     Serial.print(readings[i],HEX);
     Serial.print(" ");
    }
    Serial.println("");
}


void printTemp(){
int result = (readings[5]*256) + readings[6];
Serial.printf("Temp : %f \n",(float(result)/10));  
}


void printPH(){
int result = (readings[9]*256) + readings[10];
Serial.printf("PH : %f \n",(float(result)/10));  
}

void printMoisture(){
int result = (readings[3]*256) + readings[4];
Serial.printf("Moisture : %f \n",(float(result)/10));  
}

void printCon(){
int result = (readings[7]*256) + readings[8];
Serial.printf("Conductivity : %d \n",result);  
}

void printN(){
int result = (readings[11]*256) + readings[12];
Serial.printf("N : %f \n",(float(result)/10));  
}

void printP(){
int result = (readings[13]*256) + readings[14];
Serial.printf("P : %f \n",(float(result)/10));  
}

void printK(){
int result = (readings[15]*256) + readings[16];
Serial.printf("K : %f \n",(float(result)/10));  
}

float getTempe(){
int result = (readings[5]*256) + readings[6];
float temp=float(result)/10;
//Serial.printf("Temp : %f \n",temp);
return temp;  
}

float getMoisture(){
int result = (readings[3]*256) + readings[4];
float m=float(result)/10;
return m;  
}


float getPH(){
int result = (readings[9]*256) + readings[10];
float ph=float(result)/10;
//Serial.printf("Temp : %f \n",temp);
return ph;  
}

float getCon(){
int result = (readings[7]*256) + readings[8];
return float(result); 
}

float getN(){
int result = (readings[11]*256) + readings[12];
float n=float(result)/10;  
return n;
}

float getP(){
int result = (readings[13]*256) + readings[14];
float p=float(result)/10;
return p; 
}

float getK(){
int result = (readings[15]*256) + readings[16];
float k=float(result)/10;
return k;
}


void cleanAvgDataBuff(){
   avgReading[0]=0.0;
   avgReading[1]=0.0;
   avgReading[2]=0.0;
   avgReading[3]=0.0;
   avgReading[4]=0.0;
   avgReading[5]=0.0;
   avgReading[6]=0.0;
}

String getAvgReadings(){
  
  if(readSensor()){
        for(byte i=0; i<4; i++){
           delay(1000);
           if(readSensor()){
           avgReading[0]+=getTempe();
           avgReading[1]+=getMoisture();
           avgReading[2]+=getCon();
           avgReading[3]+=getPH();
           avgReading[4]+=getN();
           avgReading[5]+=getP();
           avgReading[6]+=getK(); 
           }
           else{
             return "";
            }
         }

         String d =String((float(avgReading[0])/4),2)+" "+String((float(avgReading[1])/4),1)+" "+String((float(avgReading[2])/4),1)+" "+String((float(avgReading[3])/4),1)+" "+String((float(avgReading[4])/4),1)+" "+String((float(avgReading[5])/4),1)+" "+String((float(avgReading[6])/4),1);
         cleanAvgDataBuff();
         if(((avgReading[0])/4)<60){
            return d;
          }
         else{
            return "";
          }
    }

  else{
      return "";
   }
}

void enableExternalPrep(){
  // pinMode(HARDWARE_SW_1,OUTPUT);
  // digitalWrite(HARDWARE_SW_1,HIGH);
   delay(100);
}

void disableExternalPrep(){
   //digitalWrite(HARDWARE_SW_1,LOW);
  // gpio_hold_en(GPIO_NUM_27);  
}

SoilData getAvgReadings_struct(){
  SoilData sData = {"","","","","","","",false};
  
  if(readSensor()){
    for(byte i=0; i<4; i++){
      delay(1000);
      if(readSensor()){
        avgReading[0]+=getTempe();
        avgReading[1]+=getMoisture();
        avgReading[2]+=getCon();
        avgReading[3]+=getPH();
        avgReading[4]+=getN();
        avgReading[5]+=getP();
        avgReading[6]+=getK(); 
      }
      else{
        #if(verbose_RS485==3)
          Serial.println("fail sensor while working");
        #endif
        return sData;
      }
    }

    sData.t=String((float(avgReading[0])/4),2);
    sData.m=String((float(avgReading[1])/4),1);
    sData.c=String((float(avgReading[2])/4),1);
    sData.ph=String((float(avgReading[3])/4),1);
    sData.n=String((float(avgReading[4])/4),1);
    sData.p=String((float(avgReading[5])/4),1);
    sData.k=String((float(avgReading[6])/4),1);
    //Serial.println(d);
    cleanAvgDataBuff();
    if(((avgReading[0])/4)<60){
      sData.w=true;
      return sData;
    }
    else{
      #if(verbose_RS485==3)
        Serial.println("data out of range");
      #endif
      return sData;
    }
  }
  else{
    #if(verbose_RS485==3)
      Serial.println("sensor failed at 1st reading");
    #endif
    return sData;
   }
}


 
