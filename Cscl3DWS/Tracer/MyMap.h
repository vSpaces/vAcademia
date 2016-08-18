#pragma once

template<class T>
class Test
{
protected:
	typedef typename void* VPTR;
	static typename VPTR pp;
};

template<class T>
typename Test<T>::VPTR Test<T>::pp = NULL;

template<class _Ty>
struct MyLess{
bool operator()(const _Ty& _X, const _Ty& _Y) const
	{return (_X < _Y); }
};

template<class _K, class _Ty, class _Pr = MyLess<_K> >
class CMyMap
{
/*
	typedef wchar_t* _K;
	typedef int _Ty;
	struct less{
		bool operator()(const _K& _X, const _K& _Y) const{
			wchar_t* pwcX=_X;
			wchar_t* pwcY=_Y;
			for(;;){
//				if(*pwcX != *pwcY) // <>
//					return (*pwcX < *pwcY); // <
//				if(*pwcY==L'\0') return false; // ==
//				pwcX++; pwcY++;
				if(*pwcX < *pwcY) return true; // <
				if(*pwcX > *pwcY) return false; // >
				if(*pwcY==L'\0') return false; // ==
				pwcX++; pwcY++;
			}
		}
	} key_compare;
*/
//	typedef _Pr key_compare;
	_Pr key_compare;
	//
	typedef size_t size_type;
	typedef size_t _SIZT;
	struct pair_{
		typedef _K _Ty1;
		typedef _Ty _Ty2;
		typedef pair_ _Myt;
		typedef _Ty1 first_type;
		typedef _Ty2 second_type;
	public:
		pair_():first(_Ty1()), second(_Ty2()){}
		pair_(const _Ty1& _Val1, const _Ty2& _Val2):first(_Val1), second(_Val2){}
		pair_(const _Myt& _Right):first(_Right.first), second(_Right.second){}

		void swap(_Myt& _Right){
			_Ty1 Tmp1=first; first=_Right.first; _Right.first=Tmp1;
			_Ty2 Tmp2=second; second=_Right.second; _Right.second=Tmp2;
		}

		_Myt& operator=(const _Myt _X){
			first = _X.first;
			second = _X.second;
			return *this;
		}

		_Ty1 first;
		_Ty2 second;
	};

	enum _Redbl {_Red, _Black};
	//
protected:
	struct _Node;
	friend struct _Node;
	typedef _Node* _Nodeptr;
	struct _Node {
		_Nodeptr _Left, _Parent, _Right;
		pair_ _Value;
		_Redbl _Color;
	};

	typedef pair_ pair_type;
	typedef pair_type value_type;
	struct _Kfn{
		const _K& operator()(const pair_type& _X) const	{return (_X.first); }
	};
    typedef _Nodeptr& _Nodepref;
    typedef const _K& _Keyref;
    typedef _Redbl& _Rbref;
    typedef pair_type& _Vref;
    static _Rbref _Color(_Nodeptr _P)
            {return ((_Rbref)(*_P)._Color); }
    static _Vref _Value(_Nodeptr _P)
            {return ((_Vref)(*_P)._Value); }
    static _Keyref _Key(_Nodeptr _P)
            {return (_Kfn()(_Value(_P))); }
    static _Nodepref _Left(_Nodeptr _P)
            {return ((_Nodepref)(*_P)._Left); }
    static _Nodepref _Parent(_Nodeptr _P)
            {return ((_Nodepref)(*_P)._Parent); }
    static _Nodepref _Right(_Nodeptr _P)
            {return ((_Nodepref)(*_P)._Right); }

