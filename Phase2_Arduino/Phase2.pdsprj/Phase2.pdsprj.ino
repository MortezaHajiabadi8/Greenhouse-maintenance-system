#include "DHT.h"
#define DHTPIN 2     
#define DHTTYPE DHT11 
#define FANPIN 3  
#define MAXTEMP 22
#define MAXHUM 60
#define MINHUM 40
#define WATERPIN 4
#define LDRPIN A0
#define LIGHTPIN 5
#define LIGHTLIMIT 250
#define BUZZERPIN 6
#define CHECKFAN A5
#define CHECKWATER A4
#define CHECKLIGHT A3

bool isFan = false;
bool isWater= false;
bool isLight = false;

float oldTemp = -1;
float oldHum = -1;
int oldLight = -1;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(FANPIN, OUTPUT);
  pinMode(WATERPIN, OUTPUT);
  pinMode(LIGHTPIN, OUTPUT);
  pinMode(BUZZERPIN, INPUT);
  pinMode(CHECKFAN, INPUT);
  pinMode(CHECKWATER, INPUT);
  pinMode(CHECKLIGHT, INPUT);
  
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  delay(2000);
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int l = analogRead(LDRPIN);
  

  if(isnan(t) || isnan(t)){
    Serial.println("Failed to read from DHT Sensor");
    buzzer();
    return;
  }
  if(l == 0){
    Serial.println("Failed to read from LDR Sensor");
    buzzer();
    return;
  }

  checkChanges(t, h, l);
  
  oldTemp = t;
  oldHum = h;
  oldLight = l;
  
  showSensorResult(t, h ,l);
  temperatureSensor(t);
  humiditySensor(h);
  ldrSensor(l);
 
Serial.println("------------------------------------------------------------------------------------");
 
}
//Functions
//----------------------------------------------------------------------------------------------------------------------------------
//CALCULATE TIME FOR FAN TO REACH TEMPERATURE TO 18.5 C
float calcFanTime(float temp){
  if(temp <= 22){
    return 0;
  }
  return (temp - 18.5)*1.5;
}
//CALCULATE TIME FOR WATER TO REACH HUMIDITY TO 50 %
float calcWaterTime(float hum){
  if(hum >= 60){
    return 0;
  }
  return ((50-hum)/3)*0.5;
}
//CALCULATE TIME FOR LIGHT TO REACH LDR TO 250
float calcLightTime(int ldr){
  if(ldr >250){
    return 0;
  }
  return ((300-ldr)/40)*10;
}

void buzzer(){
  tone(BUZZERPIN, 1000);
  delay(1000);
  noTone(BUZZERPIN);
}

void checkChanges(float t, float h, int l){
   if(h != oldHum){
    isWater= false;
  }
  if(t != oldTemp){
    isFan = false;
  }
  if(l != oldLight){
    isLight = false;
  }
}

void showSensorResult(float t, float h, int l){
  Serial.print("Result from sensors: ");
  Serial.print("Lighting : ");
  Serial.print(l);
  Serial.print(" ");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" C ");
  Serial.println();
}

bool checkFanWorking(float fanVoltage){
  if(fanVoltage < 100 && digitalRead(FANPIN) == HIGH){
    Serial.println("Fan Not Working, Fix Mistake!");
    return false;
  }
  return true;
}

bool checkWaterWorking(float waterVoltage){
  if(waterVoltage < 30 && digitalRead(WATERPIN) == HIGH){
    Serial.println("Water Not Working, Fix Mistake!");
    return false;
  }
  return true;
}

bool checkLightWorking(float lightVoltage){
  if(lightVoltage < 100 && digitalRead(LIGHTPIN) == HIGH){
    Serial.println("Light Not Working, Fix Mistake!");
    return false;
  }
  return true;
}

void temperatureSensor(float t){
  if(t > MAXTEMP && !isFan){
    float fanTime = calcFanTime(t);
    digitalWrite(FANPIN, HIGH);
    float fanVoltage = analogRead(CHECKFAN);
    if(!checkFanWorking(fanVoltage)){
      buzzer();
      return;
    }
    Serial.print("FAN ON for ");
    Serial.print(fanTime);
    Serial.println(" seconds ");
    delay(fanTime*1000);
    digitalWrite(FANPIN,LOW);
    Serial.print("FAN OFF AFTER FANNING! ");
    Serial.println(" Temperature is: 18.5 NOW ");
    isFan = true;
  }
  else if(t <= MAXTEMP || isFan){
    digitalWrite(FANPIN, LOW);
    Serial.println("FAN OFF  ");
  }
}

void humiditySensor(float h){
  if(h >= MINHUM || isWater){
    digitalWrite(WATERPIN, LOW);
    Serial.println("WATER OFF  ");
 }
 else if(h < MINHUM && !isWater){
    float waterTime = calcWaterTime(h);
    digitalWrite(WATERPIN, HIGH);
    float waterVoltage = analogRead(CHECKWATER);
    if(!checkWaterWorking(waterVoltage)){
      buzzer();
      return;
    }
    Serial.print("WATER ON for ");
    Serial.print(waterTime);
    Serial.println(" seconds ");
    delay(waterTime*1000);
    digitalWrite(WATERPIN,LOW);
    Serial.print("WATER OFF AFTER WATERING! ");
    Serial.println(" Humidity is: 50% NOW");
    isWater = true;
   }
}

void ldrSensor(int l){
  if(l < LIGHTLIMIT && !isLight){
    float lightTime = calcLightTime(l);
    digitalWrite(LIGHTPIN,HIGH);
    float lightVoltage = analogRead(CHECKLIGHT);
    if(!checkLightWorking(lightVoltage)){
      buzzer();
      return;
    }
    Serial.print("LIGHT ON for ");
    Serial.print(lightTime);
    Serial.print(" seconds ");
    delay(lightTime*1000);
    digitalWrite(LIGHTPIN,LOW);
    Serial.print("LIGHT OFF AFTER LIGHTING! ");
    Serial.println(" LDR is: 300 NOW");
    
    isLight = true;
  }
  else if(l>= LIGHTLIMIT || isLight){
    digitalWrite(LIGHTPIN,LOW);
    Serial.println("LIGHT OFF ");
}
}
