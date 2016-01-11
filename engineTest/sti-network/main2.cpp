
#include <boost/shared_ptr.hpp>
#include <iostream>
using namespace std;

class DistBase
{
public:

	template<class U>
	void add(const boost::shared_ptr<U>& node)
	{
	//	cout << "Unknown type." << endl;
		node->add();
	}

};

template<class T>
class Dist : public DistBase
{
public:


	void add(const boost::shared_ptr<T>& node)
	{
		node->add();
	}

	template<class U>
	void add(const boost::shared_ptr<U>& node)
	{
		cout << "Unknown type." << endl;
	}
};

class A
{
public:
	virtual void add() { cout << "Adding an A." << endl; }
};
class B : public A
{
public:
	virtual void add() { cout << "Adding an B." << endl; }
};
class C
{
public:
	virtual void add() { cout << "Adding an C." << endl; }
};


template<typename T>
class Base
{
public:
	template<typename Y>
	void get(Y* x)
	{
		cout << "Base::get(Y)" << endl;
		T t;
		T* tp = &t;
		getBase(tp);
		x = tp;
	}

	virtual void getBase(T* x) = 0;	//getBase
};


template<typename T, typename U = T>
class Derived : public Base<U>
{
public:
	using Base<U>::get;
	virtual void get(T* x)
	{
		cout << "get T" << endl;
	}
	//virtual void get(U* x)
	//{
	//	cout << "get U" << endl;
	//}
//private:
	virtual void getBase(U* x)
	{
		cout << "getBase U" << endl;
		T d;
		T* dp = &d;
		get(dp);
		x = (U*)dp;
	}

};


template<typename T>
class B2
{
public:
	virtual void get(T* t) = 0;
};
template<typename T, typename U = T>
class B3 : public B2<U>
{
public:
	virtual void getNode(T* t) = 0;
	virtual void get(U* u) = 0;
};
template<typename T, typename U = T>
class D3 : public B3<T,U>
{
public:
	virtual void getNode(T* t)
	{
		cout << "getNode" << endl;
	}
	virtual void get(U* u)
	{
		cout << "get" << endl;
	}
};

template<typename T, typename U>
class D2 : public B2<U>
{
public:
	~D2()
	{
	//T* (T::*test)() const = &T::Clone;
    //test; // suppress warnings about unused variables		(void) (*test)() const = &T::Clone;
		T t;
		this->get(&t);
		U u;
		this->get(&u);
	}

	using B2<U>::get;

	template<typename Y>
	void get(Y* y)
	{
		cout << "get Y" << endl;
	}
};

int main(int argc, char **argv)
{
	boost::shared_ptr<A> a(new A());
	boost::shared_ptr<A> b1(new B());
	boost::shared_ptr<B> b2(new B());
	boost::shared_ptr<A> b3 = b2;
	boost::shared_ptr<C> c(new C());

	Dist<A> d;

	d.add(a);
	d.add(b1);
	d.add(b2);
	d.add(b3);
	d.add(c);

	cout << "------------" << endl;

	DistBase* d2 = &d;


	d2->add(a);
	d2->add(b1);
	d2->add(b2);
	d2->add(b3);
	d2->add(c);

	cout << "------------" << endl << endl;

	Derived<double, int> derived1;
	Derived<double> derived2;
	double xd = 0;
	int xi = 0;
	derived1.get(&xd);
	derived1.get(&xi);
	
	derived2.get(&xd);
//	derived2.get(&xi);

	cout << "------------" << endl << endl;

//	D2<double, int> der2;

	D3<double,double> der3a;
	D3<double,int> der3b;
	D3<double> der3c;
	B2<int>* b2b = &der3b;

	der3a.get(&xd);
	der3a.getNode(&xd);
	der3b.get(&xi);
	der3b.getNode(&xd);
	b2b->get(&xi);

	return 0;
}
