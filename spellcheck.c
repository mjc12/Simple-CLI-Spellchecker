/* spellcheck.c
 * 
 * Uakes a wordlist as argument and compares user entered words to
 * correctly spelled words in the wordlist. Uses minimum hamming
 * distance to determine spelling suggestions.
 * 
 * by Matthew Cummings
 * 10/27/21
 */
// for fgets, file
#include <stdio.h>

// for strtok
#include <string.h>

// for bools
#include <stdbool.h>

// for tolower
#include <ctype.h>

#define MAX_WORD_LEN 40
#define MAX_WORDS 110000
#define LINE_END "\r\n"

// function to load word list into given array
int loadWL(FILE *input, char wordlist[][MAX_WORD_LEN])
{
    // fill wordlist[][] array of strings
    int i = 0;
    while (fgets(wordlist[i], MAX_WORD_LEN, input))
    {
        // trim tailing /r/n from word
        strtok(wordlist[i], LINE_END);
        i++;
    }

    // return length of wordlist
    return i;
}

// determines hamming distance between 2 words
// https://en.wikipedia.org/wiki/Hamming_distance 
int findHamm(char word1[], char word2[])
{
    // get length of words to loop over
    // both strings are assumed to be of same length
    int wordlen = strlen(word1);
    
    // tallied distance between both words
    int dist = wordlen;

    //printf("word1: %s\n", word1);
    //printf("word2: %s\n", word2);

    // for every character in both strings, add 1 to dist tally
    // per matching character
    for (int i = 0; i < wordlen; i++)
    {
        if (word1[i] == word2[i])
        {
            dist--;
        }
    }

    return dist;
}

int main(int argc, char **argv)
{
    // give error if user does not enter a file name
    if (argc != 2)
    {
        printf("Usage: ./spellcheck.c wordlist.txt\n");
        return 0;
    }

    // get file pointer
    FILE *input = fopen(argv[1], "r");

    // make sure file exists
    if (input == NULL)
    {
        printf("%s was not found.", argv[1]);
        return 0;
    }

    // make wordlist array to be filled by file
    char wordlist[MAX_WORDS][MAX_WORD_LEN];

    // fill wordlist using loadWL() function
    int listLen = loadWL(input, wordlist);

    // print wordlist
    //for(int i = 0; i < listLen; i++) {
    //    printf("%s\n", wordlist[i]);
    //} printf("\n");

    // get input from user for what word to check
    char word[MAX_WORD_LEN];
    printf("Please enter a word... ");
    fgets(word, MAX_WORD_LEN, stdin);
    printf("\n");

    // find length of current word
    int wordlen = strlen(word) - 1;

    // exit if no word was entered
    if (wordlen == 0) return 0;

    // set word to lowercase
    for(int k = 0; word[k]; k++){
        word[k] = tolower(word[k]);
    }
    
    // trim trailing \n from inputted word by setting it to NULL
    word[wordlen] = '\0';
    
    // boolean to keep track of if we're done with current word or not
    bool foundClosest = false;

    // holds indexes of 5 closest same length words by hamming dist
    int closest[5];

    for(int j = 0; j < 5; j++) {
        closest[j] = -1;
    }

    // index for closest[] array
    int index = 0;

    // holds current record for closest word found
    int diff = MAX_WORD_LEN;

    // current index in wordlist array
    int i = 0;

    // loop until word is length=0, first element containing newline
    while (true)
    {
        // move on to the next word if the current word isn't the same
        // length as the user entered word
        while (strlen(word) != strlen(wordlist[i]))
        {
            i++;
            if(i == listLen - 1)
            {
                foundClosest = true;
                break;
            }
        }

        // get distance between list word and user word
        int currDiff = findHamm(word, wordlist[i]);

        //printf("dist: %d\n", currDiff);

        // certain things need to be set if we found new closest word
        if (diff > currDiff && !foundClosest)
        {
            // update diff to new min diff
            diff = currDiff;

            // if the diff is 0 we have found the word in the list
            if (diff == 0)
            {
                foundClosest = true;
            }

            // clear out old top 5 closest
            for (int j = 0; j < 5; j++)
            {
                closest[j] = -1;
            }

            // reset index for closest[]
            index = 1;

            // set new closest
            closest[0] = i;
        } 
        
        // certain different things if we've found yet another word
        // with the same matching minimum hamming distance
        else if (diff == currDiff && !foundClosest)
        {
            // make sure we haven't already found the 5 closest words at this diff
            if (index < 5)
            {
                closest[index++] = i;
            }
        } 

        // if diff=0 this is special case where user word was found
        // in the given wordlist so we need to break out
        else if (currDiff == 0)
        {
            foundClosest = true;
            diff = 0;
        }
        
        // else is only triggered when currDiff > diff
        if (!foundClosest) i++;

        // make sure we haven't already iterated to the end of the wordlist
        if (i == listLen)
        {
            // need to stop iterating loop if it gets to end of list
            // otherwise infinite loop if the user word is not found
            // in the wordlist
            foundClosest = true;
        }

        // if we've found the closest word(s) or the actual word in the list then tell the user
        // and prepare for the next iteration of the loop
        if (wordlen > 0 && foundClosest) 
        {
            // if diff is 0 then word is correctly spelled
            if (diff == 0) 
            {
                printf("The spelling of \"%s\" is correct.\n", word);
            } 
            
            // else the word is incorrectly spelled and we need to print suggestions
            else 
            { 
                printf("The spelling of \"%s\" is incorrect. ", word);
                printf("Here are some spelling suggestions...\n");

                for (int i = 0; i < 5; i++) 
                {
                    if (closest[i] != -1) 
                    {
                        printf("%s ", wordlist[closest[i]]);
                    }
                }
                printf("\n");
            }

            // update word
            printf("\nPlease enter a new word... ");
            fgets(word, MAX_WORD_LEN, stdin);

            // update wordlen
            wordlen = strlen(word) - 1;

            // exit if no word was entered
            if (wordlen == 0) return 0;

            // set word to lowercase
            for(int k = 0; word[k]; k++){
                word[k] = tolower(word[k]);
            }

            // trim newline from new word
            word[wordlen] = '\0';

            // reset diff
            diff = MAX_WORD_LEN;

            // reset closest[]
            for(int j = 0; j < 5; j++) {
                closest[j] = -1;
            }

            // do some formatting to make it pretty
            printf("\n");

            // reset wordlist index to top of list
            i = 0;

            // reset foundClosest boolean
            foundClosest = false;
        }
    }
}