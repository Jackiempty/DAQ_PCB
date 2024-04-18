# Loadcell Datalogging 開發日記

[![hackmd-github-sync-badge](https://hackmd.io/6wCwDJi5RnyODmf0SUcP3A/badge)](https://hackmd.io/6wCwDJi5RnyODmf0SUcP3A)  

[**Github link**](https://github.com/Jackiempty/Loadcell_PCB)
###### tags:`Avionics`

## 線路連接
![image](https://hackmd.io/_uploads/SJgJyJ1DKa.png)


## 程式碼
### 校正程式
```cpp
#include "HX711.h"

// HX711 接線設定
const int DT_PIN = 5;
const int SCK_PIN = 4;
const int sample_weight = 1;  //基準物品的真實重量(公克)

HX711 scale;

void setup() {
  Serial.begin(9600);
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale();  // 開始取得比例參數
  scale.tare();
  Serial.println("Nothing on it.");
  Serial.println(scale.get_units(10));
  Serial.println("Please put sapmple object on it..."); //提示放上基準物品
  
}

void loop() {
  float current_weight=scale.get_units(10);  // 取得10次數值的平均
  float scale_factor=(current_weight/sample_weight);
  Serial.print("Scale number:  ");
  Serial.println(scale_factor,0);  // 顯示比例參數，記起來，以便用在正式的程式中
  
}
```

### 紀錄程式
```cpp
#include <SPI.h>
#include <SD.h>
#include "HX711.h"

File myFile;
// const int DT_PIN = 5;
// const int SCK_PIN = 4;
const int scale_factor = 14; //比例參數，從校正程式中取得

HX711 scale;
unsigned long time;
float t;

void setup() {
  // reset time
  time = 0;

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  
  // Serial.println("Initializing the scale");
  scale.begin(5, 4);
  // Serial.println("Before setting up the scale:"); 
  
  // Serial.println(scale.get_units(5), 0);	//未設定比例參數前的數值

  scale.set_scale(scale_factor);       // 設定比例參數
  scale.tare();				        // 歸零

  // Serial.println("After setting up the scale:"); 

  // Serial.println(scale.get_units(5), 0);  //設定比例參數後的數值

  // Serial.println("Readings:");  //在這個訊息之前都不要放東西在電子稱上

  //-----------------------------------------------------------------------------

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  // Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    // Serial.print("Writing to test.txt...");
    
    myFile.println("testing 1, 2, 3."); // 這裡可以填上當天的日期，時間

    // close the file:
    myFile.close();
    // Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  // re-open the file for reading:
  
  // myFile = SD.open("test.txt");
  // if (myFile) {
  //   Serial.println("test.txt:");

  //   // read from the file until there's nothing else in it:
  //   while (myFile.available()) {
  //     Serial.write(myFile.read());
  //   }
  //   // close the file:
  //   myFile.close();
  // } else {
  //   // if the file didn't open, print an error:
  //   Serial.println("error opening test.txt");
  // }
}

void loop() {
  time = millis() / 100;
  if(time % 5 == 0)
  {
    // put your main code here, to run repeatedly:
    scale.power_up();               // 結束睡眠模式
    // Serial.println(scale.get_units(1),4); 
    float weight = scale.get_units(1);
    t = (float)time/10.0;
    Serial.print(t);
    Serial.print(", ");
    Serial.println(weight, 4);
    //避免出現負數
    if(weight<=0){
      weight = 0;
    }
 
    scale.power_down();			        // 進入睡眠模式

  //-------------------------------------------------------

    myFile = SD.open("test.txt", FILE_WRITE);
    if (myFile) {
      // Serial.print("Writing to test.txt...");
      myFile.print(t);
      myFile.print(": ");
      myFile.println(weight);

      // close the file:
      myFile.close();
      // Serial.println("done.");
    } 
    else 
    {
      // if the file didn't open, print an error:
      Serial.println("error opening test.txt");
    }
  }
}
```

## 校正步驟

1. 準備好你的法碼（可以是標準法碼也可以是你是先量好重量的東西，eg:手機、水壺）
2. 將 Loadcell 本體接上接口，按照下圖（使用方法那邊） 
4. 打開校正程式，在程式的起始處填上你的法碼重量並上傳
5. 開啟 Serial Monitor
6. 會有數值一直跳出來
7. 取連續十位取平均

**補充**

7. 打開測量程式，並在程式起始處填上你的校正參數
8. 上傳後再放上法碼檢驗你的參數
9. 如果得到的結果稍有落差的話，你可以
* 在正負五之間做調整，並多驗證幾次
* 如果落差都還是很大的話，建議從步驟2在重來一次

如果驗證的結果沒問題的話

**恭喜你得到了你的校正參數了🎉**


## 使用方法
1. 將電腦接上 Arduino Nano 板
2. 將 Loadcell 本體接上接口，按照下圖

![image](https://hackmd.io/_uploads/Hk6MZwpYa.png)

4. 先個別將記憶卡拿下來清除內容再裝回去
5. 打開測量程式，將你的校正參數打在程式起始處
6. 上傳
7. 打開 Serial Monitor 和 Serial Plotter

* Serial Monitor  
![image](https://hackmd.io/_uploads/ByFIvlkha.png)  
在 Serial Monitor 我們所看到的數字分別是  
左：時間  
右：重量讀值  
* Serial Plotter  
![image](https://hackmd.io/_uploads/H1sVvxk2p.png)  
在 Serial Plotter 中，因為它畫圖的方法是把所有值都給予編號，並以編號為橫軸，讀值為縱軸列出  
所以這個圖的判讀法就是固定橫軸的值之後得到的兩個縱軸數值就是時間和重量，而非橫軸為時間，縱軸為重量  
## 洞洞板焊接結果
<img src="https://hackmd.io/_uploads/rJo_NF5j6.png" width="350"/> <img src="https://hackmd.io/_uploads/HJj_4F5sp.png" width="350"/>

這算是我焊的第一塊洞洞板，中間歷經了使用 AWG30 的線、焊錯要解焊、換使用有橡膠套的線，最後花了總時長大概四小時左右將它焊完。

經過測試，是可以運作的，沒有短路的情況發生，算是一個好的開始  
下一步應該就是要開始設計集成電路板了，也是一個我沒做過的事

## 外殼
用 Solidworks 簡單畫了一個外殼，除了有仔細算過整個總成的 Dimension 並畫出可以 fit 整片洞洞板上去，還在殼上弄出一些洞可以穿線路，最後還設計了一個可以卡上去的蓋子  

![image](https://hackmd.io/_uploads/Hy2IO4Ria.png)  

在計算總成的 Dimension 時也順便把它畫出來以方便比對外殼的大小有沒有畫對  

![image](https://hackmd.io/_uploads/SJTLcV0ja.png)  


# PCB 開發紀錄
作完了自己把所有元件焊在一片板子上之後，更進一步整合就是要作 PCB 了

## Schematic
### ver1.0
![image](https://hackmd.io/_uploads/SkswSY89p.png)  

### ver1.5
![image](https://hackmd.io/_uploads/rydkfFReC.png)  

### Update
:::info
:information_source: Update  
更 1.1 版：  
由於原本的 usb-serial 轉換器 FT232RL 聽說不好買  
在組長的建議下我換成 CH340C，之後便修改了 RX/TX 的 LED 線路位置  
以及原本 SD_Card Module 所使用的 3.3V 換成了 5V

更 1.2 版：  
我將原本存在於 Arduino nano 功能中的 VCC 轉 5V 功能給拿掉了，原本 VCC 是外接電源轉給內部 5V 使用，拿掉之後讓供電的工作交給，USB_C 就好了，省掉一些不必要的線路  
另外，原本以為是熱敏電阻的 RV1 其實是 Fuse，所以進行修正，現在是用 Schotty Diode 替代，變成 D3  

更 1.3 版：  
RESET 電路的上拉電阻畫錯了，進行修正，連同 PCB 一起  
另外還加上了 M3 鎖點  

更 1.4 版：  
將多餘的線路刪掉（VBUS、SS1P3L），並優化電容配置  

更 1.5 版：  
將 HX711 上的 rate 腳位做成上拉電阻，以增加可以改變運作頻率的選擇  

更 1.6 版：  
增加一個額外的 UART 通道，可以外接一個 GPS 算降落傘的 CD (Coefficient Drag)
:::

## Footprint
![image](https://hackmd.io/_uploads/Sy2mRd0xR.png)  


Footprint 的選擇其實有遇到一些困難  
比較有問題的幾個分別是
* SS1P3L
* 74125
* HX711
* CH340C

等，幾個比較特定功能的  
由於我這是第一次做這個工作，所以對於 Package 的辨識以及選擇較缺乏經驗  
所以花了一點時間研究和問問題  

總之目前的配置大概是這樣，**作為讀者的你如果有發現任何問題請不吝賜教**

## PCB Layout (ver1.3)
**Refer to sch ver1.5**  
:::info
:information_source: Update  
ver1.1 導了圓角  
ver1.2 將有銳角的線路修鈍，並做些小優化  
ver1.3 參照 sch ver1.5  
ver1.4 參照 sch ver1.6 並將圓角推至角落改善美觀  
:::
![image](https://hackmd.io/_uploads/SJNsQY0xR.png)  

![image](https://hackmd.io/_uploads/H1OOXYAl0.png)  

## 待更新：
- [x] HX711： Rate 腳位要做 either 上拉 or 下拉  
- [x] 增加 UART 接口  
- [ ] 修改程式碼，改進 data logging 的品質  