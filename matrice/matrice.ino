#include "LedControl.h" // need the library
#include <LiquidCrystal.h>
#include <EEPROM.h>
const byte dinPin = 12;
const byte clockPin = 6;
const byte loadPin = 10;
const byte matrixSize = 8;
const byte brightnessPin = 5;
const byte brightnessAnalogPin = A2;
const int pinY = A0; // A0 - Analog pin connected to X output
const int pinX = A1; // A1 - Analog pin connected to Y output
const int pinSW = 3; // Digital pin connected to switch output
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 13;
const byte d6 = 2;
const byte d7 = 4;
const byte buzzerPin = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


int timeToDetonate = 3000;

const uint64_t WinImage = 0xc6eefed6c6c6c600; //for display W
const uint64_t Lose = 0x7e06060606060600; //for display L
const uint64_t SettingImage = 0x07070f1830e0e0e0;
const uint64_t MenuImage = 0x0814220000221408;
const uint64_t AboutImage = 0x000c000c1830331e;
const uint64_t HighscoreImage = 0x181818183c7effff;

bool joyMoved = false;

int minThreshold = 400;
int maxThreshold = 700;

int xValue;
int yValue;

bool possiblePress = false;
bool shouldStartCounter = false;

byte buttonState = HIGH;
byte swState = LOW;
byte lastSwState = LOW;

int buttonPressed = 0;
int lastBtnPress = 0;

unsigned long currentMillis = 0;
unsigned long lastMillis = 0;

unsigned long lastDebounceTime = 0;
unsigned long BlastMillis = 0;

unsigned long lastDetonateMillis = 0;

bool Lost = false;
bool Win = false;
bool notYet = false;

bool bombHasDetonated = true;

byte state = HIGH;
byte bState = HIGH;




long lastTime = 0;
long lastJoyTime = 0;
int interval = 50;
byte reading = LOW;
byte joyState = LOW;
byte lastReading = LOW;
byte joyReading = LOW;
byte lastJoyReading = HIGH;

bool hard = true;

