#include <Adafruit_BME280.h>
#include <TFT_eSPI.h>

Adafruit_BME280 bme;
TFT_eSPI tft = TFT_eSPI();

const int fontSize = 3;
const int r = 55, arcWidth = 10;
const int ir = r - arcWidth;

void setup() {
    bme.begin(0x76);
    tft.begin();
    tft.setRotation(2);
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(fontSize);
    tft.setTextDatum(MC_DATUM);
}

void loop() { 
    float t = bme.readTemperature();
    float h = bme.readHumidity();
    float p = bme.readPressure() / 100.0f;

    drawGauge(String(t, 2) + "C", 60, 60, TFT_RED);
    drawGauge(String(h, 2) + "%", 180, 60, TFT_SKYBLUE);
    drawGauge(String(p, 2) + "hPa", 120, 180, TFT_GOLD);

    delay(1000);
}

void drawGauge(String value, int x, int y, int color) {
    tft.drawSmoothArc(x, y, r, ir, 45, 315, color, TFT_BLACK, true);
    tft.drawString(value, x, y);
}
