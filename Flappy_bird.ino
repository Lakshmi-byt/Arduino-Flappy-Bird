#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define BUTTON_PIN 2   
#define BUZZER_PIN 4   


int birdY = 32;
int velocity = 0;
int gravity = 1;
int flap = -4;


int pipeX = SCREEN_WIDTH;
int pipeGap = 30;
int pipeY = 20;


int blockX = SCREEN_WIDTH;
int blockY = 20;
bool blockActive = false;

int score = 0;

// Bird sprite (12x10 pixels, round body + beak)
static const unsigned char PROGMEM bird_bmp[] = {
  0x07, 0x00, // ....XXX.....
  0x0F, 0x80, // ...XXXXX....
  0x1F, 0xC0, // ..XXXXXXX...
  0x3F, 0xE0, // .XXXXXXXXX..
  0x3B, 0xE0, // .XXX.XXXXX..
  0x3F, 0xE0, // .XXXXXXXXX..
  0x1F, 0xC0, // ..XXXXXXX...
  0x0F, 0x80, // ...XXXXX....
  0x07, 0x40, // ....XXX.X..   <- beak
  0x02, 0x00  // .....X......
};

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for (;;); 
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Flappy Bird!");
  display.display();
  delay(2000);
}

void loop() {
  // Button flap
  if (digitalRead(BUTTON_PIN) == LOW) {
    velocity = flap;
    tone(BUZZER_PIN, 800, 100);  // flap sound
  }

  
  velocity += gravity;
  birdY += velocity;
  if (birdY < 0) birdY = 0;
  if (birdY > SCREEN_HEIGHT - 10) birdY = SCREEN_HEIGHT - 10;

  
  pipeX -= 2;
  if (pipeX < -10) {
    pipeX = SCREEN_WIDTH;
    pipeY = random(10, 30);
    score++;
    tone(BUZZER_PIN, 1200, 100);  
  }

  
  if (blockActive) {
    blockX -= 2;
    if (blockX < -8) {
      blockActive = false;
    }
  } else {
    if (random(0, 100) < 3) {  
      blockActive = true;
      blockX = SCREEN_WIDTH;
      blockY = random(10, SCREEN_HEIGHT - 20);
    }
  }

 
  if ((birdY < pipeY || birdY > pipeY + pipeGap) && (pipeX < 20 && pipeX > 5)) {
    gameOver();
  }


  if (blockActive && (birdY + 10 > blockY && birdY < blockY + 8) &&
      (blockX < 20 && blockX > 5)) {
    gameOver();
  }

  
  display.clearDisplay();

  
  display.drawBitmap(8, birdY, bird_bmp, 12, 10, SSD1306_WHITE);

  
  display.fillRect(pipeX, 0, 10, pipeY, SSD1306_WHITE);
  display.fillRect(pipeX, pipeY + pipeGap, 10, SCREEN_HEIGHT - pipeY - pipeGap, SSD1306_WHITE);

  
  if (blockActive) {
    display.fillRect(blockX, blockY, 8, 8, SSD1306_WHITE);
  }

  
  display.setCursor(100, 0);
  display.print(score);

  display.display();
  delay(40);
}

void gameOver() {
  tone(BUZZER_PIN, 300, 500);  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(20, 20);
  display.println("GAME OVER");
  display.setTextSize(1);
  display.setCursor(30, 50);
  display.print("Score: ");
  display.print(score);
  display.display();
  delay(3000);

 
  birdY = 32;
  velocity = 0;
  pipeX = SCREEN_WIDTH;
  blockActive = false;
  score = 0;
}