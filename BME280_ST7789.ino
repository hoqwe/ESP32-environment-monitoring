/*
 * Monitoring temperature, humidity, and pressure using an ESP32 with a
 * BME280 sensor and an ST7789 display showing three round gauges.
 */

#include <Adafruit_BME280.h>
#include <TFT_eSPI.h>

Adafruit_BME280 bme;
TFT_eSPI tft = TFT_eSPI();

// Colors
constexpr uint16_t BG = TFT_BLACK;
constexpr uint16_t T_ARC_FG = TFT_RED;
constexpr uint16_t H_ARC_FG = TFT_SKYBLUE;
constexpr uint16_t P_ARC_FG = TFT_LIGHTGREY;

// Gauge value bounds
constexpr int8_t T_MIN = 0, T_MAX = 50;  // Temperature, *C
constexpr uint8_t H_MIN = 0, H_MAX = 100;  // Humidity, %
constexpr uint16_t P_MIN = 960, P_MAX = 1020;  // Pressure, hPa

// Character sizes of the default Adafruit GLCD font
constexpr uint8_t FONT_SIZE = 3;
constexpr uint8_t CHAR_HEIGHT = 8 * FONT_SIZE;
constexpr uint8_t CHAR_WIDTH = 6 * FONT_SIZE;

constexpr uint8_t ALPHA = 128;
uint16_t T_ARC_BG, H_ARC_BG, P_ARC_BG;

// Arc geometry
constexpr uint8_t ARC_RADIUS = 55;
constexpr uint8_t ARC_THICKNESS = 10;
constexpr uint8_t ARC_INNER_RADIUS = ARC_RADIUS - ARC_THICKNESS;

constexpr uint16_t ARC_START_DEG = 45;
constexpr uint16_t ARC_END_DEG = 315;
constexpr uint16_t ARC_SWEEP_DEG = ARC_END_DEG - ARC_START_DEG;

// Various tracking
float tPrev = 0.0f, hPrev = 0.0f, pPrev = 0.0f;
uint8_t unitsDrawn = 0;


void setup() {
    bme.begin(0x76);

    tft.begin();
    tft.setRotation(2);
    tft.fillScreen(BG);
    tft.setTextSize(FONT_SIZE);
    tft.setTextDatum(MC_DATUM);

    // Arc background colors
    T_ARC_BG = tft.alphaBlend(ALPHA, T_ARC_FG, BG);
    H_ARC_BG = tft.alphaBlend(ALPHA, H_ARC_FG, BG);
    P_ARC_BG = tft.alphaBlend(ALPHA, P_ARC_FG, BG);
}


void loop() { 
    float t = bme.readTemperature();
    float h = bme.readHumidity();
    float p = bme.readPressure() / 100.0f;

    drawGauge(t, tPrev, T_MIN, T_MAX, "\xF7""C", 60, 60, T_ARC_FG, T_ARC_BG);
    drawGauge(h, hPrev, H_MIN, H_MAX, "%", 180, 60, H_ARC_FG, H_ARC_BG);
    drawGauge(p, pPrev, P_MIN, P_MAX, "hPa", 120, 180, P_ARC_FG, P_ARC_BG);

    tPrev = t, hPrev = h, pPrev = p;
    delay(1000);
}


void drawGauge(float value, float valuePrev, uint16_t minValue,
               uint16_t maxValue, String unit, uint8_t x, uint8_t y,
               uint16_t arcFg, uint16_t arcBg) {
    // Erase old value area if needed
    uint8_t valueWidth = String(value, 2).length() * CHAR_WIDTH;
    uint8_t prevValueWidth = String(valuePrev, 2).length() * CHAR_WIDTH;

    if (valueWidth < prevValueWidth)
        tft.fillRect(x - prevValueWidth/2, y - CHAR_HEIGHT/2, prevValueWidth,
                     CHAR_HEIGHT, BG);

    // Draw arc
    float fillPercentage = (value - minValue) / (maxValue - minValue);
    fillPercentage = constrain(fillPercentage, 0.0f, 1.0f);

    if (fillPercentage <= 0.001f)
        // Draw full background arc
        tft.drawSmoothArc(x, y, ARC_RADIUS, ARC_INNER_RADIUS, ARC_START_DEG,
                          ARC_END_DEG, arcBg, BG, true);

    else if (fillPercentage >= 0.999f)
        // Draw full foreground arc
        tft.drawSmoothArc(x, y, ARC_RADIUS, ARC_INNER_RADIUS, ARC_START_DEG,
                          ARC_END_DEG, arcFg, BG, true);

    else {
        // Draw parts of background and foreground arcs
        float fillAngle = ARC_START_DEG + ARC_SWEEP_DEG*fillPercentage;
        tft.drawSmoothArc(x, y, ARC_RADIUS, ARC_INNER_RADIUS, fillAngle,
                          ARC_END_DEG, arcBg, BG, true);
        tft.drawSmoothArc(x, y, ARC_RADIUS, ARC_INNER_RADIUS, ARC_START_DEG,
                          fillAngle, arcFg, BG, true);
    }

    // Draw value
    tft.drawString(String(value, 2), x, y);

    // Draw unit (only once)
    if (unitsDrawn < 3) {
        tft.setTextSize(FONT_SIZE - 1);
        tft.drawString(unit, x, y + CHAR_HEIGHT);
        tft.setTextSize(FONT_SIZE);
        ++unitsDrawn;
    }
}
