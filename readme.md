# DAQ Datalogging 開發日記

[![hackmd-github-sync-badge](https://hackmd.io/6wCwDJi5RnyODmf0SUcP3A/badge)](https://hackmd.io/6wCwDJi5RnyODmf0SUcP3A)  

[**Github link**](https://github.com/Jackiempty/Loadcell_PCB)
###### tags:`Avionics`

## 線路連接
![image](https://hackmd.io/_uploads/SJgJyJ1DKa.png =x800)


## 程式碼
### 校正程式
[Github 連結](https://github.com/Jackiempty/DAQ_PCB/blob/main/code/correction/correction.ino)

### 紀錄程式
[Github 連結](https://github.com/Jackiempty/DAQ_PCB/blob/main/code/DAQ/DAQ.ino)
:::warning
Reviewed by 仲其宇
1. 校正程式最好是可以跟紀錄程式放在一起，通過輸入命令來選擇是否要校正
2. 採樣率太低了。![image](https://hackmd.io/_uploads/BybIPhG-C.png)你的 dt 長達 0.1 秒，並且還要每隔 5 次才紀錄。發動機稍一次不過 1~2 秒，這樣採樣點才 4 個，基本無法分析。採樣率至少要到 80Hz。
3. ![image](https://hackmd.io/_uploads/rkV6uhzZR.png)
其實沒必要進入睡眠模式，我們沒有缺電，徒增資源浪費而已。
4. ![image](https://hackmd.io/_uploads/ryX9dnGW0.png)
不需要把時間單位轉成秒，從頭到尾用毫秒就好，因為我們需要高響應的結果。
5. ![image](https://hackmd.io/_uploads/Hk30_3fZC.png)不應該刪掉負數，雖然水平推測出現負數的機率很低。但這也代表如果出現負數，我們可以用來 debug。並且如果有諸如垂直推測的情況(雖然也是幾乎不太可能做)，負數就很重要了。
6. 因為是每紀錄一筆數據就存進 SD，所以有可能出現刷新速度不夠快的情況。因此要做取樣時間的確認，確保取樣率可以保持在 80Hz。<br>如果來不及的話就必須考慮不要每次都開檔、關檔，直到測完才關閉檔案。<br>但同時也會有意點風險，就是當發動機測到一半炸掉，檔案可能會沒存到。但這時用傳輸線回傳的資料就發揮冗於作用了。
7. 建議檔案名稱可以用 define 寫在最前面，方便測試者修改。
8. 程式太多沒用的註解碼了，不方便閱讀。如果有空的話，順便把註解寫寫。
9. 可以加入主動偵測發動機啟動紀錄及熄火停止的功能，方便數據分析。(為防止誤判損失資料，可以保留測試前後 1~10 秒的數據)
:::

### 修改內容  
:::info
reply by 簡誌加  

目前已修改 1~5 以及 7~8 的問題  
至於 6 的話會先進行每次都開關檔能否跑到 80Hz 的測試再決定去留  
:::

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

![image](https://hackmd.io/_uploads/Hy2IO4Ria.png =x300)  

在計算總成的 Dimension 時也順便把它畫出來以方便比對外殼的大小有沒有畫對  

![image](https://hackmd.io/_uploads/SJTLcV0ja.png =x300)  


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
![image](https://hackmd.io/_uploads/SJNsQY0xR.png =500x)  

![image](https://hackmd.io/_uploads/H1OOXYAl0.png =x500)  

## 待更新：
- [x] HX711： Rate 腳位要做 either 上拉 or 下拉  
- [x] 增加 UART 接口  
- [x] 修改程式碼，改進 data logging 的品質  
    
    
# PT
由於後來需要加上艙壓的測量，所以引進了 PT `(pressure transducer)` 的讀取功能進到板子裡，這裡先上 PT 的規格：  
* [**Datasheet**](https://www.yuden.com.tw/uploadfiles/423/Products/Catalog/eyc-huba-511_pressure_sensor-20190409.pdf)
* 種類：電流式
* 電流範圍：4 ~ 20 mA
* 壓力範圍：~ 100 bar
* 供電：8 ~ 33 VDC

## 使用方式
我們採取的作法是使用 Arduino 板子上的 ADC 作為我們讀取電壓的接口，由於 Arduino 的 ADC 能夠讀取的電壓範圍是 0 ~ 5 V，所以配合電流的範圍 4 ~ 20 mA 我們得到
$5V \div 0.02A = 250 \Omega$，於是我們便在 ADC 的讀取接口與地線之間串了 250 $\Omega$ 的電阻並讓 ADC 的讀值會介於 1 ~ 5 V 之間，並透過線性的對應關係就可以知道壓力的值為多少。  

然而，由於我們的供電沒辦法完全給夠 5V，所以最後的最高位參考電壓會變成是供電的電壓，也就是 4.5V，並且不穩定。為了讓參考電壓是穩定的，我們將 `analogRead()` 的參考電壓設為 1.1V，方法是透過在 `setup()` 裡面加上一段：
```cpp
void setup() {
    .......
    analogReference(INTERNAL);
    .......
}
```
詳細說明請見[這裡](https://www.arduino.cc/reference/en/language/functions/analog-io/analogreference/)

## 程式碼
不囉唆，直接上：
```cpp
float read_pt() {
  float raw = analogRead(A1);
  float Max = MAX;
  float Min = 1;
  float V1R1_actual = 1.1;
  float Max_ADC = 0.02 * 47 / V1R1_actual * 1023;
  float Bar_1_Value = 224;
  float slope = float(Max - Min) / float(Max_ADC - Bar_1_Value);
  float pressure = (Min + slope * (raw - Bar_1_Value)) * 1000.0;
  return pressure;
}
```

原理很簡單，讀取到的電壓為 1 ~ 1.1 V，對應到板子所顯示的數字是約 204 ~ 1023，所以程式碼裡面 `raw` 的範圍就會是 204 ~ 1023。但由於我們的電阻沒辦法剛剛好就是精準的 55 Ohms，所以我們的斜率實際上是要用 $\Delta$壓力 / $\Delta$角位讀值，才能夠精準地讓讀值跟我們所求的數值互相對應。  

剩下的就是線性對應的算式，沒什麼好解釋的，反正重點就是最後 `pressure` 這個變數就會是我們要求的氣壓值。  

## 測試結果
前幾天 7/4 的時候我有去柏漢的實驗室用那邊的鋼瓶給氣壓去測 PT 是否能正常運作，那時候我們上面接的電阻實際量出來約為 242，所以對應到前面程式碼的話 204 就會變成 197，除此之外都一樣，量出來的數值雖然會浮動，但整體的範圍還算正確。  

{%pdf https://doggy8088.github.io/A-Whirlwind-Tour-of-Python-zh-tw/ %}
