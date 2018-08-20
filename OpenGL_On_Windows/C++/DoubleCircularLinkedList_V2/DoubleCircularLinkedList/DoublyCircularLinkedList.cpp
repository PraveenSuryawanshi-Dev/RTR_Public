#include<iostream>
#include "IDoublyCircularLinkedList.h"

#define NEW_NODE (NODE*)malloc(sizeof(NODE))

using namespace std;
using namespace Collections;


IDoublyCircularLinkedList * Collections::IDoublyCircularLinkedList::GetNewInstance()
{
	return new IDoublyCircularLinkedList();
}


Collections::IDoublyCircularLinkedList::~IDoublyCircularLinkedList()
{
	if (_head != nullptr)
	{
		NODE* current = _head;

		while(true)
		{
			NODE *nextNode = current->next;
			free(current);
			current = nextNode;

			if (current == _head)
				break;
		}
	}
}


void* Collections::IDoublyCircularLinkedList::GetNewNodeInstance(int item)
{
	NODE* newNode = NEW_NODE;
	newNode->data = item;
	newNode->next = nullptr;
	newNode->previous = nullptr;
	return newNode;
}


Result Collections::IDoublyCircularLinkedList::IsEmpty()
{
	if (_head == nullptr)
		return Result::SUCCESS;

	return Result::FAIL;
}


void Collections::IDoublyCircularLinkedList::AddLast(int item)
{
	NODE* newNode =(NODE*)GetNewNodeInstance(item);

	if (IsEmpty() == Result::SUCCESS)
	{
		_head = newNode;
		newNode->previous = _head;
		newNode->next = _head;
	}
	else
	{
		NODE* last = _head->previous;
		last->next = newNode;
		newNode->previous = last;
		newNode->next = _head;
		_head->previous = newNode;
	}
}


Result Collections::IDoublyCircularLinkedList::RemoveFirst()
{
	if (IsEmpty() != Result::SUCCESS)
	{
		NODE* first = _head;

		_head = first->next;
		_head->previous = first->previous;

		NODE *last = _head->previous;
		last->next = _head;

		int data = first->data;
		free(first);
		return Result::SUCCESS;
	}

	return Result::FAIL;
}


Result Collections::IDoublyCircularLinkedList::RemoveLast()
{
	if (IsEmpty() != Result::SUCCESS)
	{
		NODE* last = _head->previous;
		NODE* secondLast = last->previous;

		secondLast->next = _head;
		_head->previous = secondLast;

		int data = last->data;
		free(last);
		return Result::SUCCESS;
	}

	return Result::FAIL;
}


Result Collections::IDoublyCircularLinkedList::AddFirst(int item)
{
	if (IsEmpty() == Result::SUCCESS)
		return Result::FAIL;

	NODE* current = _head;
	NODE* last = current->previous;
	NODE* newNode = (NODE*) GetNewNodeInstance(item);

	newNode->next = current;
	current->previous = newNode;

	newNode->previous = last;
	last->next = newNode;
	_head = newNode;

	return Result::SUCCESS;
}


void Collections::IDoublyCircularLinkedList::DisplayData()
{
	NODE* current = _head;

	if (current == nullptr)
	{
		cout << "\n LIST=[]";
		return;
	}

	cout << "\n LIST=[";
	while (true)
	{
		cout << current->data << ",";
		current = current->next;

		if (current == _head)
			break;
	}
	cout << "]";
}


Result Collections::IDoublyCircularLinkedList::Search(int data , int &Foundposition)
{
	NODE* current = _head;
	int position = 0;

	if (current == nullptr)
	{
		return Result::FAIL;
	}

	while (true)
	{
		if (current->data == data)
		{
			Foundposition = position;
			return Result::SUCCESS;
		}

		position++;

		current = current->next;

		if (current == _head)
			break;
	}

	return Result::FAIL;
}


Result Collections::IDoublyCircularLinkedList::AddBefore(int dstItem ,int srcItem)
{
	NODE* current = _head;
	NODE* prev = nullptr, *next = nullptr;
	int pos = 0;

	while(true)
	{
		if (current->data == srcItem)
		{

			NODE * node = (NODE*) GetNewNodeInstance(dstItem);

			next = current;
			prev = current->previous;

			prev->next = node;
			node->previous = prev;

			node->next = next;
			next->previous = node;

			return Result::SUCCESS;
		}

		pos++;
		current = current->next;

		if (current == _head)
			break;
	}

	return Result::FAIL;
}

Result Collections::IDoublyCircularLinkedList::AddAfter(int srcItem,int destItem)
{
	NODE* current = _head;
	NODE* prev = nullptr, *next = nullptr;
	int position = 0;

	while(true)
	{
		if (current->data == srcItem)
		{

			NODE * node = (NODE*)GetNewNodeInstance(destItem);

			next = current->next;
			prev = current;

			prev->next = node;
			node->previous = prev;

			node->next = next;
			next->previous = node;

			return Result::SUCCESS;
		}
		
		position++;

		current = current->next;
		if (current == _head)
			break;
	} 

	return Result::FAIL;
}



Result Collections::IDoublyCircularLinkedList::RemoveBefore(int item)
{
	NODE* current = _head;
	NODE*  prev= nullptr, *del = nullptr, *next = nullptr;
	int position = 0;

	if (current == nullptr)
		return Result::FAIL;

	while (true)
	{
		if (current->data == item)
		{

			next = current;
			del = current->previous;
			prev = del->previous;

			prev->next = next;
			next->previous = prev;
          
			free(del);			
			return Result::SUCCESS;
		}

		position++;

		current = current->next;

		if (current == _head)
			break;
	} 

	return Result::FAIL;
}


Result Collections::IDoublyCircularLinkedList::RemoveAfter(int item)
{
	NODE* current = _head;
	NODE*  prev = nullptr, *del = nullptr, *next = nullptr;
	int position = 0;

	if (current == nullptr)
		return Result::FAIL;

	while(true)
	{
		if (current->data == item)
		{
			prev = current;
			del = prev->next;
			next = del->next;

			prev->next = next;
			next->previous = prev;

			free(del);
			return Result::SUCCESS;
		}

		position++;
		
		current = current->next;

		if (current == _head)
			break;
	}

	return Result::FAIL;
}



