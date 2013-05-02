//  main.c
//  TMcCormickEvilHangman
//
//  Program that creates a sorted array of lists, in which each list contains all
//  the words in the English langauge of a given length. Allows the user to
//  chose a length, and stores this for later usage in one helluva hangman bout
//  that you wont want to miss. Tune in next week!
//
//  Created by Tristan J. McCormick on 2012-09-24.
//  Copyright (c) 2012 Tristan J. McCormick. All rights reserved.
//

#include "Header.h"


int main(int argc, const char * argv[])
{
    //Creates the dictionary data structure (array of pointers to lists)
    int listLength = LISTLENGTH;
    node *lists[listLength];
    
    //Reads the dictionary and sorts words into lists by length
    char buff[BUFFER];
    char mode1[] = "r";
    char *file1 = "dictionary.txt";
    FILE *file_ptr1 = NULL;
    file_ptr1 = fopen(file1, mode1);
    if(!file_ptr1) {
        fprintf(stderr, "No file found!");
        return 0;
    }
    //reads in the library of Scrabble words from the  file
    //creates node containing that word, and inserts in the list of words of that length
    while(fscanf(file_ptr1, "%s", buff) != EOF) {
        long wordList = strlen(buff);
        addWordToList(&lists[wordList], buff);
    }
    
    int lengthOfWord = lengthOfWordPrompt(lists);
    int numAllowedGuesses = getNumAllowedGuesses();
    int wrongGuesses = 0;
    char guess;
    node* newWordList = lists[lengthOfWord];
    while(1){
        guess = getGuess(&head);
        printGuessedLetterList(head);
        generateFamilies(newWordList, head, &headFam);
        newWordList = (getNewWordList(headFam) -> nextMember);
        char* currentState = getCurrentState(getNewWordList(headFam));
        wrongGuesses = numberOfWrongGuesses(head, getNewWordList(headFam) -> description);
        printf("Current state of the game is: %s\n", currentState);
        printf("Number of wrong guesses right now is %i\n", wrongGuesses);
#ifdef DEBUG
        printf("The number of words left is %i\n", (getNewWordList(headFam) -> number));
#endif
        if(wrongGuesses == numAllowedGuesses) {
            printf("You lose my friend!\n");
            exit(0);
        }
        if(wonGame(getNewWordList(headFam))) {
            printf("You won!\n");
            exit(0);
        }
        headFam = NULL;//Couldn't get rid of memory leak
    }
}
/*
 Takes a famNode and returns 0 if it's description
 is completely revealed and 1 if it still has asterisks
 */
int wonGame(famNode* finalFamily) {
    for (char* counter = finalFamily -> description; *counter != '\0'; counter++) {
        if (*counter == '*') {
            return 0;
        }
    }
    return 1;
}
/*
 Checks to see if a given word contains a given char,
 returning 0 if it is not, and the index + 1 if it is.
 */
int containsletter(node *currentWord, char data) {
    char *current = currentWord -> word;
    for(int i = 0; current[i] != '\0'; i++) {
        if(current[i] == data) {
            return i + 1;
        }
    }
    return 0;
    
}
/*
 Prompts the user to input the desired number of guesses for the game.
 Subject to the constant minimum and maximum's set in the header.
 */
int getNumAllowedGuesses() {
    int numGuesses = 0;
    printf("Enter in the number of guesses: ");
    while(1) {
        scanf("%i", &numGuesses);
        if(numGuesses <= MINNUMGUESSES || numGuesses >= MAXNUMGUESSES) {
            printf("Enter an integer between %i and %i\n", MINNUMGUESSES, MAXNUMGUESSES);
            flush_stdin();
        }
        else {
            printf("Number of guesses is: %i\n", numGuesses);
            flush_stdin();
            return numGuesses;
        }
    }
}
/*
 Takes a given list of nodes, and prints
 out all of the words contained therein.
 */
void printLibrary(node* lists[]) {
    int minLength = MINLENGTH;
    int maxLength = MAXLENGTH;
    for(int i = minLength; i < maxLength; i++) {//Displays all the words in each length category
        if(lists[i] == NULL) {
        }
        else {
            printf("Words of length %i: ", i);
            printList(lists[i]);
            printf("\n");
        }
    }
}
/*
 Takes a word list, the list of guessed Chars, and a reference to the
 head of the family list of lists. Generates the description of every word
 in the word list given the set of guessed characters and for each word from
 the word list checks a) if a family already exists with that description in
 which case the word is added or b) if the word's description is novel in which
 case it makes a new family node, which it appends to the family list of lists.
  */