int lastPosInMem = 14;
int highscore[3];
char highscoreNames[10];

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER
byte matrixBrightness = 2;
byte matrix[16][16] = {
    {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

byte matrix2[16][16] = {
    {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int offsetX = 0;
int offsetY = 0;




int score = 0;
int lifes = 3;


bool inMainMenu = true; //default 
bool inGame = false;
bool inSettings = false;
bool inSetName = false;
bool inAbout = false;
bool inReset = false;
bool inSound = false;
bool inLCDBr = false;
bool inDiff = false;
bool startGame = false;

int buzzerTone = 600;

bool buzzerOn = true;
bool inMatrixBr = false;

int brightnessLevel;
const int brightnessLevelAddress = 1;
const int matrixIntensityLevelAddress = 2;
const int buzzerAddress = 15;
const int difficultyModeAddress = 16;
int intensityLevels[] = {1,3,6,9,12,15};
int brightnessAnalogLevel;
int matrixIntensityLevel;

char gameovermsg[] = "You lost"; 
char gameWin[] = "You win";

#include "mainMenu.h"

void displayMap();

struct coordinate{
  int x;
  int y;
  coordinate(){}
  //for left, right, up, down
  coordinate(int _x, int _y){
    x = _x;
    y = _y;
  }
};

void moveOnMap(int &offset, int &coordinate);

void checkMatrix();
// To move the player on the map
coordinate left(-1,0);
coordinate right(1,0);
coordinate up(0,-1);
coordinate down(0,1);

void moveOnMap(int &offset, int &coordinate, int nr){
      offset+=nr;
      coordinate -=nr;
      displayMap();
}

class Bomb{
  coordinate bCoordinates;
  int delayBlink;

public:
  //Initialize the coordinates in the constructor to avoid accessing residual values for the first placed bomb.
  Bomb(){
    delayBlink = 100;
    this->bCoordinates.x = -1;
    this->bCoordinates.y = -1;
  }
  Bomb(int x, int y)
  {
    this->bCoordinates.x = x;
    this->bCoordinates.y = y;
    delayBlink = 100;
  }
  void spawnBomb(){
    lc.setLed(0,this->bCoordinates.y - offsetY, this->bCoordinates.x - offsetX, bState);//Blink led
    if(millis() - BlastMillis > this->delayBlink)
    {
      bState=!bState;
      BlastMillis = millis();
    }
  }
  void detonateBomb()
  {
    //Turn off all the LEDs that are in the area of the bomb.
    if((millis() - lastDetonateMillis > timeToDetonate) && bombHasDetonated == false){
      
      for (int i = this->bCoordinates.x-1; i <= this->bCoordinates.x+1; i++)
        for(int j = this->bCoordinates.y-1; j <= this->bCoordinates.y+1; j++)
        {
          if(matrix[j][i] == 1)
          {
            lc.setLed(0, j, i, LOW);
            matrix[j][i] = 0;
            score++;
          }

        }
      bombHasDetonated = true;
      
    }
  }
  //setter
  void setCoordinates(int x, int y){
    this->bCoordinates.x = x;
    this->bCoordinates.y = y;
  }
  //getter
  coordinate getBombCoordinate(){
    return bCoordinates;
  }
};

class Player{
  coordinate pCoordonates;
  int delayBlink;
  Bomb bombs;
  bool placeBomb = false;

public:
  Player(int x, int y){
    this->pCoordonates.x = x;
    this->pCoordonates.y = y;
    delayBlink = 600;
    
  }
  void blinkPlayer(){
  lc.setLed(0,this->pCoordonates.y, this->pCoordonates.x, state);
  if(millis() - lastMillis > this->delayBlink)
  {
    state=!state;
    lastMillis = millis();
  }
  }
  void movePlayer(){
    xValue = analogRead(pinX);
    yValue = analogRead(pinY);
    this->blinkPlayer();
    checkMatrix();
    // Spawn the bomb only when the player has left the placement location.
    if(placeBomb == true && (bombs.getBombCoordinate().x !=this->pCoordonates.x + offsetX || bombs.getBombCoordinate().y != this->pCoordonates.y + offsetY) && bombHasDetonated ==false){
        bombs.spawnBomb();
        bombs.detonateBomb();
        checkPLayer();
    }
    //Move the player on the map
    if(xValue<minThreshold && joyMoved == false&&matrix[this->pCoordonates.y + offsetY + left.y][this->pCoordonates.x+ offsetX +left.x] == 0 && this->pCoordonates.x + offsetX>0)
    {
      
      move(left);
      
    }else{
      if(xValue>maxThreshold && joyMoved == false&&matrix[this->pCoordonates.y + offsetY + right.y][this->pCoordonates.x +offsetX +right.x] == 0 && this->pCoordonates.x + offsetX<2*matrixSize-1)
      {
      move(right);    
      }else {
        if(yValue>maxThreshold && joyMoved == false && matrix[this->pCoordonates.y + offsetY + up.y][this->pCoordonates.x + offsetX + up.x] == 0 && this->pCoordonates.y + offsetY>0){
          move(up);
        }else{
          if(yValue<minThreshold && joyMoved == false && matrix[this->pCoordonates.y +offsetY + down.y][this->pCoordonates.x + offsetX + down.x] == 0 && this->pCoordonates.y + offsetY <2*matrixSize-1){
            move(down);
        }
        } 
      }
    }
    //reset joyMoved
    if(xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold && joyMoved == true){
      joyMoved = false;
   }
   checkBomb();
  }
  void move(coordinate direction){
    state = HIGH;
    lc.setLed(0,this->pCoordonates.y, this->pCoordonates.x, LOW);
    this->pCoordonates.x +=direction.x;
    this->pCoordonates.y += direction.y;
    //The player will move with one led back
    if(this->pCoordonates.x > matrixSize/2 && offsetX<matrixSize)
    {
      // offsetX++;
      // this->pCoordonates.x -=direction.x;
      // displayMap();
      moveOnMap(offsetX, this->pCoordonates.x, 1);
    }else if(this->pCoordonates.x < matrixSize/2 && offsetX > 0)
    {
      // offsetX --; 
      // this->pCoordonates.x -=direction.x;
      // displayMap();
      moveOnMap(offsetX, this->pCoordonates.x, -1);
    }else if(this->pCoordonates.y > matrixSize/2 && offsetY<matrixSize)
    {

      moveOnMap(offsetY, this->pCoordonates.y, 1);
    }else if(this->pCoordonates.y < matrixSize/2 && offsetY > 0)
    {

      moveOnMap(offsetY, this->pCoordonates.y, -1);
    }
    joyMoved = true;
  }  

  void checkBomb()
  {
    if (possiblePress) {
      // Start the debounce counter if needed
      if (shouldStartCounter) {
        lastDebounceTime = millis();
        shouldStartCounter = false;
      }
      if((millis() - lastDebounceTime) > 50){
        swState = digitalRead(pinSW);
        if (swState != buttonState) {
          
          if (swState == LOW) {
            buttonPressed++;
          }
          possiblePress = false;
        }
      }
      
    }

    lastSwState = swState;
    if(buttonPressed != lastBtnPress && bombHasDetonated == true){
      //Turn off previous bomb
      if(bombs.getBombCoordinate().x!=-1 && bombs.getBombCoordinate().y != -1)
        lc.setLed(0, bombs.getBombCoordinate().y, bombs.getBombCoordinate().x, LOW);
      lastDetonateMillis = millis();
      this->bombs.setCoordinates(this->pCoordonates.x+offsetX,this->pCoordonates.y + offsetY);
      placeBomb = true;
      lastBtnPress = buttonPressed;
      bombHasDetonated = false;
      
    }

  }
  void checkPLayer(){
    //At the moment of detonation, we check if the player is in the bomb's damage area.
    if(bombHasDetonated == true){
      for (int i = this->bombs.getBombCoordinate().x - offsetX -1 ; i <= this->bombs.getBombCoordinate().x - offsetX + 1; i++)
        for (int j = this->bombs.getBombCoordinate().y - offsetY -1 ; j <= this->bombs.getBombCoordinate().y- offsetY +1; j++)
        {
          if(this->pCoordonates.x == i && this->pCoordonates.y ==j){
            lifes--;
            if(lifes == 0)
              {
                Lost = true;
                this->pCoordonates.x = 0;
                this->pCoordonates.y = 0;
                offsetX = 0;
                offsetY = 0;
                lifes = 3;
                
              }
          }
        }
    }
  }
};

void resetMatrix()
{
  for(int i = 0; i < 16; i++)
  {
    for(int j = 0; j < 16; j++)
      matrix[i][j]=matrix2[i][j];
  }
}

Player myplayer(0,0);

void introMessage()
{
  lcd.begin(16, 2);
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print("Bomberman");
  delay(2000);
  lcd.clear();
}

void displayLCD(char msg[])
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg);
  lcd.setCursor(0,1);
  lcd.print("Your score is:" + String(score));
  delay(2000);
}

void displayMatrix(const uint64_t image){
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, bitRead(row, j));
    }
  }
  
}

