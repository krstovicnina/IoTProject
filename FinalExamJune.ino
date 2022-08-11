
#include <TimerOne.h>

#define TEMP A0
#define PHR A1

const int LED_RELAY = 13;
const int DC_RELAY = 12;
const int HEATER_RELAY = 11;
const int SECURITY = 2;
                                          
float illumination[10], temperature[10];  //array that stores data and takes the average measurment (decreases variance)
int illuminationCounter, temperatureCounter; //counters for collecting data that will be insterted into arrays
boolean measureTemperature, measureIllumination; 
boolean autoLight, secureMode, autoTemp, movementDetected, notifiedSecurity, canChangeLed;
int ledStatus, dcStatus, heaterStatus;
int pauseCounter;
int securityCounter;
int almDuration, smDuration;

float averageMeasurement(float *measurements, int counter) { //function that calculates the average of measurements found in the measurment arrays
  float sum=0;
  for (int i=0; i<counter; i++){
    sum+=measurements[i];
    measurements[i]=0;
  }
  return sum/float(counter);
}

void smartHouse(){
  
    if(autoLight){
      almDuration++;
    }
    if(measureIllumination){
      illumination[illuminationCounter]=analogRead(A1);
      illuminationCounter++;
      
      if(illuminationCounter==5){ //five measurments are taken before the calculation of the average
        float averageIllumination = averageMeasurement(illumination, illuminationCounter);
      
        Serial.print("DATA_ILLU_");  //The format of data that is sent to Python is DATA_TYPE_VALUE
        Serial.print(averageIllumination);
        Serial.print("\n");

        illuminationCounter = 0; //resetting the counter

        measureIllumination = false; //  this is to ensure that illumination won't be measured until it's time

        if(averageIllumination<306 && autoLight){
          if(ledStatus == LOW){
            Serial.println("NOTI_LED_ON_ALM");
            ledStatus = HIGH;
            digitalWrite(13,ledStatus);
          }
        } else if(ledStatus == HIGH && autoLight){
            Serial.println("NOTI_LED_OFF_ALM");
            ledStatus = LOW;
            digitalWrite(LED_RELAY,ledStatus);
        }
      }
    }
  
    if(measureTemperature){   //Same as for the light measurment, but awith the exception that autoTemp (hysterisis control) is always on
      temperature[temperatureCounter]=analogRead(TEMP)*0.489;
      temperatureCounter++;

      if(temperatureCounter==5){
        float averageTemperature = averageMeasurement(temperature,temperatureCounter);
        Serial.print("DATA_TEMP_");
        Serial.print(averageTemperature);
        Serial.print("\n");
      
      if(averageTemperature>=27 && autoTemp){  //activating cooling

        if(dcStatus == LOW){ //Cooling and heating cannot be on at the same time, so we can make do  with checking only one of them
          dcStatus = HIGH;
          heaterStatus = LOW;
          digitalWrite(DC_RELAY,dcStatus);
          digitalWrite(HEATER_RELAY,heaterStatus);
          Serial.println("NOTI_COOLING_ON");
          Serial.println("NOTI_HEATING_OFF");
        }
      }

        if(averageTemperature<27 && autoTemp){ //activating heating
          
          if(heaterStatus == LOW){
          dcStatus = LOW;
          heaterStatus = HIGH;
          digitalWrite(DC_RELAY,dcStatus);
          digitalWrite(HEATER_RELAY,heaterStatus);
          Serial.println("NOTI_COOLING_OFF");
          Serial.println("NOTI_HEATING_ON");
          }
        }

      temperatureCounter = 0; //resetting the counter
      measureTemperature = false; //pausing the measuring
    }
  }

  
  if(secureMode){
    smDuration++;
    if(movementDetected){

      if(!notifiedSecurity){  // Checking if the notificaton was already sent because we should notify only once per cycle of movement detection
        Serial.println("NOTI_MOTION_DETECTED");
        notifiedSecurity = true;
      }
      securityCounter++; //used for tracking if 10 seconds passed since the movement was detected 
      ledStatus = HIGH;
      digitalWrite(LED_RELAY, ledStatus);

      if(securityCounter/2 >= 10){ //true only after 10sec have passed 
        ledStatus = LOW;
        digitalWrite(LED_RELAY,ledStatus);
        movementDetected = false; //resetting
        notifiedSecurity = false; //resetting
        canChangeLed = true; // ensures that the light won'tbe turned off until 10sec pass
      }
    }
  }
  pauseCounter++; //increases regardless of measurments taken in order to ensure that data is sent periodically, at the same time

  if(pauseCounter/2 >= 10){
    measureTemperature = true;
    measureIllumination = true;
    pauseCounter = 0;
    Serial.print("DATA_DURATION_ALM_"); //send data for the duration of auto-light mode
    Serial.print(almDuration/2);
    Serial.print("\n");
    Serial.print("DATA_DURATION_SM_"); //send data for the duration of security mode
    Serial.print(smDuration/2);
    Serial.print("\n");
  }
}

