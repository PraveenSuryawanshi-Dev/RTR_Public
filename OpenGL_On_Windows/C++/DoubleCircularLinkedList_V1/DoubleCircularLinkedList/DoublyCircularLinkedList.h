#pragma once
namespace Collections
{
	//template <typename T>
	class DoublyCircularLinkedList
	{

	public:

		DoublyCircularLinkedList();

		bool IsListEmpty();

		void InsertEnd(int item);

		void InsertBegining(int item);

		int InsertBefore(int dataToAdd,int beforeData);
		
		int InsertAfter(int afterData,int dataToAdd);

		void PrintList() const;

		int DeleteBegining();

		int DeleteEnd() ;


		int DeleteBefore(int dataToDelete);

		int DeleteAfter(int dataToDelete);

		int FindData(int data);

		

		~DoublyCircularLinkedList();

	private:

		typedef struct _node_
		{
			int data;
			struct _node_* next;
			struct _node_* previous;
		} NODE;

		NODE* _head;
	};
}
