#include <stm32f031x6.h>
#include "display.h"
#include "musical_notes.h"

#define SIZE 8

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
volatile uint32_t milliseconds;
void initTimer(void);
void playNote(uint32_t Freq,uint32_t duration);
void playTune(uint32_t freqs[], uint32_t times[], uint32_t count);
void playTune2(uint32_t freqs[], uint32_t times[], uint32_t count);
void playTune3(uint32_t freqs[], uint32_t times[], uint32_t count);
void playTune4(uint32_t freqs[], uint32_t times[], uint32_t count);
void eputchar(char c);
char egetchar(void);
void eputs(char *String);
void SerialBegin(void);
void pauseOn(void);
void pauseOff(void);
void overOn(void);
void overOff(void);
void levelOn(void);
void levelOff(void);

#define BW 16
#define BH 9
#define BAT_WIDTH 10
#define MAX_BRICI_LEVELS 4

uint32_t ball_x;
uint32_t ball_y;
uint32_t bat_x;
uint32_t bat_y;

typedef struct  {
	uint16_t colour;
	uint16_t x;
	uint16_t y;	
	uint16_t visible;
} block_t;

#define YELLBLK 0x1f0e
#define GREENBLK 0x001f
#define BLUEBLK 0x00f8
#define ORANGEBLK 0xff00
#define BLOCKCOUNT 36
block_t Blocks[BLOCKCOUNT] = {
{ YELLBLK, 0, 18, 0 }, { YELLBLK, 16, 18, 0 }, { YELLBLK, 32, 18, 0 }, { YELLBLK, 48, 18, 0 }, { YELLBLK, 64, 18, 0 }, { YELLBLK, 80, 18, 0}, { YELLBLK, 96, 18, 0 }, { YELLBLK, 112, 18, 0 }, 
{ GREENBLK, 0, 27, 0 }, { GREENBLK, 16, 27, 0 }, { GREENBLK, 32, 27, 0 }, { GREENBLK, 48, 27, 0 }, { GREENBLK, 64, 27, 0 }, { GREENBLK, 80, 27, 0 }, { GREENBLK, 96, 27, 0 }, { GREENBLK, 112, 27, 0 },
{ BLUEBLK, 0, 36, 0 }, { BLUEBLK, 16, 36, 0 }, { BLUEBLK, 32, 36, 0 }, { BLUEBLK, 48, 36, 0 }, { BLUEBLK, 64, 36, 0}, { BLUEBLK, 80, 36, 0 }, { BLUEBLK, 96, 36, 0 }, { BLUEBLK, 112, 36, 0 },
{ ORANGEBLK, 0, 45, 0 }, { ORANGEBLK, 16, 45, 0 }, { ORANGEBLK, 32, 45, 0 }, { ORANGEBLK, 48, 45, 0 }, { ORANGEBLK, 64, 45, 0}, { ORANGEBLK, 80, 45, 0 }, { ORANGEBLK, 96, 45, 0 }, { ORANGEBLK, 112, 45, 0 },

};
  
void hideBlock(uint32_t index);
void showBlock(uint32_t index);
void hideBall(void);
void showBall(void);
void moveBall(uint32_t newX, uint32_t newY);
void hideBat(void);
void showBat(void);
void moveBat(uint32_t newX, uint32_t newY);
int blockTouching(int Index,uint16_t ball_x,uint16_t ball_y);
int UpPressed(void);
int DownPressed(void);
int LeftPressed(void);
int RightPressed(void);
int Paused = 1; //initalise paused variable to 1
void randomize(void);
uint32_t random(uint32_t lower, uint32_t upper);
void playBrici(void);
uint32_t sound_duration = 0;
//Array of notes and times for intro tune
uint32_t myTune[] = {E6,B6,E6,B6,E6,C6,E6,G6,G5};
uint32_t myTime[] = {100,50,100,50,100,300,300,300,300};
//Array of notes and times for level Up Tune
uint32_t levelUpTune[]= {E4,G4,E5,C5,D5,G5};
uint32_t levelUpTime[]= {100,100,100,100,100,100};
//Array of notes and times for when you lose
uint32_t looseLevelTune[]= {C4,CS4_Db4,D4,B4,F4,F4,F4,E5,D4,C4,E3,E3,C3};
uint32_t looseLevelTime[]= {100,100,100,300,100,100,300,300,300,300,200,200,300};
//Array of notes and times for the victory tune
uint32_t victoryTune[]={E4,D4,E4,D4,E4,C4,A4,AS4_Bb4,A4,C5,F5};
uint32_t victoryTime[]={100,50,100,50,100,100,100,100,100,100,100};
uint32_t note_index = 0;


