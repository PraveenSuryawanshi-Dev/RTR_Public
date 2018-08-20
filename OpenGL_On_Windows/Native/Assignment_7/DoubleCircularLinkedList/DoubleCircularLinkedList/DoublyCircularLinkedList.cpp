#include<iostream>
#include "DoublyCircularLinkedList.h"
#define NEW_NODE (NODE*)malloc(sizeof(NODE))

using namespace std;
using namespace Collections;

/*
template <typename T>
DoublyCircularLinkedList<T>::
*/
DoublyCircularLinkedList::DoublyCircularLinkedList() : _head(nullptr)
{
}


/*
template <typename T>
DoublyCircularLinkedList<T>::
*/
DoublyCircularLinkedList::~DoublyCircularLinkedList()
{
	if (_head != nullptr)
	{
		NODE* current = _head;

		do
		{
			NODE *nextNode = current->next;
			cout << "\n*** memory de-allocated for node: " << current->data;
			free(current);
			current = nextNode;
		} while (current != _head);
		cout << "\n****memory de-allocated successfully...***";
	}
}





bool Collections::DoublyCircularLinkedList::IsListEmpty()
{
	if (_head == nullptr)
		return true;

	return false;
}

/*
template <typename T>
void DoublyCircularLinkedList<T>::*/
void DoublyCircularLinkedList::InsertEnd(int item)
{
	NODE* newNode = NEW_NODE;
	newNode->data = item;
	newNode->next = nullptr;
	newNode->previous = nullptr;

	if (IsListEmpty())
	{
		_head = newNode;
		newNode->previous = _head;
		newNode->next = _head;
		cout << "\n***first node: " << newNode->data << " added successfully...***";
	}
	else
	{
		NODE* last = _head->previous;

		last->next = newNode;
		newNode->previous = last;
		newNode->next = _head;
		_head->previous = newNode;
		cout << "\n***last node :" << newNode->data << " added successfully...***";
	}
}


/*template <typename T>
T DoublyCircularLinkedList<T>::*/
int DoublyCircularLinkedList::DeleteBegining()
{
	if (!IsListEmpty())
	{
		NODE* first = _head;

		_head = first->next;
		_head->previous = first->previous;

		NODE *last = _head->previous;
		last->next = _head;

		int data = first->data;

		free(first);
		cout << "\n***first node: " << data << " de-allocated successfully and returned first node data...***";
		return data;
	}
	return{};
}


/*template <typename T>
T DoublyCircularLinkedList<T>::*/

int DoublyCircularLinkedList::DeleteEnd() 
{
	if (!IsListEmpty())
	{
		NODE* last = _head->previous;
		NODE* secondLast = last->previous;

		secondLast->next = _head;
		_head->previous = secondLast;

		int data = last->data;
		free(last);
		cout << "\n***last node: " << data << " de-allocated successfully and returned last data...***";
		return data;
	}
	return{};
}


/*template <typename T>
void DoublyCircularLinkedList<T>::*/
void DoublyCircularLinkedList::InsertBegining(int item)
{
	if (IsListEmpty())
	{
		InsertEnd(item);
		return;
	}

	NODE* current = _head;
	NODE* last = current->previous;
	NODE* newNode = NEW_NODE;

	newNode->data = item;
	newNode->next = nullptr;
	newNode->previous = nullptr;

	newNode->next = current;
	current->previous = newNode;

	newNode->previous = last;
	last->next = newNode;
	_head = newNode;

	cout << "\n***first node: " << newNode->data << " added successfully...***";
}


/*template <typename T>
void DoublyCircularLinkedList<T>::*/
void DoublyCircularLinkedList::PrintList() const
{
	NODE* current = _head;

	if (current == nullptr)
	{
		cout << "\nLIST=[]";
		return;
	}

	cout << "\nLIST=[";
	do
	{
		cout << current->data << ",";
		current = current->next;
	} while (current != _head);
	cout << "]";
}



int Collections::DoublyCircularLinkedList::FindData(int data)
{
	NODE* current = _head;
	int pos = 0;
	if (current == nullptr)
	{
		return -1;
	}

	do
	{
		if (current->data == data)
			return pos;

		pos++;

		current = current->next;
	} while (current != _head);

	return -1;
}



int Collections::DoublyCircularLinkedList::InsertBefore(int data ,int beforeData)
{
	NODE* current = _head;
	NODE* prev = nullptr, *next = nullptr;
	int pos = 0;

	do
	{
		if (current->data == beforeData)
		{
			if (pos == 0)
			{
				InsertBegining(data);
				return pos;
			}

			NODE * node = (NODE*)malloc(sizeof(NODE));
			node->data = data;
			node->next = nullptr;
			node->previous = nullptr;

			next = current;
			prev = current->previous;

			prev->next = node;
			node->previous = prev;

			node->next = next;
			next->previous = node;


			return pos;
		}

		pos++;

		current = current->next;
	} while (current != _head);

	InsertEnd(data);
	return pos;
}

int Collections::DoublyCircularLinkedList::InsertAfter(int afterData,int data)
{
	NODE* current = _head;
	NODE* prev = nullptr, *next = nullptr;
	int pos = 0;


	do
	{
		if (current->data == afterData)
		{

			NODE * node = (NODE*)malloc(sizeof(NODE));
			node->data = data;
			node->next = nullptr;
			node->previous = nullptr;

			next = current->next;
			prev = current;

			prev->next = node;
			node->previous = prev;

			node->next = next;
			next->previous = node;

			return pos;
		}

		pos++;

		current = current->next;
	} while (current != _head);

	InsertEnd(data);
	return pos;
}



int Collections::DoublyCircularLinkedList::DeleteBefore(int dataToDelete)
{
	NODE* current = _head;
	NODE*  prev= nullptr, *del = nullptr, *next = nullptr;
	int pos = 0;

	do
	{
		if (current->data == dataToDelete)
		{
			if (pos == 0)
			{
				DeleteBegining();
				return pos;
			}


			next = current;
			del = current->previous;
			prev = del->previous;

			prev->next = next;
			next->previous = prev;
          
			delete del;
			
			return pos;
		}

		pos++;

		current = current->next;
	} while (current != _head);

	return -1;
}




int Collections::DoublyCircularLinkedList::DeleteAfter(int dataToDeleteAfter)
{
	NODE* current = _head;
	NODE*  prev = nullptr, *del = nullptr, *next = nullptr;
	int pos = 0;

	do
	{
		if (current->data == dataToDeleteAfter)
		{

			prev = current;
			del = prev->next;
			next = del->next;

			prev->next = next;
			next->previous = prev;

			delete del;

			return pos;
		}

		pos++;

		current = current->next;
	} while (current != _head);

	return -1;
}



