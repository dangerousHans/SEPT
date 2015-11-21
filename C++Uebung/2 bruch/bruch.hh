#include <iostream>
#include <string>

int ggT(int a, int b);

class bruch
{	
	private:
		int num;
		int denom;
		
	public:
		bruch(int n = 1, int d = 1);
		int getNum();
		int getDenom();
		void setNum(int n);
		void setDenom(int d);
		float toFloat();
		std::string toString();
			
		bruch kuerzen();
		void kuerzenInPlace();	
		bruch mult(int m);
		bruch mult(bruch b);
		bruch div(bruch b);	
		bruch div(int d);
		bruch add(int a);
		bruch add(bruch b);	
};