int main()
{
	initClock();
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	initSysTick();
	initTimer();
	display_begin();
	SerialBegin();
	//invertDisplay();// Uncomment this line if you have a display with a blue PCB
	pinMode(GPIOB,0,1); //MAKE GPIOB bit 0 an output
	pinMode(GPIOB,4,0); //MAKE GPIOB bit 4 an input
	pinMode(GPIOB,5,0); //MAKE GPIOB bit 5 an input
	pinMode(GPIOA,8,0); //MAKE GPIOA bit 8 an input
	pinMode(GPIOA,1,1); //MAKE GPIOA bit 1 an output
	pinMode(GPIOA,0,1); //MAKE GPIOA bit 0 an output
	pinMode(GPIOB,3,1); //MAKE GPIOB bit 3 an output
	enablePullUp(GPIOB,0);
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
	//Turn off all LED's on start
	pauseOff();
	overOff();
	levelOff();
	while(1)
	{
		playBrici();
	}
}
void playBrici()
{
	//Turn off teh game over led when the game restarts
	overOff();
	//Ascci art using the eputs function to display on start
	eputs("===========================================================\t\r\n");
	eputs(" ________  ________  ___  ________  ___ \t\r\n");
	eputs("|\\   __  \\|\\   __  \\|\\  \\|\\   ____\\|\\  \\ \t\r\n");
	eputs("\\ \\  \\|\\ /\\ \\  \\|\\  \\ \\  \\ \\  \\___|\\ \\  \\ \t\r\n");
	eputs(" \\ \\   __  \\ \\   _  _\\ \\  \\ \\  \\    \\ \\  \\ \t\r\n");
	eputs("  \\ \\  \\|\\  \\ \\  \\\\  \\\\ \\  \\ \\  \\____\\ \\  \\ \t\r\n");
	eputs("   \\ \\_______\\ \\__\\\\ _\\\\ \\__\\ \\_______\\ \\__\\ \t\r\n");
	eputs("    \\|_______|\\|__|\\|__|\\|__|\\|_______|\\|__| \t\r\n");
	eputs("\t\r\n");
	eputs("===========================================================\t\r\n");
	eputs("WELCOME TO BRICI. COPYRIGHT 2023. ALL RIGHTS RESERVED.\t\r\n");
	eputs("===========================================================\t\r\n");
	eputs("\t\r\n");
	eputs("  __________________________\t\r\n");
  eputs(" |                          |\t\r\n");
  eputs(" | .----------------------. |\t\r\n");
  eputs(" | |  .----------------.  | |\t\r\n");
  eputs(" | |  |   ,_,     ,_,  |  | |\t\r\n");
  eputs(" | |  |  (.,.)   (o,o) |  | |\t\r\n");
  eputs(" | |  |  (   )   (   ) |  | |\t\r\n");
  eputs(" | |  |---|-|-----|-|--|  | |\t\r\n");       
  eputs(" | |  |                |  | |\t\r\n");
  eputs(" | |  |                |  | |\t\r\n");
  eputs(" | |  |	   WELCOME!!   |  | |\t\r\n");
  eputs(" | |  |       TO       |  | |\t\r\n");
  eputs(" | |  |     BRICI!!    |  | |\t\r\n");
  eputs(" | |  |                |  | |\t\r\n");
  eputs(" | |  '----------------'  | |\t\r\n");
  eputs(" | |__BRICI BOY___________//|\t\r\n");
  eputs(" |          ________        |\t\r\n");
  eputs(" |    .      (BRICI)        |\t\r\n");
  eputs(" |  _| |_              .-.  |\t\r\n");
  eputs(" |-[_   _]-       .-. (   ) |\t\r\n");
  eputs(" |   |_|         (   ) '-'  |\t\r\n");
  eputs(" |    '           '-'   A   |\t\r\n");
  eputs(" |                 B        |\t\r\n");
  eputs(" |          ___   ___       |\t\r\n");
  eputs(" |         (___) (___)  ,., |\t\r\n");
  eputs(" |        select start ;:;: |\t\r\n");
  eputs(" |                    ,;:;'//\t\r\n");
  eputs(" |                   ,:;:'.'\t\r\n");
  eputs(" '-----------------------`\t\r\n");
	eputs("\t\r\n");
	eputs("   ______________________________\t\r\n");
	eputs(" / \\                             \\\t\r\n");
	eputs("|   |                            |\t\r\n");
	eputs(" \\_ |        INSTRUCTIONS        |\t\r\n");
	eputs("    |                            |\t\r\n");
	eputs("    |    MOVE THE PLATFORM TO    |\t\r\n");
	eputs("    |  SAVE THE BALL USING THE   |\t\r\n");
	eputs("    |   BUTTONS OR THE A/D KEYS  |\t\r\n");
	eputs("    |      ON YOUR KEYBORD       |\t\r\n");
	eputs("    |                            |\t\r\n");
	eputs("    |  USE THE DOWN BUTTON OR S  |\t\r\n");
	eputs("    |    KEY TO PAUSE THE GAME   |\t\r\n");
	eputs("    |                            |\t\r\n");
	eputs("    |     USE THE UP BUTTON TO   |\t\r\n");
	eputs("    |   INCREASE THE BALL COUNT  |\t\r\n");
	eputs("    |                            |\t\r\n");
	eputs("    |        GOOD LUCK !!        |\t\r\n");
	eputs("    |   _________________________|___\t\r\n");
	eputs("    |  /                            /\t\r\n");
	eputs("    \\_/____________________________/\t\r\n");
	eputs("\t\r\n");
	eputs("  ___    __    __  __  ____    ____    __   ____   __   \t\r\n");
	eputs(" / __)  /__\\  (  \\/  )( ___)  (  _ \\  /__\\ (_  _) /__\\ \t\r\n");
	eputs("( (_-. /(__)\\  )    (  )__)    )(_) )/(__)\\  )(  /(__)\\ \t\r\n");
	eputs(" \\___/(__)(__)(_/\\/\\_)(____)  (____/(__)(__)(__)(__)(__)\t\r\n");
	eputs("\t\r\n");
	
	//Function call for PlayTune function 
	playTune(myTune,myTime,9); //FREQS,TIMES,COUNT
	
	
  int Level = MAX_BRICI_LEVELS;
  int LevelComplete = 0;
  unsigned int BallCount = 5;
  unsigned int Index;
  int32_t BallXVelocity = 1;
  int32_t BallYVelocity = 1;
  // Blank the screen to start with
  fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  bat_x = 20;
  bat_y = SCREEN_HEIGHT - 20;	
  
	//Display intro messages on the LCD with different colours using rgb values
  printText("Welcome to Brici", 10, 40, RGBToWord(51, 255, 221), RGBToWord(0, 0, 0));
	printText("Press Left", 30, 50, RGBToWord(255, 51, 187), RGBToWord(0, 0, 0));
	printText("     ", 30, 60, RGBToWord(15, 4, 4), RGBToWord(0, 0, 0));
	printText("For more inform-", 10, 70, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
	printText("ation, Check the", 10, 80, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
	printText("serial output.", 20, 90, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
	
	while(!LeftPressed());
	randomize();  
	ball_x = random(0,SCREEN_WIDTH);
  ball_y = random(50,(bat_y - 10));  	
	// draw the red squares indicating the number of remaining lives.
	for (Index = BallCount; Index > 0; Index--)
		fillRectangle(SCREEN_WIDTH - Index * 8, SCREEN_HEIGHT-10, 7, 7, RGBToWord(0xff, 0xf, 0xf));
  while (Level > 0)
  {
    moveBall(random(10, SCREEN_WIDTH-10), SCREEN_HEIGHT/2);
    if (random(0,2) > 0) // "flip a coin" to choose ball x velocity
      BallXVelocity = 1;
    else
      BallXVelocity = -1;
    LevelComplete = 0;
    BallYVelocity = -1;  // initial ball motion is up the screen
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_WIDTH, 0); // clear the screen
		// draw the blocks.
    for (Index=0;Index<BLOCKCOUNT;Index++)
		{
			showBlock(Index);
		}
		showBall();
		showBat();
    printText("Level", 5, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
    printNumber(MAX_BRICI_LEVELS - Level + 1, 45, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
    while (!LevelComplete)
    {
      if (RightPressed())
      {
        // Move right
        if (bat_x < (SCREEN_WIDTH - BAT_WIDTH))
        {
          moveBat(bat_x + 2, bat_y); // Move the bat faster than the ball
        }
      }	
			//IF statement to pause the game if the downPressed function returns a value (e.g. if the down button is pressed)
			if (DownPressed ())
			{
				//Resume the game if the game is already paused and the button is pressed	
				if (Paused == 0)
				{
					Paused = 1;
					//Function call to turn off the pause indicator LED (yellow)
					pauseOff();
				}
				//ELSE IF to pause the game if the button is pressed and the game is ongoing
				else if (Paused == 1)
				{
					Paused = 0;
					//Calling the function to turn on the pause indicator LED (yellow)
					pauseOn();
					//ASCII ART to display while game is paused
					eputs("     ________________________\t\r\n");
					eputs("    /                        \\\t\r\n");
					eputs("    |  YOU PAUSED THE GAME!  |\t\r\n");
					eputs("    |                        |\t\r\n");
					eputs("    |   To resume the game,  |\t\r\n");
					eputs("    |  press the down button |\t\r\n");
					eputs("    |  or press the |S| key  |\t\r\n");
					eputs("    |      at any time!      |\t\r\n");
					eputs("    \\_______________________/\t\r\n");
					eputs("         ___           \t\r\n");
					eputs(" _(\\    |@@|                  \t\r\n");
					eputs("(__/\\__ \\--/ __               \t\r\n"); 
					eputs("   \\___|----|  |   __         \t\r\n");
					eputs("       \\ }{ /\\ )_ / _\\        \t\r\n");
					eputs("       /\\__/\\ \\__O (__         \t\r\n");
					eputs("      (--/\\--)    \\__/         \t\r\n");
					eputs("      _)(  )(_                 \t\r\n");
					eputs("     `---''---`                \t\r\n");
				}
				//Keep the game paused when the down button is pressed so it doesn't have to be held down
				while (DownPressed());
			}
			
				if (Paused == 0)
				{
					continue;
				}
			//IF statement to add extra balls if the downPressed function returns a value (e.g. if the up button is pressed)
			if (UpPressed())
				BallCount++;

      if (LeftPressed())
      {
        // Move left
        if (bat_x > 0)
        {
          moveBat(bat_x - 2, bat_y); // Move the bat faster than the ball
        }
      }
      if ((ball_y == bat_y) && (ball_x >= bat_x) && (ball_x <= bat_x + BAT_WIDTH))
      {
        BallYVelocity = -BallYVelocity;
      }
      showBat(); // redraw bat as it might have lost a pixel due to collisions

			moveBall(ball_x+BallXVelocity,ball_y+BallYVelocity);
      
      if (ball_x == 2)
			{
				BallXVelocity = -BallXVelocity;
				//Play note and display text to standard output if the ball hits the LEFT WALL
				playNote(2349, 20);
				eputs("YOUT HIT THE LEFT WALL!\r\n");
				
			}
        
      if (ball_x == SCREEN_WIDTH - 2)
			{
				BallXVelocity = -BallXVelocity;
				//Play note and display text to standard output if the ball hits the RIGHT WALL
				playNote(2349, 20);
				eputs("YOU HIT THE RIGHT WALL!\r\n");
			}
        
      if (ball_y == 2)
			{
				BallYVelocity = -BallYVelocity;
				//Play note and display text to standard output if the ball hits the TOP
				playNote(932, 20);
				eputs("YOU HIT THE TOP!\r\n");
			}
        

      if (ball_y >= bat_y+3)  // has the ball pass the bat vertically?
      {
				//Play note,remove ball and display text to standard output if you lose a ball
        BallCount--;
				playNote(1760,20);
				eputs("YOU LOST THE BALL!\r\n");
				//If you have no balls left, end the game
        if (BallCount == 0)
        {
					//Function call to turn on the game over undicator LED (red)
					overOn();
          fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
					playTune2(looseLevelTune,looseLevelTime,13);
					//ASCII ART that displays to serial out when the game is over
					eputs("  ________                                                   \t\r\n");
					eputs(" /  _____/_____    _____   ____     _______  __ ___________  \t\r\n");
					eputs("/   \\  ___\\__  \\  /     \\_/ __ \\   /  _ \\  \\/ // __ \\_  __ \\ \t\r\n");
					eputs("\\    \\_\\  \\/ __ \\|  Y Y  \\  ___/  (  <_> )   /\\  ___/|  | \\/ \t\r\n");
					eputs(" \\______  (____  /__|_|  /\\___  >  \\____/ \\_/  \\___  >__|    \t\r\n");
					eputs("        \\/     \\/      \\/     \\/                   \\/       \t\r\n");
					eputs("\t\r\n");
          printText("GAME OVER!!!", 40, 100, RGBToWord(0xff, 0xff, 0xff), 0);
          printText("Up to restart", 18, 120, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0));
          while (!UpPressed());
          return;
        }
				
        if (random(0,100) & 1)
          BallXVelocity = 1;
        else
          BallXVelocity = -1;

        BallYVelocity = -1;
        moveBall(random(10, SCREEN_WIDTH - 10), random(90, 120));
        fillRectangle(SCREEN_WIDTH-5*15, SCREEN_HEIGHT-10, 120, 10, 0);
        for (Index = BallCount; Index > 0; Index--)
					fillRectangle(SCREEN_WIDTH - Index * 8, SCREEN_HEIGHT-10, 7, 7, RGBToWord(0xff, 0xf, 0xf));
      }
			// check for ball hitting blocks and if this level is done.
			LevelComplete = 1;
      for (Index = 0; Index < BLOCKCOUNT; Index++)
      {
        int touch = blockTouching(Index,ball_x,ball_y);
        if (touch)
        {
					//Display 
					eputs("YOU HIT A BLOCK!\r\n");
          hideBlock(Index);
          if ( (touch == 1) || (touch == 3) )
            BallYVelocity = -BallYVelocity;
          if ( (touch == 2) || (touch == 4) )
            BallXVelocity = -BallXVelocity;
        }
				if (Blocks[Index].visible) // any blocks left?
          LevelComplete = 0;
      }
      // No Blocks left, Move to next level.
      if ((LevelComplete == 1) && (Level > 0))
      {
        Level--;
        printText("Level",5, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
        printNumber(MAX_BRICI_LEVELS - Level + 1, 45, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
				//Call function to turn on the next level indicator LED (blue)
				levelOn();
				//ASCII ART for the next level
				eputs("\t\r\n");
				eputs(" ___________________");
				eputs("\t\r\n");
				eputs("/                    \\ \t\r\n");
				eputs("|     NEXT LEVEL!!   |\t\r\n");
				eputs("\\___________________/\t\r\n");
				eputs("\t\r\n");
				eputs("       c~~p ,---------. \t\r\n");
				eputs("  ,---'oo  )           \\\t\r\n");
				eputs(" ( O O                  )/\t\r\n");
				eputs("  `=?='                 /\t\r\n");
				eputs("        \\    ,     .   /\t\r\n");
				eputs("        \\  |-----'|  /\t\r\n");
				eputs("        ||__|    |_|__|\t\r\n");
				eputs("\t\r\n");
				//Call function to play the next level tune 
				playTune3(levelUpTune,levelUpTime,6);//PARAMETERS:FREQS,TIMES,COUNT
				//Call function to turn on next level indicator LED (blue)
				levelOff();

      }
			/*Additional if statement to increment the level if the up and down buttons are pressed simultaenously by calling the upPressed/downPressed
			(debug/cheat code)*/
			else if (UpPressed()&& DownPressed())
			{
				Level--;
        printText("Level",5, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
        printNumber(MAX_BRICI_LEVELS - Level + 1, 45, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
				//Call function to turn on the next level indicator LED (blue)
				levelOn();
				//ASCII ART for the next level
				eputs("\t\r\n");
				eputs(" ___________________");
				eputs("\t\r\n");
				eputs("/                    \\ \t\r\n");
				eputs("|     NEXT LEVEL!!   |\t\r\n");
				eputs("\\___________________/\t\r\n");
				eputs("\t\r\n");
				eputs("       c~~p ,---------. \t\r\n");
				eputs("  ,---'oo  )           \\\t\r\n");
				eputs(" ( O O                  )/\t\r\n");
				eputs("  `=?='                 /\t\r\n");
				eputs("        \\    ,     .   /\t\r\n");
				eputs("        \\  |-----'|  /\t\r\n");
				eputs("        ||__|    |_|__|\t\r\n");
				eputs("\t\r\n");
				//Call function to play the next level tune 
				playTune3(levelUpTune,levelUpTime,6);
				//Call function to turn off the next level indicator LED (blue)
				levelOff();
			}
      delay(10+Level*5); // Slow the game to human speed and make it level dependant.
    }
  }
	//ASCII ART is the user wins the game
	eputs("__     __             ____   _____   U  ___ u   ____     __   __   \t\r\n");
	eputs(" \\ \\   /\"/u  ___    U /\"___| |_ \" _|   \\/\"_ \\/U |  _\"\\ u  \\ \\ / /  \t\r\n");
	eputs("  \\ \\ / //  |_\"_|   \\| | u     | |     | | | | \\| |_) |/   \\ V /   \t\r\n");
	eputs("  /\\ V /_,-. | |     | |/__   /| |\\.-,_| |_| |  |  _ <    U_|\"|_u  \t\r\n");
	eputs(" U  \\_/-(_/U/| |\\u    \\____| u |_|U \\_)-\\___/   |_| \\_\\     |_|    \t\r\n");
	eputs("   //   .-,_|___|_,-._// \\\\  _// \\\\_     \\\\     //   \\\\_.-,//|(_   \t\r\n");
	eputs("  (__)   \\_)-' '-(_/(__)(__)(__) (__)   (__)   (__)  (__)\\_) (__)  \t\r\n");
	eputs("\t\r\n");
	//Function call to play the victory tune when the user completes the game
	playTune4(victoryTune,victoryTime,10);
  fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_WIDTH, RGBToWord(0, 0, 0xff));
  printText("VICTORY!",40, 100, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0xff));
  printText("Up to restart", 18, 120, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0xff));
  while (!UpPressed());
  return;
}


void initSysTick(void)
{
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7; 
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}

void initClock(void)
{
// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
  // Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
        // inserted into Flash memory interface
				
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));
        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 4);
        // set PLL multiplier to 12 (yielding 48MHz)
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) ); 

        // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
        RCC->CFGR |= (1<<14);

        // and turn the PLL back on again
        RCC->CR |= (1<<24);        
        // set PLL as system clock source 
        RCC->CFGR |= (1<<1);
}
void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}

