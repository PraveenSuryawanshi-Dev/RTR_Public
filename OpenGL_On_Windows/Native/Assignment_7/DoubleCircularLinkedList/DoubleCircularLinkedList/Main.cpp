#include <cstdio>
#include "DoublyCircularLinkedList.h"
#include<iostream>
int main()
{
	auto* list = new Collections::DoublyCircularLinkedList();

	list->InsertEnd(2);
	list->InsertEnd(21);
	list->InsertEnd(22);
	list->InsertEnd(23);
	list->InsertEnd(26);
	list->InsertEnd(45);
	list->InsertEnd(88);
	list->PrintList();

	std::cout << "\nData at Postion : " << list->FindData(22);

	list->InsertBegining(2222);
	list->PrintList();

	list->InsertEnd(555);
	list->PrintList();

	list->DeleteBegining();
	list->PrintList();

	list->DeleteEnd();
	list->PrintList();

	std::cout << "\nInset Before :" << list->InsertBefore(3,55);
	list->PrintList();


	std::cout << "\nInset Before :" << list->InsertBefore(26,23);
	list->PrintList();

	std::cout << "\nInset Before :" << list->InsertBefore(1,2);
	list->PrintList();


	std::cout << "\nInset After :" << list->InsertAfter(2, 55);
	list->PrintList();


	std::cout << "\nInset After :" << list->InsertAfter(26, 23);
	list->PrintList();

	std::cout << "\nInset After :" << list->InsertAfter(88, 2);
	list->PrintList();


	std::cout << "\nDelete Before :" << list->DeleteBefore(55);
	list->PrintList();


	std::cout << "\nDelete Before :" << list->DeleteBefore(23);
	list->PrintList();

	std::cout << "\nDelete Before :" << list->DeleteBefore(2);
	list->PrintList();



	std::cout << "\nDelete After :" << list->DeleteAfter(55);
	list->PrintList();


	std::cout << "\nDelete After :" << list->DeleteAfter(23);
	list->PrintList();

	std::cout << "\nDelete After :" << list->DeleteAfter(2);
	list->PrintList();

	delete list;

	getchar();
	return 1;
}
