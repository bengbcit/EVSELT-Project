#include <bluefruit.h>
#include <avr/dtostrf.h>

uint16_t uvindexvalue = 0x0;
char message[] = "Hello!";
char single[] = " ";
String results;
int i;
int sizes;

char result1[50];
char result2[50];

//ADC
int adcin_1 = A0;
int adcin_2 = A1;
int adc1 = 0;
int adc2 = 0;

float result_AC = 0.0;
float result_DC = 0.0;



#define UUID16_SVC_ENVIRONMENTAL_SENSING 0x181A
#define UUID16_CHR_STRING 0x2A3D

BLEService environmental_sensing_service = BLEService(UUID16_SVC_ENVIRONMENTAL_SENSING);
BLECharacteristic string_characteristic = BLECharacteristic(UUID16_CHR_STRING);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Start!");

  Bluefruit.begin();
  Bluefruit.setName("EVSELT");

  setupESService();
  startAdv();

  results = String();
}

void loop() {

//Get ADC value
  adc1 = analogRead(adcin_1); // AC Voltage
  adc2 = analogRead(adcin_2); // DC Voltage

  //Convert to AC and DC values (eg. 0 - 30A; 0 - 240V)
  result_AC = adc1 * (30.0F / 934.0F);
  result_DC = adc2 * (240.0F / 934.0F);

  dtostrf(result_AC, 4, 2, result1);
  dtostrf(result_DC, 4, 2, result2);

  results = String(result1) + " " + String(result2) + "=";

  Serial.println(results);
  sizes = results.length();
  //Serial.println("%d", sizes);
  //Serial.println(message);

  for (i = 0; i < sizes; i++)
    {
      Serial.print(single);
      single[0] = results[i];

      if (string_characteristic.indicate(&single, sizeof(single))) {
        Serial.println(" DATA SENT");
      } else {
        Serial.println(" NOT SET!");
        i = sizes;
    }
  }

}

void startAdv(void) {
  Bluefruit.Advertising.addService(environmental_sensing_service);

  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0);
}

void setupESService(void) {
  environmental_sensing_service.begin();
  string_characteristic.setProperties(CHR_PROPS_INDICATE);
  string_characteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  string_characteristic.setFixedLen(100);
  string_characteristic.begin();
  string_characteristic.write(&single, sizeof(single));
}
