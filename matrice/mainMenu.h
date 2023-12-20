#include "Arduino.h"
const byte arrows[][8] = {
  { 0b00000,
    0b00100,
    0b01010,
    0b10001,
    0b00000,
    0b00000,
    0b00000,
    0b00000 
  },
  { 0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b10001,
    0b01010,
    0b00100,
    0b00000 
  },
  { 0b00100,
    0b01010,
    0b10001,
    0b00000,
    0b00000,
    0b10001,
    0b01010,
    0b00100 
  },

  { 0b00000,
    0b00000,
    0b00010,
    0b00100,
    0b01000,
    0b00100,
    0b00010,
    0b00000 
  },
  { 0b00000,
    0b00000,
    0b01000,
    0b00100,
    0b00010,
    0b00100,
    0b01000,
    0b00000 }
};

const uint8_t Squares[][8] = {
  { 0b00000,
    0b11111,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b11111,
    0b00000 },
  { 0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00000 }
};

String const mainMenuOptions[] = { "START", "SETTINGS", "HIGH SCORE", "GAME INFO", "ABOUT", "Difficulty" };

int currentOption = 0;
const int firstPosition = 0;
const int lastPosition = sizeof(mainMenuOptions) / sizeof(String) - 1;

const int arrowUp = 0;
const int arrowDown = 1;
const int arrowBoth = 2;
const int arrowLeft = 3;
const int arrowRight = 4;

const int delayMainMenu = 1500;
const int messageDelay = 1000;
const int defaultLetter = 65;
const int defaultPoints = 0;

String const settingsOptions[] = { "LCD Brightness", "Matrix Brigtness", "Set Name", "Reset HS","Sound On/Off"};
int currentSetting = 0;


const int lastSettingsPosition = sizeof(settingsOptions) / sizeof(String) - 1;

int smallLet1;
int smallLet2;
int smallLet3;
int currentLetter = 0;
int name[3] = {65,65,65};




void SetMatrixBrightness();
void enterSettings();
void setName();
void resetHighscore();
void soundSettings();
void displayLCDBr();
void readScore();
void displayMatrix(const uint64_t image);


void mainMenu()
{
  inMainMenu = true;
  inGame = false;
  inSettings = false;
  inSetName = false;
  inAbout = false;

  // menuMatrix();
  displayMatrix(MenuImage);
  lcd.createChar(3, arrows[arrowBoth]);
  lcd.createChar(2, arrows[arrowUp]);
  lcd.createChar(1, arrows[arrowDown]);

  lcd.clear();
  lcd.setCursor(4, 0);  //title
  lcd.print("MAIN MENU");

  lcd.setCursor(1, 1);  //current option
  lcd.print(">");
  lcd.print(mainMenuOptions[currentOption]);

  lcd.setCursor(14, 1);
  //print up or down arrow
  if (currentOption == firstPosition) {
    lcd.write(1);
  } else if (currentOption == lastPosition) {
    lcd.write(2);
  } else {
    lcd.write(3);
  }
  delay(100);

}
void about();
void displayScoreInH();

void autoScroll(int characters)
{
    for (int i = 0; i < characters; i++) {  //autoscroll to left
    lcd.scrollDisplayLeft();
    delay(400);
  }
}

void displayInfo()
{
  displayMatrix(AboutImage);
  int characters = 20;
  int delayPrint = 400;
  lcd.clear();
  lcd.setCursor(3, 0);
  //print title
  lcd.print(mainMenuOptions[currentOption]);
  delay(5*delayPrint);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Move the player with the joystick.");
  autoScroll(characters);
  delay(delayPrint);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Press switch button to place a bomb");
  autoScroll(characters);
  delay(delayPrint);
  mainMenu();
  
}



void displayDifficulty()
{
  inDiff = true;
  lcd.createChar(4, Squares[0]);  // empty
  lcd.createChar(5, Squares[1]);  

  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print(mainMenuOptions[currentOption]);
  lcd.setCursor(2,1);

  if (hard) {
    lcd.write(4);
    lcd.print("Easy");

    lcd.write(" ");
    lcd.write(5);
    lcd.print("Hard");
  } else {
    lcd.write(5);
    lcd.print("Easy");

    lcd.write(" ");
    lcd.write(4);
    lcd.print("Hard");
  }
  inMainMenu = false;

}