//function to return an output if a button is pressed using standard input (keyboard)
int keyPressed(void)
{
	//returns 1 if button is pressed (serial port)
	//0 otherwise
	if ( (USART1->ISR & (1 << 5)) == 0)
		return 0;
	else
		return 1;
}
void initTimer()
{
	// Power up the timer module
	RCC->APB1ENR |= (1 << 8);
	pinMode(GPIOB,1,2); // Assign a non-GPIO (alternate) function to GPIOB bit 1
	GPIOB->AFR[0] &= ~(0x0fu << 4); // Assign alternate function 0 to GPIOB 1 (Timer 14 channel 1)
	TIM14->CR1 = 0; // Set Timer 14 to default values
	TIM14->CCMR1 = (1 << 6) + (1 << 5);
	TIM14->CCER |= (1 << 0);
	TIM14->PSC = 48000000UL/65536UL; // yields maximum frequency of 21kHz when ARR = 2;
	TIM14->ARR = (48000000UL/(uint32_t)(TIM14->PSC))/((uint32_t)440);
	TIM14->CCR1 = TIM14->ARR/2;	
	TIM14->CNT = 0;
}

//playNote function to play a tone
void playNote(uint32_t Freq,uint32_t duration)//PARAMETERS: FREQUENCY OF TONE, DURATION OF TONE
{
	TIM14->CR1 = 0; // Set Timer 14 to default values
	TIM14->CCMR1 = (1 << 6) + (1 << 5);
	TIM14->CCER |= (1 << 0);
	TIM14->PSC = 48000000UL/65536UL; // yields maximum frequency of 21kHz when ARR = 2;
	TIM14->ARR = (48000000UL/(uint32_t)(TIM14->PSC))/((uint32_t)Freq);
	TIM14->CCR1 = TIM14->ARR/2;	
	TIM14->CNT = 0;
	TIM14->CR1 |= (1 << 0);
	sound_duration = duration;
}
//playTune function to play a series of notes to make a tune by taking in an array of notes and the times
void playTune(uint32_t freqs[], uint32_t times[], uint32_t count)//PARAMETERS: ARRAY OF FREQS, ARRAY OF TIMES, COUNT OF ITEMS IN THE ARRAYS
{
	uint32_t index=0;
	while(index < count)
	{
		playNote(myTune[index], times[index]);
		delay(myTime[index]);
		index++;
	}
}

