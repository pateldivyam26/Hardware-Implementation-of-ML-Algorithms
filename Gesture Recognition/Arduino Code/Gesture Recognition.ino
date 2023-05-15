#include "Arduino_BMI270_BMM150.h"
#include <TensorFlowLite.h>
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>

#include "model.h"

const float accelerationThreshold = 2.5; // Acceleration threshold value under gravitational conditions
const int numSamples = 119;  // The number of samples which we want to consider

int samplesRead = numSamples;

// All global variables used for TensorFlow Lite (Micro)
tflite::MicroErrorReporter tflErrorReporter;  // TFlite error reporter for debugging

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::AllOpsResolver tflOpsResolver;   

const tflite::Model* tflModel = nullptr; // Initialize final Model instance
tflite::MicroInterpreter* tflInterpreter = nullptr; // Initialize Interpreter final Model instance
TfLiteTensor* tflInputTensor = nullptr; // Input tensor Instance
TfLiteTensor* tflOutputTensor = nullptr; // Output tensor Instance

// Creating a static memory buffer for TFLM, 
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));

// array to map gesture index to a name
const char* GESTURES[] = {
  "punch 👊👊👊👊👊",
  "slash 🤚🤚🤚🤚🤚",
  "stab 🔪🔪🔪🔪🔪",
  "upper 👐👐👐👐👐",
  "flex 💪💪💪💪💪"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // initialize the IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // print the Accelerometer samples rate and Gyroscope sample rate of the IMU at initialization
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.print("Gyroscope sample rate = ");
  Serial.print(IMU.gyroscopeSampleRate());
  Serial.println(" Hz");

  Serial.println();

  // Load the model byte array into TFL represenatation
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!"); // check for model mismatch
    while (1);
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // memory allocation for input and output tenosrs
  tflInterpreter->AllocateTensors();

  // Getting access pointers for input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);
}

void loop() {
  float aX, aY, aZ, gX, gY, gZ;
  float xGyro, yGyro, zGyro;

  // wait for any external motion
  while (samplesRead == numSamples) {
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable())
     {
      // read the acceleration data
      IMU.readAcceleration(aX, aY, aZ); // Read real time acceleration readings 
      IMU.readGyroscope(xGyro, yGyro, zGyro); // Read real time Gyroscope readings

      // take the sum of the absolute values of acc.
      float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

      // checking for threshold
      if (aSum >= accelerationThreshold) {
        // reset the sample read count
        samplesRead = 0;
        break;
      }
    }
  }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples) {
    // check if new acceleration AND gyroscope data is available
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      // read the acceleration and gyroscope data
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);

      // normalize the IMU data between 0 to 1 and store in the model's
      // input tensor
      tflInputTensor->data.f[samplesRead * 6 + 0] = (aX + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 1] = (aY + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 2] = (aZ + 4.0) / 8.0;
      tflInputTensor->data.f[samplesRead * 6 + 3] = (gX + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 4] = (gY + 2000.0) / 4000.0;
      tflInputTensor->data.f[samplesRead * 6 + 5] = (gZ + 2000.0) / 4000.0;

      samplesRead++;

      if (samplesRead == numSamples) {
        // Run inferencing
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk) {
          Serial.println("Invoke failed!");
          while (1);
          return;
        }
        float maximum_value = 0.0;
        int max_index = 0;
        // Loop through the output tensor values from the model
        for (int i = 0; i < NUM_GESTURES; i++) {

            if(tflOutputTensor->data.f[i]>maximum_value){
                  maximum_value = tflOutputTensor->data.f[i];
                  max_index = i;
            }
        }
          Serial.print(GESTURES[max_index]);
          Serial.print(":");
          Serial.println(tflOutputTensor->data.f[max_index], 6);
        Serial.println();
      }
    }
  }
}
