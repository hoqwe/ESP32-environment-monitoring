/*
 * Monitoring temperature, humidity, and pressure using an ESP32 with a
 * BME280 sensor and an ST7789 display with three round gauges.
 */

#include <Adafruit_BME280.h>
#include <TFT_eSPI.h>

Adafruit_BME280 bme;
TFT_eSPI tft = TFT_eSPI();

// Font
const int fontSize = 3;
const int charHeight = 8 * fontSize;
const int charWidth  = 6 * fontSize;

// Colors
const int bg = 0x0000;
const int alpha = 128;
const int tArcFg = 0xF800, tArcBg = tft.alphaBlend(alpha, tArcFg, bg);
const int hArcFg = 0x867D, hArcBg = tft.alphaBlend(alpha, hArcFg, bg);
const int pArcFg = 0xD69A, pArcBg = tft.alphaBlend(alpha, pArcFg, bg);

// Arc geometry
const int r = 55, arcThickness = 10;
const int ir = r - arcThickness;
const int startAngle = 45, endAngle = 315;
const int arcDegrees = endAngle - startAngle;

float tPrev = 0.0f, hPrev = 0.0f, pPrev = 0.0f;
int unitsDrawn = 0;


void setup() {
    bme.begin(0x76);

    tft.begin();
    tft.setRotation(2);
    tft.fillScreen(bg);
    tft.setTextSize(fontSize);
    tft.setTextDatum(MC_DATUM);
}


void loop() { 
    float t = bme.readTemperature();
    float h = bme.readHumidity();
    float p = bme.readPressure() / 100.0f;

    drawGauge(t, tPrev, 0, 50, "\xF7" "C", 60, 60, tArcFg, tArcBg);
    drawGauge(h, hPrev, 0, 100, "%", 180, 60, hArcFg, hArcBg);
    drawGauge(p, pPrev, 960, 1020, "hPa", 120, 180, pArcFg, pArcBg);

    tPrev = t, hPrev = h, pPrev = p;
    delay(1000);
}


void drawGauge(float value, float valuePrev, int minValue, int maxValue,
               String unit, int x, int y, int arcFg, int arcBg) {
    // Erase old value area if needed
    int valueWidth = String(value, 2).length() * charWidth;
    int prevValueWidth = String(valuePrev, 2).length() * charWidth;

    if (valueWidth < prevValueWidth)
        tft.fillRect(x - prevValueWidth/2, y - charHeight/2, prevValueWidth,
                     charHeight, bg);

    // Draw arc
    float fillPercentage = (value - minValue) / (maxValue - minValue);
    fillPercentage = constrain(fillPercentage, 0.0f, 1.0f);

    if (fillPercentage <= 0.001f)
        // Draw full background arc
        tft.drawSmoothArc(x, y, r, ir, startAngle, endAngle, arcBg, bg, true);

    else if (fillPercentage >= 0.999f)
        // Draw full foreground arc
        tft.drawSmoothArc(x, y, r, ir, startAngle, endAngle, arcFg, bg, true);

    else {
        // Draw parts of background and foreground arcs
        float fillAngle = startAngle + arcDegrees*fillPercentage;
        tft.drawSmoothArc(x, y, r, ir, fillAngle, endAngle, arcBg, bg, true);
        tft.drawSmoothArc(x, y, r, ir, startAngle, fillAngle, arcFg, bg, true);
    }

    // Draw value
    tft.drawString(String(value, 2), x, y);

    // Draw unit (only once)
    if (unitsDrawn < 3) {
        tft.setTextSize(fontSize - 1);
        tft.drawString(unit, x, y + charHeight);
        tft.setTextSize(fontSize);
        ++unitsDrawn;
    }
}