void enterMainMenu()
{
  if(currentOption == 0)
  {
    inMainMenu = false;
    inGame = true;
    inSettings = false;
    inSetName = false;
    inAbout = false;
  }
  else if(currentOption == 1)
  {
    enterSettings();
  }
  else if(currentOption == 4)
  {
    about();
  }else if (currentOption ==2) {
    displayScoreInH();
  }
  else if(currentOption ==3)
  {
    displayInfo();
  }
  else if(currentOption == 5)
  {
    displayDifficulty();
  }
}



void moveToRight()
{
    if (currentSetting == 1) // matrix brightnesss
    {
       
      if (matrixIntensityLevel < 5)
        {matrixIntensityLevel ++; }
      SetMatrixBrightness(); 
          
    }
    else if(currentSetting == 4)
    {
      if(buzzerOn == true)
        buzzerOn = ! buzzerOn;
      soundSettings();
    }
    else if(currentOption==5)
    {
      if(!hard)
      {
        hard = !hard;
      }
      displayDifficulty();
    }
    
}

void moveToLeft()
{
  if(currentSetting == 1)
  {
    if(matrixIntensityLevel > 0)
      matrixIntensityLevel --;
    SetMatrixBrightness();
  }
  else if(currentSetting == 4)
  {
    if(buzzerOn == false)
    {
      buzzerOn = ! buzzerOn;
      tone(buzzerPin,buzzerTone,200);
    }
        
    soundSettings();
  }
  else if(currentOption == 5)
  {
    if(hard)
      hard = !hard;
    displayDifficulty();
  }
}

void resetHighscore()
{
  for(int i = 3; i<=lastPosInMem; i=i+4)
  {
    EEPROM.update(i,defaultLetter);
    EEPROM.update(i+1,defaultLetter);
    EEPROM.update(i+2,defaultLetter);
    EEPROM.update(i+3,defaultPoints);
  } 
}

void setName()
{
  inSettings = false;
  //create arrows
  lcd.createChar(3, arrows[arrowBoth]);
  lcd.createChar(4, arrows[arrowLeft]);
  lcd.createChar(5, arrows[arrowRight]);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Set name & press");

  lcd.setCursor(5, 1);
  //The current letter will be uppercase, the rest will be lowercase.
  if (currentLetter == 0) {  
    smallLet1 = name[0];
    smallLet2 = name[1] + 32;
    smallLet3 = name[2] + 32;

  } else if (currentLetter == 1) {  

    smallLet1 = name[0] + 32;
    smallLet2 = name[1];
    smallLet3 = name[2] + 32;
  } else {  

    smallLet1 = name[0] + 32;
    smallLet2 = name[1] + 32;
    smallLet3 = name[2];
  }
  char first = smallLet1; //convert to String
  char second = smallLet2;
  char third = smallLet3;

  lcd.write(4);
  lcd.print(first);
  lcd.print(second);
  lcd.print(third);
  lcd.write(5);

  lcd.setCursor(13, 1);
  lcd.write(3);
  lcd.setCursor(6 + int(currentLetter),1);
  lcd.cursor();
}

void lcdMatrixBrightness(int level) {

  lcd.createChar(4, Squares[0]);  // empty
  lcd.createChar(5, Squares[1]);  //full


  //title
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(settingsOptions[currentSetting]);

  lcd.setCursor(4, 1);
  lcd.print("-");
  for (int lvl = 0; lvl < 6; lvl++) {
    if (level >= 0) {
      lcd.write(5);
    } else {
      lcd.write(4);
    }
    level--;
  }
  lcd.print("+");

  lcd.setCursor(15, 1);
  lcd.write(3);
}



void setLCDBrightness(){
  inLCDBr = true;    
  inSettings = false;  
  brightnessAnalogLevel = analogRead(brightnessAnalogPin);
  brightnessLevel = map(brightnessAnalogLevel, 0, 1024, 0, 255);
  analogWrite(brightnessPin, brightnessLevel); // make change on lcd
  Serial.println(brightnessLevel);
  //update in EEPROM
  EEPROM.update(brightnessLevelAddress, brightnessLevel);
  brightnessLevel = EEPROM.read(brightnessLevelAddress);
  //update in EEPROM

    
    
}

