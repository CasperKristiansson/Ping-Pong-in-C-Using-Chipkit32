/* mipslabwork.c
   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall
   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):
   This file modified 2017-04-31 by Ture Teknolog 
   Thias file modified 2021-03-02 by Casper Kristiansson and Hampus Nilsson
   For copyright and licensing, see file COPYING */

#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t display[32][128];
uint8_t oled_display[512];

//Menu variables
float current_menu = 1;
float settings_menu = 0;

//Game specific variables
float game_active = 0;
float game_mode = 0;
float score_player1 = 0;
float score_player2 = 0;
int ai_difficulty = 16; // 4Easy, 2Medium, 1Hard
float player = 0;
int light_counter = 0;
int player_lives = 3;
//Paddle specific variables
float paddle_height = 8;
float paddle_width = 3;
float paddle_speed = 1;

float paddle1_xPos = 0;
float paddle1_yPos = 32 / 2 - 2;

float paddle2_xPos = 128 - 4;
float paddle2_yPos = 32 / 2 - 2;

float paddle2_up = 0;
float paddle2_down = 0;
float paddle1_up = 0;
float paddle1_down = 0;


//Ball specific variables
float ball_size = 2;
float ball_speedx = 1;
float ball_speedy = 0;

int ball_xPos = 128 / 2 - 5;
int ball_yPos = 32 / 2;
int count = 0;

int i = 0;
int j = 0;
int k = 0;
int m = 0;




/* Interrupt Service Routine */
void user_isr( void )
{
  return;
}

/* Lab-specific initialization goes here */
void labinit( void )
{
  volatile int *trise = (volatile int *)0xbf886100;
  TRISECLR = 0xff;  // Set as output (LED:S)
  TRISDSET = 0xfe0; // Set as input (BTN 2-4, SW 1-4)
  TRISFSET = 0x2;   // Set as input (BTN 1)
  PORTE = 0x0;      // LED:s
}

// Set array slots to ones or zeros in the 2D array.
void setPixelArray(int xPos, int yPos, int width, int height) {
  int row, column;

  for (row = 0; row < 32; row++) {
    for (column = 0; column < 128; column++) {
      if (row >= yPos && row <= (yPos + height) && column >= xPos && column <= (xPos + width)) {
        display[row][column] = 1;
      }
    }
  }
}

// Translate the 2D array into the 1D array with 512 slots.
void translateToImage() {
  int page, column, row, c, k;
  uint8_t powerOfTwo = 1;
  uint8_t oledNumber = 0;
  int survivalMode = 0;

  for(page = 0; page < 4; page++) {
    for(column = 0; column < 128; column++) {
      powerOfTwo = 1;
      oledNumber = 0;

      for(row = 0; row < 8; row++) {
        if(display[8 * page + row][column]) {
          oledNumber |= powerOfTwo;
        }
        powerOfTwo <<= 1;
      }
      oled_display[column + page * 128] = oledNumber;
    }
  }
}

void clearDisplay() {
  int row, column, i;

  for(row = 0; row < 32; row++) {
    for(column = 0; column < 128; column++) {
      display[row][column] = 0;
    }
  }

  for (i = 0; i < 512; i++) {
    oled_display[i] = 0;
  }
}

void menu() {
  delay(200);
  if(current_menu == 1) {
    display_string(0, "> Two Player");
    display_string(1, "  One Player");
    display_string(2, "  Settings");

    if (getbtns() & 0x1) {
      reset_game();
      game_mode = 1;
      game_active = 1;
      string_clear();
    }
  }
  else if(current_menu == 2) {
    display_string(0, "  Two Player");
    display_string(1, "> One Player");
    display_string(2, "  Settings");

    if (getbtns() & 0x1) {
      reset_game();
      game_mode = 2;
      game_active = 1;
      string_clear();
    }
  }
  else if(current_menu == 3) {
    display_string(0, "  Two Player");
    display_string(1, "  One Player");
    display_string(2, "> Settings");

    if (getbtns() & 0x1) {
      settings_menu = 1;
      current_menu = 1;
      string_clear();
    }
  }

  else if(current_menu == 4) {
    display_string(0, "  One Player");
    display_string(1, "  Settings");
    display_string(2, "> Score Mode");

    if (getbtns() & 0x1) {
      reset_game();
      game_mode = 3;
      game_active = 1;
      string_clear();
    }
  }

  if ((getbtns() & 0x2) && (current_menu < 4)) {
    current_menu++;
  }
  if ((getbtns() & 0x4) && (current_menu > 1)) {
    current_menu--;
  }

  display_update();
}

