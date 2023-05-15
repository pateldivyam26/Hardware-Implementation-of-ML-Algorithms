#include <EloquentTinyML.h>
#include <eloquent_tinyml/tensorflow.h>
#include "model.h"
#include <Arduino_HS300x.h>
#include <Arduino_LPS22HB.h>

#define NUMBER_OF_INPUTS 3
#define NUMBER_OF_OUTPUTS 5
#define TENSOR_ARENA_SIZE 8*1024

Eloquent::TinyML::TensorFlow::TensorFlow<NUMBER_OF_INPUTS, NUMBER_OF_OUTPUTS, TENSOR_ARENA_SIZE> ml;

void setup() {
  Serial.begin(9600);
  ml.begin(model);
  Serial.println("Communication started");
  if (!HS300x.begin()) {
    Serial.println("Failed to initialize Humidity and Temperature sensor!");
    while (1);
  }
  if (!BARO.begin()) {
    Serial.println("Failed to initialize Pressure sensor!");
    while (1);
  }
}

void loop() {
  float temperature = HS300x.readTemperature();
  float humidity = HS300x.readHumidity();
  float pressure = BARO.readPressure() * 10;

  float input[NUMBER_OF_INPUTS] = {temperature, humidity, pressure};
  float output[NUMBER_OF_OUTPUTS] = {0, 0, 0, 0, 0};
  ml.predict(input, output);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C   Humidity: ");
  Serial.print(humidity);
  Serial.print("%   Pressure: ");
  Serial.print(pressure);
  Serial.println("kPa");
  Serial.print("Predicted Weather: ");

  float max_val=0;
  int max_ind=0;
  for(int i = 0; i<5; i++){
      if(output[i] > max_val){
          max_val = output[i];
          max_ind = i; 
      }
  }
  
  if(max_ind == 0){
     Serial.print("Sunny  ");
     
   }
   else if(max_ind == 1){
     Serial.print("Partly cloudy  ");
   }
   else if(max_ind == 2){
     Serial.print("Cloudy  ");
   }
   else if(max_ind == 3){
     Serial.print("Overcast  ");
   }
   else if(max_ind == 4){
     Serial.print("Patchy rain possible  ");
   }
   Serial.print("  Probability: ");
   Serial.println(max_val);
  delay(1000);
}
