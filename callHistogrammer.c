/*-------------------------------------------------------------------------*
 *---									---*
 *---		callHistogrammer.c					---*
 *---									---*
 *---	    This file defines a histogram-calling function.		---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---									---*
 *-------------------------------------------------------------------------*/

//	Compile with:
//	$ gcc wordHistogramServer.c callHistogrammer.c -o wordHistogramServer -lpthread

//---		Header file inclusion					---//

#include	"header.h"


//  PURPOSE:  To make a process that histograms words starting at 'wordIndex'
//  	and count 'wordCount' words, get the word histogram from that process
//	and send it back to the client via socket file-descriptor 'toClientFd'.
//	No return value.
void		callHistogrammer(int		toClientFd,
				 int		wordIndex,
				 int		wordCount
				)
{
  int	childToParent[2];
  pid_t	childPid;
  int	endOrErr	= htonl(-1);	//  CHANGE THAT 0!

  //  MAKE PIPE AND THEN CHILD
  pipe(childToParent);
  childPid = fork();
  if (childPid<0){
	write(toClientFd,&endOrErr,4);
 	write(toClientFd,"\n",1);
	exit(EXIT_FAILURE);}
  if  (childPid == 0)
  {
    char	wordIndexBuffer[BUFFER_LEN];
    char	wordCountBuffer[BUFFER_LEN];

    //  CLOSE AND RE-DIRECT
    close(childToParent[0]);
   	 dup2(childToParent[1],STDOUT_FILENO);

    //  CALL PROGRAM_NAME WITH COMMAND LINE ARGUMENTS
    snprintf(wordIndexBuffer,BUFFER_LEN,"%d",wordIndex);
	    execl("./histogrammer","histogrammer",wordIndexBuffer,NULL);
    //  HANDLE ERROR CASE
    write(toClientFd,&endOrErr,4);
    write(toClientFd,"\n",1);
    exit(EXIT_FAILURE);
  }else{
	

  //  CLOSE, HANG-OUT, SEND-SIGNAL
  close(childToParent[1]);
  sleep(wordCount);
  kill(childPid,SIGINT);
  char	buffer[BUFFER_LEN];
  FILE*	inputPtr	= fdopen(childToParent[0],"r");

  while  (fgets(buffer,BUFFER_LEN,inputPtr) != NULL)
  {
    int		count;
    char	word[BUFFER_LEN];

    word[0]	= '\0';

    //  PARSE, CONVERT AND SEND TO CLIENT
    sscanf(buffer,"%d %s",&count,&word);
    count = htonl(count);
    write(toClientFd,&count,4);
    write(toClientFd,&word,strlen(word));
    write(toClientFd,"\n",1);

  }
  endOrErr =0;
  write(toClientFd,&endOrErr,4);
  write(toClientFd,"\n",1);
  close(childToParent[0]);
  wait(NULL);
  //  MORE STUFF OUT HERE
}
}