//playTune function to play a series of notes to make a tune by taking in an array of notes and the times
void playTune2(uint32_t freqs[], uint32_t times[], uint32_t count)//PARAMETERS: ARRAY OF FREQS, ARRAY OF TIMES, COUNT OF ITEMS IN THE ARRAYS
{
	uint32_t index=0;
	while(index < count)
	{
		playNote(looseLevelTune[index], looseLevelTime[index]);
		delay(looseLevelTime[index]);
		index++;
	}
}

//playTune function to play a series of notes to make a tune by taking in an array of notes and the times
void playTune3(uint32_t freqs[], uint32_t times[], uint32_t count)//PARAMETERS: ARRAY OF FREQS, ARRAY OF TIMES, COUNT OF ITEMS IN THE ARRAYS
{
	uint32_t index=0;
	while(index < count)
	{
		playNote(levelUpTune[index], levelUpTime[index]);
		delay(myTime[index]);
		index++;
	}
}

//playTune function to play a series of notes to make a tune by taking in an array of notes and the times
void playTune4(uint32_t freqs[], uint32_t times[], uint32_t count)//PARAMETERS: ARRAY OF FREQS, ARRAY OF TIMES, COUNT OF ITEMS IN THE ARRAYS
{
	uint32_t index=0;
	while(index < count)
	{
		playNote(victoryTune[index], victoryTime[index]);
		delay(myTime[index]);
		index++;
	}
}