	typedef int difference_type;
	typedef pair_type *pointer;
	typedef const pair_type *const_pointer;
	typedef pair_type& reference;
	typedef const pair_type& const_reference;
/*	class _A{
	public:
		pointer address(reference _X) const
			{return (&_X); }
		const_pointer address(const_reference _X) const
			{return (&_X); }
		pointer allocate(size_type _N, const void *)
			{return (_Allocate((difference_type)_N, (pointer)0)); }
		char *_Charalloc(size_type _N)
			{return (_Allocate((difference_type)_N,
			(char *)0)); }
		void deallocate(void *_P, size_type)
			{operator delete(_P); }
		void construct(pointer _P, const _Ty& _V)
			{_Construct(_P, _V); }
		void destroy(pointer _P)
			{_Destroy(_P); }
		_SIZT max_size() const
			{_SIZT _N = (_SIZT)(-1) / sizeof (_Ty);
			return (0 < _N ? _N : 1); }
	} allocator;
*/

/*
template<class _C, class _Ty, class _D = ptrdiff_t>
	struct iterator {
		typedef _C iterator_category;
		typedef _Ty value_type;
		typedef _D distance_type;
	};
	struct input_iterator_tag {};
	struct forward_iterator_tag
		: public input_iterator_tag {};
	struct bidirectional_iterator_tag
		: public forward_iterator_tag {};
*/
	class iterator;
    class const_iterator;
    friend class const_iterator;
	// _C = bidirectional_iterator_tag
	// _Ty = pair_type
	// _D = difference_type
	class const_iterator{
	protected:
		_Nodeptr _Ptr;
	public:
		const_iterator(){}
		const_iterator(_Nodeptr _P):_Ptr(_P) {}
//		const_iterator(const iterator& _X):_Ptr(_X._Ptr) {}
		pair_type& operator*() const
				{return (_Value(_Ptr)); }
		pair_type* operator->() const
				{return (&**this); }
		const_iterator& operator++()
				{_Inc();
				return (*this); }
		const_iterator operator++(int)
				{const_iterator _Tmp = *this;
				++*this;
				return (_Tmp); }
		const_iterator& operator--()
				{_Dec();
				return (*this); }
		const_iterator operator--(int)
				{const_iterator _Tmp = *this;
				--*this;
				return (_Tmp); }
		bool operator==(const const_iterator& _X) const
				{return (_Ptr == _X._Ptr); }
		bool operator!=(const const_iterator& _X) const
				{return (!(*this == _X)); }
		void _Dec(){
			if (_Color(_Ptr) == _Red && _Parent(_Parent(_Ptr)) == _Ptr)
				_Ptr = _Right(_Ptr);
			else if (_Left(_Ptr) != _Nil)
				_Ptr = _Max(_Left(_Ptr));
			else{
				_Nodeptr _P;
				while (_Ptr == _Left(_P = _Parent(_Ptr)))
						_Ptr = _P;
				_Ptr = _P; 
			}
		}
		void _Inc(){
			if (_Right(_Ptr) != _Nil)
				_Ptr = _Min(_Right(_Ptr));
			else{
				_Nodeptr _P;
				while (_Ptr == _Right(_P = _Parent(_Ptr)))
					_Ptr = _P;
				if (_Right(_Ptr) != _P)
					_Ptr = _P; 
			}
		}
		_Nodeptr _Mynode() const{return (_Ptr); }
	};
	friend class iterator;
	class iterator : public const_iterator {
	public:
		iterator(){}
		iterator(_Nodeptr _P):const_iterator(_P) {}
		reference operator*() const
				{return (_Value(_Ptr)); }
		pointer operator->() const
				{return (&**this); }
		iterator& operator++()
				{_Inc();
				return (*this); }
		iterator operator++(int)
				{iterator _Tmp = *this;
				++*this;
				return (_Tmp); }
		iterator& operator--()
				{_Dec();
				return (*this); }
		iterator operator--(int)
				{iterator _Tmp = *this;
				--*this;
				return (_Tmp); }
		bool operator==(const iterator& _X) const
				{return (_Ptr == _X._Ptr); }
		bool operator!=(const iterator& _X) const
				{return (!(*this == _X)); }
	};
//	void _Distance(iterator _F, iterator _L, size_type& _N){
//		for (; _F != _L; ++_F) ++_N; 
//	}
//	void _Distance(const_iterator _F, const_iterator _L, size_type& _N){
//		for (; _F != _L; ++_F) ++_N; 
//	}

