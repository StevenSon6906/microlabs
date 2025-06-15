#include <SPI.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR        0x27
#define LCD_COLS        12
#define LCD_ROWS        2
#define TRIG_PIN        4
#define ECHO_PIN        A1
#define READ_INTERVAL   200  
#define DISPLAY_INTERVAL 1000
#define ERROR_LED_PIN   13
float avgBiffer[DISPLAY_INTERVAL/READ_INTERVAL]; 
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
char buf[10];
unsigned long lastReadTime = 0;
unsigned long lastReadTimeShow = 0;
void measureDistance();
void updateDisplay();
float distance = 0.0f;
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Distance:");
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ERROR_LED_PIN, OUTPUT);
  digitalWrite(ERROR_LED_PIN, LOW);
}

float getAverageDistance() {
  float sum = 0.0f;
  int count = 0;
  for (int i = 0; i < (DISPLAY_INTERVAL / READ_INTERVAL); i++) {
    if (avgBiffer[i] > 0) {
      sum += avgBiffer[i];
      count++;
    }
  }
  return (count > 0) ? (sum / count) : 1000.0f;
}

void loop() {
  unsigned long now = millis();
  if (now - lastReadTime >= READ_INTERVAL) {
    lastReadTime = now;
    measureDistance();
    avgBiffer[(now / READ_INTERVAL) % (DISPLAY_INTERVAL / READ_INTERVAL)] = distance;
  }

  unsigned long now1 = millis();
  if (now1 - lastReadTimeShow >= DISPLAY_INTERVAL) {
    lastReadTimeShow = now1;
    distance = getAverageDistance();
    updateDisplay();
  }
}

void measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  float distanceCm = pulseIn(ECHO_PIN, HIGH);
  distance = (distanceCm * 0.0343f) / 2.0f;
  Serial.print("Distance: ");
  Serial.print(distance, 2);
  Serial.println(" cm");
}

void updateDisplay() {
  lcd.setCursor(0, 1);
  if (distance > 610.0f) {
    lcd.print("Irrelevant   ");
    digitalWrite(ERROR_LED_PIN, HIGH);
    return;
  }
  dtostrf(distance, 5, 2, buf);
  digitalWrite(ERROR_LED_PIN, LOW);
  lcd.print(buf);
  lcd.print(" cm   ");
}