//Function to turn off the pause (yellow) LED
void pauseOff(void)
{
	GPIOA->ODR = GPIOA-> ODR | (1<<1);
}

//Function to turn on the pause (yellow) LED
void pauseOn(void)
{
	GPIOA->ODR = GPIOA-> ODR & ~ (1<<1);
}

//Function to turn off the level up (blue) LED
void levelOff(void)
{
	GPIOA->ODR = GPIOA-> ODR | (1<<0);
}

//Function to turn on the level up (blue) LED
void levelOn(void)
{
	GPIOA->ODR = GPIOA-> ODR & ~ (1<<0);
}

//Function to turn off the game over (red) LED
void overOff(void)
{
	GPIOB ->ODR = GPIOB-> ODR | (1<<3);
}

//Function to turn on the game over (red) LED
void overOn(void)
{
	GPIOB ->ODR = GPIOB-> ODR & ~ (1<<3);
}

void SysTick_Handler(void)
{
	milliseconds++;
	if (sound_duration != 0)
	{
		sound_duration --;	
		if (sound_duration == 0)
		{
			TIM14->CR1 &= ~(1u << 0); // sound off
		}
	}

}
//Function for up pressed
int UpPressed(void)
{
	//IF statement using keypressed function and egetchar to read key from serial input and return 1 when pressed
	if (keyPressed())
	{
		if (egetchar() == 'W')
			return 1;
		else if (egetchar() == 'w')
			return 1;
	}
	//IF statement to return 1 if button is pressed
	if ( (GPIOA->IDR & (1<<8)) == 0)
		return 1;
	else
		return 0;
}
//Function for down pressed
int DownPressed(void)
{
	//IF statement using keypressed function and egetchar to read key from serial input and return 1 when pressed
	if (keyPressed())
	{
		if (egetchar() == 'S')
			return 1;
		else if (egetchar() == 's')
			return 1;
		
	}
	//IF statement to return 1 if button is pressed
	if ( (GPIOA->IDR & (1<<11)) == 0)
		return 1;
	else
		return 0;
}