void menu_settings() {
  delay(200);
  if(current_menu == 1) {
    display_string(0, ">  Easy");
    display_string(1, "   Normal");
    display_string(2, "   Hard");

    if (getbtns() & 0x1) {
      ai_difficulty = 4;
      settings_menu = 0;
      current_menu = 1;
      string_clear();
    }
  }
  else if (current_menu == 2) {
    display_string(0, "   Easy");
    display_string(1, ">  Normal");
    display_string(2, "   Hard");

    if (getbtns() & 0x1) {
      ai_difficulty = 3;
      settings_menu = 0;
      current_menu = 1;
      string_clear();
    }
  }
  else if (current_menu == 3) {
    display_string(0, "   Easy");
    display_string(1, "   Normal");
    display_string(2, ">  Hard");

    if (getbtns() & 0x1) {
      ai_difficulty = 1;
      settings_menu = 0;
      current_menu = 1;
      string_clear();
    }
  }

  if ((getbtns() & 0x2) && (current_menu < 3)) {
    current_menu++;
  }
  if ((getbtns() & 0x4) && (current_menu > 1)) {
    current_menu--;
  }

  display_update();
}

void player_movement_two(btns) {
  paddle2_up = 0;
  paddle2_down = 0;
  paddle1_up = 0;
  paddle1_down = 0;

  if ((btns & 0x1) && (paddle2_yPos < (32 - paddle_height))) {
    paddle2_yPos += paddle_speed;
    paddle2_up = 1;
  }
  if ((btns & 0x2) && (paddle2_yPos > 0)) {
    paddle2_yPos -= paddle_speed;
    paddle2_down = 1;
  }
  if ((btns & 0x4) && (paddle1_yPos < (32 - paddle_height))) {
    paddle1_yPos += paddle_speed;
    paddle1_up = 1;
  }
  if ((btns & 0x8) && (paddle1_yPos > 0)) {
    paddle1_yPos -= paddle_speed;
    paddle1_down = 1;
  }
}

void player_movement_one(btns) {
  paddle1_up = 0;
  paddle1_down = 0;
  if ((btns & 0x4) && (paddle1_yPos < (32 - paddle_height))) {
    paddle1_yPos += paddle_speed;
    paddle1_down = 1;
  }
  if ((btns & 0x8) && (paddle1_yPos > 0)) {
    paddle1_yPos -= paddle_speed;
    paddle1_up = 1;
  }
}

void paddle1_physics() {
  if(paddle1_up == 1) {
    if(ball_speedy < 0) {
      ball_speedy = ball_speedy * 2;
    }
    else if(ball_speedy > 0) {
      ball_speedy = ball_speedy / 2;
    }
    else {
      ball_speedy = -1;
    }
  }

  if(paddle1_down == 1) {
    if(ball_speedy < 0) {
      ball_speedy = ball_speedy / 2;
    }
    else if(ball_speedy > 0) {
      ball_speedy = ball_speedy * 2;
    }
    else {
      ball_speedy = 1;
    }
  }
  
  if((ball_yPos + ball_size > paddle1_yPos) && (ball_yPos <= paddle1_yPos + 2)){
      ball_speedy += -0.7;
  }

  if((ball_yPos > paddle1_yPos + 2) && (ball_yPos <= paddle1_yPos + 4)){
      ball_speedy += -0.35;
  }

  if((ball_yPos > paddle1_yPos + 4) && (ball_yPos <= paddle1_yPos + 6)){
      ball_speedy += 0.35;
  }

  if((ball_yPos > paddle1_yPos + 6) && (ball_yPos - ball_size < paddle1_yPos + 8)){
      ball_speedy += 0.7;
  }

}