void displayMap()
{
    for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row+offsetY][col+offsetX]);
    }
  }
}

void handleInterrupt() {
  possiblePress = true; // Indicate that a button press might have occurred
  shouldStartCounter = true; // Indicate the need to start the debounce counter
}
//check if the player win the game
void checkMatrix(){
  for(int i=0;i<2*matrixSize;i++)
    for(int j=0; j<2*matrixSize; j++){
      if(matrix[i][j] == 1)
        notYet = true;
    }
  if(notYet == false)
  {
    Win = true;
    
  }
  notYet = false;

}

void displayScore()
{
  lcd.clear();
  lcd.setCursor(8,0);
  lcd.print("Score:" + String(score));
  lcd.setCursor(0,0);
  lcd.print("Lifes:" + String(lifes));
  lcd.setCursor(8,1);
  lcd.print("N:" + String(static_cast<char>(name[0])) + String(static_cast<char>(name[1])) + String(static_cast<char>(name[2])));
  lcd.setCursor(0,1);
  if(hard)
    lcd.print("HARD");
  else lcd.print("EASY");
}

void putPlayerInHighscore()
{
  int Index = 0;
  while(Index <3 && highscore[Index]>score)
  {
    Index++;
  }
  // Index++;
  int playerAddress = 3 + Index * 4;
  for(int i = lastPosInMem; i > playerAddress; i = i-4)
  {
    int playerBefore = i - 4;
    int let1 = EEPROM.read(playerBefore - 3);
    int let2 = EEPROM.read(playerBefore - 2);
    int let3 = EEPROM.read(playerBefore - 1);
    int plScore = EEPROM.read(playerBefore);

    EEPROM.update(i - 3, let1);
    EEPROM.update(i - 2, let2);
    EEPROM.update(i - 1, let3);
    EEPROM.update(i, plScore);

  }
  // playerAddress++;
  //insert the new player
  EEPROM.update(playerAddress, name[0]);
  EEPROM.update(playerAddress + 1, name[1]);
  EEPROM.update(playerAddress + 2, name[2]);
  EEPROM.update(playerAddress + 3, score);
}

