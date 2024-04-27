#include <SD.h>
#include <SPI.h>

#include "HX711.h"

// pre-defined macro
#define file "test.txt"  // 檔案名稱，可以直接從這修改

// Declaration of global variables
File myFile;
HX711 scale;
unsigned long time;
float t;
int mode;
const int scale_factor = 14;  // 比例參數，從校正程式中取得
const int sample_weight = 1;  // 基準物品的真實重量(公克)
bool cal_first;
bool log_first;

void calibration();
void logging();
void setup_calibration();
void setup_logging();
void setup_sdcard();

void read_sd();

void setup() {
  Serial.begin(115200);
  mode = 1;

  setup_logging();
  setup_sdcard();
  cal_first = false;
  log_first = true;
}

void loop() {
  if (Serial.available() > 0) {
    mode =
        Serial.parseInt();  // set mode to either calibration(0) or logging(1)
  }

  if (mode == 0) {
    if (cal_first == true) {
      setup_calibration();
      cal_first = false;
      log_first = true;
    }
    calibration();
  } else {
    if (log == true) {
      setup_logging();
      setup_sdcard();
      cal_first = true;
      log_first = false;
    }
    logging();
  }
}

void calibration() {
  float current_weight = scale.get_units(10);  // 取得10次數值的平均
  float scale_factor = (current_weight / sample_weight);
  // 顯示比例參數，記起來，以便用在正式的程式中
  Serial.print("Scale number:  ");
  Serial.println(scale_factor, 0);
}

void logging() {
  time = millis();

  float weight = scale.get_units(1);
  t = (float)time / 1000.0;
  Serial.print(t);
  Serial.print(", ");

  Serial.println(weight, 4);
  Serial.println(" ");

  //-------------------------------------------------------

  if (myFile) {
    // Serial.print("Writing to test.txt...");
    myFile.print(t);
    myFile.print(": ");
    myFile.println(weight);

    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
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
  scale.begin(5, 4);
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