void paddle2_physics() {
  if(paddle2_up == 1) {
    if(ball_speedy < 0) {
      ball_speedy = ball_speedy * 2;
    }
    else if(ball_speedy > 0) {
      ball_speedy = ball_speedy / 2;
    }
    else {
      ball_speedy = -1;
    }
  }

  else if(paddle2_down == 1) {
    if(ball_speedy < 0) {
      ball_speedy = ball_speedy / 2;
    }
    else if(ball_speedy > 0) {
      ball_speedy = ball_speedy * 2;
    }
    else {
      ball_speedy = 1;
    }
  }

  if((ball_yPos + ball_size > paddle2_yPos) && (ball_yPos <= paddle2_yPos + 2)){
      ball_speedy += -0.7;
  }

  if((ball_yPos > paddle2_yPos + 2) && (ball_yPos <= paddle2_yPos + 4)){
      ball_speedy += -0.35;
  }
  if((ball_yPos > paddle2_yPos + 4) && (ball_yPos <= paddle2_yPos + 6)){
      ball_speedy += 0.35;
  }
  if((ball_yPos > paddle2_yPos + 6) && (ball_yPos - ball_size < paddle2_yPos + 8)){
      ball_speedy += 0.7;
  }
  
}


void paddle_hit() {
  if (ball_xPos == paddle_width) {
    if (((ball_yPos + ball_size) > paddle1_yPos) && (ball_yPos - ball_size) < (paddle1_yPos + paddle_height)) {
      ball_speedx = -(ball_speedx);
      paddle1_physics();
    }
  }
  else if(ball_xPos < -21) {
    goal(2);
  }

  if (ball_xPos == (128 - paddle_width - 4)) {
    if (((ball_yPos + ball_size) > paddle2_yPos) && (ball_yPos - ball_size) < (paddle2_yPos + paddle_height)) {
      ball_speedx = -(ball_speedx);
      paddle2_physics();
    }
  }
  else if (ball_xPos > 138) {
    goal(1);
  }
}

void lives_animation(int lives_left){
  switch(lives_left){
    case 3:
      display_string(2, "Lives left: 3");
      display_update();
      delay(500);
      string_clear();
      delay(200);
      display_string(2, "Lives left: 3");
      display_update();
      delay(500);
      string_clear();
      delay(200);
      display_string(2, "Lives left: 3");
      display_update();
      delay(500);
      string_clear();
      delay(200);
      display_string(2, "Lives left: 2");
      display_update();
      delay(800);
      break;

      case 2:
      display_string(2, "Lives left: 2");
      display_update();
      delay(500);
      string_clear();
      delay(200);
      display_string(2, "Lives left: 2");
      display_update();
      delay(500);
      string_clear();
      delay(200);
      display_string(2, "Lives left: 2");
      display_update();
      delay(500);
      string_clear();
      delay(200);
      display_string(2, "Lives left: 1");
      display_update();
      delay(800);
      break;

      case 1:
      display_string(2, "Lives left: 1");
      display_update();
      delay(500);
      string_clear();
      delay(200);
      display_string(2, "Lives left: 1");
      display_update();
      delay(500);
      string_clear();
      delay(200);
      display_string(2, "Lives left: 1");
      display_update();
      delay(500);
      string_clear();
      delay(200);
      display_string(2, "Lives left: 0");
      display_update();
      delay(800);
      break;
  }
}

