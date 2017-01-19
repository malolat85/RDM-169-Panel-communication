
// ====================================================================
// Arduino read codes sends beetwen radio and front panel
// panel's pins:
// 11 - SS_PIN
// 7 - DATA
// 6 - CLK
// Artur Troc 19-01-2017
// ====================================================================


// SPI
#define SCK_PIN   13  // D13 = pin19 = PortB.5
#define MISO_PIN  12  // D12 = pin18 = PortB.4
#define MOSI_PIN  11  // D11 = pin17 = PortB.3
#define SS_PIN    10  // D10 = pin16 = PortB.2
#define NOT_PRESSED 0xEE

//variables
byte word1, word2, word3, word4, word5;
bool aux = 1;
char lcd[8] = {0, 0, 0, 0, 0, 0, 0, 0};
char cd[8] = {0, 0, 0, 0, 0, 0, 0, 0};
char cmd[10]= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
String debugText = "";
bool auxMode = 1;
byte button;


// ======================================================================
// wait for start transmission and read spi
// set:
// 1) debugText to print on serial 
// 2) button's id if pressed
// 3) aux mode if there are a AUX on radio's LCD 
// =====================================================================
void waitForCmd() {
  debugText = String("");
  while (digitalRead(SS_PIN) == 1) {} // wait until SlaveSelect goes low (active)
  word1 = ReadSPI(); //read SPI byte
  switch (word1) {
    case 0x45: //button
      word2 = ReadSPI();
      switch (word2) {
        case 0xc5: //pressed
          word3 = ReadSPI();
          word4 = ReadSPI();
          debugText += "Button " + String(word3, HEX) + " pressed, CMD: " + String(word1, HEX) + ", " + String(word2, HEX) + ", " + String(word3, HEX) + ", " + String(word4, HEX);
          button = word3;
          break;
        case 0xc6: //released
          word3 = ReadSPI();
          word4 = ReadSPI();
          debugText += "Button " + String(word3, HEX) + " relased, CMD: " + String(word1, HEX) + ", " + String(word2, HEX) + ", " + String(word3, HEX) + ", " + String(word4, HEX);
          break;
        case 0xc7: //volume pot
          word3 = ReadSPI();
          word4 = ReadSPI();
          if (word3 == 0x41) {
            debugText += "Volume + ";

          }
          else if (word3 == 0x1) {
            debugText += "Volume - ";
          }
          debugText += ", CMD: " + String(word1, HEX) + ", " + String(word2, HEX) + ", " + String(word3, HEX) + ", " + String(word4, HEX);
          break;

        default:
          debugText += String(word1, HEX) + ", " + String(word2, HEX);
          break;
      }
      break;

    case 0xf6: //VOLUME LCD
      word2 = ReadSPI();
      word3 = ReadSPI();
      debugText += "Panel vol: " + String(word2, HEX) + ", " + String(word3, HEX);
      break;
    case 0xf5: //Display mode ST1, ST2, ST3 ...
      lcd[0] = ReadSPI();
      lcd[1] = ReadSPI();
      lcd[2] = ReadSPI();
      lcd[3] = ReadSPI();
      lcd[4] = ReadSPI();
      lcd[5] = ReadSPI();
      lcd[6] = ReadSPI();
      lcd[7] = ReadSPI();
      debugText += "Display mode LCD: |" + String(lcd[0], HEX) + ", " + String(lcd[1], HEX) + ", " + String(lcd[2], HEX) + ", " + String(lcd[3], HEX) + ", " + String(lcd[4], HEX) + ", " + String(lcd[5], HEX) + ", " + String(lcd[6], HEX) + ", " + String(lcd[7], HEX);
      break;
    case 0xc1: //CD ???
      cd[0] = ReadSPI();
      cd[1] = ReadSPI();
      cd[2] = ReadSPI();
      cd[3] = ReadSPI();
      cd[4] = ReadSPI();
      cd[5] = ReadSPI();
      cd[6] = ReadSPI();
      cd[7] = ReadSPI();
      debugText += "CD ???: " + String(cd[0], HEX) + ", " + String(cd[1], HEX) + ", " + String(cd[2], HEX) + ", " + String(cd[3], HEX) + ", " + String(cd[4], HEX) + ", " + String(cd[5], HEX) + ", " + String(cd[6], HEX) + ", " + String(cd[7], HEX);
      break;

    case 0xf2: //CMD ???
      cmd[0] = ReadSPI();
      cmd[1] = ReadSPI();
      cmd[2] = ReadSPI();
      cmd[3] = ReadSPI();
      cmd[4] = ReadSPI();
      cmd[5] = ReadSPI();
      cmd[6] = ReadSPI();
      cmd[7] = ReadSPI();
      cmd[8] = ReadSPI();
      cmd[9] = ReadSPI();
      debugText += "CMD ???: " + String(cmd[0], HEX) + ", " + String(cmd[1], HEX) + ", " + String(cmd[2], HEX) + ", " + String(cmd[3], HEX) + ", " + String(cmd[4], HEX) + ", " + String(cmd[5], HEX) + ", " + String(cmd[6], HEX) + ", " + String(cmd[7], HEX) + ", " + String(cmd[8], HEX) + ", " + String(cmd[9], HEX);
      break;

    case 0xf7: //?????
      word2 = ReadSPI();
      word3 = ReadSPI();
      word4 = ReadSPI();
      word5 = ReadSPI();
      debugText += "?? 0xF5: " + String(word2, HEX) + ", " + String(word3, HEX) + ", " + String(word4, HEX) + String(word5, HEX) ;
      break;
    case 0xe4: //LCD TEXT
      lcd[0] = ReadSPI();
      lcd[1] = ReadSPI();
      lcd[2] = ReadSPI();
      lcd[3] = ReadSPI();
      lcd[4] = ReadSPI();
      lcd[5] = ReadSPI();
      lcd[6] = ReadSPI();
      lcd[7] = ReadSPI();
      word2 = ReadSPI(); //chksum
      //  debugText += "\nLCD: " + String(lcd) + "chksm: " + String(word2, HEX) + "\n";
      debugText += "LCD: |" + String(lcd[0]) + String(lcd[1]) + String(lcd[2]) + String(lcd[3]) + String(lcd[4]) + String(lcd[5]) + String(lcd[6]) + String(lcd[7]) + "| chksm: " + String(word2, HEX);
      if (debugText == "LCD: |  AUX   | chksm: 72") { //setting aux mode
        aux = 1;
      }
      else aux = 0;
      break;
    case 0xa4: //LCD small (TIME, etc...)
      lcd[0] = ReadSPI();
      lcd[1] = ReadSPI();
      lcd[2] = ReadSPI();
      lcd[3] = ReadSPI();
      if (lcd[1] == ':') {
        debugText += "Small LCD(x:xx): |" + String(lcd[0]) + String(lcd[1]) + String(lcd[2]) + String(lcd[3]) + "|";
      }
      else {
        lcd[4] = ReadSPI();
        debugText += "Small LCD(xx:xx): |" + String(lcd[0]) + String(lcd[1]) + String(lcd[2]) + String(lcd[3]) + String(lcd[4]) + "|";
      }
      break;
    default:
      debugText += String(word1, HEX);
      break;
  }
}