void soundSettings()
{
  lcd.createChar(4, Squares[0]);  // empty
  lcd.createChar(5, Squares[1]);  //full
  inSound = true;
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print(settingsOptions[currentSetting]);
  lcd.setCursor(2,1);
  //Print empty square or full square depending on the case
  if (buzzerOn) {
    Serial.println("true");
    lcd.write(5);
    lcd.print("ON");

    lcd.write(" ");
    lcd.write(4);
    lcd.print("OFF");
  } else {
    Serial.println("false");
    lcd.write(4);
    lcd.print("ON");

    lcd.write(" ");
    lcd.write(5);
    lcd.print("OFF");
  }
  //To avoid entering the Settings case again on button press.
  inSettings = false;
}

void resetWarning()
{
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Are you sure?");
  inReset = true;
  inSettings = false;
}

void enterSettingsOptions(int currentSetting)
{
  if(currentSetting == 0)
  {
    displayLCDBr();
    setLCDBrightness();
  }
  else if (currentSetting == 1)
  {
    SetMatrixBrightness();
  } 
  else if(currentSetting == 2)
  {
    setName();
    inSetName = true;
  }
  else if(currentSetting == 3)
  {
    resetWarning();
  }
  else if(currentSetting == 4)
  {
    soundSettings();
  }
}
void about()
{
  lcd.clear();
  lcd.setCursor(5, 0);


  delay(delayMainMenu / 2);
  int delayOnScroll = 400;
  int characters = 25;
  displayMatrix(AboutImage);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Title: Bomberman");  //about
  lcd.setCursor(1, 1);
  lcd.print("Author: Marin Iulian, Github: Iulianm05");

  for (int I = 0; I < characters; I++) {  // autoscroll to left

    lcd.scrollDisplayLeft();

    delay(delayOnScroll);
  }

  delay(delayMainMenu);

  mainMenu();
}

void SetMatrixBrightness()
{
  inMatrixBr = true;
  lcdMatrixBrightness(matrixIntensityLevel);       
  lc.setIntensity(0,intensityLevels[matrixIntensityLevel]);
  for(int i=0;i<8;i++)
  {
    for(int j=0; j<8; j++)
    {
      lc.setLed(0, i, j, 1);
    }
  }

  //update in EEPROM
  EEPROM.update(matrixIntensityLevelAddress, matrixIntensityLevel);
  brightnessLevel = EEPROM.read(matrixIntensityLevelAddress);
  inSettings = false;
}

void displayLCDBr()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Rotate the");
  lcd.setCursor(0,1);
  lcd.print("potentiometer");
}

void displayScoreInH() {

  readScore();
  int indexInHighscore = 0;
  displayMatrix(HighscoreImage);

  //print on display the top 3
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("HIGHSCORE");  //title

  delay(2000);


  int lastPosArray = 3;
  for (int index = 0; index < lastPosArray; index++) {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("HIGHSCORE");

    lcd.setCursor(3, 1);
    lcd.print(index + 1);  //place index(1,2,3)
    lcd.print(". ");
    lcd.print(highscoreNames[indexInHighscore]);  //name
    lcd.print(highscoreNames[indexInHighscore + 1]);
    lcd.print(highscoreNames[indexInHighscore + 2]);
    lcd.print(" ");
    lcd.print(highscore[index]);  //score

    delay(2000);
    //adding 3 for the next name
    indexInHighscore += 3;
  }

  mainMenu();  // return to main menu
}


void enterSettings()
{
  inMainMenu = false;
  inSettings = true;
  inGame = false;
  inSetName = false;
  inAbout = false;
  displayMatrix(SettingImage);
  lcd.clear();
  lcd.setCursor(2, 0);  //title
  lcd.print("SETTINGS MENU");

  lcd.setCursor(0, 1);  //current option
  lcd.print(">");
  lcd.print(settingsOptions[currentSetting]);

  lcd.setCursor(15, 1);
  //print up or down arrow
  if (currentSetting == firstPosition) {
    lcd.write(1);
  } else if (currentSetting == lastSettingsPosition) {
    lcd.write(2);
  } else {
    lcd.write(3);
  }
  delay(100);
}
