#include "ICollection.h"

namespace Collections
{
	class IDoublyCircularLinkedList : virtual public ICollection
	{

	private:


		typedef struct _node_
		{

			int data;
			struct _node_* next;
			struct _node_* previous;

		} NODE;


		void* GetNewNodeInstance(int item);


		NODE* _head;


	public:
				

		static IDoublyCircularLinkedList* GetNewInstance();


		Result IsEmpty();


		void AddLast(int item);


		Result AddFirst(int item);


		Result AddBefore(int dstItem, int srcItem);


		Result AddAfter(int srcItem, int dstItem);


		void DisplayData();


		Result RemoveFirst();


		Result RemoveLast();


		Result RemoveBefore(int item);


		Result RemoveAfter(int item);


		Result Search(int data, int &Foundposition);


		~IDoublyCircularLinkedList();
	
	};

}