//Function for left pressed
int LeftPressed(void)
{
	//IF statement using keypressed function and egetchar to read key from serial input and return 1 when pressed
	if (keyPressed())
	{
		if (egetchar() == 'A')
			return 1;
		else if (egetchar() == 'a')
			return 1;
	}
	//IF statement to return 1 if button is pressed
	if ( (GPIOB->IDR & (1<<5)) == 0)
		return 1;
	else
		return 0;	
}

//Function for right pressed
int RightPressed(void)
{
	//IF statement using keypressed function and egetchar to read key from serial input and return 1 when pressed
	if (keyPressed())
	{
		if (egetchar() == 'D')
			return 1;
		else if (egetchar() == 'd')
			return 1;
	}
	//IF statement to return 1 if button is pressed
	if ( (GPIOB->IDR & (1<<4)) == 0)
		return 1;
	else
		return 0;
}


void hideBlock(uint32_t index)
{
	fillRectangle(Blocks[index].x,Blocks[index].y,BW,BH,0);
	Blocks[index].visible = 0;
}
void showBlock(uint32_t index)
{	
	fillRectangle(Blocks[index].x,Blocks[index].y,BW,BH,Blocks[index].colour);
	Blocks[index].visible = 1;
}
void hideBall(void)
{
	fillRectangle(ball_x,ball_y,2,2,0);
}
void showBall(void)
{
	fillRectangle(ball_x,ball_y,2,2,RGBToWord(255,255,0));
}
void moveBall(uint32_t newX, uint32_t newY)
{
	hideBall();
	ball_x = newX;
	ball_y = newY;
	showBall();
}