void generateFamilies(node* wordList, letterNode* head, famNode** headRefFam) {
    for(node* dictCounter = wordList; dictCounter != NULL; dictCounter = dictCounter -> next) {
        char* currentWord = dictCounter -> word;
        char* newDescription = getDescription(currentWord, head);
        if(*headRefFam == NULL) {
            createFamily(newDescription, headRefFam);
            insertToFamily(headRefFam, dictCounter);
        }
        else {
            for(famNode* famCount = *headRefFam; famCount != NULL; famCount = famCount -> nextFam) {
                if(!strcmp(newDescription, famCount -> description)) {
                    insertToFamily(&famCount, dictCounter);
                    break;
                }
                if(famCount -> nextFam == NULL) {
                    createFamily(newDescription, headRefFam);
                    insertToFamily(headRefFam, dictCounter);
                    break;
                }
            }
        }
    }
}
/*
 Runs through the whole family list of lists and 
 returns the one with the most members.
 */
famNode* getNewWordList(famNode* headRefFam) {
    int maxMembership = 0;
    famNode* familyWithMax = headRefFam;
    for(famNode* currentFam = headRefFam; currentFam != NULL; currentFam = currentFam -> nextFam) {
        if(currentFam -> number > maxMembership) {
            familyWithMax = currentFam;
            maxMembership = currentFam -> number;
        }
    }
    return familyWithMax;
    
}
/*
 Selects each guessed letter in turn and compares it
 to each character in the current description keeping
 track of the number of correct guesses. Returns the 
 number of wrong guesses by subtracting the number of 
 right from the total number of guesses.
 */
int numberOfWrongGuesses(letterNode* head, char* description) {
    int correctGuesses = 0;
    for(letterNode* currentGuess = head; currentGuess != NULL; currentGuess = currentGuess -> next) {
        for(char* currentPos = description; *currentPos != '\0'; currentPos++) {
            if(currentGuess -> letter == *currentPos) {
                correctGuesses++;
                break;
            }
        }
    }
    return (listCount(head) - correctGuesses);
}
/*
 Takes a given word and the list of guessed characters and returns
 a string with any matching letters filled in and the rest as asterisks
 */
char* getDescription(char* word, letterNode* setOfGuesses) {
    char* currentState = malloc(sizeof(word));
    for(letterNode* current = setOfGuesses; current != NULL; current = current-> next) {
        char guess = current -> letter;
        //inputs the given guessed char into the state string
        for(int i = 0; word[i] != '\0'; i++) {
            if (isalpha(currentState[i])) {
            }
            else if(word[i] == guess) {
                currentState[i] = guess;
            }
            else if(word[i] == ' ') {
                currentState[i] = ' ';
            }
            else
                currentState[i] = '*';
        }
    }
    return currentState;
}
/*
 Takes a description strink for the new family
 and creates a famNode with that description and
 initialized values for the other fields.
 Note that the head of the family list of lists
 is passed as a "reference" pointer to the head pointer
 so that we can alter it directly and not a copy
 of it.
  */
void createFamily(char* description, famNode** headRefFam) {
    famNode* newFamily = malloc(sizeof(famNode));
    newFamily -> description = description;
    newFamily -> number = 0;
    newFamily -> nextMember = NULL;
    newFamily -> nextFam = *headRefFam;
    *headRefFam = newFamily;
}
/*
 Passed a word in a buffer array and a reference pointer
 to the beginning of the list. Then puts scanned word from
 buff into a new node and inserts into the given list
 Used for reading in the library.
 */
void addWordToList(node** listPtr, char* buff) {
    node* newNode = malloc(sizeof(node));
    newNode -> word = malloc(strlen(buff));
    strcpy(newNode -> word, buff);
    newNode -> next = *listPtr;
    *listPtr = newNode;
}
/*
 Takes a reference pointer to a family and inserts
 a new node containing a given string into it
 */
