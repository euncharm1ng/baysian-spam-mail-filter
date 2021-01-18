# baysian-spam-mail-filter
It is a spam mail fiter using baysian theorum. 
The theorum goes like this:
  p(F|E)={p(E|F)p(F)} / {p(E|F)p(F)+p(E|not F)p(not F)}
It is used to calculate the possibility of the object being F when it has the p(E) possiblity of being E and p(F) possibility of being F

The program reads the samples of spam mails and non-spam mails and create a "dictionary" of the possibility of a word occurring in the spam mails and non-spam mails. Then, a test mail is given to the program. the program take every word from the sample mail and see if it is in both spam mail and non-spam mail word; if the possibility of being spam or non-spam is zero, the possibility will be a zero. If the word occured both in the spam and non-spam samples, put them to the equation and calculate the possibility. Both the possibilites of being spam and non-spam in the beginning are set to 0.5 

So the program firstly trains itself with the samples and obtain a linked list of probabilities. The punctuations are trimmed and the letters are unified to lower case to maximize the count, and the word containing only the punctuations (something like :) or ^^) are ignored. The list containes the count of the word that appeared in the total samples and the occurance of the word that counts the number of mails that contained the word. after two lists are created, it uses the list to calculate the possiblity of the test mail. As it is stated above the program take every word from the test mail and search it in the list, if the word appeared in both lists, the program take the possiblity and calculate or update the possibility. 

Some sideways for the spam mails to pass this program is typo. Since it is a program that differentiate 'something' and 'somethimg', spam mails can hide the trigger word and reduce the possibility using this method, yet remain understandable for human users. 