void hideBat(void)
{
	fillRectangle(bat_x,bat_y,10,3,0);
}
void showBat(void)
{
	fillRectangle(bat_x,bat_y,10,3,RGBToWord(127,127,255));
}
void moveBat(uint32_t newX, uint32_t newY)
{
	hideBat();
	bat_x = newX;
	bat_y = newY;
	showBat();
}
int blockTouching(int Index,uint16_t x,uint16_t y)
{
	
	  // This function returns a non zero value if the object at x,y touches the sprite
    // The sprite is assumed to be rectangular and returns a value as follows:
    // 0 : not hit
    // 1 : touching on top face (lesser Y value)
    // 2 : touching on left face (lesser X value)
    // 3 : touching on bottom face (greater Y value)    
    // 4 : touching on right face (greater X value)
    if (Blocks[Index].visible == 0)
        return 0;
    if ( Blocks[Index].y == ball_y  )
    {  // top face?
      if ( (x>=Blocks[Index].x) && ( x < (Blocks[Index].x+BW) ) )
        return 1;      
    }
    if ( x == Blocks[Index].x )
    {
      // left face
      if ( (y>=Blocks[Index].y) && ( y < (Blocks[Index].y+BH) ) )
        return 2;
    }
    if ( y == (Blocks[Index].y+BH-1)  )
    {  // bottom face?
      if ( (x>=Blocks[Index].x) && ( x < (Blocks[Index].x+BW) ) )
        return 3;      
    }
    if ( x == (Blocks[Index].x + BW-1) )
    {
      // right face
      if ( (y>=Blocks[Index].y) && ( y < (Blocks[Index].y+BH) ) )
        return 4;
    }

    return 0; // not touching
}

