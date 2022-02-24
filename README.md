# Sub-G Solar Power Asset Tracker
<div align="center">
<img src="./asset/main_image.png">
</div>
<br>
<table>
<tr>
<tr align="center">
  <td>TOP</td>
  <td>BOTTOM</td>
</tr>
  <tr align="center">
    <td><img src="./asset/Asset_tracker_PCB_TOP_FLAT.jpeg"></td>
    <td><img src="./asset/Asset_tracker_PCB_BTM_FLAT.jpeg"></td>
  </tr>
</table>

-------------------------

Sub-G Solar Power Asset Tracker 은 AxDen 의 Aggregator Platform 를 이용하여 온도, 가속도, GPS 위치와 같은 Asset tracking 에 필요한 핵심적인 정보를 수집하고 태양광 충전을 이용하여 배터리를 충전합니다.
<br>
1Km 의 이상의 장거리 통신 또는 2G/3G/4G 통신이 필요한 다양한 서비스 시나리오를 빠르게 테스트 할 수 있도록 제공하는 예제입니다.
<br>
<br>
관련 하드웨어 키트는 네이버 스마트 스토어에서 구매 가능합니다.
<br>
[구매링크 : 네이버 스마트 스토어](https://smartstore.naver.com/axden)
<br>
<br>
Sub-G Solar Power Asset Tracker uses AxDen's Aggregator Platform to collect key information required for asset tracking such as temperature, acceleration, and GPS location.
<br>
This is an example that provides a quick test for various service scenarios that require long-distance communication of 1Km or more or 2G/3G/4G communication.
<br>
<br>
Related hardware kits can be purchased from the Naver Smart Store.
<br>
[Purchase Link : naver smart store](https://smartstore.naver.com/axden)
<br>
<br>
## Sub-G Solar Power Asset Tracker 의 주요 특징 및 기능

MCU | Description
:-------------------------:|:-------------------------:
CC1312R1 | TI ROTS, EasyLink

*\* TI Sensor Controller 이용한 저전력 기술은 Production version 제품군에서 확인하실 수 있습니다.*
<br>
*\* Low-power technology with TI Sensor Controller is available in the Production version family.*

Sensor | Description
:-------------------------:|:-------------------------:
BMA400 | 3-Axis acceleration sensor
SI7051 | temperature sensor
L70 | GPS sensor
NEO-M8N | GPS sensor
BG95/96 | 4G (LTE-M, Cat-M1)
SARA-U2 | 2G, 3G
SPV1050 | Solar battery charger (Max charge current 80mA)
Solar | On board
Battery | 3.7V Lithium Battery

Sub-G Solar Power Asset Tracker 예제는 온도, 가속도, GPS 위치와 같은 Asset tracking 에 필요한 핵심적인 정보를 수집하고 태양광 충전을 이용하여 배터리를 충전합니다.
<br>
Sub-G 또는 2G/3G/4G 통신으로 수집된 정보를 전송합니다.
<br>
<br>
AxDen Aggregator Platform 과 연동하여 서버, DB 와 같은 인프라 구축 없이 Web 과 Mobile 에서 센서 정보를 확인합니다.
<br>
<br>
AxDen Aggregator Platform 에 저장된 센서 정보를 이용하여 Edge AI 를 학습시킵니다.
<br>

### 터미널을 이용한 확인 방법
어댑터 보드가 있다면 터미널을 통해 통신 확인이 가능합니다.
<br>
어덥테 보드를 PC 에 연결합니다.

### 서버를 이용한 확인 방법
TCP 서버가 있다면 서버를 통해 확인이 가능합니다.
<br>
해당 예제에는 AxDen 에서 제공하는 예제 서버의 IP 와 Port 번호가 임시로 지정되어 있습니다.
<table>
  <tr align="center">
    <td>RF RX Sub-G example terminal</td>
    <td>RF RX 2G/3G/4G Server example log</td>
  </tr>
  <tr align="center">
    <td><img src="./asset/RX_Sub_G.png"></td>
    <td><img src="./asset/Server_Log.png"></td>
  </tr>
</table>

### Solar battery charge 확인 방법
아래 이미지와 같이 멀티미터를 이용하여 태양광을 이용한 배터리 충전 전류량을 확인할 수 있습니다.
<img src="./asset/CharCurrent.png">

### AxDen Aggregator Platform 을 이용한 확인 방법
AxDen Aggregator 홈페이지에서 회원 가입 후 기기의 MAC Address 를 등록합니다.
<br>
AxDen Aggregator 홈페이지에서 제공하는 COMPANY ID, DEVICE ID 를 Protocol.h 파일의 COMPANY_ID, DEVICE_ID 에 입력합니다.
<br>
<br>
`#define COMPANY_ID 0`
<br>
`#define DEVICE_TYPE 0`
<br>
<br>
컴파일 후 플래싱을 합니다.
<br>
<br>
COMPANY_ID, DEVICE_ID 가 정상적으로 적용되었는지 확인합니다.
<br>
<br>
아래 이미지처럼 센서 정보를 Web 또는 Mobile 에서 확인할 수 있습니다.
<br>
<br>
<img src="./asset/GPS_Log.png">

### 서버 변경
bg96.c 파일의 `bg96_tcp_data_upload` Function 의 `set_bg96_socket_connect` Function 에서 IP, Port 변경이 가능합니다.
<br>
sara_u2.c 파일의 `sara_u2_tcp_data_upload` Function 의 `set_sara_socket_connect` Function 에서 IP, Port 변경이 가능합니다.

### 센서 교체
Protocl.h 파일에서 센서 교체가 가능합니다.
<br>
```
#define BG96
#define SARA_U2

#define QUECTEL_GPS
#define UBLOX_GPS
````
BG95/96 사용 시 -> #define SARA_U2 / 주석처리
<br>
아래 이미지와 동일하게 .syscfg 파일에서 UART Pin 설정
<br>
<img src="./asset/BG96_UART_Setup.png">
<br>
<br>
SARA-U2 사용 시 -> #define BG96 / 주석처리
<br>
아래 이미지와 동일하게 .syscfg 파일에서 UART Pin 설정
<br>
<img src="./asset/SARA_U2_UART_Setup.png">
<br>
<br>
L70 GPS 사용 시 -> #define UBLOX_GPS / 주석처리
<br>
<br>
NEO-M8N 사용 시 -> #define QUECTEL_GPS / 주석처리

### Hardware 핀맵
board_define.h 파일에서 확인 가능합니다.

### [펌웨어 설정 및 컴파일](https://github.com/AxDen-Dev/CC1312R1_Ping_Pong_example_gcc)

### [펌웨어 플래싱](https://github.com/AxDen-Dev/CC1312R1_Ping_Pong_example_gcc)
