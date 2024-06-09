#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <WiFi.h>           // Include WiFi library for ESP32
#include <FirebaseESP32.h>  // Include Firebase library for ESP32

// Firebase configuration
#define FIREBASE_HOST "https://mksrobotics.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyAcHklpkVYko5ZyuQXaeHhGxa8F3UzxDp0"

FirebaseData fbdo;
FirebaseConfig config;
FirebaseAuth auth;

// WiFi credentials
#define WIFI_SSID "MIKRO"
#define WIFI_PASSWORD "IDEAlist"


// Inisialisasi LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Inisialisasi Keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 13, 12, 14, 27 }; /* connect to the row pinouts of the keypad */
byte colPins[COLS] = { 26, 25, 33, 32 }; /* connect to the column pinouts of the keypad */

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Pin untuk Buzzer
const int buzzerPin = 18;

// Variabel permainan
int targetNumber;  // Angka rahasia
String pin = "";
String pinUpdate = "";
String inputNumber = "";

void setup() {
  lcd.begin();
  lcd.backlight();
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected to WiFi");

  // Firebase configuration
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  pin = generateRandomPin();
  lcd.print(pin);
  sendData(pin);
  delay(3000);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("ANGKA BOOM");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Masukkan");
  lcd.setCursor(0,1);
  lcd.print("Angka: ");
  targetNumber = random(5);
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    if (key == '#') {
      checkNumber();

    } else if (key == '*') {
      inputNumber = "";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Masukkan");
      lcd.setCursor(0, 1);
      lcd.print("Angka:");
    } else {
      inputNumber += key;
      lcd.setCursor(0, 1);
      lcd.print("Angka: ");
      lcd.setCursor(7, 1);
      lcd.print(inputNumber);
    }
  }
}

void checkNumber() {
  lcd.clear();
  int input = inputNumber.toInt();
  if (input == targetNumber) {
    lcd.setCursor(0, 0);
    lcd.print("Benar!");
    buzz(500);
    ESP.restart();
  } else if (input < targetNumber) {
    lcd.setCursor(0, 0);
    lcd.print("Terlalu Kecil!");
    buzz(200);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Terlalu Besar!");
    buzz(200);
  }
  inputNumber = "";
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Masukkan");
  lcd.setCursor(0, 1);
  lcd.print("angka:");
}

void buzz(int duration) {
  digitalWrite(buzzerPin, HIGH);
  delay(duration);
  digitalWrite(buzzerPin, LOW);
}

void sendData(String pin) {
  if (Firebase.setString(fbdo, "/event/games/angka-boom/status/pin", pin)) {
    Serial.print("Data sent to Firebase: ");
    Serial.println(pin);
  } else {
    Serial.println("Failed to send data to Firebase");
  }
}

String generateRandomPin() {
  for (int i = 0; i < 6; i++) {
    pin += String(random(0, 10));  // Append a random digit (0-9)
  }
  return pin;
}