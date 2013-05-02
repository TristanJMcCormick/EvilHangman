//
//  Header.h
//  tmccormickevilhangman
//
//  Created by Tristan J. McCormick on 10/1/12.
//  Copyright (c) 2012 Tristan J. McCormick. All rights reserved.
//

#ifndef tmccormickevilhangman_Header_h
#define tmccormickevilhangman_Header_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct node {
    char *word;
    struct node *next;
}typedef node;

struct famNode {
    char* description;
    int number;
    struct node *nextMember;
    struct famNode *nextFam;
}typedef famNode;

struct letterNode {
    char letter;
    struct letterNode *next;
}typedef letterNode;

//Creates the guessed letters data structure (list)
letterNode *head = NULL;

//Creates the family data structure (lists of lists)
famNode *headFam = NULL;

int wonGame(famNode* finalFamily);
int  containsletter(node* currentWord, char data);
int  getNumAllowedGuesses();
void printLibrary(node* lists[]);
void generateFamilies(node* head, letterNode* guess, famNode** headRefFam);
famNode* getNewWordList(famNode* headRefFam);
int numberOfWrongGuesses(letterNode* head, char* description);
char* getDescription(char* word, letterNode* setOfGuesses);
void createFamily(char* description, famNode** headRefFam);
void addWordToList(struct node** listPtr, char* buff);
void insertToFamily(famNode** family, node *data);
void printGuessedLetterList(letterNode* head);
int listCount(letterNode* head);
void addLetterToList(letterNode** headRef, char data);
int  listContains(letterNode* head, char data);
char getGuess(letterNode** head);
void printList(node* head);
void printFamily(famNode* head);
int  lengthOfWordPrompt(node* lists[]);
char* getCurrentState(famNode* headRef);
void flush_stdin();

#define LISTLENGTH 35
#define MINLENGTH 2
#define MAXLENGTH 30
#define BUFFER 50
#define MINNUMGUESSES 3
#define MAXNUMGUESSES 26
#define DEBUG 1


#endif
