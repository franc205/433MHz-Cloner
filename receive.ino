#include <RCSwitch.h>
#include <SD.h>

/*Variable and Constants Declaration*/
File logged;
RCSwitch mySwitch = RCSwitch();
unsigned long time,sec;
static const char* bin2tristate(const char* bin);
static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength);
const int sdpin = 4;
/*End of Variable and Constants Declaration*/

/*Setup*/
void setup() 
{
  Serial.begin(9600);
  Serial.print("Starting SD ...");
    if (!SD.begin(sdpin)){
      Serial.println("Error, cannot start SD :C");  
    }
    else{
      Serial.println("Successfully started!");
    }
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
}
/*End of Setup*/

void loop() {
  if (mySwitch.available()) { //If get a signal
      output(mySwitch.getReceivedValue(), mySwitch.getReceivedBitlength(), mySwitch.getReceivedDelay(), mySwitch.getReceivedRawdata(),mySwitch.getReceivedProtocol()); //Get parameters and send it to output
      mySwitch.resetAvailable();
  }
}


void output(unsigned long decimal, unsigned int length, unsigned int delay, unsigned int* raw, unsigned int protocol) {
  logged = SD.open("sniffed.txt", FILE_WRITE);
  const char* b = dec2binWzerofill(decimal, length);
  time = millis();
  sec = time/1000;
  /*Serial Print Results*/
  Serial.print("Time detected: ");
  Serial.print(sec);
  Serial.println(" seconds");
  Serial.print("Decimal: ");
  Serial.print(decimal);
  Serial.print(" (");
  Serial.print(length);
  Serial.print("Bit) Binary: ");
  Serial.print(b);
  Serial.print(" Tri-State: ");
  Serial.print(bin2tristate(b));
  Serial.print(" PulseLength: ");
  Serial.print(delay);
  Serial.print(" microseconds");
  Serial.print(" Protocol: ");
  Serial.println(protocol);
  Serial.print("Raw data: ");
  for (unsigned int i=0; i<= length*2; i++) {
    Serial.print(raw[i]);
    Serial.print(",");
  }
  Serial.println();
  Serial.println(); 
  /*End of Serial Print Results*/
  
  /*SD Prints*/
  if (logged) { 
    logged.print("Time detected: ");
    logged.print(sec);
    logged.println(" seconds");
    logged.print("Decimal: ");
    logged.print(decimal);
    logged.print(" (");
    logged.print(length);
    logged.print("Bit) Binary: ");
    logged.print(b);
    logged.print(" Tri-State: ");
    logged.print(bin2tristate(b));
    logged.print(" PulseLength: ");
    logged.print(delay);
    logged.print(" microseconds");
    logged.print(" Protocol: ");
    logged.println(protocol);
    logged.print("Raw data: ");
    for (unsigned int i=0; i<= length*2; i++) {
      logged.print(raw[i]);
      logged.print(",");
    }
    logged.println();
    logged.println();
    logged.close(); 
  }
  else {
      Serial.println("Error at opening the file");
  }
  /*End of SD Prints*/
  
}

static const char* bin2tristate(const char* bin) {
  static char returnValue[50];
  int pos = 0;
  int pos2 = 0;
  while (bin[pos]!='\0' && bin[pos+1]!='\0') {
    if (bin[pos]=='0' && bin[pos+1]=='0') {
      returnValue[pos2] = '0';
    } 
    else if (bin[pos]=='1' && bin[pos+1]=='1') {
      returnValue[pos2] = '1';
    } 
    else if (bin[pos]=='0' && bin[pos+1]=='1') {
      returnValue[pos2] = 'F';
    }
    else {
      return "not applicable";
    }
    pos = pos+2;
    pos2++;
  }
  returnValue[pos2] = '\0';
  return returnValue;
}

static char * dec2binWzerofill(unsigned long Dec, unsigned int bitLength) {
  static char bin[64]; 
  unsigned int i=0;
  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }
  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    } 
    else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';
  return bin;
}