	_Nodeptr _Head;
	bool _Multi;
	size_type _Size;
	static typename _Nodeptr _Nil;
	static size_t _Nilrefs;
	void _Init();
    _Nodeptr& _Lmost(){ return (_Left(_Head)); }
    _Nodeptr& _Lmost() const{ return (_Left(_Head)); }
    _Nodeptr& _Rmost(){return (_Right(_Head)); }
    _Nodeptr& _Rmost() const{return (_Right(_Head)); }
    _Nodeptr& _Root(){return (_Parent(_Head)); }
    _Nodeptr& _Root() const{return (_Parent(_Head)); }
    iterator begin(){return (iterator(_Lmost())); }
    const_iterator begin() const{return (const_iterator(_Lmost())); }
    iterator end(){return (iterator(_Head)); }
    const_iterator end() const{return (const_iterator(_Head)); }
	void _Lrotate(_Nodeptr _X);
	void _Rrotate(_Nodeptr _X);
    static _Nodeptr _Max(_Nodeptr _P){
        while (_Right(_P) != _Nil)
			_P = _Right(_P);
		return (_P); 
	}
	static _Nodeptr _Min(_Nodeptr _P){
		while (_Left(_P) != _Nil)
			_P = _Left(_P);
		return (_P); 
	}
	_Nodeptr _Buynode(_Nodeptr _Parg, _Redbl _Carg){
			_Nodeptr _S = new _Node;
			_Parent(_S) = _Parg;
			_Color(_S) = _Carg;
			return (_S); 
	}
//void _Construct(_T1 _FARQ *_P, const _T2& _V)
//{new ((void _FARQ *)_P) _T1(_V); }
    void _Consval(value_type* _P, const value_type& _V){
		*_P=_V; // ??
//		_P = new value_type(_V); 
	}
    void _Destval(pair_* _P){ return; }
    void _Freenode(_Nodeptr _S){
		delete _S; 
	}
	//typedef pair<iterator, bool> _Pairib;
	struct _Pairib{
		iterator first;
		bool second;
		_Pairib(const iterator& _Val1, const bool& _Val2):first(_Val1), second(_Val2){}
	};
	iterator _Insert(_Nodeptr _X, _Nodeptr _Y, const value_type& _V);
	_Pairib _insert_(const value_type& _V){
		_Nodeptr _X = _Root();
		_Nodeptr _Y = _Head;
		bool _Ans = true;
			while (_X != _Nil)
				{_Y = _X;
				_Ans = key_compare(_Kfn()(_V), _Key(_X));
				_X = _Ans ? _Left(_X) : _Right(_X); }
		if (_Multi)
				return (_Pairib(_Insert(_X, _Y, _V), true));
		iterator _P = iterator(_Y);
		if (!_Ans)
				;
		else if (_P == begin())
				return (_Pairib(_Insert(_X, _Y, _V), true));
		else
				--_P;
		if (key_compare(_Key(_P._Mynode()), _Kfn()(_V)))
				return (_Pairib(_Insert(_X, _Y, _V), true));
		return (_Pairib(_P, false)); 
	}
    _Nodeptr _Lbound(const _K& _Kv) const{
        _Nodeptr _X = _Root();
        _Nodeptr _Y = _Head;
        while (_X != _Nil){
            if (key_compare(_Key(_X), _Kv)){
				_X = _Right(_X);
            }else{
				_Y = _X, _X = _Left(_X);
            }
		}
        return (_Y); 
	}
    _Nodeptr _Ubound(const _K& _Kv) const{
        _Nodeptr _X = _Root();
        _Nodeptr _Y = _Head;
        while (_X != _Nil)
            if (key_compare(_Kv, _Key(_X))){
                _Y = _X, _X = _Left(_X);
            }else{
                _X = _Right(_X);
			}
        return (_Y); 
	}
    iterator lower_bound(const _K& _Kv){return (iterator(_Lbound(_Kv))); }
    const_iterator lower_bound(const _K& _Kv) const{return (const_iterator(_Lbound(_Kv))); }
	iterator upper_bound(const _K& _Kv){return (iterator(_Ubound(_Kv))); }
	const_iterator upper_bound(const _K& _Kv) const{return (iterator(_Ubound(_Kv))); }
    iterator _find_(const _K& _Kv){
        iterator _P = lower_bound(_Kv);
        return (_P == end() || key_compare(_Kv, _Key(_P._Mynode()))
                        ? end() : _P); 
	}
//    const_iterator _find_(const _K& _Kv) const{
//        const_iterator _P = lower_bound(_Kv);
//        return (_P == end() || key_compare(_Kv, _Key(_P._Mynode()))
//                    ? end() : _P); 
//	}
	struct _Pairii{
		iterator first;
		iterator second;
		_Pairii(iterator& _Val1, iterator& _Val2):first(_Val1), second(_Val2){}
	};
	struct _Paircc{
		const_iterator first;
		const_iterator second;
		_Paircc(const_iterator& _Val1, const_iterator& _Val2):first(_Val1), second(_Val2){}
	};
    _Pairii equal_range(const _K& _Kv){return (_Pairii(lower_bound(_Kv), upper_bound(_Kv))); }
	_Paircc equal_range(const _K& _Kv) const{return (_Paircc(lower_bound(_Kv), upper_bound(_Kv))); }
	void _Erase(_Nodeptr _X);
	iterator _erase_(iterator _P);
	iterator _erase_(iterator _F, iterator _L);
	size_type _erase_(const _K& _X);
	void _erase_(const _K *_F, const _K *_L){
		for (; _F != _L; ++_F) erase(*_F); 
	}
	size_type size() const{return (_Size); }
	size_type _count_(const _K& _Kv) const;
public:
	CMyMap(){ _Init(); }
	~CMyMap(){
        _erase_(begin(), end());
        _Freenode(_Head);
        _Head = 0, _Size = 0;
        if (--_Nilrefs == 0){
            _Freenode(_Nil);
            _Nil = 0; 
		}
	}

