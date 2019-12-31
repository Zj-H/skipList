//
// Created by Yusuf Pisan on 4/26/18.
//

#include <climits>
#include <cstdlib>
#include <iostream>

#include "skiplist.h"
#include <assert.h>

using namespace std;

ostream &operator<<(ostream &Out, const SkipList &SkipL) {
	//loop to output every level
	for(int i=SkipL.Depth-1;i>=0;i--){
		Out<<"Level: "<<i<<" -- ";
		
		SkipList::SNode *node=SkipL.FrontGuards[i];
		//loop to output this level
		while(node!= nullptr){
			Out<<node->Data<<", ";
			
			node=node->Next;
		}
		Out<<endl;
	}
	return Out;
}

SkipList::SNode::SNode(int Data) {
	this->Data=Data;
	
	this->Next= nullptr;
    // link to Prev SNode
    this->Prev= nullptr;
    // link to up one level
    this->UpLevel= nullptr;
    // link to down one level
    this->DownLevel= nullptr;
}

SkipList::SkipList(int Depth) {
	this->Depth=Depth;
	
	//allocate space for skiplist
	this->FrontGuards=new SNode *[Depth];
	this->RearGuards=new SNode *[Depth];
	
	int i;
	//loop to initialize the skiplist 
	for(i=0;i<Depth;i++){
		this->FrontGuards[i]=new SNode(INT_MIN);
		this->RearGuards[i]=new SNode(INT_MAX);
		this->FrontGuards[i]->Next=this->RearGuards[i];
		this->RearGuards[i]->Prev=this->FrontGuards[i];
		
		//link to the downlevel
		if(i>0){
			this->FrontGuards[i]->DownLevel=this->FrontGuards[i-1];
			this->FrontGuards[i-1]->UpLevel=this->FrontGuards[i];
			this->RearGuards[i]->DownLevel=this->RearGuards[i-1];
			this->RearGuards[i-1]->UpLevel=this->RearGuards[i];
		}
	}
	
	this->FrontGuards[0]->DownLevel= nullptr;
	this->FrontGuards[Depth-1]->UpLevel= nullptr;
	this->RearGuards[0]->DownLevel= nullptr;
	this->RearGuards[Depth-1]->UpLevel= nullptr;
}

bool SkipList::alsoHigher() const {
	int number = rand() % 2;
	
	//50% chance to add to higher level
	if(number==1)
		return  true;
	
	return false; 
}

bool SkipList::add(int Data) {
	SNode *nextNode=FrontGuards[0]->Next;
	
	//As long as nextNode->next is not null and nextNode->data < data, keep moving nextNode to the right
	while(nextNode->Next!= nullptr && nextNode->Data < Data)
		nextNode=nextNode->Next;		
	
	//if duplicate
	if(nextNode->Data == Data)
		return false;
	
	//create the node
	SNode *newNode=new SNode(Data);
	//add before nextNode
	addBefore(newNode, nextNode);
	
	int count=1;
	while(count<Depth){
		if(alsoHigher()){//add to upper list
			SNode *newUpper=new SNode(Data);
			newUpper->DownLevel=newNode;
			newNode->UpLevel=newUpper;
			
			SNode *node=newNode->Prev;
			//get the last node uplevel
			while(node->UpLevel == nullptr)
				node=node->Prev;			
			node=node->UpLevel;
			
			//add newUpper
			node=node->Next;			
			addBefore(newUpper, node);
			
			//set to newNode
			newNode=newUpper;
			count++;
		}else
			break;
	}
	
	return true;
}

void SkipList::addBefore(SNode *NewNode, SNode *NextNode) {
	assert(NewNode != nullptr && NextNode != nullptr &&NewNode->Data < NextNode->Data);
	//add before NextNode
	SNode *prevNode=NextNode->Prev;
	prevNode->Next=NewNode;
	NewNode->Prev=prevNode;
	
	NewNode->Next=NextNode;
	NextNode->Prev=NewNode;
	
	assert (NewNode->Next == NextNode&&NextNode->Prev == NewNode);
	assert (NewNode->Prev != nullptr &&NewNode->Prev->Data < NewNode->Data);
}

SkipList::~SkipList() {
	// need to delete individual nodes
	SNode *node,*last;
	
	int i;
	//loop to delete nodes
	for(i=Depth-1;i>=0;i--){
		node=FrontGuards[i];
		while(node!= nullptr){
			last=node;
			
			node=node->Next;
			delete last;
		}
	}
	
	//delete skiplist guards array
	delete [] FrontGuards;
	delete [] RearGuards;
}

bool SkipList::remove(int Data) { 
	SNode *current=FrontGuards[Depth-1];
	SNode *last;
	
	//search from the highest skiplist
	while(current!= nullptr){
		//Keep moving right as long as the current->next->data is less than value
		while(current->Next->Data<Data)
			current=current->Next;		
		
		//if find the data
		if(current->Next->Data==Data){
			
			while(current!= nullptr){
				last=current;
				
				//down one level
				current=current->DownLevel;
				
				//delete node last
				last->Prev->Next=last->Next;
				delete last;
			}
			return true;
		}
		
		current=current->DownLevel;
	}
	
	return false; 
}

bool SkipList::contains(int Data) {
	SNode *current=FrontGuards[Depth-1];
	
	//search from the highest skiplist
	while(current!= nullptr){
		//Keep moving right as long as the current->next->data is less than value
		while(current->Next->Data<Data)
			current=current->Next;			
		
		//if find the data
		if(current->Next->Data==Data)
			return true;
		
		//down one level from current node
		current=current->DownLevel;
	}
	
	return false;
}
