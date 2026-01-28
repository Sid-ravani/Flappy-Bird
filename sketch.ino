#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <U8glib.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
const unsigned char FlappyBird [] PROGMEM = {
	0x03, 0xf0, 0x00, 0x0c, 0x28, 0x00, 0x10, 0x44, 0x00, 0x78, 0x4a, 0x00, 0x84, 0x4a, 0x00, 0x82, 
	0x22, 0x00, 0x82, 0x1f, 0x00, 0x44, 0x20, 0x80, 0x38, 0x5f, 0x00, 0x08, 0x21, 0x00, 0x06, 0x1f, 
	0x00, 0x01, 0xe0, 0x00
};

const unsigned char topBottomPipe [] PROGMEM = {
	0xff, 0xf8, 0xff, 0xf8, 0x3f, 0xe0, 0x27, 0xe0, 0x23, 0xe0, 0x21, 0xe0, 0x21, 0xe0, 0x21, 0xe0, 
	0x23, 0x20, 0x25, 0x20, 0x23, 0x20, 0x23, 0x20, 0x21, 0x20, 0x21, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0xe0, 0x21, 0xe0, 0x21, 0xa0, 0x22, 0xa0, 0x23, 0x20, 0x24, 0x20, 0x24, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x24, 0x20, 0x2c, 0x20, 0x2c, 0x20, 0x3c, 0x20, 0x3c, 0x20, 0x3c, 0x20, 0x3c, 0x20, 0x3c, 0x20, 
	0x3c, 0x20, 0x3c, 0x20, 0x3c, 0x20, 0x34, 0x20, 0x38, 0x20, 0x28, 0x60, 0x28, 0x60, 0x28, 0x60, 
	0x28, 0xa0, 0x28, 0xa0, 0x28, 0xa0, 0x29, 0x20, 0x29, 0x20, 0x29, 0x20, 0x29, 0x20, 0x28, 0x20, 
	0x28, 0x20, 0x28, 0x20, 0x28, 0x20, 0x29, 0x60, 0x2f, 0xe0, 0x3f, 0xe0, 0xff, 0xf8, 0xff, 0xf8
  };

const int BUTTON_PIN = 2;

// Bird Physics
float birdYaxis = 30;
float velocity = 0;
float gravity = 0.8;
float jumpForce = -3.5;

int score = 0;
int highScore = 0;
char bufferSC[20];
char bufferHi[20];
bool gameOver = false;

//Pipes 
float pipeXaxis_1 = 128;
float pipeXaxis_2 = 170.6;
float pipeXaxis_3 = 213.2;
int pipeGap = 29;
int pipeTopHeight_1;
int pipeTopHeight_2;
int pipeTopHeight_3;

void resetGame() {
  birdYaxis = 32;
  velocity = 0;
  pipeXaxis_1 = 128;
  pipeXaxis_2 = 168;
  pipeXaxis_3 = 208;
  score = 0;
  pipeTopHeight_1 = random(30,55);
  pipeTopHeight_2 = random(30,55);
  pipeTopHeight_3 = random(30,55);
  gameOver = false;
}

void setup() {
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  display.clearDisplay();
  display.display();
  
  resetGame();

  u8g.setFont(u8g_font_tpssb);
  u8g.setColorIndex(1);
}

void showGameOver(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,5);
  display.print("GAME OVER");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 35);
  display.print("Score: ");
  display.print(score);

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 50);
  display.print("High Score: ");
  display.print(highScore);

  display.display();
}

void saveHighScore(){
  highScore = score;
}

//collision check
void isCollision(float pipeXaxis, int pipeTopHeight){
  // Collision with Pipe
  if(pipeXaxis < 20 && pipeXaxis > 0){
    if( (birdYaxis )< 64 - pipeTopHeight || birdYaxis+12 > (64-(pipeTopHeight-pipeGap))){
      gameOver = true;
    }
  }
}

void Score(float* pipeXaxis, int* pipeTopHeight){
   if(*pipeXaxis < 0){
    *pipeXaxis = 128;
    *pipeTopHeight = random(30,55);
    score++;
  }
}

void loop() {
 if(gameOver){
  showGameOver();
  if(digitalRead(BUTTON_PIN)==LOW){
    delay(200);
    resetGame();
  }
  return;
 }

  if(digitalRead(BUTTON_PIN)==LOW){
    velocity = jumpForce;
  }

  velocity +=gravity;
  birdYaxis += velocity;

  pipeXaxis_1 -= 2;
  pipeXaxis_2 -= 2;
  pipeXaxis_3 -= 2;
  
  //score check
  Score(&pipeXaxis_1, &pipeTopHeight_1);
  Score(&pipeXaxis_2, &pipeTopHeight_2);
  Score(&pipeXaxis_3, &pipeTopHeight_3);

// Falling bird Collision
  if(birdYaxis < 0 || birdYaxis > SCREEN_HEIGHT - 12){
    gameOver = true;
  }

  isCollision(pipeXaxis_1, pipeTopHeight_1);
  isCollision(pipeXaxis_2, pipeTopHeight_2);
  isCollision(pipeXaxis_3, pipeTopHeight_3);

// Saving HighScore
  if(score > highScore){
    saveHighScore();
  }

  u8g.firstPage();
    do {
  // drawing commands
    u8g.drawBitmapP(10, birdYaxis, 3, 12, FlappyBird);

    //pipe 1
    u8g.drawBitmapP(pipeXaxis_1, -pipeTopHeight_1, 2, 64, topBottomPipe);
    u8g.drawBitmapP(pipeXaxis_1, (64-(pipeTopHeight_1-pipeGap)), 2, 64, topBottomPipe);

    //pipe 2
    u8g.drawBitmapP(pipeXaxis_2, -pipeTopHeight_2, 2, 64, topBottomPipe);
    u8g.drawBitmapP(pipeXaxis_2, (64-(pipeTopHeight_2-pipeGap)), 2, 64, topBottomPipe);

    //pipe 3
    u8g.drawBitmapP(pipeXaxis_3, -pipeTopHeight_3, 2, 64, topBottomPipe);
    u8g.drawBitmapP(pipeXaxis_3, (64-(pipeTopHeight_3-pipeGap)), 2, 64, topBottomPipe);

    u8g.setFont(u8g_font_5x7);
    u8g.drawStr(0, 6, "Scrore: ");
    sprintf(bufferSC, "%d", score); // number → text
    u8g.drawStr(40, 6, bufferSC);

    u8g.setFont(u8g_font_5x7);
    u8g.drawStr(90, 6, "Hi: ");
    sprintf(bufferHi, "%d", highScore); // number → text
    u8g.drawStr(110, 6, bufferHi);
  } while (u8g.nextPage());

}
