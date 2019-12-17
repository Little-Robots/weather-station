#include <LiquidCrystal.h>
#include <SimpleDHT.h>
#include <MFRC522.h>
#include <IRremote.h>

// Arduino digital pins
#define PIN_LCD_EN 0
#define PIN_LCD_RS 1
#define PIN_LCD_D7 2
#define PIN_LCD_D6 3
#define PIN_LCD_D5 4
#define PIN_LCD_D4 5
#define PIN_DHT11 6
#define PIN_IR_REMOTE 7
#define PIN_BUZZER 8
#define PIN_RFID_RST 9
#define PIN_RFID_SDA 10
#define PIN_RFID_MOSI 11
#define PIN_RFID_MISO 12
#define PIN_RFID_SCK 13

// Arduino analog pins
#define PIN_WATER_SENSOR A0
#define PIN_LIGHT_SENSOR A1

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

// Measured variables
byte temperature = 0;
byte humidity = 0;
byte water = 0;
byte light = 0;

// Modes
enum modes
{
  MODE_TEMPERATURE,
  MODE_HUMIDITY,
  MODE_LIGHT,
  MODE_WATER
};
int display_mode = MODE_TEMPERATURE;

void setup()
{
  setup_LCD();

  // Print a boot message
  print_LCD("*** WEATHER ***", "*** STATION ***");
  delay(1500);
  lcd.clear();
}

void loop()
{
  read_sensors();
  update_LCD();
}

void read_sensors()
{
  readWaterSensor();
  readLightSensor();
  readTemperatureAndHumiditySensor();
}

void setup_LCD()
{
  lcd.begin(16, 2);
}

void print_LCD(char *top_row, char *bottom_row)
{
  lcd.setCursor(0, 0);
  lcd.print(top_row);
  lcd.setCursor(0, 1);
  lcd.print(bottom_row);
}

void update_LCD()
{
  switch(display_mode) {
    case MODE_TEMPERATURE:
      print_LCD("Temperature: ", String(temperature) + String(" \337C"));
      break;
    case MODE_HUMIDITY:
      print_LCD("Humidity: ", String(humidity) + String(" %"));
      break;
    case MODE_LIGHT:
      print_LCD("Light: ", String(light) + String(" lumens"));
      break;
    case MODE_WATER:
      print_LCD("Water: ", String(water) + String(" %"));
      break;
    default:
      print_LCD("", "");
      break;
  }
}