void goal(player) {
  //For score mode
  if((player == 1) && (game_mode == 3)){
    score_player1 += 1;
    PORTE += 1;
    if(score_player1 <= 3){
      ai_difficulty = 4;
    }
    else if((score_player1 > 3) && (score_player1 < 5)){
      ai_difficulty = 3;
    }
    else if(score_player1 > 5){
      ai_difficulty = 1;
    }
  }
  if((player == 2) && (game_mode == 3)){
    
    if(player_lives == 3){
      lives_animation(3);
      player_lives--;
    }
    else if(player_lives == 2){
      lives_animation(2);
      player_lives--;
    }
    else if(player_lives == 1){
     lives_animation(1);
     player_lives--;
    }
    else if(player_lives == 0){
      ai_win();
    }
  }
  else if (player == 1) {
    score_player1 += 1;
    display_string(2, "Player 1 scored!");
    display_update(); 
    delay(2000);
  }
  else if (player == 2) {
    score_player2 += 1;
    display_string(2, "Player 2 scored!"); 
    display_update();
    delay(2000);
  }
  if(game_mode != 3){
    if (score_player1 == 1) {
      PORTE |= 0x80;
    }
    if (score_player1 == 2){
      PORTE |= 0xC0;
    }
    if (score_player1 == 3){
      PORTE |= 0xE0;
    }
    if (score_player1 == 4){
      PORTE |= 0xF0;
      player1_win();
    }

    if (score_player2 == 4){
      PORTE |= 0x8;
      player2_win();
    }
    if (score_player2 == 3){
      PORTE |= 0x4;
    }
    if (score_player2 == 2){
      PORTE |= 0x2;
    }
    if (score_player2 == 1){
      PORTE |= 0x1;
    }
  }

  reset_game();
}

void player1_win(){
  display_string(2, "Player 1 wins!");
  display_update();
  PORTE = 0;
  for(light_counter = 256; light_counter >= 1; light_counter/=2){
    delay(150);
    PORTESET = light_counter;
  }
  delay(100);
  PORTE = 0;
  delay(300);
  PORTESET = 511;

  delay(1000);
  quit();

}

void player2_win(){
  display_string(2, "Player 2 wins!");
  display_update();
  PORTE = 1;
  for(light_counter = 2; light_counter < 256; light_counter*=2){
    delay(150);
    PORTESET = light_counter;
  }
  delay(100);
  PORTE = 0;
  delay(300);
  PORTESET = 511;

  delay(1000);
  quit();
}
void ai_win(){
  display_string(2, "AI wins");
  display_update();
  PORTE = 0;
  for(light_counter = 256; light_counter >= 1; light_counter/=2){
    delay(150);
    PORTESET = light_counter;
  }
  delay(100);
  PORTE = 0;
  delay(300);
  PORTESET = 511;

  delay(1000);
  quit();
}
void quit() {
  delay(3000);
  PORTE = 0x0;
  game_active = 0;
  game_mode = 0;
  score_player1 = 0;
  score_player2 = 0;

  clearDisplay();
  translateToImage();
  display_image(0, oled_display);
}

void reset_game() {
  i = 0;
  j = 0;
  k = 0;
  m = 0;
  paddle1_xPos = 0;
  paddle1_yPos = 32 / 2 - 2;

  paddle2_xPos = 128 - 4;
  paddle2_yPos = 32 / 2 - 2;

  ball_xPos = 128 / 2 - 5;
  ball_yPos = 32 / 2;

  if (score_player1 > score_player2) {
    ball_speedx = -1;
  }
  else if (score_player1 < score_player2) {
    ball_speedx = 1;
  }
  else {
    ball_speedx = 1;
  }
  ball_speedy = 0;

  clearDisplay();
  setPixelArray(paddle1_xPos, paddle1_yPos, paddle_width, paddle_height);
  setPixelArray(paddle2_xPos, paddle2_yPos, paddle_width, paddle_height);
  setPixelArray(ball_xPos, ball_yPos, ball_size, ball_size);
  translateToImage();
  display_image(0, oled_display);
  delay(1000);
}

void string_clear() {
  display_string(0, "");
  display_string(1, "");
  display_string(2, "");
  display_string(3, "");
  display_string(4, "");
  display_update();
}