void setup() {

  pinMode(LED_RELAY,OUTPUT);
  pinMode(DC_RELAY,OUTPUT);
  pinMode(HEATER_RELAY,OUTPUT);
  pinMode(SECURITY,INPUT);
  digitalWrite(LED_RELAY, LOW);
  digitalWrite(DC_RELAY, LOW);
  digitalWrite(HEATER_RELAY, LOW);
  
  Serial.begin(9600);
  Timer1.initialize(500000); //  half a second
  Timer1.attachInterrupt(smartHouse);
  
  illuminationCounter = 0;
  temperatureCounter = 0;
  pauseCounter = 100; //pause counter is set to a high number to ensure that the data is sent in the correct order
                      //this is especially important for updating thingspeak 
  securityCounter = 0;
  almDuration = 0;
  smDuration = 0;
  ledStatus = LOW;
  dcStatus = LOW;
  heaterStatus = LOW;
  autoLight = true; //set default to true
  secureMode = false; //set default to false
  movementDetected = false;
  measureTemperature = false; // set to false (in combination with pause control being a high number ensures the correct order of data sending)
  measureIllumination = false;
  autoTemp = true; //always true
  notifiedSecurity = false;
  canChangeLed = true; 
}

void loop() {
  
   if(digitalRead(SECURITY)==0 && secureMode){
    movementDetected = true;
    canChangeLed = false; 
    securityCounter = 0;
   }
   if(Serial.available() > 0) {
     String data = Serial.readString();
      if((data == "LED_ON" || data == "LED_ON\n") && canChangeLed){
        if(ledStatus == HIGH){
          Serial.println("NOTI_LED_WAS_ON"); 
        }else{
          Serial.println("NOTI_LED_IS_ON");
          ledStatus = HIGH;
          digitalWrite(LED_RELAY,ledStatus);
        }
        if(autoLight){
          autoLight = false;
          Serial.println("NOTI_ALM_OFF");
        }
        if(secureMode){
          secureMode = false;
          Serial.println("NOTI_SM_OFF");
        }
     }else if ((data == "LED_OFF" || data == "LED_OFF\n") && canChangeLed){
        if(ledStatus == LOW){
          Serial.println("NOTI_LED_WAS_OFF");
        }else{
          Serial.println("NOTI_LED_IS_OFF");
          ledStatus = LOW;
          digitalWrite(LED_RELAY,ledStatus);
        }
        if(autoLight){
          autoLight = false;
          Serial.println("NOTI_ALM_OFF");
        }
        if(secureMode){
          secureMode = false;
          Serial.println("NOTI_SM_OFF");
        }
     } else if (data == "AUTO_LIGHT_ON" || data == "AUTO_LIGHT_ON\n"){
        illuminationCounter = 0;
        autoLight = true;
        Serial.println("NOTI_ALM_ON");
        if(secureMode){
          secureMode = false;
          Serial.println("NOTI_SM_OFF");
        }
     } else if (data == "AUTO_LIGHT_OFF" || data == "AUTO_LIGHT_OFF\n"){
        autoLight = false;
        Serial.println("NOTI_ALM_OFF");
     } else if (data == "SECURE_MODE_ON" || data == "SECURE_MODE_ON\n"){
        secureMode = true;
        ledStatus = LOW;
        digitalWrite(LED_RELAY,ledStatus);
        Serial.println("NOTI_SM_ON");
        if(autoLight){
          autoLight = false;
          Serial.println("NOTI_ALM_OFF");
        }
     } else if (data == "SECURE_MODE_OFF" || data == "SECURE_MODE_OFF\n"){
        secureMode = false;
        Serial.println("NOTI_SM_OFF");
        ledStatus = LOW;
        digitalWrite(LED_RELAY,ledStatus);
     } else if (data == "HEAT_ON" || data == "HEAT_ON\n"){
        if(heaterStatus == HIGH){
          Serial.println("NOTI_HEATER_WAS_ON");
        }else{
          heaterStatus = HIGH;
          digitalWrite(HEATER_RELAY,heaterStatus);
          Serial.println("NOTI_HEATER_IS_ON");
          dcStatus = LOW;
          digitalWrite(DC_RELAY,dcStatus);
          Serial.println("NOTI_COOLING_IS_OFF");
      }
     } else if (data == "HEAT_OFF" || data == "HEAT_OFF\n"){
        if(heaterStatus == LOW){
          Serial.println("NOTI_HEATER_WAS_OFF");
        }else{
          heaterStatus = LOW;
          digitalWrite(HEATER_RELAY,heaterStatus);
          Serial.println("NOTI_HEATER_IS_OFF");
        }
     } else if (data == "COOLING_ON" || data == "COOLING_ON\n"){
        if(dcStatus == HIGH){
          Serial.println("NOTI_COOLING_WAS_ON");
        }else{
          heaterStatus = LOW;
          digitalWrite(HEATER_RELAY,heaterStatus);
          Serial.println("NOTI_HEATER_IS_OFF");
          dcStatus = HIGH;
          digitalWrite(DC_RELAY,dcStatus);
          Serial.println("NOTI_COOLING_IS_ON");
        }
     } else if (data == "COOLING_OFF" || data == "COOLING_OFF\n"){
        if(dcStatus == LOW){
          Serial.println("NOTI_COOLING_WAS_OFF");
        }else{
          dcStatus = LOW;
          digitalWrite(DC_RELAY,heaterStatus);
          Serial.println("NOTI_COOLING_IS_OFF");
        }
     }
   }
 }