void displayHighScore()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You are in");
  lcd.setCursor(0, 1);
  lcd.print("Highscore");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Congratulation!");
  delay(2000);
}

void checkHighscore()
{
  if(newHighscore())
  {
    displayHighScore();
    readScore();
    putPlayerInHighscore();
    score = 0;
  }
}

void playGame()
{
  if(!startGame)
    displayMap();
  if(!Lost)
  {
    if(!Win)
    {
      displayScore();
      myplayer.movePlayer();
    } 
    else {
      displayMatrix(WinImage);
      displayLCD(gameWin);
      checkHighscore();
      Win = !Win;
      startGame = false;
      // checkHighscore();
      score = 0;
      mainMenu();
    }
    delay(10); 
  }
  else{
    displayMatrix(Lose);
    displayLCD(gameovermsg);
    checkHighscore();
    resetMatrix();
    Lost = !Lost;
    startGame = false;
    score = 0;
    // checkHighscore();
    mainMenu();
  }
}


void sound()
{
  if(buzzerOn)
    tone(buzzerPin,buzzerTone,200);
}





void readFromJoystick()
{
   if(inGame)
  {
    playGame();
  }
  else
  {
    //joystick button (pressed or not)
  reading = digitalRead(pinSW);

  if (reading != lastReading) {
    lastTime = millis();
  }
  if (millis() - lastTime > interval) {
    if (reading != swState) {
      swState = reading;
      if (swState == LOW) {
        if (inMainMenu) {  //we are in main menu
          enterMainMenu();
        }
        else if(inSettings)
        {
          enterSettingsOptions(currentSetting);

        }
        else if(inSetName)
        {
          inSettings = true;
          enterMainMenu();
          lcd.noCursor(); //hide the cursor
        }
        else if(inReset)
        {
          //back to main menu
          inSettings = true;
          inReset = false;
          resetHighscore();
          enterMainMenu();
        }
        else if(inSound)
        {
          //back to main menu
          inSettings = true;
          inSound = false;
          EEPROM.update(buzzerAddress,buzzerOn);
          enterMainMenu();
        }
        else if(inMatrixBr){
          inSettings = true;
          inMatrixBr = false;
          enterMainMenu();

        }
        else if(inLCDBr)
        {
          inSettings = true;
          inLCDBr = false;
          enterMainMenu();
        }
        else if(inDiff)
        {
          inDiff=false;
          if(!hard)
            timeToDetonate = 3000;
          else 
            timeToDetonate = 800;
          EEPROM.update(difficultyModeAddress, hard);
          mainMenu();
        }

        
      }
    }
  }
  //If I'm in the LCD Brightness menu, I can rotate the potentiometer to adjust the LCD brightness.
  if(inLCDBr)
  {
     setLCDBrightness();
  }
  lastReading = reading;
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  


  if (yValue > maxThreshold && joyMoved == false) {  //up
    joyMoved = true;
    //scroll up
    if (inMainMenu) {  //we are in main menu
      if (currentOption > firstPosition) {
        currentOption--;
        mainMenu();
        sound();
      }
    }else if(inSettings)
    {
      if (currentSetting > firstPosition) {
        currentSetting--;
        enterSettings();
        sound();
      }
    }else if(inSetName)
    {
      //change the letters in the name
      if(name[currentLetter < 90])
      {
        name[currentLetter]++;
      }
      else {
        name[currentLetter] = 65;
      
      }
      setName();
    }
  }
  if (yValue < minThreshold && joyMoved == false) {  //down
    joyMoved = true;
    if (inMainMenu) {  //we are in main menu
      if (currentOption < lastPosition) {
        currentOption++;
        mainMenu();
        sound();
      }
      
    }else if(inSettings)
    {
      if (currentSetting < lastSettingsPosition) {
        currentSetting++;
        enterSettings();
        sound();
      }
    }
    else if(inSetName)
    {
      if(name[currentLetter] > 65)
      {
        name[currentLetter]--;
      }else{
        name[currentLetter] = 90;
      }
      setName();
    }
  

  }
    if (xValue > maxThreshold && joyMoved == false) {
      //move to right or back in menu
      joyMoved = true;
      if(inMatrixBr)
      {
        moveToRight();
      }
      
      else if(inSettings)
      {  
        mainMenu();
        currentSetting = 0;
      }
      else if (inSetName) {
        if(currentLetter < 3)
          currentLetter++;
        else
          currentLetter = 0;
        setName();
      } else if(inSound || inDiff)
      {
        moveToRight();

      } 
    }

    if(xValue < minThreshold && joyMoved == false)
    {
      joyMoved = true;
      if(inMatrixBr)
      {
        moveToLeft();
      }
      if(inSetName)
      {
        if(currentLetter > 0)
        {
          currentLetter--;
        }
        else {
          currentLetter = 2;
        }
        setName();
      }else if(inSound || inDiff)
      {
        moveToLeft();
      }

    }
    //If the joystick is in its normal position, we reset joyMoved.
    if(xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold && joyMoved == true){
      joyMoved = false;
   }
   delay(20);
  }
  
}

