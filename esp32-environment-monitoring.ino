/*
 * Monitoring temperature, humidity, and pressure using an ESP32 with a
 * BME280 sensor and an ST7789 display showing three round gauges.
 */

#include <Adafruit_BME280.h>
#include <TFT_eSPI.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include "./server/secrets.h"

Adafruit_BME280 bme;
TFT_eSPI tft = TFT_eSPI();

HTTPClient http;
const char* URL = "http://" SERVER_IP ":" SERVER_PORT "/data";

// Colors
constexpr uint16_t BG = TFT_BLACK;
constexpr uint16_t GAUGE_T_FG = TFT_RED;
constexpr uint16_t GAUGE_H_FG = TFT_SKYBLUE;
constexpr uint16_t GAUGE_P_FG = TFT_LIGHTGREY;

constexpr uint8_t GAUGE_BG_ALPHA = 128;
uint16_t GAUGE_T_BG, GAUGE_H_BG, GAUGE_P_BG;

// Gauge value bounds
constexpr int8_t GAUGE_T_MIN = 0, GAUGE_T_MAX = 50;  // Temperature, *C
constexpr uint8_t GAUGE_H_MIN = 0, GAUGE_H_MAX = 100;  // Humidity, %
constexpr uint16_t GAUGE_P_MIN = 960, GAUGE_P_MAX = 1020;  // Press, hPa

// Character sizes of the default Adafruit GLCD font
constexpr uint8_t FONT_SIZE = 3;
constexpr uint8_t CHAR_HEIGHT = 8 * FONT_SIZE;
constexpr uint8_t CHAR_WIDTH = 6 * FONT_SIZE;

// Gauge geometry
constexpr uint8_t GAUGE_RADIUS = 55;
constexpr uint8_t GAUGE_BAR_THICKNESS = 10;
constexpr uint8_t GAUGE_INNER_RADIUS = GAUGE_RADIUS - GAUGE_BAR_THICKNESS;

constexpr uint16_t GAUGE_START_DEG = 45;
constexpr uint16_t GAUGE_END_DEG = 315;
constexpr uint16_t GAUGE_SWEEP_DEG = GAUGE_END_DEG - GAUGE_START_DEG;

// Various tracking
float tPrev = 0.0f, hPrev = 0.0f, pPrev = 0.0f;
uint8_t unitsDrawn = 0;

// Networking
char json[128];
uint16_t responseCode = 0;


void setup() {
    Serial.begin(115200);

    bme.begin(0x76);

    tft.begin();
    tft.setRotation(2);
    tft.fillScreen(BG);
    tft.setTextSize(FONT_SIZE);
    tft.setTextDatum(MC_DATUM);

    // Gauge background colors
    GAUGE_T_BG = tft.alphaBlend(GAUGE_BG_ALPHA, GAUGE_T_FG, BG);
    GAUGE_H_BG = tft.alphaBlend(GAUGE_BG_ALPHA, GAUGE_H_FG, BG);
    GAUGE_P_BG = tft.alphaBlend(GAUGE_BG_ALPHA, GAUGE_P_FG, BG);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
}


void loop() {
    float t = bme.readTemperature();
    float h = bme.readHumidity();
    float p = bme.readPressure() / 100.0f;

    drawGauge(t, tPrev, GAUGE_T_MIN, GAUGE_T_MAX, "\xF7""C", 60, 60,
              GAUGE_T_FG, GAUGE_T_BG);
    drawGauge(h, hPrev, GAUGE_H_MIN, GAUGE_H_MAX, "%", 180, 60,
              GAUGE_H_FG, GAUGE_H_BG);
    drawGauge(p, pPrev, GAUGE_P_MIN, GAUGE_P_MAX, "hPa", 120, 180,
              GAUGE_P_FG, GAUGE_P_BG);

    if (WiFi.status() == WL_CONNECTED) {
        http.begin(URL);
        http.addHeader("Content-Type", "application/json");

        snprintf(
            json,
            sizeof(json),
            "{\"temperature\":%.2f,\"humidity\":%.2f,\"pressure\":%.2f}",
            t, h, p
        );

        responseCode = http.POST(json);
        http.end();
    }
    Serial.printf("%.2f*C %.2f%% %.2fhPa %d\n", t, h, p, responseCode);

    tPrev = t, hPrev = h, pPrev = p;
    delay(1000);
}


void drawGauge(float value, float valuePrev, uint16_t gaugeMinValue,
               uint16_t gaugeMaxValue, const char *unit, uint8_t x, uint8_t y,
               uint16_t gaugeFg, uint16_t gaugeBg) {
    // Erase old value area if needed
    char valueBuf[16];
    char valuePrevBuf[16];

    snprintf(valueBuf, sizeof(valueBuf), "%.2f", value);
    snprintf(valuePrevBuf, sizeof(valuePrevBuf), "%.2f", valuePrev);

    uint8_t valueWidth = strlen(valueBuf) * CHAR_WIDTH;
    uint8_t prevValueWidth = strlen(valuePrevBuf) * CHAR_WIDTH;

    if (valueWidth < prevValueWidth)
        tft.fillRect(x - prevValueWidth/2, y - CHAR_HEIGHT/2, prevValueWidth,
                     CHAR_HEIGHT, BG);

    // Draw arc
    float fillPercentage = (value - gaugeMinValue) / (gaugeMaxValue
                                                      - gaugeMinValue);
    fillPercentage = constrain(fillPercentage, 0.0f, 1.0f);
    uint16_t fillAngle = GAUGE_START_DEG + GAUGE_SWEEP_DEG*fillPercentage;

    if (fillAngle == GAUGE_START_DEG)
        // Draw full background arc
        tft.drawSmoothArc(x, y, GAUGE_RADIUS, GAUGE_INNER_RADIUS,
                          GAUGE_START_DEG, GAUGE_END_DEG, gaugeBg, BG, true);

    else if (fillAngle == GAUGE_END_DEG)
        // Draw full foreground arc
        tft.drawSmoothArc(x, y, GAUGE_RADIUS, GAUGE_INNER_RADIUS,
                          GAUGE_START_DEG, GAUGE_END_DEG, gaugeFg, BG, true);

    else {
        // Draw parts of background and foreground arcs
        tft.drawSmoothArc(x, y, GAUGE_RADIUS, GAUGE_INNER_RADIUS, fillAngle,
                          GAUGE_END_DEG, gaugeBg, BG, true);
        tft.drawSmoothArc(x, y, GAUGE_RADIUS, GAUGE_INNER_RADIUS,
                          GAUGE_START_DEG, fillAngle, gaugeFg, BG, true);
    }

    // Draw value
    tft.drawString(valueBuf, x, y);

    // Draw unit (only once)
    if (unitsDrawn < 3) {
        tft.setTextSize(FONT_SIZE - 1);
        tft.drawString(unit, x, y + CHAR_HEIGHT);
        tft.setTextSize(FONT_SIZE);
        ++unitsDrawn;
    }
}
