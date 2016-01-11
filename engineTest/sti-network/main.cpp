
template<typename T>
class A
{
public:
	typedef T value_type;
	
	virtual value_type* get() = 0;
};

class B;

class B : A<B>
{
public:
	B(double y) : x(y) {}
	double x;
	
	virtual A<B>::value_type* get()
	{
		return this;
	}
	
};


int main(int argc, char **argv)
{
	B b(2);

	double y = b.get()->x;
	return 0;
};