void insertToFamily(famNode** family, node* data) {
    node* newNode = malloc(sizeof(node));
    newNode -> word = data -> word;
    newNode -> next = (*family) -> nextMember;
    (*family) -> nextMember = newNode;
    (*family) -> number++;
}
/*
 prints out the letters guessed so far
 */
void printGuessedLetterList(letterNode* head) {
    printf("Guessed letters: \n");
    letterNode *current = head;
    if(head == NULL) {
    }
    else {
        while(current != NULL) {
            printf("%c ", current -> letter);
            current = current -> next;
        }
    }
}
/*
 returns number of elements in the guessed 
 character list, or 0 if list is empty
 */
int listCount(letterNode* head) {
    int counter = 0;
    letterNode *current = head;
    if(head == NULL) {
        return 0;
    }
    else {
        while(current != NULL) {
            current = current -> next;
            counter++;
        }
        printf("\n");
        return counter;
    }
}
/*
 Takes the list of guessed letters and a new character
 and pushes the character to the front of the list.
 */
void addLetterToList(letterNode** headRef, char data) {
    letterNode* newNode = malloc(sizeof(letterNode));
    newNode -> letter = data;
    newNode -> next = *headRef;
    *headRef = newNode;
}
/*
 Checks to see if a given char is contained within the
 list of guessed letters. returns 1 if it is, 0 if not.
 */
int listContains(letterNode* head, char data) {
    letterNode* current = head;
    while(current != NULL) {
        if(current -> letter == data) {
            return 1;
        }
        else
            current = current -> next;
    }
    return 0;
}
/*
 Prompts user for a new guess, and returns that guess when valid,
 meaning it a) hasn't already been guessed and  b) is alphabetical
 */
char getGuess(letterNode** head) {
    char guess;
    printf("Please enter a guess: ");
    while(1) {
        guess = getchar();
        guess = tolower(guess);
        flush_stdin();
        if(!listContains(*head, guess) && isalpha(guess)) {
            addLetterToList(head, guess);
            return guess;
        }
        else if(listContains(*head, guess)) {
            printf("You already guessed that letter. ");
            printf("Please enter another guess\n");
        }
        else if (!isalpha(guess)) {
            if(guess == '\n') {
                printf("You guessed a blank space.\n");
                printf("Try again\n");
            }
            else {
                printf("Input an alphabetical letter.\n");
                printf("Try again\n");
            }
        }
    }
}
/*
 Prints each word from the given list of nodes
 */
void printList(node* head) {
    node *current = head;
    if(head == NULL) {
    }
    else {
        while(current != NULL) {
            printf("%s, ", current -> word);
            current = current -> next;
        }
        printf("\n");
    }
    
}
/*
 Prints out each of the words contained
 in a given family list of nodes
 */
void printFamily(famNode* head) {
    if(head == NULL) {
        printf("There are no families yet");
    }
    else {
        printf("These words are in the family %s: \n", head -> description);
        node* current = (head -> nextMember);
        char* currentWord = malloc(sizeof(node));
        currentWord = current -> word;
        if(current == NULL) {
        }
        else {
            while(current != NULL) {
                printf("%s ", current -> word);
                current = current -> next;
            }
            printf("\n");
        }
    }
}
/*
 Prompts the user to select the length of word they
 would like to play Hangman with, checking that it is
 within the bounds of the shortest and longest words and 
 that words exist of that length.
 */
int lengthOfWordPrompt(node* lists[]) {
    int lengthOfWord = 0;
    printf("Enter in length of word: ");
    while(1) {
        scanf("%i", &lengthOfWord);
        if(lengthOfWord <= MINLENGTH || lengthOfWord >= MAXLENGTH) {
            printf("Enter an integer between %i and %i\n", MINLENGTH, MAXLENGTH);
            flush_stdin();
        }
        
        else if(lists[lengthOfWord] == NULL) {
            printf("There are no words of that length. Enter in length again.\n");
            flush_stdin();
        }
        else {
            printf("Length of secret word: %i\n", lengthOfWord);
            flush_stdin();
            return lengthOfWord;
        }
    }
}
/*
 returns the current description of the game
 */
char* getCurrentState(famNode* headRef) {
    return headRef -> description;
}

//Flushes the printf pipe, from D. McAvoy
void flush_stdin() {
    char c;
    while((c = getchar()) != '\n');
}