static uint32_t prbs_shift_register=0;
void randomize(void)
{
	while(prbs_shift_register ==0) // can't have a value of zero here
		prbs_shift_register=milliseconds;
	
}

//Used to write a string to display to serial output
void eputs(char *String)
{
	while(*String) // keep printing until a NULL is found
	{
		eputchar(*String);
		String++;
	}
}
//eputchar function to enable serial output
void eputchar(char c)
{
	while( (USART1->ISR & (1 << 6)) == 0); // wait for any ongoing
	USART1->ICR=0xffffffff;
	// transmission to finish
	USART1->TDR = c;
}

//egetchar function to enable serial input
char egetchar()
{
	while( (USART1->ISR & (1 << 5)) == 0); // wait for a character
	return (char)USART1->RDR;
}


//SerialBegin function
void SerialBegin()
{
	/* On the nucleo board, TX is on PA2 while RX is on PA15 */
	RCC->AHBENR |= (1 << 17); // enable GPIOA
	RCC->APB2ENR |= (1 << 14); // enable USART1
	pinMode(GPIOA,2,2); // enable alternate function on PA2
	pinMode(GPIOA,15,2); // enable alternate function on PA15
	// AF1 = USART1 TX on PA2
	GPIOA->AFR[0] &= 0xfffff0ff;
	GPIOA->AFR[0] |= (1 << 8);
	// AF1 = USART1 RX on PA2
	GPIOA->AFR[1] &= 0x0fffffff;
	GPIOA->AFR[1] |= (1 << 28);
	// De-assert reset of USART1 
	RCC->APB2RSTR &= ~(1u << 14);
	
	USART1->CR1 = 0; // disable before configuration
	USART1->CR3 |= (1 << 12); // disable overrun detection
	USART1->BRR = 48000000/9600; // assuming 48MHz clock and 9600 bps data rate
	USART1->CR1 |= (1 << 2) + (1 << 3); // enable Transmistter and receiver
	USART1->CR1 |= 1; // enable the UART

}
uint32_t random(uint32_t lower, uint32_t upper)
{
	uint32_t new_bit=0;	
	uint32_t return_value;
	new_bit= ((prbs_shift_register & (1<<27))>>27) ^ ((prbs_shift_register & (1<<30))>>30);
	new_bit= ~new_bit;
	new_bit = new_bit & 1;
	prbs_shift_register=prbs_shift_register << 1;
 
	prbs_shift_register=prbs_shift_register | (new_bit);
	return_value = prbs_shift_register;
	return_value = (return_value)%(upper-lower)+lower;
	return return_value;
}