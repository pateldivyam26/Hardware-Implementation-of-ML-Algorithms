# [Weather Prediction Using Arduino Nano 33 BLE Sense Board](https://github.com/pateldivyam26/Hardware-Implementation-of-ML-Algorithms/tree/main/Weather%20Prediction)

This repository contains the implementation of Weather Prediction using Arduino Nano 33 BLE Sense board.

## Requirements

- Arduino Nano 33 BLE Sense board
- USB cable
- Arduino IDE
- Python environment with scikit-learn, numpy, and pandas libraries
- Weather data for training the model
- Computer or laptop with Bluetooth capability

## Installation

1. Clone the project repository to your local machine.
2. Install the Arduino IDE and Python environment on your computer.
3. Connect the Arduino Nano 33 BLE Sense board to your computer via the USB cable.
4. Open the Arduino IDE and load the `Weather_Prediction.ino` sketch from the Arduino directory.
5. Upload the sketch to the board.
6. Open the `Weather_Prediction.ipynb` file.
7. Train the machine learning model using the weather dataset.
8. Connect the Arduino Nano 33 BLE Sense board to your computer via Bluetooth.

## Challenges Faced

During the development of this project, we encountered several challenges:

- **Hardware compatibility:** Initially, we planned to use the Arduino Mega 2560 board, but it required external sensors for temperature, humidity, and pressure measurements. Switching to the Arduino Nano 33 BLE Sense board, which has built-in sensors, made the task easier.
- **Library compatibility:** We faced issues with library compatibility when using the Arduino Nano 33 BLE Sense Rev2 board. We had to modify the code and resolve errors by changing the library for temperature and humidity measurements from HTS221 to HS300x.
- **Memory constraints:** The Arduino Nano 33 BLE Sense board has limited memory, which posed a challenge when deploying deep learning models. We optimized the code and reduced the model's size to fit within the device's memory constraints.
- **Real-time prediction:** Ensuring real-time prediction on the Arduino board required code optimization and reducing the inference time to provide accurate predictions.

For more details, please refer to the [project report](https://github.com/pateldivyam26/Hardware-Implementation-of-ML-Algorithms/blob/main/B20EE082_B20CS079_REPORT.pdf) included in this repository.

Demo: https://drive.google.com/file/d/1vIARap5UC5B6_v0u4I6HDvQCfVVt4T0C/view?usp=sharing
