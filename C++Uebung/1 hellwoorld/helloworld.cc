#include <iostream>

using namespace std;

int main()
{
	int *intZeiger = 0;
	int intVar = 5;
	intZeiger = &intVar;
	
	cout << "Hello World\n" << *intZeiger;
	return 1;
}
