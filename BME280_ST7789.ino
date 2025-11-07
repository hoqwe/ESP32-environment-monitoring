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

// Arc
const int r = 55, arcWidth = 10;
const int ir = r - arcWidth;
const int startAngle = 45, endAngle = 315;
const int arcDegrees = endAngle - startAngle;

float tPrev = 0.0f, hPrev = 0.0f, pPrev = 0.0f;

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
    // Padding
    int prevValueWidth = String(valuePrev, 2).length() * charWidth;
    tft.fillRect(x - prevValueWidth/2, y - charHeight/2, prevValueWidth,
                 charHeight, bg);

    // Background arc
    tft.drawSmoothArc(x, y, r, ir, startAngle, endAngle, arcBg, bg, true);

    // Foreground arc
    float fillPercentage = (value - minValue) / (maxValue - minValue);
    fillPercentage = constrain(fillPercentage, 0.0f, 1.0f);
    // Prevent drawing an empty arc
    if (fillPercentage != 0) {
        float endFillAngle = startAngle + arcDegrees*fillPercentage;
        tft.drawSmoothArc(x, y, r, ir, startAngle, endFillAngle, arcFg, bg, true);
    }

    // Value
    tft.drawString(String(value, 2), x, y);

    // Unit
    tft.setTextSize(fontSize - 1);
    tft.drawString(unit, x, y + charHeight);
    tft.setTextSize(fontSize);
}
