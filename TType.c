#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

//use curses.h or ncurses.h for special character (TAB, SUPER, etc.) recognition
/*To Do:
  allow a bit of user control and input at the start of game (and make sure it doesn't start immediately
  upload to github
  make sure all functions have documentation
  maybe allow to take two integer vector inputs to specify number of words and word length
 */


void clearTerminal() {
  /* Completely clears terminal using quicker regex method.*/
  printf("\e[1;1H\e[2J");
}

void fgetsNNLU(char* str, int n, FILE* stream) {
  /*
    "fgets no new line unsafe"
    An unsafe but simple wrapper for fgets that removes the new line character at the end and retains the same syntax as fgets.
  */
  fgets(str, n, stream);
  str[strlen(str)-1]='\0';
}
  

float rand01() {
  /*
    Generates a random number between 0 and 1.
    NOTE: Initialize srand() in the code which calls this function,
         since initializing inside this function returns the same
	 value if rand01() is called in rapid succession.
  */
  int divisor=10e3;
  int randomInteger=rand() % (divisor+1);
  float output=1.0;
  output=(float) randomInteger/divisor;

  return output;
}


int randRange(int lower, int upper) {
  /*
    Generates a random number between lower and upper (inclusive).
    NOTE: Initialize srand() in the code which calls this function,
         since initializing inside this function returns the same
	 value if rand01() is called in rapid succession.
  */
  int output=(rand() % (upper-lower+1))+lower;
  return output;
}


int startTimer() {
  /*
    Return starting (reference) time.
   */
  struct timespec startTime;
  int startTimeInt;
  clock_gettime(CLOCK_MONOTONIC, &startTime);
  startTimeInt=(int) startTime.tv_sec;
  return startTimeInt;
}


int getElapsedTime(int startTime) {
  /*
    Computes elapsed time (in seconds) given starting time.
   */
  struct timespec curTime;
  int t_elapsed;
  clock_gettime(CLOCK_MONOTONIC,&curTime);
  t_elapsed=(int) curTime.tv_sec-startTime; //compute time elapsed in seconds
  return t_elapsed;
}


