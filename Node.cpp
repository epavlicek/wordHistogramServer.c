/*-------------------------------------------------------------------------*
 *---									---*
 *---		Node.cpp						---*
 *---									---*
 *---	    This file defines the methods and functions related to	---*
 *---	class Node.							---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1a		2021 August 16		Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	"header.h"
#include	"Node.h"


//  PURPOSE:  To release the resources of '*this'.  No parameters.  No
//	return value.
Node::~Node			()
{
delete(leftPtr_);
delete(rightPtr_);
free(wordCPtr_);
  //  YOUR CODE HERE

}


//  PURPOSE:  To either increment the count of the node at or under '*nodePtr'
//	that corresponds to word 'wordCPtr' when such a node exists, or to
//	insert a 'new Node()' for 'wordCPtr' under '*nodePtr' that maintains
//	the sorted nature of the subtree.  No return value.
void		insert		(Node*		nodePtr,
				 const char*	wordCPtr
				)
{
  int	compRes	= strcmp(nodePtr->getWordCPtr(),wordCPtr);

  if  (compRes > 0)
  {
   insert(nodePtr->getLeftPtr(),wordCPtr);
   int count_ =count_++;	
    //  Go to the left
    //  YOUR CODE HERE

  }
  else
  if  (compRes < 0)
  {
   insert(nodePtr->getRightPtr(),wordCPtr);
   int count_=count_++;
    //  Go to the right
    //  YOUR CODE HERE
  }
  else
  {
   Node* newNode = NULL;
   newNode = new Node(wordCPtr);
   insert(newNode,wordCPtr);
   wordCPtr = NULL;
    //  YOUR CODE HERE
  }
}


//  PURPOSE:  To print out the subtree pointed to by 'nodePtr' in an in-fix
//	(sorted) fashion.  No return value.
void		print		(const Node*	nodePtr
				)
{
  if(nodePtr==NULL)
	return;
  print(nodePtr->getLeftPtr());
  //  YOUR CODE HERE
  printf("%d\t%s\n",nodePtr->getCount(),nodePtr->getWordCPtr());
  //  YOUR CODE HERE
  print(nodePtr->getRightPtr());
}

