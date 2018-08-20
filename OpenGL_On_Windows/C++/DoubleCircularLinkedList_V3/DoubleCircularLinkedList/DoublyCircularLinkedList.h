#include "IDoublyCircularLinkedList.h"

namespace Collections
{

	class DoublyCircularLinkedList : virtual public IDoublyCircularLinkedList
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

		~DoublyCircularLinkedList();
			
		
		// Inherited via IDoublyCircularLinkedList

		virtual Result IsEmpty() override;

		virtual void AddLast(int item) override;

		virtual Result AddFirst(int item) override;

		virtual Result AddBefore(int dstItem, int srcItem) override;

		virtual Result AddAfter(int srcItem, int dstItem) override;

		virtual void DisplayData() override;

		virtual Result RemoveFirst() override;

		virtual Result RemoveLast() override;

		virtual Result RemoveBefore(int item) override;

		virtual Result RemoveAfter(int item) override;

		virtual Result Search(int data, int & Foundposition) override;

		virtual void Dispose() override;

	};

}