char* makeRandomString(int length) {
  /*Makes a random string of specified length.
    ---Inputs---
    length: desired length of output string, int
    ---Outputs---
    output: character array of length random characters, char array
  */

  char* output=malloc(length+1);
  float lowerWeight=0.5;
  float upperWeight=0.1;
  float specialWeight=0.4;
  
  char lowerLetters[27]="abcdefghijklmnopqrstuvwxyz";
  char upperLetters[27]="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char special[60]="1234567890`~!@#$%^&*()-_=+[]{}\\|;:'\"/?,<.>";
  int numChars=strlen(lowerLetters)+strlen(upperLetters)+strlen(special);

  char* allCharacters=malloc(numChars+1);
  strcpy(allCharacters,lowerLetters);
  strcat(allCharacters,upperLetters);
  strcat(allCharacters,special);
  //printf("all possible characters: \n%s \n",allCharacters);
  free(allCharacters);

  float typeRand;
  int randomIndex;
  char toWrite[2]={'a','\0'};
  int i;
  for (i=0; i<length; i++) {
    typeRand=rand01();
    if (typeRand<=lowerWeight) {
      randomIndex=rand() % strlen(lowerLetters);
      toWrite[0]=lowerLetters[randomIndex];
    }
    else if (typeRand<=(lowerWeight+upperWeight)) {
      randomIndex=rand() % strlen(upperLetters);
      toWrite[0]=upperLetters[randomIndex];
    }
    else {
      randomIndex=rand() % strlen(special);
      toWrite[0]=special[randomIndex];
    }

    strcat(output,toWrite);
  }

  return output;
}


float stringFractionCorrect(char* prompt, char* entry) {
  /*Computes the fraction of a string which is correct.
    ---Inputs---
    prompt: the string to which the user entry is compared, char array
    entry: character array intended to emulate prompt, char array
    ---Outputs---
    fracCorrect: fraction of string which is correct, float
  */
  int lengthPromptOriginal=strlen(prompt);
  int lengthEntryOriginal=strlen(entry);
  char* bufferChar="A";
  char* promptEqual;
  char* entryEqual;
  int i;

  if (lengthPromptOriginal>lengthEntryOriginal) {
    int overage=lengthPromptOriginal-lengthEntryOriginal;
    promptEqual=malloc(lengthPromptOriginal+1);
    entryEqual=malloc(lengthPromptOriginal+1);
    strcpy(promptEqual,prompt);
    strcpy(entryEqual,entry);
    for (i=0; i<overage; i++) {
      strcat(entryEqual,bufferChar);
    }
  }
  else if (lengthEntryOriginal>lengthPromptOriginal) {
    int overage=lengthEntryOriginal-lengthPromptOriginal;
    entryEqual=malloc(lengthEntryOriginal+1);
    promptEqual=malloc(lengthEntryOriginal+1);
    strcpy(entryEqual,entry);
    strcpy(promptEqual,prompt);
    for (i=0; i<overage; i++) {
      strcat(promptEqual,bufferChar);
    }
  }
  else {
    entryEqual=malloc(lengthEntryOriginal+1);
    promptEqual=malloc(lengthPromptOriginal+1);
    strcpy(entryEqual,entry);
    strcpy(promptEqual,prompt);
  }

  int charsCorrect=0;
  int charsTotal=strlen(entryEqual);
  float fracCorrect;
  for (i=0; i<charsTotal; i++) {
    if (entryEqual[i]==promptEqual[i]) {
      charsCorrect+=1;
    }
  }

  fracCorrect=(float) charsCorrect/charsTotal;
  return fracCorrect;
}


int computeWPM(int charsTotal,int charsCorrect,int t_elapsed) {
  /*
    Computes WPM (accounting for errors) according to the formula
    given on humanbenchmark.com's typing test.
    ---Inputs---
    charsTotal: total number of chars user has typed
    charsCorrect: total number of characters user has typed correctly
    t_elapsed: time elapsed since start of game (in seconds)
    ---Outputs---
    WPM: error accounting WPM
  */
  float t_min=(float) t_elapsed/60;
  float fractionCorrect=(float) charsCorrect/charsTotal;
  float rawWPM=(float) charsTotal/(5*t_min);
  int WPM=(int) (rawWPM*fractionCorrect);
  return WPM;
}


char* makePrompt() {
  /*
    Makes a full prompt with a random number of words
    (which each have a random number of characters).
    ---Inputs---
    NA
    ---Outputs---
    prompt, a pointer to the character array containing the prompt
*/
  int minWords=3;
  int maxWords=5;
  int numWords=randRange(minWords,maxWords);
  int minWordLength=4;
  int maxWordLength=9;
  int maxPromptLength=maxWords*(maxWordLength+1);
  char* prompt=malloc(maxPromptLength+1);

  int wordLength;
  int curWord;
  char* writeBuffer;
  for (curWord=0; curWord<numWords; curWord++) {
    wordLength=randRange(minWordLength,maxWordLength);
    writeBuffer=makeRandomString(wordLength);
    strcat(prompt,writeBuffer);
    strcat(prompt," ");
  }
  prompt[strlen(prompt)-1]='\0'; //truncate final space

  return prompt;
}


void runTurn(int* round,int* charsTotal,int* charsCorrect,int startingTime) {
  /*
    Displays current statistics (accuracy, score, WPM) and randomized prompt.
    Then takes user input and updates the statistics.
    ---Inputs---
    charsTotal: total number of chars user has typed
    charsCorrect: total number of characters user has typed correctly
    startingTime: time (in seconds) at which the game was started
    //possibly character type ratios?
    ---Outputs---
    NA
  */
  int fgetsMax=256;
  char* prompt=makePrompt();
  char* userInput=malloc(fgetsMax+1);
  int promptLength=strlen(prompt);
  float overallFractionCorrect;
  float roundFractionCorrect;
  int t_elapsed=getElapsedTime(startingTime);
  int accuracy;
  int WPM;

  printf("p_______________________q\n");
  printf("| ROUND %2d              |\n",*round);
  if (*round==1) {
    overallFractionCorrect=0;
    accuracy=0;
    WPM=0;
  }
  else {
    overallFractionCorrect=(float) (*charsCorrect)/(*charsTotal);
    accuracy=(int) (overallFractionCorrect*100);
    WPM=computeWPM(*charsTotal,*charsCorrect,t_elapsed); //should be initialized by a function call to computeWPM
  }
  printf("| ACC: %3d    WPM: %3d  | \n",accuracy,WPM);
  printf("b-----------------------d \n\n");
  printf("%s \n",prompt);
  fgetsNNLU(userInput,fgetsMax,stdin);
  roundFractionCorrect=stringFractionCorrect(prompt,userInput);
  *charsCorrect+=(int) (roundFractionCorrect*promptLength);
  *charsTotal+=promptLength;
  free(userInput);
  *round+=1;
}


void runGame() {
  /*
    Repeatedly runs a single turn and clear screen in between turns.
    ---Inputs---
    NA
    ---Outputs---
    NA
   */
  //set time equal to zero
  srand(time(NULL)); //initializes random number generator
  int round=1, charsTotal=0, charsCorrect=0;
  int t0=startTimer();

  printf(" \n "); //prints simple character so first clear works
  while (true) {
    clearTerminal();
    runTurn(&round, &charsTotal, &charsCorrect, t0);
  }
}


int main() {

  runGame();

  return 0;
}

