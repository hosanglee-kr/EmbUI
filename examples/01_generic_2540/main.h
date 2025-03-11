/**
 * EmbUI 예제: 01_generic
 *
 * EmbUI 프레임워크를 사용하여 기본적인 웹 인터페이스를 구축하는 방법을 보여줍니다.
 * 이 예제는 메뉴, 데모 페이지, LED 제어 및 텍스트 입력을 포함합니다.
 *
 * 주요 기능:
 * - 메인 프레임 섹션: 웹 인터페이스의 기본 구조를 설정합니다.
 * - 메뉴 블록: 다른 섹션으로 이동하는 메뉴를 제공합니다.
 * - 데모 페이지: LED 제어 및 텍스트 입력 데모를 제공합니다.
 * - 액션 핸들러: LED 상태 변경 및 폼 데이터 처리를 담당합니다.
 * - 파라미터 생성: 필요한 변수와 액션을 EmbUI에 등록합니다.
 */
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#include "EmbUI.h"
// #include "interface.h"
#include "basicui.h"

// #include "uistrings.h"

EmbUI embui;

// LED 제어를 위한 핀 정의 (ESP32의 경우 LED_BUILTIN 사용 가능)
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

// UI 문자열 정의
enum UI_STRINGS {
    T_DEMO,
    T_SET_DEMO
};

// 구성 변수 정의
enum CONFIG_VARS {
    V_LED,
    V_VAR1,
    V_VAR2
};

// UI 문자열 매핑
const char *T_DICT[][2] = {
    {"Demo", "데모"},
    {"Set Demo", "데모 설정"},
};

// 언어 설정 (0: 영어, 1: 한국어)
int lang = 0;

// 메뉴 블록 생성
void block_menu(Interface *interf, JsonObjectConst data, const char *action);

// 데모 페이지 생성
void block_demopage(Interface *interf, JsonObjectConst data, const char *action);

// LED 상태 변경 액션 핸들러
void action_blink(Interface *interf, JsonObjectConst data, const char *action);

// 데모 페이지 데이터 처리 액션 핸들러
void action_demopage(Interface *interf, JsonObjectConst data, const char *action);

// 메인 프레임 섹션 생성
void section_main_frame(Interface *interf, JsonObjectConst data, const char *action) {
    if (!interf) return;

    interf->json_frame_interface();
    interf->json_section_manifest("EmbUI Example", embui.macid(), 0, "v1.0");
    interf->json_section_end();

    block_menu(interf, data, NULL);
    interf->json_frame_flush();

    if (WiFi.getMode() & WIFI_MODE_STA) {
        block_demopage(interf, data, NULL);
    } else {
        LOG(println, "UI: 네트워크 설정 페이지 열기");
        basicui::page_settings_netw(interf, {});
    }
}

// 메뉴 블록 생성
void block_menu(Interface *interf, JsonObjectConst data, const char *action) {
    if (!interf) return;
    interf->json_section_menu();
    interf->option(T_DEMO, T_DICT[lang][T_DEMO]);
    basicui::menuitem_settings(interf);
    interf->json_section_end();
}

// 데모 페이지 생성
void block_demopage(Interface *interf, JsonObjectConst data, const char *action) {
    interf->json_section_main(T_SET_DEMO, T_DICT[lang][T_SET_DEMO]);
    interf->comment("데모 컨트롤 세트");

    interf->checkbox(V_LED, embui.getConfig()[V_LED], "온보드 LED", true);
    interf->text(V_VAR1, embui.getConfig()[V_VAR1].as<JsonVariant>(), "텍스트 필드 1");
    interf->text(V_VAR2, "기본 값", "텍스트 필드 2");

    interf->button(button_t::submit, T_SET_DEMO, "전송", P_GRAY);
    interf->json_section_end();
    interf->json_frame_flush();
}

// LED 상태 변경 액션 핸들러
void action_blink(Interface *interf, JsonObjectConst data, const char *action) {
    bool state = data[V_LED];
    embui.getConfig()[V_LED] = state;
    digitalWrite(LED_BUILTIN, !state);
    Serial.printf("LED: %u\n", state);
}

// 데모 페이지 데이터 처리 액션 핸들러
void action_demopage(Interface *interf, JsonObjectConst data, const char *action) {
    if (!data) return;

    LOG(println, "데모 섹션 처리 중");

    embui.getConfig()[V_VAR1] = data[V_VAR1];
    embui.autosave();

    const char *text = data[V_VAR1];
    Serial.printf("변수 1 값: %s\n", text);

    text = data[V_VAR2];
    Serial.printf("변수 2 값: %s\n", text);
}

// 파라미터 생성 및 등록
void create_parameters() {
    LOG(println, "UI: 애플리케이션 변수 생성");

    embui.action.set_mainpage_cb(section_main_frame);
    embui.action.add(T_DEMO, block_demopage);
    embui.action.add(T_SET_DEMO, action_demopage);
    embui.action.add(V_LED, action_blink);
}

// 설정 초기화
void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    create_parameters();
    embui.begin();
}

// 메인 루프
void loop() {
    embui.handle();
}