// ===================================================================
// Set SPI starting value
// ==================================================================
void SlaveInit(void) {
  // Set MISO output, all others input
  pinMode(SCK_PIN, INPUT);
  pinMode(MOSI_PIN, INPUT);
//  pinMode(MISO_PIN, OUTPUT);  // (only if bidirectional mode needed)
  pinMode(SS_PIN, INPUT);
}

// ======================================================================
// Read a byte from SPI bus and return it
// ======================================================================
byte ReadByte(void) {
  byte c;
  while (!(SPSR & (1 << SPIF))); // SPIF bit set when 8 bits received
  c = SPDR;               // store high-order byte
  return c; // send back unsigned short value
}


// ======================================================================
// Set SPI on, read a byte from SPI bus, turn of SPI and return readed byte
// ======================================================================
byte ReadSPI() {
  byte c;
  SPCR = (1 << SPE) | (0 << DORD) | (0 << MSTR) | (0 << CPOL) | (0 << CPHA) | (0 << SPR1) | (1 << SPR0); // SPI on
  c = ReadByte();          // read unsigned short value
  SPCR = (0 << SPE) | (0 << DORD) | (0 << MSTR) | (0 << CPOL) | (0 << CPHA) | (0 << SPR1) | (1 << SPR0); // SPI off
  return c;
}






void setup() {
  Serial.begin(115200);
  SlaveInit();  // set up SPI slave mode
  delay(10);
  Serial.println("RDM 169 Reader v0.1");
}

void loop() {
  button = NOT_PRESSED;
  waitForCmd();
  Serial.println(debugText);

  if ((aux) && (button != NOT_PRESSED)) { //if in AUX mode 
    Serial.print("AUX + KLAWISZ: ");  
    Serial.println(button);
  }
}
