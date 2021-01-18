//
//  main.c
//  discreteMath
//
//  Created by euncharming on 2020/11/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 WORDLEN is the length of the word in the linked list named as dictionary
 MAILEND is the totken to differentiate the mails that are included in one file.
 it is also the token to update updated in linkedlist
 */
#define WORDLEN 20
#define MAILEND "@#@#@"

/*
 define DEBUG as 0 to disable debug mode and to 1 to enable the mode.
 */
#define DEBUG 0


typedef struct dictionary dict;
typedef dict* pDict;
typedef enum {false, true} bool;


/*
 linked list, count is for the total word count and
 occur is the numbers of the mails that had the word
 */
struct dictionary{
    char word[WORDLEN];
    int occur;
    int count;
    pDict next;
    bool updated; //false if it starts to read a new mail, set to true when it is the first time appeared int he mail
};



/*
if there is no matching words after searching
 create a new node
 */
pDict newDict(char *string, pDict next){
    pDict target = (pDict)malloc(sizeof(dict));
    int i=0;
    while(string[i]!='\0'&&i<WORDLEN){ //if i exceeds wordLen, then it is probably a link
        target->word[i]=string[i];
        i++;
    }
    target->count=1;
    target->next=next;
    target->occur=1;
    target->updated=true;
    return target;
}//end of function

/*
 preprocessor, convert string to all lower case
 and trim the puctuations at the back
 */
char* preprocess(char* string){
#if DEBUG
    printf("\nbefore preprocess, string: %s \n", string);
#endif
    int i=0;

    while(string[i]!='\0'){ //to lower case
        if(string[i]>=65&&string[i]<90)
            string[i]+=32;
            i++;
    }
    
    while(true){
        i--;
        if(i<0){
#if DEBUG
            printf("word of only punctuations \n");
#endif
            return NULL;
        }
        else if((string[i]>=97&&string[i]<=122)||(string[i]>=48&&string[i]<=57)){
            string[i+1]='\0';
            break;
        }
    }
    i=0;
    while(true){
        if((string[i]>=97&&string[i]<=122)||(string[i]>=48&&string[i]<=57)){
            break;
        }
        else{
            string=&string[i+1];
        }
        i++;
    }
#if DEBUG
    printf("_after preprocess, string: %s\n", string);
#endif
    return string;
}//end of preprocess

/*
 update pos->updated so that pos->occur can increase
 */
void updateUpdate(pDict pos){
    while(pos!=NULL){
        pos->updated=false;
        pos=pos->next;
    }
}

/*
 search matching words, if a matching word is found
 add 1 to count and return true
 no matching word, then return false
 todo: deal with https://
 */
pDict searchDict(char* string, pDict pos){
    pDict first=pos;
    if(strcmp(string, MAILEND)==0){//end of an email
        updateUpdate(first);
        return first;
    }
    string=preprocess(string);
    if(string==NULL)
        return first;
    
    int result;
    pDict prevPos=pos;
    while(pos!=NULL){
        if((result=strcmp(pos->word, string))==0){
            if(!pos->updated){
                pos->occur++;
                pos->updated=true;
            }
            pos->count++;
            return first;
        }
        else if(result>0){ //has passed the position and no matching word, need to add node.
            if(prevPos==pos){
                return newDict(string,pos);
            }
            prevPos->next=newDict(string, pos);
            return first;
        }
        prevPos=pos;
        pos=pos->next;
    }
    //printf("null found\n");
    prevPos->next=newDict(string, pos);
    return first;
}//end of searchDict

/*
 prints all dict word and count in alphabet order
 */
void printDict(pDict pos){
    printf("=================printDict starts===================================\n");
    while(pos!=NULL){
        printf("c:%3d o:%2d |%s\n", pos->count, pos->occur, pos->word);
        pos=pos->next;
    }
    printf("=================printDict ends===================================\n\n");
}

/*
 initiatialize training
 reads the sample mails and create a linked list of
 the percentages of the words occuring in the samples
 */
pDict initializeTraining(char* filename){
    FILE *fp=NULL;
    fp=fopen(filename, "r");
    if(fp==NULL){
        printf("no such file");
        return NULL;
    }
    
    char* string;
    string=(char*)malloc(sizeof(char)*50);
    pDict first=NULL;
    fscanf(fp, "%s\n", string);
    string=preprocess(string);
    first=newDict(string, NULL);
    
    while(fscanf(fp,"%s", string)==1){
        first=searchDict(string, first);
    }
    fclose(fp);
    free(string);
    return first;
}//end of initializeTraining

/*
 search pDict for matching words from test mail
 */
int searchTest(char *string, pDict pos){
    if(string==NULL)
        return 0;
    int result;
    while(pos!=NULL){
        if((result=strcmp(pos->word, string))==0){
            return pos->occur;
        }
        else if(result>0){
            return 0;
        }
        pos=pos->next;
    }
    return 0;
}//end of searchTest

/*
 tests whether the mail is a spam or not
 takes the filename of the test mail,
 return true or false depending on the mail content
 */
bool a_simple_bayesian_spam_filter(char * filename_for_test_email){
    pDict spamDict=initializeTraining("spam.txt");
    pDict nonspamDict=initializeTraining("nonspam.txt");
    
#if DEBUG
    printDict(spamDict);
    printDict(nonspamDict);
#endif
    
    FILE *stream=NULL;
    stream=fopen(filename_for_test_email, "r");
    if(stream==NULL){
        printf("no such file");
        return false;
    }
    
    char* string;
    string=(char*)malloc(sizeof(char)*50);
    
    int ps, pn; // proba of spam mail and non-spam mail
    double prior=0.5;
    while(fscanf(stream,"%s", string)==1){
        char* tempString; // this is for handling the errors happening because of the string being null from preprocessing
        tempString =preprocess(string);
        if(tempString==NULL){
            continue;
        }

        if((ps=searchTest(tempString, spamDict))!=0){
            if((pn=searchTest(tempString, nonspamDict))!=0){
                printf("ps:%3d, pn:%3d, %s\n", ps, pn, tempString);
                prior=prior*(double)ps/((prior*(double)ps+(1-prior)*(double)pn));
                printf("this prior: %.10f\n", prior);
            }
        }
    }
    //printf("%d, %d \n", pSProduct, pNProduct);
    //proba=(float)(pSProduct/(pSProduct+pNProduct));
    printf("proba of being spam mail: %.10f\n", prior);
    fclose(stream);
    free(string);
    if (prior>0.9) return true;
    return false;
}//end of a_simple_baysian_spam_filter

int main(int argc, const char * argv[]) {
    bool tester=a_simple_bayesian_spam_filter("test.txt");
    if(tester) printf("it is a spam mail!\n");
    else printf("it is not a spam mail!\n");
    return 0;
}