void readScore() {
  int indexInHighscore = 0;
  int i = 0;
  //"The high score in memory starts from position 3.
  for (int index = 3; index <=  lastPosInMem; index = index + 4) {  
                                                             

    //read the first 3 addresses starting from the first one to take the saved names.
    int letter1 = EEPROM.read(index);
    int letter2 = EEPROM.read(index + 1);
    int letter3 = EEPROM.read(index + 2);

    //read the score
    //letters in char
    char first = letter1;
    char second = letter2;
    char third = letter3;


    highscore[i] = EEPROM.read(index + 3);  //score
    highscoreNames[indexInHighscore] = first;
    highscoreNames[indexInHighscore + 1] = second;
    highscoreNames[indexInHighscore + 2] = third;

    indexInHighscore += 3;
    i++;
  }
}

bool newHighscore()
{
  int lastScore = EEPROM.read(14);
  if(score > lastScore)
  {
    return true;
  }else return false;
}


void setup() {
  Serial.begin(9600);
  pinMode(pinSW, INPUT_PULLUP);
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.clearDisplay(0);// clear screen
  brightnessLevel = EEPROM.read(brightnessLevelAddress);
  analogWrite(brightnessPin, brightnessLevel);
  matrixIntensityLevel = EEPROM.read(matrixIntensityLevelAddress);
  lc.setIntensity(0,intensityLevels[matrixIntensityLevel]);
  buzzerOn=EEPROM.read(buzzerAddress);
  hard = EEPROM.read(difficultyModeAddress);
  if(hard)
  {
    timeToDetonate = 800;
  }
  introMessage();
  mainMenu();
  displayMatrix(MenuImage);
  attachInterrupt(digitalPinToInterrupt(pinSW), handleInterrupt, FALLING);
}

void loop() {
  readFromJoystick();
}



