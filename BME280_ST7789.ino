#include <Adafruit_BME280.h>
#include <TFT_eSPI.h>

Adafruit_BME280 bme;
TFT_eSPI tft = TFT_eSPI();

const int fontSize = 3, fontHeight = 8, fontWidth = 6;
const int r = 55, arcWidth = 10;
const int ir = r - arcWidth;

// Colors
const int bg = 0x0000, alpha = 128;
const int tArcFg = 0xF800, tArcBg = tft.alphaBlend(alpha, tArcFg, bg);
const int hArcFg = 0x867D, hArcBg = tft.alphaBlend(alpha, hArcFg, bg);
const int pArcFg = 0xD69A, pArcBg = tft.alphaBlend(alpha, pArcFg, bg);

// Arc
const int startAngle = 45, endAngle = 315;
const int arcDegrees = endAngle - startAngle;

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

    drawGauge(t, 10, 40, "\xF7" "C", 60, 60, tArcFg, tArcBg);
    drawGauge(h, 0, 100, "%", 180, 60, hArcFg, hArcBg);
    drawGauge(p, 960, 1020, "hPa", 120, 180, pArcFg, pArcBg);

    delay(1000);
}

void drawGauge(float value, int minValue, int maxValue, String unit,
               int x, int y, int arcFg, int arcBg) {
    // Background arc
    tft.drawSmoothArc(x, y, r, ir, startAngle, endAngle, arcBg, bg, true);

    // Foreground arc
    float fillPercentage = (value - minValue) / (maxValue - minValue);
    float fillAngle = arcDegrees * fillPercentage;
    float endFillAngle = startAngle + fillAngle;
    tft.drawSmoothArc(x, y, r, ir, startAngle, endFillAngle, arcFg, bg, true);

    // Value
    tft.drawString(String(value, 2), x, y);
    tft.setTextSize(fontSize - 1);
    tft.drawString(unit, x, y + fontSize*fontHeight);
    tft.setTextSize(fontSize);
}
