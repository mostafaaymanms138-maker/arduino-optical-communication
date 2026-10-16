/*
  Arduino Optical Communication System
  Arduino Uno sketch for framed optical message transmission and reception.
  The physical circuit has not been independently verified.
*/

#define STX 0x70
#define ETX 0x71

// TX/RX buffers
char txButton, txTilt, txPot, txA, txB, txC, txD;
char txBuffer[8] = {0,0,0,0,0,0,0,ETX};
char rxBuffer[7];
char rxButton, rxTilt, rxPot, rxA, rxB, rxC, rxD;
int  rx_index = 0;

// --- UPDATED 7-SEGMENT PINS (YOUR REAL CIRCUIT) ---
int segmentPins[] = {5, 6, 9, 10, 12, 13, A2};

// SAME DIGITS
byte digits[8][7] = {
  {1,1,1,1,1,1,0}, // 0
  {0,1,1,0,0,0,0}, // 1
  {1,1,0,1,1,0,1}, // 2
  {1,1,1,1,0,0,1}, // 3
  {0,1,1,0,0,1,1}, // 4
  {1,0,1,1,0,1,1}, // 5
  {1,0,1,1,1,1,1}, // 6
  {1,1,1,0,0,0,0}  // 7
};

// TX timing
const long txInterval = 200;
int tx_state = 0;
char chr;
unsigned long previousTxMillis = 0;

#define TX_START_OF_TEXT -1
int tx_string_state = TX_START_OF_TEXT;

// RX timing
const long rxInterval = 20;
int rx_state = 0;
char rx_char;
unsigned long previousRxMillis = 0;
int rx_bits[10]={0};

// Prototypes
void txChar();
void rxChar();
char getTxChar();
char encrypt(char in_char);
char decrypt(char in_char);

void setup()
{
  // === UPDATED INPUT PINS ===
  pinMode(8, INPUT_PULLUP);  // Button (was D2)
  pinMode(2, INPUT_PULLUP);  // Tilt (was D5)

  // === UPDATED OUTPUT PINS ===
  pinMode(11, OUTPUT);  // Buzzer (was D4)
  pinMode(4, OUTPUT);   // Tilt LED (was A4)
  pinMode(7, OUTPUT);   // Fan PWM (was D6)
  pinMode(3, OUTPUT);   // TX LED

  Serial.begin(9600);
  Serial.println("Joystick 0..127; fan ON at >=50");

  // 7-segment pins
  for (int i = 0; i < 7; i++)
    pinMode(segmentPins[i], OUTPUT);
}

void readInputs()
{
  // Button + Tilt (UPDATED PINS)
  txButton = (digitalRead(8) == LOW) ? 1 : 0;
  txTilt   = (digitalRead(2) == LOW) ? 1 : 0;

  // Potentiometer 0..7 (NO LOGIC CHANGE)
  int potValue = analogRead(A1);
  txPot = map(potValue, 0, 1023, 0, 7);

  // Joystick on A4 (UPDATED PIN)
  int joyValue = analogRead(A4);
  txA = map(joyValue, 0, 1023, 0, 127);

  // Unused
  txB = 0;
  txC = 0;
  txD = 0;
}

void writeOutputs()
{
  // Buzzer mirrors Button (UPDATED PIN)
  digitalWrite(11, (rxButton != 0) ? HIGH : LOW);

  // Tilt LED mirrors Tilt (UPDATED PIN)
  digitalWrite(4, (rxTilt != 0) ? HIGH : LOW);

  // D7 is not PWM-capable on the Uno; use it for fan on/off control.
  digitalWrite(7, (rxA >= 50) ? HIGH : LOW);

  // 7-segment display
  int d = constrain(rxPot, 0, 7);
  for (int i = 0; i < 7; i++)
    digitalWrite(segmentPins[i], digits[d][i]);
}

char encrypt(char x){return x;}
char decrypt(char x){return x;}

char getTxChar()
{
  char c;
  switch (tx_string_state)
  {
    case TX_START_OF_TEXT:
      tx_string_state = 0;
      txBuffer[0] = txButton;
      txBuffer[1] = txTilt;
      txBuffer[2] = txPot;
      txBuffer[3] = txA; 
      txBuffer[4] = txB;
      txBuffer[5] = txC;
      txBuffer[6] = txD;
      return STX;

    default:
      c = txBuffer[tx_string_state];
      tx_string_state++;
      if (c == ETX) {
        tx_string_state = TX_START_OF_TEXT;
        return ETX;
      } else {
        return c;
      }
  }
}

void txChar()
{
  unsigned long now = millis();

  if (now - previousTxMillis >= txInterval)
  {
    previousTxMillis += txInterval;

    switch (tx_state)
    {
      case 0:
        chr = encrypt(getTxChar());
        digitalWrite(3, HIGH);
        tx_state++;
        break;

      case 1: case 2: case 3: case 4:
      case 5: case 6: case 7:
        digitalWrite(3, (chr & 0x40) ? HIGH : LOW);
        chr <<= 1;
        tx_state++;
        break;

      case 8:
        digitalWrite(3, HIGH);
        tx_state++;
        break;

      default:
        digitalWrite(3, LOW);
        tx_state++;
        if (tx_state > 20) tx_state = 0;
    }
  }
}

void rxChar()
{
  unsigned long now = millis();
  int sensorValue;

  if (now - previousRxMillis >= rxInterval)
  {
    previousRxMillis += rxInterval;
    sensorValue = analogRead(A0);

    switch (rx_state)
    {
      case 0:
        if (sensorValue >= 900) {
          rx_bits[0]++;
          rx_state++;
        }
        break;

      case 100:
        if (rx_bits[0] >= 6 && rx_bits[8] >= 6) {
          rx_char = 0;
          for (int i = 1; i < 8; i++) {
            rx_char <<= 1;
            if (rx_bits[i] >= 6) rx_char |= 1;
          }

          rx_char = decrypt(rx_char);
          Serial.println(rx_char + 0);

          switch (rx_char)
          {
            case STX:
              rx_index = 0;
              break;

            case ETX:
              rxButton = rxBuffer[0];
              rxTilt   = rxBuffer[1];
              rxPot    = rxBuffer[2];
              rxA      = rxBuffer[3];
              rxB      = rxBuffer[4];
              rxC      = rxBuffer[5];
              rxD      = rxBuffer[6];
              rx_index = 0;
              break;

            default:
              if (rx_index < 7)
                rxBuffer[rx_index++] = rx_char;
              break;
          }
        }
        for (int i = 0; i < 10; i++) rx_bits[i] = 0;
        rx_state = 0;
        break;

      default:
        if (sensorValue >= 900) rx_bits[rx_state/10]++;
        rx_state++;
        break;
    }
  }
}

void loop()
{
  readInputs();
  txChar();
  rxChar();
  writeOutputs();
}
