#ifndef __FU_UTILS_SINGLETON_H__
#define __FU_UTILS_SINGLETON_H__


template<class Type>
class UtilsSingleton
{
public:
	static Type*	getSingletonPtr(void);
	static Type&	getSingletonRef(void);

	void destroySelf(void);

protected:
	UtilsSingleton();
	~UtilsSingleton();
	UtilsSingleton(const UtilsSingleton& thatObject){}
	void operator=(const UtilsSingleton& thatObject){}

private:
	static Type*	_SingletonObjectPtr;
	//static int	_RefrenceNum;
};
template<class Type>
Type*	UtilsSingleton<Type>::_SingletonObjectPtr = 0;
//int UtilsSingleton::_RefrenceNum = 0;

template<class Type>
UtilsSingleton<Type>::UtilsSingleton()
{
	/*assert(!_SingletonObjectPtr);
	if (!_SingletonObjectPtr)
	{
	_SingletonObjectPtr = static_cast<Type*>(this);
	}
	*/
}

template<class Type>
UtilsSingleton<Type>::~UtilsSingleton()
{
	//destroySelf();
}

template<class Type>
Type*	UtilsSingleton<Type>::getSingletonPtr(void)

{
	if (!_SingletonObjectPtr)
	{
		_SingletonObjectPtr = new Type; //static_cast<Type*>(this);
	}
	return _SingletonObjectPtr;
}

template<class Type>
Type&	UtilsSingleton<Type>::getSingletonRef(void)
{
	if (!_SingletonObjectPtr)
	{
		_SingletonObjectPtr = new Type; //static_cast<Type*>(this);
	}
	return (*_SingletonObjectPtr);
}

template<class Type>
void UtilsSingleton<Type>::destroySelf(void)
{
	if (_SingletonObjectPtr){
		delete _SingletonObjectPtr;
		_SingletonObjectPtr = 0;
	}
}

#endif