	void insert(_K ajss, _Ty ajso){
		_insert_(pair_(ajss,ajso));
	}
	_Ty find(_K ajss){
	    iterator iter=_find_(ajss);
		if(iter==end()) return _Ty();
		return iter->second;
	}
	void erase(_K ajss){
		_erase_(ajss);
	}
	void clear(){ _erase_(begin(), end()); }
};

template<class _K, class _Ty, class _Pr>
typename CMyMap<_K, _Ty, _Pr>::_Nodeptr CMyMap<_K, _Ty, _Pr>::_Nil = 0;

template<class _K, class _Ty, class _Pr>
size_t CMyMap<_K, _Ty, _Pr>::_Nilrefs = 0;

template<class _K, class _Ty, class _Pr>
void CMyMap<_K, _Ty, _Pr>::_Init(){
	if (_Nil == 0){
		_Nil = _Buynode(0, _Black);
		_Left(_Nil) = 0, _Right(_Nil) = 0; 
	}
	++_Nilrefs;
	_Head = _Buynode(_Nil, _Red), _Size = 0;
	_Lmost() = _Head, _Rmost() = _Head; 
}

template<class _K, class _Ty, class _Pr>
void CMyMap<_K, _Ty, _Pr>::_Lrotate(_Nodeptr _X){

	_Nodeptr _Y = _Right(_X);
	_Right(_X) = _Left(_Y);
	if (_Left(_Y) != _Nil)
		_Parent(_Left(_Y)) = _X;
	_Parent(_Y) = _Parent(_X);
	if (_X == _Root())
		_Root() = _Y;
	else if (_X == _Left(_Parent(_X)))
		_Left(_Parent(_X)) = _Y;
	else
		_Right(_Parent(_X)) = _Y;
	_Left(_Y) = _X;
	_Parent(_X) = _Y; 
}

template<class _K, class _Ty, class _Pr>
void CMyMap<_K, _Ty, _Pr>::_Rrotate(_Nodeptr _X){

	_Nodeptr _Y = _Left(_X);
	_Left(_X) = _Right(_Y);
	if (_Right(_Y) != _Nil)
		_Parent(_Right(_Y)) = _X;
	_Parent(_Y) = _Parent(_X);
	if (_X == _Root())
		_Root() = _Y;
	else if (_X == _Right(_Parent(_X)))
		_Right(_Parent(_X)) = _Y;
	else
		_Left(_Parent(_X)) = _Y;
	_Right(_Y) = _X;
	_Parent(_X) = _Y; 
}