void ball_movement() {
  ball_xPos += ball_speedx;
  ball_yPos += ball_speedy;
  if(ball_speedy > 1.7){
    ball_speedy = 1.7;
  }
  if(ball_speedy < -1.7){
    ball_speedy = -1.7;
  }

  if (ball_yPos < 1 || ball_yPos > (31 - ball_size)) {
    ball_speedy = -(ball_speedy);
  }
  
   if((ball_speedy < 0.3) && (ball_yPos <= 10)) {
     count++;
     if(count >= 10){
       ball_speedy = 1;
       count = 0;
     }
   }
   else{
     count = 0;
   }
}

void ai_move(void) {
  paddle2_down = 0;
  paddle2_up = 0;
  if ((ball_yPos % ai_difficulty) == 0) {
    if ((ball_yPos < paddle2_yPos) && (paddle2_yPos > 0)) {
      paddle2_yPos -= paddle_speed;
      paddle2_up = 1;
    }
    if ((ball_yPos > paddle2_yPos) && (paddle2_yPos < (32 - paddle_height))) {
      paddle2_yPos += paddle_speed;
      paddle2_down = 1;
    }
  }
}

void one_player(btns) {
  player_movement_one(btns);
  ai_move();
  ball_movement();
  paddle_hit();

  clearDisplay();
  setPixelArray(paddle1_xPos, paddle1_yPos, paddle_width, paddle_height);
  setPixelArray(paddle2_xPos, paddle2_yPos, paddle_width, paddle_height);
  setPixelArray(ball_xPos, ball_yPos, ball_size, ball_size);
  translateToImage();
  display_image(0, oled_display);
}

void score_mode(btns) {
  player_movement_one(btns);
  ai_move();
  ball_movement();
  paddle_hit();

  clearDisplay();
  setPixelArray(paddle1_xPos, paddle1_yPos, paddle_width, paddle_height);
  setPixelArray(paddle2_xPos, paddle2_yPos, paddle_width, paddle_height);
  setPixelArray(ball_xPos, ball_yPos, ball_size, ball_size);
  translateToImage();
  display_image(0, oled_display);
}

void two_player(btns) {
  player_movement_two(btns);
  ball_movement();
  paddle_hit();

  clearDisplay();

  /*
  if((ball_yPos + ball_size > paddle1_yPos) && (ball_yPos <= paddle1_yPos + 2) && (ball_xPos == paddle_width)){
      i = 1;
  }
  if((ball_yPos > paddle1_yPos + 2) && (ball_yPos <= paddle1_yPos + 4) && (ball_xPos == paddle_width)){
      j = 1;
  }
  if((ball_yPos > paddle1_yPos + 4) && (ball_yPos <= paddle1_yPos + 6) && (ball_xPos == paddle_width)){
      k = 1;
  }
  if((ball_yPos > paddle1_yPos + 6) && (ball_yPos - ball_size < paddle1_yPos + 8) && (ball_xPos == paddle_width)){
      m = 1;
  }
  if(i){
    setPixelArray(10, 10, 2, 2);
  }
  
  if(j){
    setPixelArray(50, 10, 2, 2);
  }
  
  if(k){
    setPixelArray(90, 10, 2, 2);
  }
  
  if(m){
    setPixelArray(120, 10, 2, 2);
  }
  */


  setPixelArray(paddle1_xPos, paddle1_yPos, paddle_width, paddle_height);
  setPixelArray(paddle2_xPos, paddle2_yPos, paddle_width, paddle_height);
  setPixelArray(ball_xPos, ball_yPos, ball_size, ball_size);
  translateToImage();
  display_image(0, oled_display);
}

/* This function is called repetitively from the main program */
void labwork(void) {
  int btns = getbtns();
  int sw = getsw();

  if(sw & 0x1) {
    quit();
  }

  if((!game_active) && (!settings_menu)) {
    menu(btns);
  }
  if((!game_active) && (settings_menu)) {
    menu_settings(btns);
  }

  if((game_active) && (game_mode == 1)){
    two_player(btns);
  }

  if((game_active) && (game_mode == 2)){
    one_player(btns);
  }
  if((game_active) && (game_mode == 3)){
    score_mode(btns);
  }
}
