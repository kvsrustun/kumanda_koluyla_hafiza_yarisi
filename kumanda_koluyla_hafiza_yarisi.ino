#include <Deneyap_OLED.h>
#include <Deneyap_KumandaKolu.h>

OLED oled;
Joystick joystick;

int level = 1;           
int sequenceLength = 1;   
int sequence[10];         
int userIndex = 0;

enum Direction {LEFT = 0, RIGHT, UP, DOWN};

const int threshold = 300;  

bool inputStarted = false;

void generateSequence() {
  sequenceLength = level;   
  for (int i = 0; i < sequenceLength; i++) {
    sequence[i] = random(0, 4);
  }
}

void showSequenceOnOLED() {
  oled.clearDisplay();
  oled.setTextXY(0, 0);
  oled.putString("Seviye: ");
  oled.putInt(level);

  oled.setTextXY(1, 0);
  oled.putString("YONLER:");

  oled.setTextXY(2, 0);
  for (int i = 0; i < sequenceLength; i++) {
    switch(sequence[i]) {
      case LEFT:  oled.putString("Sol"); break;
      case RIGHT: oled.putString("Sag"); break;
      case UP:    oled.putString("Yukari"); break;
      case DOWN:  oled.putString("Asagi"); break;
    }
    if (i < sequenceLength - 1) {
      oled.putString(" | ");
    }
  }
  delay(4000);
  oled.clearDisplay();
}

int getJoystickDirection() {
  int x = joystick.xRead(); 
  int y = joystick.yRead();

  if (x < (512 - threshold)) return DOWN;   
  if (x > (512 + threshold)) return UP;    
  if (y < (512 - threshold)) return RIGHT;
  if (y > (512 + threshold)) return LEFT;

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

  oled.setTextXY(4, 0);
  oled.putString("Giris yapiniz...");
}

void loop() {
  int dir = getJoystickDirection();

  if (dir != -1) {
    if (!inputStarted) {
      inputStarted = true;
      userIndex = 0;
    }

    delay(300); // Hızlı giriş için kısa bekleme

    // Kullanıcının girdiği yön doğru mu kontrol et
    if (dir == sequence[userIndex]) {
      userIndex++;

      // Eğer kullanıcı tüm diziyi doğru girdi ise
      if (userIndex == sequenceLength) {
        oled.clearDisplay();
        oled.setTextXY(2, 2);
        oled.putString("Tebrikler!");
        delay(1000);

        level++;
        if (level > 10) level = 1;

        userIndex = 0;
        inputStarted = false;

        generateSequence();
        showSequenceOnOLED();

        oled.setTextXY(4, 0);
        oled.putString("Giris yapiniz...");
      }

    } else {
      // Yanlış giriş: tekrar başa dön
      level = 1;
      userIndex = 0;
      inputStarted = false;

      oled.clearDisplay();
      oled.setTextXY(3, 3);
      oled.putString("Yanlis!");
      oled.setTextXY(4, 2);
      oled.putString("Tekrar dene");
      delay(1000);

      generateSequence();
      showSequenceOnOLED();

      oled.setTextXY(4, 0);
      oled.putString("Giris yapiniz...");
    }

    delay(200);
  }
}
