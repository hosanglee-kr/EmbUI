/**
 * @file 02_sensors.ino
 * @brief EmbUI를 사용하여 DS18B20 및 DHT 센서 데이터를 웹 인터페이스에 표시하는 예제입니다.
 *
 * 기능 요약:
 * - DS18B20 및 DHT 센서로부터 온도 및 습도 데이터를 읽어옵니다.
 * - 읽어온 데이터를 시리얼 모니터에 출력합니다.
 * - EmbUI 웹 인터페이스를 통해 센서 데이터를 실시간으로 표시합니다.
 * - 센서 읽기 실패 시 에러 메시지를 표시합니다.
 * - 사용자 정의 웹 인터페이스를 설정합니다.
 * - uistrings.h 파일에 정의된 문자열을 사용하여 메모리 사용량을 최적화합니다.
 */
//

#include <Arduino.h>
#include <EmbUI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// 센서 핀 정의
#define ONE_WIRE_BUS 4 // DS18B20 센서가 연결된 핀
#define DHTPIN 5       // DHT 센서가 연결된 핀
#define DHTTYPE DHT11    // DHT 센서 타입 (DHT11 또는 DHT22)

// 센서 객체 생성
OneWire oneWire(ONE_WIRE_BUS);         // OneWire 객체 생성
DallasTemperature sensors(&oneWire); // DallasTemperature 객체 생성
DHT dht(DHTPIN, DHTTYPE);            // DHT 객체 생성

// 사용자 정의 인터페이스 관련 함수 선언
void setupInterface(); // 사용자 정의 인터페이스 설정 함수 선언
void handleButtonClick(); // 버튼 클릭 핸들러 함수 선언

// UI 문자열 정의 (uistrings.h 파일 내용 포함)
namespace Uistrings {
  // General
  static const char T_HEADLINE[] PROGMEM = "EmbUI Demo";    // 프로젝트 이름

  // Our variable names
  static const char V_LED[] PROGMEM = "vLED";              // LED 상태 변수 이름
  static const char V_VAR1[] PROGMEM = "v1";               // 변수 1 이름
  static const char V_VAR2[] PROGMEM = "v2";               // 변수 2 이름
  static const char V_UPDRATE[] PROGMEM = "updrt";         // 업데이트 속도 변수 이름

  // UI blocks
  static const char T_DEMO[] PROGMEM = "demo";             // "demo" UI 섹션 이름

  // UI handlers
  static const char T_SET_DEMO[] PROGMEM = "do_demo";      // "demo" 섹션 데이터 처리 핸들러 이름
  static const char T_SET_MORE[] PROGMEM = "do_more";      // "more" 섹션 데이터 처리 핸들러 이름
  static const char Temperature[] PROGMEM = "Temperature";  // 온도 문자열
  static const char Humidity[] PROGMEM = "Humidity";     // 습도 문자열
  static const char Button[] PROGMEM = "My Button";       // 버튼 문자열
  static const char ButtonStatus[] PROGMEM = "Button Status"; // 버튼 상태 문자열
}

/**
 * @brief EmbUI 파라미터를 생성합니다.
 *
 * EmbUI 시작 시 호출되어 웹 인터페이스에 표시할 파라미터를 정의합니다.
 */
void create_parameters() {
  EmbUI.addParam(FPSTR(Uistrings::Temperature), "N/A");  // 온도 파라미터 추가
  EmbUI.addParam(FPSTR(Uistrings::Humidity), "N/A");     // 습도 파라미터 추가
  EmbUI.addParam(FPSTR(Uistrings::ButtonStatus), "N/A"); // 버튼 상태 파라미터 추가
}

/**
 * @brief 사용자 정의 웹 인터페이스를 설정합니다.
 *
 * 웹 인터페이스에 버튼을 추가하고, 버튼 클릭 시 호출될 핸들러를 설정합니다.
 */
void setupInterface() {
  EmbUI.addButton(FPSTR(Uistrings::Button), handleButtonClick); // 버튼 추가 및 핸들러 설정
}

/**
 * @brief 버튼 클릭 시 호출되는 핸들러입니다.
 *
 * 버튼 클릭 시 시리얼 모니터에 메시지를 출력하고, EmbUI 파라미터를 업데이트합니다.
 */
void handleButtonClick() {
  Serial.println("Button Clicked!"); // 시리얼 모니터에 메시지 출력
  EmbUI.addParam(FPSTR(Uistrings::ButtonStatus), "Clicked"); // 버튼 상태 파라미터 업데이트
}

void setup() {
  Serial.begin(115200); // 시리얼 통신 시작
  EmbUI.begin();         // EmbUI 초기화
  create_parameters(); // EmbUI 파라미터 생성
  EmbUI.setPubInterval(0); // 데이터 전송 간격 설정 (0: 데이터 변경 시 즉시 전송)
  setupInterface();    // 사용자 정의 인터페이스 설정

  sensors.begin(); // DS18B20 센서 초기화
  dht.begin();     // DHT 센서 초기화
}

void loop() {
  EmbUI.update(); // EmbUI 업데이트

  sensors.requestTemperatures();          // DS18B20 센서 온도 요청
  float temperatureC = sensors.getTempCByIndex(0); // DS18B20 센서 온도 읽기

  float humidity = dht.readHumidity(); // DHT 센서 습도 읽기

  if (isnan(temperatureC) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!"); // 센서 읽기 실패 메시지 출력
    EmbUI.addParam(FPSTR(Uistrings::Temperature), "N/A");  // 온도 파라미터 "N/A"로 업데이트
    EmbUI.addParam(FPSTR(Uistrings::Humidity), "N/A");     // 습도 파라미터 "N/A"로 업데이트
  } else {
    Serial.print("Temperature: "); // 온도 출력
    Serial.print(temperatureC);
    Serial.print(" °C, Humidity: "); // 습도 출력
    Serial.print(humidity);
    Serial.println(" %");

    EmbUI.addParam(FPSTR(Uistrings::Temperature), String(temperatureC) + " °C"); // 온도 파라미터 업데이트
    EmbUI.addParam(FPSTR(Uistrings::Humidity), String(humidity) + " %");    // 습도 파라미터 업데이트
  }

  delay(2000); // 2초 지연
}