template<class _K, class _Ty, class _Pr>
typename CMyMap<_K, _Ty, _Pr>::iterator CMyMap<_K, _Ty, _Pr>::_Insert(_Nodeptr _X, _Nodeptr _Y, const value_type& _V)
{
	_Nodeptr _Z = _Buynode(_Y, _Red);
	_Left(_Z) = _Nil, _Right(_Z) = _Nil;
	_Consval(&_Value(_Z), _V);
	++_Size;
	if (_Y == _Head || _X != _Nil
		|| key_compare(_Kfn()(_V), _Key(_Y)))
	{_Left(_Y) = _Z;
	if (_Y == _Head)
	{_Root() = _Z;
	_Rmost() = _Z; }
	else if (_Y == _Lmost())
		_Lmost() = _Z; }
	else
	{_Right(_Y) = _Z;
	if (_Y == _Rmost())
		_Rmost() = _Z; }
	for (_X = _Z; _X != _Root()
		&& _Color(_Parent(_X)) == _Red; )
		if (_Parent(_X) == _Left(_Parent(_Parent(_X))))
		{_Y = _Right(_Parent(_Parent(_X)));
	if (_Color(_Y) == _Red)
	{_Color(_Parent(_X)) = _Black;
	_Color(_Y) = _Black;
	_Color(_Parent(_Parent(_X))) = _Red;
	_X = _Parent(_Parent(_X)); }
	else
	{if (_X == _Right(_Parent(_X)))
	{_X = _Parent(_X);
	_Lrotate(_X); }
	_Color(_Parent(_X)) = _Black;
	_Color(_Parent(_Parent(_X))) = _Red;
	_Rrotate(_Parent(_Parent(_X))); }}
		else
		{_Y = _Left(_Parent(_Parent(_X)));
	if (_Color(_Y) == _Red)
	{_Color(_Parent(_X)) = _Black;
	_Color(_Y) = _Black;
	_Color(_Parent(_Parent(_X))) = _Red;
	_X = _Parent(_Parent(_X)); }
	else
	{if (_X == _Left(_Parent(_X)))
	{_X = _Parent(_X);
	_Rrotate(_X); }
	_Color(_Parent(_X)) = _Black;
	_Color(_Parent(_Parent(_X))) = _Red;
	_Lrotate(_Parent(_Parent(_X))); }}
		_Color(_Root()) = _Black;
		return (iterator(_Z)); 
}

