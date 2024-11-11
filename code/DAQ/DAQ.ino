#include <SD.h>
#include <SPI.h>

#include "HX711.h"

// pre-defined macro
#define file "test.txt"  // 檔案名稱，可以直接從這修改
#define MAX 100

// ******** Declaration of global variables **********
File myFile;
HX711 scale;
unsigned long time;
float t;
const float scale_factor = 14.6;  // 比例參數，從校正程式中取得
// ************* Declaration of functions ***********
void logging();
void setup_calibration();
void setup_logging();
void setup_sdcard();
float read_pt();
void read_sd();

void setup() {
  Serial.begin(115200);

  pinMode(A1, INPUT);
  analogReference(INTERNAL);

  setup_logging();
}

void loop() {
  logging();
}

void logging() {
  time = millis();

  scale.power_up();
  float weight = scale.get_units(1);
  t = (float)time / 1000.0;
  Serial.print(t, 4);
  Serial.print(", ");
  Serial.print(weight, 4);
  Serial.print(", ");
  Serial.println(read_pt(), 4);
  scale.power_down();

  //-------------------------------------------------------

  // if (myFile) {
  //   // Serial.print("Writing to test.txt...");
  //   myFile.print(t);
  //   myFile.print(", ");
  //   myFile.print(weight);
  //   myFile.print(", ");
  //   myFile.println(read_pt());

  //   // close the file:
  //   myFile.close();
  // } else {
  //   // if the file didn't open, print an error:
  //   Serial.println("error opening test.txt");
  // }
}

void setup_sdcard() {
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(file, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("type today's date here");
    myFile.println("start from here --------------------------------------");

    // close the file:
    myFile.close();

    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void setup_logging() {
  // reset time
  time = 0;

  Serial.println("Initializing the scale");
  // scale.begin(5,4);
  scale.begin(3, 2);
  Serial.println("Before setting up the scale:");

  Serial.println(scale.get_units(5), 0);  // 未設定比例參數前的數值

  scale.set_scale(scale_factor);  // 設定比例參數
  scale.tare();                   // 歸零

  Serial.println("After setting up the scale:");

  Serial.println(scale.get_units(5), 0);  // 設定比例參數後的數值

  Serial.println("Readings:");  // 在這個訊息之前都不要放東西在電子稱上
}

void setup_calibration() {
  scale.begin(5, 4);  // DT_PIN = 5, SCK_PIN = 4
  scale.set_scale();  // 開始取得比例參數
  scale.tare();
  Serial.println("Nothing on it.");
  Serial.println(scale.get_units(10));
  Serial.println("Please put sapmple object on it...");  // 提示放上基準物品
}

float read_pt() {
  float raw = analogRead(A1);
  float Max = MAX;
  float Min = 1;
  float V1R1_actual = 1.1;
  float Max_ADC = 0.02 * 47 / V1R1_actual * 1023;
  float Bar_1_Value = 174.84;
  float slope = float(Max - Min) / float(Max_ADC - Bar_1_Value);
  float pressure = (Min + slope * (raw - Bar_1_Value)) * 1000.0;
  return pressure;
}

void read_sd() {
  // re-open the file for reading:
  myFile = SD.open(file);
  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
