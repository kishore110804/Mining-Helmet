#define BLYNK_TEMPLATE_ID "TMPL3osL5Risq"
#define BLYNK_TEMPLATE_NAME "MPMC Helmet"
#define BLYNK_AUTH_TOKEN "6emQYz_XOw3HkYoXhe82Ydi8y_5BBSfB"

#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// LCD display configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Blynk configuration
#define BLYNK_PRINT Serial
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Iot";  // Wi-Fi name
char pass[] = "123456789";  // Wi-Fi password

// DHT11 sensor configuration
#define DHTPIN D7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Timer configuration
BlynkTimer timer;

// Buzzer pin
#define BUZZER_PIN D0

void sendSensor() {
    int smokeLevel = analogRead(A0); // Read smoke sensor value
    int sos = digitalRead(D4); // Read SOS button state

    if (sos == LOW) {
        lcd.clear();
        lcd.print("EMERGENCY");
        lcd.setCursor(0, 1);
        lcd.print("NEED HELP!!!");
        Blynk.virtualWrite(V3, "EMERGENCY NEED HELP");
        delay(3000);
    }

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    Blynk.virtualWrite(V0, smokeLevel);
    Blynk.virtualWrite(V1, temperature);
    Blynk.virtualWrite(V2, humidity);

    lcd.clear();
    lcd.print("T:");
    lcd.print(temperature);
    lcd.print(" H:");
    lcd.print(humidity);
    lcd.setCursor(0, 1);
    lcd.print("GAS:");
    lcd.print(smokeLevel);
    delay(300);

    if (smokeLevel > 500) {
        lcd.clear();
        lcd.print("Harmful gas");
        Blynk.virtualWrite(V3, "Harmful gas detected");
        digitalWrite(BUZZER_PIN, HIGH);
        delay(300);
        digitalWrite(BUZZER_PIN, LOW);
        delay(300);
    } else {
        Blynk.virtualWrite(V3, "Gas level normal");
    }
}

void setup() {
    lcd.begin();
    lcd.backlight();
    dht.begin();
    
    pinMode(D4, INPUT_PULLUP); // SOS button pin
    pinMode(BUZZER_PIN, OUTPUT); // Buzzer pin
    
    Serial.begin(9600);

    lcd.clear();
    lcd.print("Connecting to WiFi");
    Blynk.begin(auth, ssid, pass);

    lcd.clear();
    lcd.print("WiFi connected");
    delay(2000);

    timer.setInterval(2500L, sendSensor); // Send sensor data every 2.5 seconds
}

void loop() {
    Blynk.run();
    timer.run();
}
