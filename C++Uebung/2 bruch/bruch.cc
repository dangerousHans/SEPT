#include <iostream>
#include <sstream>
#include <string>
#include "bruch.hh"

 int ggT(int a, int b){
    if(b == 0)
        return a;
    else return ggT(b, a % b);
}

bruch::bruch(int n, int d) : num(n), denom(d) 
{
}

int bruch::getNum()
{
	return num;
}

int bruch::getDenom()
{
	return denom;
}

void bruch::setNum(int n)
{
	num = n;
}

void bruch::setDenom(int d)
{
	num = d;
}

float bruch::toFloat()
{
	return (float)num / float(denom);
}

std::string bruch::toString()
{
	std::ostringstream strout;
	strout << num << "/" << denom;
	return strout.str();
}

 // _____-----_____-----_____-----_____-----_____-----
 // _____-----_____-----_____-----_____-----_____-----
 bruch bruch::kuerzen()
{
	int ggT_var = ggT(num, denom);
	if (ggT > 0)
		return bruch(num / ggT_var, denom /ggT_var);
	else return bruch(num, denom);
}

void bruch::kuerzenInPlace()
{
	int ggT_var = ggT(num, denom);
	if (ggT > 0)
	{
		num /= ggT_var;
		denom /= ggT_var;
	} 
}

bruch bruch::mult(int m)
{
	return bruch(num * m, denom);
}

bruch bruch::mult(bruch b)
{
	return bruch(num * b.getNum(), denom * b.getDenom());
}

bruch bruch::div(int d)
{
	return mult(bruch(1, d));
}

bruch bruch::div(bruch b)
{
	return bruch(num * b.getDenom(), denom * b.getNum());
}

bruch bruch::add(int a)
{
	return bruch(num + a * denom, denom);
}

bruch bruch::add(bruch b)
{
	return bruch(num * b.getDenom() + b.getNum() * denom, denom * b.getDenom());
}

// _____-----_____-----_____-----_____-----_____-----
// _____-----_____-----_____-----_____-----_____-----
int main()
{
	bruch b1 = bruch(1, 2);
	int m = 5;
	bruch b2 = b1.mult(m);
	bruch b3 = b1.mult(b2);
	
	bruch b4 = bruch(2, 3);
	bruch b5 = bruch(2, 3);
	bruch b6 = b4.div(b5);
	
	int a = 3;
	bruch b8 = b1.add(a);
	
	bruch b9 = b2.add(b8);
	
	int d = 4;
	bruch b10 = b9.div(d);
	bruch b11 = b10.kuerzen();
	
	std::cout	<< b1.toString() << " * " << m << " = " << b2.toString() << "\n"
				<< b1.toString() << " * " << b2.toString() << " = " << b3.toString() << "\n"
				<< b4.toString() << " : " << b5.toString() << " = " << b6.toString() << "\n"
				<< b1.toString() << " + " << a << " = " << b8.toString() << "\n"
				<< b2.toString() << " + " << b8.toString() << " = " << b9.toString() << "\n"
				<< b9.toString() << " : " << d << " = " << b10.toString() << "\n"
				<< b10.toString() << " = " << b11.toString() << "\n"
				<< b10.toString() << " = " << b10.kuerzen().toString() << "\n";	
				
	//char word[3];
	//*word = 'H';
	//*(word+1) = 'a';
	//*(word+2) = 'l';
	//std::cout << *word << word[1] << word[2];

	return 0;
}