template<class _K, class _Ty, class _Pr>
typename CMyMap<_K, _Ty, _Pr>::iterator CMyMap<_K, _Ty, _Pr>::_erase_(iterator _P){
	_Nodeptr _X;
	_Nodeptr _Y = (_P++)._Mynode();
	_Nodeptr _Z = _Y;
	if (_Left(_Y) == _Nil)
		_X = _Right(_Y);
	else if (_Right(_Y) == _Nil)
		_X = _Left(_Y);
	else
		_Y = _Min(_Right(_Y)), _X = _Right(_Y);
	if (_Y != _Z){
		_Parent(_Left(_Z)) = _Y;
		_Left(_Y) = _Left(_Z);
		if (_Y == _Right(_Z))
			_Parent(_X) = _Y;
		else
		{_Parent(_X) = _Parent(_Y);
		_Left(_Parent(_Y)) = _X;
		_Right(_Y) = _Right(_Z);
		_Parent(_Right(_Z)) = _Y; }
		if (_Root() == _Z)
			_Root() = _Y;
		else if (_Left(_Parent(_Z)) == _Z)
			_Left(_Parent(_Z)) = _Y;
		else
			_Right(_Parent(_Z)) = _Y;
		_Parent(_Y) = _Parent(_Z);
		_Redbl tmp=_Color(_Y); _Color(_Y)=_Color(_Z); _Color(_Z)=tmp;
		_Y = _Z;
	}else{
		_Parent(_X) = _Parent(_Y);
		if (_Root() == _Z)
			_Root() = _X;
		else if (_Left(_Parent(_Z)) == _Z)
			_Left(_Parent(_Z)) = _X;
		else
			_Right(_Parent(_Z)) = _X;
		if (_Lmost() != _Z)
			;
		else if (_Right(_Z) == _Nil)
			_Lmost() = _Parent(_Z);
		else
			_Lmost() = _Min(_X);
		if (_Rmost() != _Z)
			;
		else if (_Left(_Z) == _Nil)
			_Rmost() = _Parent(_Z);
		else
			_Rmost() = _Max(_X); 
	}
	if (_Color(_Y) == _Black){
		while (_X != _Root() && _Color(_X) == _Black)
			if (_X == _Left(_Parent(_X)))
			{_Nodeptr _W = _Right(_Parent(_X));
		if (_Color(_W) == _Red)
		{_Color(_W) = _Black;
		_Color(_Parent(_X)) = _Red;
		_Lrotate(_Parent(_X));
		_W = _Right(_Parent(_X)); }
		if (_Color(_Left(_W)) == _Black
			&& _Color(_Right(_W)) == _Black)
		{_Color(_W) = _Red;
		_X = _Parent(_X); }
		else
		{if (_Color(_Right(_W)) == _Black)
		{_Color(_Left(_W)) = _Black;
		_Color(_W) = _Red;
		_Rrotate(_W);
		_W = _Right(_Parent(_X)); }
		_Color(_W) = _Color(_Parent(_X));
		_Color(_Parent(_X)) = _Black;
		_Color(_Right(_W)) = _Black;
		_Lrotate(_Parent(_X));
		break; }}
			else
			{_Nodeptr _W = _Left(_Parent(_X));
		if (_Color(_W) == _Red)
		{_Color(_W) = _Black;
		_Color(_Parent(_X)) = _Red;
		_Rrotate(_Parent(_X));
		_W = _Left(_Parent(_X)); }
		if (_Color(_Right(_W)) == _Black
			&& _Color(_Left(_W)) == _Black)
		{_Color(_W) = _Red;
		_X = _Parent(_X); }
		else
		{if (_Color(_Left(_W)) == _Black)
		{_Color(_Right(_W)) = _Black;
		_Color(_W) = _Red;
		_Lrotate(_W);
		_W = _Left(_Parent(_X)); }
		_Color(_W) = _Color(_Parent(_X));
		_Color(_Parent(_X)) = _Black;
		_Color(_Left(_W)) = _Black;
		_Rrotate(_Parent(_X));
		break; }}
			_Color(_X) = _Black; 
	}
	_Destval(&_Value(_Y));
	_Freenode(_Y);
	--_Size;
	return (_P); 
}

template<class _K, class _Ty, class _Pr>
void CMyMap<_K, _Ty, _Pr>::_Erase(_Nodeptr _X){
	for (_Nodeptr _Y = _X; _Y != _Nil; _X = _Y){
		_Erase(_Right(_Y));
		_Y = _Left(_Y);
		_Destval(&_Value(_X));
		_Freenode(_X); 
	}
}

template<class _K, class _Ty, class _Pr>
typename CMyMap<_K, _Ty, _Pr>::iterator CMyMap<_K, _Ty, _Pr>::_erase_(iterator _F, iterator _L){
	if (size() == 0 || _F != begin() || _L != end()){
		while (_F != _L)
			_erase_(_F++);
		return (_F); 
	}else{
		_Erase(_Root());
		_Root() = _Nil, _Size = 0;
		_Lmost() = _Head, _Rmost() = _Head;
		return (begin()); 
	}
}

template<class _K, class _Ty, class _Pr>
typename CMyMap<_K, _Ty, _Pr>::size_type CMyMap<_K, _Ty, _Pr>::_count_(const _K& _Kv) const{
	_Paircc _Ans = equal_range(_Kv);
	size_type _N = 0;
	//    _Distance(_Ans.first, _Ans.second, _N);
	for (; _Ans.first != _Ans.second; ++_Ans.first) ++_N; 
	return (_N); 
}

template<class _K, class _Ty, class _Pr>
typename CMyMap<_K, _Ty, _Pr>::size_type CMyMap<_K, _Ty, _Pr>::_erase_(const _K& _X){
	_Pairii _P = equal_range(_X);
	size_type _N = 0;
	//    _Distance(_P.first, _P.second, _N);
	for (; _P.first != _P.second; ++_P.first) ++_N; 
	_erase_(_P.first, _P.second);
	return (_N); 
}
