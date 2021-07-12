//Arduman by Seth A. Robinson (rtsoft.com)

//Thanks to: http://fuopy.github.io/arduboy-image-converter/

#include <Arduboy2.h>
//#include <ArduboyPlaytune.h>
#include <ArduboyTones.h>

#include "level_bitmaps.h"
#include "Player.h"
#include "Ghost.h"
#include "Music.h"
#include "ardu_main.h"
#include "Highscores.h"
#include "Fruit.h"

//function declarations added by Seth because we're a real .cpp file now.  Also changed loop() to main_loop and setup() to
//main_setup(), as our simple Arduboy.ino file calls them

//Highscore code taken from the Breakout example (by nootropic design?)

//***********************

Arduboy2 arduboy;
//ArduboyPlaytune tunes(arduboy.audio.enabled);
ArduboyTones sound(arduboy.audio.enabled);
Player player;
uint8_t g_level = 1;

GAME_MODE g_mode = MODE_MENU;

void DrawScore()
{
  arduboy.setCursor(40, 6);
  arduboy.print("Score:");
  printNumberFancy(55, 16, player.m_score, true, false);
}

void StartGame()
{
  arduboy.initRandomSeed();
  g_mode = MODE_PLAYING;
  //tunes.playScore(level_intro);
  ClearAndRedrawLevel();
  arduboy.display();

#ifndef SKIP_INTRO
  delay(1000);

  DrawScore();
  arduboy.setCursor(30, 46);
  arduboy.print("Level ");
  arduboy.print(g_level);

  arduboy.display();
  delay(1500);
  ClearAndRedrawLevel();
  //DrawScore();
  delay(300);

  /*
    arduboy.setCursor(25,46);
    arduboy.print("Get ready!");
    arduboy.display();
    delay(1000);
  */
#endif
  ClearAndRedrawLevel();
  ResetGhostPositions(true);
  arduboy.display();
  g_mode = MODE_PLAYING;
}

void ClearAndRedrawLevel()
{
  arduboy.clear();
  arduboy.drawBitmap(0, 0, level, WIDTH, HEIGHT, 1);
  player.Render();
  fruit.RenderSideFruits();
}

void main_setup()
{
  // audio setup
  //tunes.initChannel(PIN_SPEAKER_1);
  //tunes.initChannel(PIN_SPEAKER_2);

  arduboy.begin();

  if (!arduboy.audio.enabled())
  {
    //if I don't do this, audio plays even though it shouldn't
    arduboy.audio.off();
    arduboy.audio.saveOnOff();
  }
  //arduboy.boot();         // if we wanted to skip the arduboy title
  //arduboy.audio.begin();

  arduboy.setFrameRate(20);
  //arduboy.clear();
}

void PlayingGame()
{
  player.UnRender();
  fruit.UnRender();

  for (int i = 0; i < GHOST_COUNT; i++)
  {
    ghosts[i].UnRender();
  }

  player.Update();
  fruit.Update();

  for (int i = 0; i < GHOST_COUNT; i++)
  {
    ghosts[i].Update();
  }

  player.Render();
  fruit.Render();

  for (int i = 0; i < GHOST_COUNT; i++)
  {
    ghosts[i].Render(false);
  }

  player.OnEndOfLoop();
  arduboy.display();

  if (player.LevelPassed())
  {
    //tunes.playScore(music_finish_level);
    g_level++;
    delay(3500);
    player.ResetLevel();
    StartGame();
  }
}
uint8_t g_selection = 0;

void DrawMenuSelection(char *pText, uint8_t id, uint8_t curSel, uint8_t x, uint8_t y)
{
  arduboy.setCursor(x, y);
  if (id == curSel) arduboy.print("[ ");
  arduboy.print(pText);
  if (id == curSel) arduboy.print(" ]");
}

enum eMainMenu
{ MENU_PLAY,
  MENU_HIGHSCORES,
  MENU_AUDIO_TOGGLE,
  //add more options above here
  MENU_COUNT
};

void DrawMenu()
{
  arduboy.clear();
  arduboy.setTextSize(2);
  arduboy.setCursor(5, 5);
  arduboy.println("arduman");
  arduboy.setTextSize(1);
  //menu
  DrawMenuSelection("Play", MENU_PLAY, g_selection, 5, 25);
  DrawMenuSelection("High Scores", MENU_HIGHSCORES, g_selection, 5, 35);

  if (arduboy.audio.enabled())
  {
    DrawMenuSelection("Sound is: ON", MENU_AUDIO_TOGGLE, g_selection, 5, 45);
  } else
  {
    DrawMenuSelection("Sound is: OFF", MENU_AUDIO_TOGGLE, g_selection, 5, 45);
  }
  arduboy.display();
}

void StartMenu()
{
  DrawMenu();

  uint8_t oldChoice = g_selection;
  if (arduboy.pressed(UP_BUTTON))
  {
    g_selection = --g_selection % MENU_COUNT;
  }

  if (arduboy.pressed(DOWN_BUTTON))
  {
    g_selection = ++g_selection % MENU_COUNT;
  }

  if (arduboy.pressed(A_BUTTON))
  {
    DrawMenu();
    sound.tone(623, 20);
    delay(200);

    if (g_selection == MENU_AUDIO_TOGGLE)
    {
      if (arduboy.audio.enabled())
      {
        arduboy.audio.off();
      } else
      {
        arduboy.audio.on();
      }
      arduboy.audio.saveOnOff();
      return;
    }
    if (g_selection == MENU_PLAY)
    {
      g_mode = MODE_START;
      return;
    }

    if (g_selection == MENU_HIGHSCORES)
    {
      g_mode = MODE_HIGHSCORES;
      return;
    }
  }
  if (g_selection != oldChoice)
  {
    DrawMenu();
    sound.tone(523, 20);
    delay(200);
  }
}

void main_loop()
{
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;

  //arduboy.pollButtons();

  switch (g_mode)
  {
    case MODE_START:
      StartGame();
      break;
    case MODE_PLAYING:
      PlayingGame();
      break;
    case MODE_MENU:
      player.ResetGame();
      StartMenu();
      break;
    case MODE_HIGHSCORES:
      displayHighScores(EEPROM_HIGH_SCORE_SAVE_SLOT);
      break;

  }
}
