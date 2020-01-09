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

// IR Remote command keys
#define IR_COMMAND_1 0x9716BE3F
#define IR_COMMAND_2 0x3D9AE3F7
#define IR_COMMAND_3 0x6182021B
#define IR_COMMAND_4 0x8C22657B
#define IR_COMMAND_5 0x488F3CBB

// Liquid Crystal Display
LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

// Infrared Remote
IRrecv irrecv(PIN_IR_REMOTE);
decode_results ir_command;

// DHT11 sensor
SimpleDHT11 dht11(PIN_DHT11);

// Measured variables
byte temperature = 0;
byte humidity = 0;
byte water = 0;
byte light = 0;

long last = 0;
long mode_timer = 0;
long time_temp = 0;

// Modes
enum modes
{
  MODE_TEMPERATURE,
  MODE_HUMIDITY,
  MODE_LIGHT,
  MODE_WATER,
  MODE_RANDOM
};

int selected = MODE_TEMPERATURE;
int display_mode = MODE_TEMPERATURE;

void setup()
{
  // Serial.begin(9600);
  lcd.begin(16, 2);
  irrecv.enableIRIn();

  // Print a boot message
  print_LCD("*** WEATHER ***", "*** STATION ***");
  delay(1500);
  lcd.clear();
}

void loop()
{
  read_irremote();
  read_sensors();
  update_LCD();
}

void read_irremote()
{
  if (irrecv.decode(&ir_command))
  {
    Serial.println(ir_command.value, HEX);
    switch (ir_command.value)
    {
    case IR_COMMAND_1:
      display_mode = MODE_TEMPERATURE;
      break;

    case IR_COMMAND_2:
      display_mode = MODE_WATER;
      break;

    case IR_COMMAND_3:
      display_mode = MODE_LIGHT;
      break;

    case IR_COMMAND_4:
      display_mode = MODE_HUMIDITY;
      break;

    case IR_COMMAND_5:
      display_mode = MODE_RANDOM;
      break;

    default:
      break;
    }
    irrecv.resume();
  }
}

void readTemperatureAndHumiditySensor()
{

  if ((millis() - time_temp) > (1000))
  { // 1 SEC
    time_temp = millis();
    TIMSK2 = 0x00;
    int err = dht11.read(&temperature, &humidity, NULL);
    TIMSK2 = 0x02;
  }
}

void readWaterSensor()
{
  water = map(analogRead(PIN_WATER_SENSOR), 0, 640, 0, 100); // 640 TOP LIMIT
}

void readLightSensor()
{
  light = map(min(analogRead(PIN_LIGHT_SENSOR), 512), 0, 512, 0, 100); // 512 es mucha luz ya
}

void read_sensors()
{
  readWaterSensor();
  readLightSensor();
  readTemperatureAndHumiditySensor();
}

void print_LCD(String top_row, String bottom_row)
{
  if ((millis() - last) > (1000 / 20))
  { //FPS TOP
    last = millis();
    lcd.clear();
  }
  lcd.setCursor(0, 0);
  lcd.print(top_row);
  lcd.setCursor(0, 1);
  lcd.print(bottom_row);
}

void update_LCD()
{

  if ((millis() - mode_timer) > (1000))
  { // 1 SEC
    mode_timer = millis();
    selected++;
    if (selected > MODE_WATER)
    {
      selected = MODE_TEMPERATURE;
    }
  }

  switch (display_mode)
  {
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
  case MODE_RANDOM:
    if ((millis() - mode_timer) > (1000))
    { // 1 SEC
      mode_timer = millis();
      selected++;
      if (selected > MODE_WATER)
      {
        selected = MODE_TEMPERATURE;
      }
    }
    display_mode = selected;
    break;
  default:
    print_LCD("", "");
    break;
  }
}
