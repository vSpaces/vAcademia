#pragma once

namespace rmml{

class mlMapJSS2JSO
{
	typedef JSString* _K;
	typedef JSObject* _Ty;
	struct less{
		bool operator()(const _K& _X, const _K& _Y) const{
			jschar* pjscX=JS_GetStringChars(_X);
			jschar* pjscY=JS_GetStringChars(_Y);
			for(;;){
				if(*pjscX < *pjscY) return true; // <
				if(*pjscX > *pjscY) return false; // >
				if(*pjscY==L'\0') return false; // ==
				pjscX++; pjscY++;
			}
		}
	} key_compare;
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

		_Ty1 first;
		_Ty2 second;
	};

	enum _Redbl {_Red, _Black};
	//
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
	static _Nodeptr _Nil;
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
			_Nodeptr _S = MP_NEW( _Node);
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
		 MP_DELETE( _S); 
	}
	//typedef pair<iterator, bool> _Pairib;
	struct _Pairib{
		iterator first;
		bool second;
		_Pairib(const iterator& _Val1, const bool& _Val2):first(_Val1), second(_Val2){}
	};
	iterator _Insert(_Nodeptr _X, _Nodeptr _Y, const value_type& _V);
	_Pairib mlMapJSS2JSO::_insert_(const value_type& _V){
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
	void clear(){ _erase_(begin(), end()); }
	size_type size() const{return (_Size); }
	size_type _count_(const _K& _Kv) const;
public:
	mlMapJSS2JSO(){ _Init(); }
	~mlMapJSS2JSO(){
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
		if(iter==end()) return NULL;
		return iter->second;
	}
	void erase(_K ajss){
		_erase_(ajss);
	}
};

}