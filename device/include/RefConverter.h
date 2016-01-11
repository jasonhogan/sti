#ifndef STI_UTILS_REFCONVERTER_H
#define STI_UTILS_REFCONVERTER_H

#include <boost/shared_ptr.hpp>

namespace STI
{
namespace Utils
{

template<typename T1, typename T2>
class RefConverter
{
protected:
	typedef boost::shared_ptr<T1> T1_ptr;
	typedef boost::shared_ptr<T2> T2_ptr;

public:

	virtual bool convert(const T1_ptr& t1, T2_ptr& t2) = 0;
	virtual bool convert(const T2_ptr& t2, T1_ptr& t1) = 0;

};


template<typename T>
class RefConverter<T, T>
{
	typedef boost::shared_ptr<T> T_ptr;

	virtual bool convert(const T_ptr& in, T_ptr& out) { out = in; return true; }
//	virtual bool convert21(const T_ptr& in, T_ptr& out) { out = in; return true; }
};

template<typename T1, typename T2>
class SymmetricRefConverter : public RefConverter<T1, T2>, public RefConverter<T2, T1>
{
public:
	typedef boost::shared_ptr<T1> T1_ptr;
	typedef boost::shared_ptr<T2> T2_ptr;
	
	virtual bool convert(const T1_ptr& t1, T2_ptr& t2) = 0;
	virtual bool convert(const T2_ptr& t2, T1_ptr& t1) = 0;
};

//Specialization takes care of degenerate case (both types the same).
template<typename T> class SymmetricRefConverter<T, T> : public RefConverter<T, T> {}




/////////////////////////////////////////////////////////
//
//template<typename T1, typename T2>
//class RefConverter
//{
//protected:
//	typedef boost::shared_ptr<T1> T1_ptr;
//	typedef boost::shared_ptr<T2> T2_ptr;
//
//public:
//
//	virtual bool convert(const T1_ptr& t1, T2_ptr& t2) = 0;
//};
//
//template<typename T1, typename T2>
//class SymRefConverter : public RefConverter<T1, T2>, public RefConverter<T2, T1>
//{
//	virtual bool convert(const T1_ptr& t1, T2_ptr& t2) = 0;
//	virtual bool convert(const T1_ptr& t1, T2_ptr& t2) = 0;
//};

//
//template<typename T1, typename T2>
//class RefConverter<T2, T1>
//{
//protected:
//	typedef boost::shared_ptr<T1> T1_ptr;
//	typedef boost::shared_ptr<T2> T2_ptr;
//
//public:
//
//	virtual bool convert(const T1_ptr& t1, T2_ptr& t2) = 0;
//	virtual bool convert(const T2_ptr& t2, T1_ptr& t1) = 0;
//};



//
//template<typename T, typename U>
//class EqualTest
//{
//	EqualTest()
//	{
//		T* t;
//		U* u;
//		test(t);
//		equal = (test(u) > 1);
//	}
//	template<class Y> int test(const Y* y) const
//	{
//		static int x = 0;
//		x++;
//		return x;
//	}
//	bool equal;
//};
//
//template<typename T, typename T>
//class Equal {};






} //namespace Utils
} //namespace STI


#endif
