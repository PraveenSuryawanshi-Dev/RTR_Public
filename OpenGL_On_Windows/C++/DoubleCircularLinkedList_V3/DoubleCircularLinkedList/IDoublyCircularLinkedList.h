#pragma once
namespace Collections
{
	enum Result
	{
		FAIL,
		SUCCESS,
	};


	class IDoublyCircularLinkedList
	{

	public:
		
	    static IDoublyCircularLinkedList * GetNewInstance();

	
		virtual Result IsEmpty()=0;


		virtual void AddLast(int item)=0;


		virtual Result AddFirst(int item)=0;


		virtual Result AddBefore(int dstItem, int srcItem)=0;


		virtual Result AddAfter(int srcItem, int dstItem)=0;


		virtual void DisplayData()=0;


		virtual Result RemoveFirst()=0;


		virtual Result RemoveLast()=0;


		virtual Result RemoveBefore(int item)=0;


		virtual Result RemoveAfter(int item)=0;


		virtual Result Search(int data, int &Foundposition)=0;

		virtual void Dispose() = 0;


	};
}