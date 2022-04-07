/*-------------------------------------------------------------------------*
 *
 *---									---*
 *---		histogrammer.cpp					---*
 *---									---*
 *---	    This file defines the public functions for the word-	---*
 *---	histogramming program.						---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	"header.h"
#include	<pthread.h>
#include	"Node.h"

//	Compile with:
//	$ g++ histogrammer.cpp Node.cpp -o histogrammer -lpthread



//	----	----	----	----	----	----	----	----	//
//									//
//			Global constants:				//
//									//
//	----	----	----	----	----	----	----	----	//

const int	LINE_LEN		= 4096;

#define		SEPARATORY_CHAR_ARRAY	" \t\n\r.!,:;?<>()[]{}\\\"|+-*%=^&/"




//	----	----	----	----	----	----	----	----	//
//									//
//			Global variables:				//
//									//
//	----	----	----	----	----	----	----	----	//

//  PURPOSE:  To tell the index of the word at which to start.
int		wordIndex;

//  PURPOSE:  To hold the address of the next word to histogram.
const char*	wordPtr	= NULL;

//  PURPOSE:  To hold 'true' while the program should still run,
//	or 'false' otherwise.
bool		shouldRun	= true;

//  PURPOSE:  To control access to 'wordPtr'.
pthread_mutex_t	wordPtrLock;

//  PURPOSE:  To be signaled on when 'wordPtr' is set to the address of a word.
pthread_cond_t	wordPtrSet;

//  PURPOSE:  To be signaled on when 'wordPtr' is set to 'NULL'
pthread_cond_t	wordPtrClear;


//	----	----	----	----	----	----	----	----	//
//									//
//			Global functions:				//
//									//
//	----	----	----	----	----	----	----	----	//

//  PURPOSE:  To 'fprintf()' to 'stderr' 'errorMsgCPtr', and 'exit()' the
//  	process with 'EXIT_FAILURE'.  No return value.
void		exitFailure	(const char*	errorMsgCPtr
				)
{
  fprintf(stderr,"%s\n",errorMsgCPtr);
  printf("-1\n");
  exit(EXIT_FAILURE);
}


//  PURPOSE:  To return the next word in the file being read by 'inputPtr'.  If
//	at end of file then 'rewind()'s to beginning of file and re-reads first.
//	If there are no words then 'exit()'s process with 'EXIT_FAILURE'.  No
//	parameters.
const char*	getNextWord	(FILE*		inputPtr
				)
{
  static
  char		line[LINE_LEN];
  static
  char*		localTokenPtr	= NULL;
  const char*	toReturn;
  int		rewindCount	= 0;

  while  (localTokenPtr == NULL)
  {
    if  (fgets(line,LINE_LEN,inputPtr) == NULL)
    {
      if  (rewindCount > 0)
      {
        exitFailure("No words in file!");
      }

      rewind(inputPtr);
      rewindCount++;

      if  (fgets(line,LINE_LEN,inputPtr) == NULL)
      {
        exitFailure("Empty file!");
      }

    }

    localTokenPtr	= strtok(line,SEPARATORY_CHAR_ARRAY);
  }

  toReturn	= localTokenPtr;
  localTokenPtr	= strtok(NULL,SEPARATORY_CHAR_ARRAY);
  return(toReturn);
}


//  PURPOSE:  To set global vars 'wordIndex' to a legal value from
//	the 'argc' command line arguments given in 'argv[]'.  Prints error
//	message and 'exit()'s with 'EXIT_FAILURE' on error.  No return value.
void		initializeWordIndexAndCount
				(int		argc,
				 char*		argv[]
				)
{
  if  (argc < 2)
  {
    exitFailure("Usage:\thistogrammer 'wordIndex'");
  }

  wordIndex	= strtol(argv[1],NULL,0);

  if  (wordIndex < 0)
  {
    exitFailure("'wordIndex' must be non-negative.");
  }

}


//  PURPOSE:  To attempt to initialize 'inputPtr' by fopen()ing 'FILENAME'.
//	Prints error message and 'exit()'s with 'EXIT_FAILURE' on error.
//	Return ptr to open file.
FILE*		initializeFilePtr
				()
{
  FILE*	inputPtr	= fopen(FILENAME,"r");

  if  (inputPtr == NULL)
  {
    exitFailure("Cannot open " FILENAME);
  }

  return(inputPtr);
}


//  PURPOSE:  To be run by the histogram-making thread, which makes a tree
//	that represents a map (in C++ terms) or a dictionary (in Java terms)
//	of words to the number of times that they have been read.  Ignores
//	'vPtr'.  Returns 'NULL'.
void*		histogramMaker	(void*		vPtr
				)
{
  Node*	rootPtr	= NULL;
  pthread_mutex_lock(&wordPtrLock);
  while  (shouldRun)
  {
    //  YOUR CODE HERE
   // pthread_mutex_lock(&wordPtrLock);
    pthread_cond_wait(&wordPtrSet,&wordPtrLock);
    //  BEGINNING OF CRITICAL SECTION
    if  (rootPtr == NULL)
      rootPtr	= new Node(wordPtr);
    else
      insert(rootPtr,wordPtr);

    wordPtr	= NULL;
    //  END OF CRITICAL SECTION

    //  YOUR CODE HERE
    pthread_cond_signal(&wordPtrClear);
    pthread_mutex_unlock(&wordPtrLock);
  }

  print(rootPtr);
  delete(rootPtr);
  return(NULL);
}


//  PURPOSE:  To keep reading words until 'shouldRun' becomes 'false'.  Words
//  	read from 'inputPtr', and 'wordPtr' set to point to them, so the
//	counting thread can count them.  No return value.
void		reader		(FILE*		inputPtr
				)
{
  while  (shouldRun)
  {
    sleep(1);	// Leave this outside critical section
    pthread_mutex_lock(&wordPtrLock);
    pthread_cond_wait(&wordPtrClear,&wordPtrLock);
    //  YOUR CODE HERE

    //  BEGINNING OF CRITICAL SECTION
    wordPtr	= getNextWord(inputPtr);
    //  END OF CRITICAL SECTION
    pthread_cond_signal(&wordPtrSet);
    pthread_mutex_unlock(&wordPtrLock);
    //  YOUR CODE HERE
  }
}


//  PURPOSE:  To set 'shouldRun' to 'false' when 'SIGINT' is received.
//	Ignores 'sigNum'.  No return value.
void		sigIntHandler	(int		sigNum
				)
{
  shouldRun	= false;
}


//  PURPOSE:  To set 'sigIntHandler' as the 'SIGINT' handler for this
//	process.  Please use flag SA_RESTART.  No parameters.  No return value.
void		installSigIntHandler
				()
{
  struct sigaction	act;
  memset(&act,'\0',sizeof(struct sigaction));
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_NOCLDSTOP | SA_RESTART;;
  act.sa_handler = sigIntHandler;
  sigaction(SIGINT,&act,NULL);
  //  YOUR CODE HERE
}


int		main		(int		argc,
				 char*		argv[]
				)
{
  //  I.  Application validity check (done below):

  //  II.  Make histogram:
  pthread_t		histogramThread;
  FILE*			inputPtr;

  //  II.A.  Initialize vars:
  pthread_mutex_init(&wordPtrLock,NULL);
  pthread_cond_init(&wordPtrSet,NULL);
  pthread_cond_init(&wordPtrClear,NULL);
  initializeWordIndexAndCount(argc,argv);
  installSigIntHandler();
  inputPtr	= initializeFilePtr();

  //  II.B.  Fast-forward for first indexed word:
  while  (wordIndex-- > 0)
    getNextWord(inputPtr);

  //  II.C.   Start histogramming thread:
  pthread_create(&histogramThread,NULL,histogramMaker,NULL);

  //  II.D.   The parent reads words until 'shouldRun' set to 'false':
  reader(inputPtr);

  //  II.E.  Reading is over, tell child thread to stop, and wait for it:
  pthread_join(histogramThread,NULL);

  //  II.F.  Release resources:
  pthread_cond_destroy(&wordPtrClear);
  pthread_cond_destroy(&wordPtrSet);
  pthread_mutex_destroy(&wordPtrLock);
  fclose(inputPtr);

  //  III. Finished:
  return(EXIT_SUCCESS);  
}
