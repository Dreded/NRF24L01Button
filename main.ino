

// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10

const byte slaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

char txNum = '0';

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 20; // send once per second

uint8_t buttonState = 0;
uint8_t lastButtonState = 0;
uint8_t buttonPin = 7;
bool retry = 0;

struct dataStruct
{
  uint8_t buttonState;
  char data[10] = "Message 0";
} myData;

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);

  Serial.println("SimpleTx Starting");
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(3, 5); // delay, count
  radio.openWritingPipe(slaveAddress);
}

//====================

void loop()
{
  currentMillis = millis();
  buttonState = digitalRead(buttonPin);
  if (currentMillis - prevMillis >= txIntervalMillis)
  {
    if (buttonState != lastButtonState)
    {
      lastButtonState = buttonState;
      send();
    }
    prevMillis = millis();
  }
}

//====================

void send()
{
  myData.buttonState = buttonState;
  bool rslt = 0;
  while (!rslt)
  {
    rslt = radio.write(&myData, sizeof(myData));
    // Always use sizeof() as it gives the size as the number of bytes.
    // For example if dataToSend was an int sizeof() would correctly return 2

    Serial.print("Data Sent ");
    Serial.print(myData.data);
    if (rslt)
    {
      Serial.println("  Acknowledge received");
      updateMessage();
    }
    else
    {
      Serial.println("  Tx failed.. retrying.");
      delay(100);
    }
  }
}

//================

void updateMessage()
{
  // so you can see that new data is being sent
  txNum += 1;
  if (txNum > '9')
  {
    txNum = '0';
  }
  myData.data[8] = txNum;
}
