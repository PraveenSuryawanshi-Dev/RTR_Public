#pragma once
namespace Collections
{
	enum Result
	{
		FAIL,
		SUCCESS,
	};

	class ICollection
	{

	public:

	    virtual Result IsEmpty() = 0;

		virtual void DisplayData() = 0;

		virtual Result Search(int data ,int &Foundposition) = 0;
	};
}