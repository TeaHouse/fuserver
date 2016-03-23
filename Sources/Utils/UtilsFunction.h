#ifndef __FU_UTILS_FUNCTION_H__
#define __FU_UTILS_FUNCTION_H__


#define FU_SAFE_DELETE(handler) {if (handler) delete handler; handler = NULL;}
#define FU_SAFE_DELETE_ARRAY(handler) {if (handler) delete [] handler; handler = NULL;}


#define CLASS_GET_OR_SET_PRIVATE(param, type, name) \
private: \
	type param; \
public: \
	const type& Get##name() { return param; } \
	void Set##name(const type& data) { param = data; }


#define CLASS_GET_OR_SET_PROTECTED(param, type, name) \
protected: \
	type param; \
public: \
	const type& Get##name() { return param; } \
	void Set##name(const type& data) { param = data; }


#define CLASS_GET_OR_SET_PUBLIC(param, type, name) \
public: \
	type param; \
public: \
	const type& Get##name() { return param; } \
	void Set##name(const type& data) { param = data; }



#endif