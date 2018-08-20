#include <cstdio>
#include "IDoublyCircularLinkedList.h"
#include<iostream>

int main()
{
	int item = 0;

	auto list = Collections::IDoublyCircularLinkedList::GetNewInstance();

	list->AddFirst(2);
	list->AddLast(21);
	list->AddLast(22);
	list->AddLast(23);
	list->AddLast(26);
	list->AddLast(45);
	list->AddLast(88);
	

	/*Listing Data*/
	list->DisplayData();
	/**/




	/*Search data*/
	int postion = 0;

	if (list->Search(22, postion) == Collections::Result::SUCCESS)
	{
		std::cout << "\n item at position : " << postion;
	}
	else
	{
		std::cout << "\n item not found !!! ";
	}

	/**/


	/*add data first to list*/
	list->AddFirst(2222);
	list->DisplayData();
	/**/

	/*add data last to list*/
	list->AddLast(555);
	list->DisplayData();
	/**/



	/*remove first item*/
	if (list->RemoveFirst() == Collections::Result::SUCCESS)
	{
		std::cout << "\n first item removed : ";
	}
	else
	{
		std::cout << "\n no item removed !!! ";
	}
	list->DisplayData();
	/**/



	/*remove last item*/
	if (list->RemoveLast() == Collections::Result::SUCCESS)
	{
		std::cout << "\n last item removed : " ;
	}
	else
	{
		std::cout << "\n no item removed !!! ";
	}
	list->DisplayData();
	/**/


	/*Isert item before item*/
	if (list->AddBefore(3,55) == Collections::Result::SUCCESS)
	{
		std::cout << "\n item added : ";
	}
	else
	{
		std::cout << "\n item fail to add !!! ";
	}
	list->DisplayData();
	/**/


	/*Isert item before item*/
	if (list->AddBefore(26, 23) == Collections::Result::SUCCESS)
	{
		std::cout << "\n item added : ";
	}
	else
	{
		std::cout << "\n item fail to add !!! ";
	}
	list->DisplayData();
	/**/


	/*Isert item before item*/
	if (list->AddBefore(1, 2) == Collections::Result::SUCCESS)
	{
		std::cout << "\n item added : ";
	}
	else
	{
		std::cout << "\n item fail to add !!! ";
	}
	list->DisplayData();
	/**/

	/*Isert item after item*/
	if (list->AddAfter(2, 55) == Collections::Result::SUCCESS)
	{
		std::cout << "\n item added : ";
	}
	else
	{
		std::cout << "\n item fail to add !!! ";
	}
	list->DisplayData();
	/**/


	/*Isert item after item*/
	if (list->AddAfter(26, 23) == Collections::Result::SUCCESS)
	{
		std::cout << "\n item added : ";
	}
	else
	{
		std::cout << "\n item fail to add !!! ";
	}
	list->DisplayData();
	/**/


	/*Isert item after item*/
	if (list->AddAfter(88, 2) == Collections::Result::SUCCESS)
	{
		std::cout << "\n item added : ";
	}
	else
	{
		std::cout << "\n item fail to add !!! ";
	}
	list->DisplayData();
	/**/


	/*remove item before item*/
	if (list->RemoveBefore(55) == Collections::Result::SUCCESS)
	{
		std::cout << "\n removed item : ";
	}
	else
	{
		std::cout << "\n fail to remove item !!! ";
	}
	list->DisplayData();
	/**/


	/*remove item before item*/
	if (list->RemoveBefore(23) == Collections::Result::SUCCESS)
	{
		std::cout << "\n removed item : ";
	}
	else
	{
		std::cout << "\n fail to remove item !!! ";
	}
	list->DisplayData();
	/**/


	/*remove item before item*/
	if (list->RemoveBefore(2) == Collections::Result::SUCCESS)
	{
		std::cout << "\n removed item : ";
	}
	else
	{
		std::cout << "\n fail to remove item !!! ";
	}
	list->DisplayData();
	/**/

	/*remove item after */
	if (list->RemoveAfter(55) == Collections::Result::SUCCESS)
	{
		std::cout << "\n removed item : ";
	}
	else
	{
		std::cout << "\n fail to remove item !!! ";
	}
	list->DisplayData();
	/**/



	/*remove item after */
	if (list->RemoveAfter(23) == Collections::Result::SUCCESS)
	{
		std::cout << "\n removed item : ";
	}
	else
	{
		std::cout << "\n fail to remove !!! ";
	}
	list->DisplayData();
	/**/


	list->Dispose();
	//delete list;

	getchar();
	return 1;
}
