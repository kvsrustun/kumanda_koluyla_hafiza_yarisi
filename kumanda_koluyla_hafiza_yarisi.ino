#include <Deneyap_OLED.h>
#include <Deneyap_KumandaKolu.h>

OLED oled;
Joystick joystick;

const int sequenceLength = 4;
int sequence[sequenceLength];
int userIndex = 0;

enum Direction {LEFT = 0, RIGHT, UP, DOWN};

const int threshold = 300;  

bool inputStarted = false;
unsigned long startTime = 0;
unsigned long endTime = 0;

void generateSequence() {
  for (int i = 0; i < sequenceLength; i++) {
    sequence[i] = random(0, 4);
  }
}

void showSequenceOnOLED() {
  oled.clearDisplay();
  oled.setTextXY(0, 0);
  oled.putString("YONLER:");

  oled.setTextXY(1, 0);
  for (int i = 0; i < sequenceLength; i++) {
    switch(sequence[i]) {
      case LEFT:  oled.putString("Sol "); break;
      case RIGHT: oled.putString("Sag "); break;
      case UP:    oled.putString("Yukari "); break;
      case DOWN:  oled.putString("Asagi "); break;
    }
  }
  delay(5000);
  oled.clearDisplay();
}

int getJoystickDirection() {
  int x = joystick.xRead(); 
  int y = joystick.yRead();

  if (x < (512 - threshold)) return RIGHT;
  if (x > (512 + threshold)) return LEFT;
  if (y < (512 - threshold)) return UP;
  if (y > (512 + threshold)) return DOWN;

  return -1;  
}

void setup() {
  Serial.begin(115200);
  if (!joystick.begin(0x1B)) {
    delay(3000);
    Serial.println("I2C bağlantısı başarısız joystick ");
  }

  if (!oled.begin(0x7A)) {
    delay(3000);
    Serial.println("I2C bağlantısı başarısız OLED");
  }

  oled.clearDisplay();
  randomSeed(analogRead(A0));

  generateSequence();
  showSequenceOnOLED();

  oled.setTextXY(0, 0);
  oled.putString("Giris yapiniz");
}

void loop() {
  int dir = getJoystickDirection();

  if (dir != -1) {
    if (!inputStarted) {
      inputStarted = true;
      startTime = millis();
      userIndex = 0;
    }

    delay(300); 

    oled.clearDisplay();
    oled.setTextXY(0, 0);
    oled.putString("Girdi: ");

    switch (dir) {
      case LEFT:  oled.putString("Sol"); break;
      case RIGHT: oled.putString("Sag"); break;
      case UP:    oled.putString("Yukari"); break;
      case DOWN:  oled.putString("Asagi"); break;
    }

    if (dir == sequence[userIndex]) {
      userIndex++;
      if (userIndex == sequenceLength) {
        endTime = millis();
        unsigned long elapsed = endTime - startTime;

        oled.clearDisplay();
        oled.setTextXY(0, 0);
        oled.putString("Tebrikler!");
        oled.setTextXY(1, 0);
        oled.putString("Sure(ms):");
        oled.setTextXY(2, 0);
        oled.putInt(elapsed);

        delay(1500);  
        
        userIndex = 0;
        inputStarted = false;
        generateSequence();
        showSequenceOnOLED();

        oled.setTextXY(0, 0);
        oled.putString("Giris yapiniz");
      }
    } else {
    
      userIndex = 0;
      inputStarted = false;

      oled.clearDisplay();
      oled.setTextXY(0, 0);
      oled.putString("Yanlis!");
      oled.setTextXY(1, 0);
      oled.putString("Tekrar dene");

      delay(1500);

      generateSequence();
      showSequenceOnOLED();

      oled.clearDisplay();
      oled.setTextXY(0, 0);
      oled.putString("Giris yapiniz");
    }

    delay(500); 
  }
}
