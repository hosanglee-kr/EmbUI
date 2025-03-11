/**
 * @file 02_sensors.ino
 * @brief EmbUI를 사용하여 DS18B20 온도 센서 및 DHT11 습도/온도 센서의 데이터를 웹 인터페이스에 표시하는 예제입니다.
 *
 * 기능 요약:
 * - DS18B20 및 DHT11 센서로부터 온도 및 습도 데이터를 읽어옵니다.
 * - 읽어온 데이터를 시리얼 모니터에 출력합니다.
 * - EmbUI 웹 인터페이스를 통해 센서 데이터를 실시간으로 표시합니다.
 * - 센서 읽기 실패 시 에러 메시지를 표시합니다.
 *
 * 상세 기능 설명:
 * - OneWire 및 DallasTemperature 라이브러리를 사용하여 DS18B20 센서를 제어합니다.
 * - DHT 라이브러리를 사용하여 DHT11 센서를 제어합니다.
 * - EmbUI 라이브러리를 사용하여 웹 인터페이스를 구축하고 센서 데이터를 표시합니다.
 * - 센서 데이터를 주기적으로 읽어와 업데이트합니다.
 * - 센서 읽기 오류 발생 시 "N/A"를 표시하여 사용자에게 알립니다.
 *
 * 누락된 로직 확인 및 추가:
 * - 센서 초기화 실패에 대한 별도 처리 로직은 원본에도 없으므로 추가하지 않음.
 * - 센서 데이터 유효성 검사 (NaN 체크) 로직은 원본에 존재하므로 유지.
 * - EmbUI 파라미터 업데이트 시 기존 값을 덮어쓰는 방식으로 작동하므로, 누락된 로직 없음.
 */

#include <Arduino.h>
#include <EmbUI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// DS18B20 센서 핀 정의
#define ONE_WIRE_BUS 4

// DHT11 센서 핀 정의
#define DHTPIN 5
#define DHTTYPE DHT11

// OneWire 및 DallasTemperature 객체 생성
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// DHT11 객체 생성
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // 시리얼 통신 시작
  Serial.begin(115200);

  // EmbUI 초기화
  EmbUI.begin();

  // DS18B20 센서 초기화
  sensors.begin();

  // DHT11 센서 초기화
  dht.begin();
}

void loop() {
  // EmbUI 업데이트
  EmbUI.update();

  // DS18B20 센서로부터 온도 데이터 읽기
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);

  // DHT11 센서로부터 습도 데이터 읽기
  float humidity = dht.readHumidity();

  // 센서 데이터 유효성 검사 및 처리
  if (isnan(temperatureC) || isnan(humidity)) {
    // 센서 읽기 실패 시 에러 메시지 출력 및 EmbUI에 "N/A" 표시
    Serial.println("Failed to read from DHT sensor!");
    EmbUI.addParam("Temperature", "N/A");
    EmbUI.addParam("Humidity", "N/A");
  } else {
    // 센서 데이터 시리얼 모니터에 출력
    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // EmbUI에 센서 데이터 표시
    EmbUI.addParam("Temperature", String(temperatureC) + " °C");
    EmbUI.addParam("Humidity", String(humidity) + " %");
  }

  // 2초 지연
  delay(2000);
}
