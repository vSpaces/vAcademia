
#include "mlRMML.h" // for precompile heades only
#include "jsapi.h"
#include "mlMapWCS2Int.h"
#include "config/prolog.h"

namespace rmml{

mlMapWCS2Int::_Nodeptr mlMapWCS2Int::_Nil=NULL;
size_t mlMapWCS2Int::_Nilrefs=0;

void mlMapWCS2Int::_Init(){
	if (_Nil == 0){
		_Nil = _Buynode(0, _Black);
		_Left(_Nil) = 0, _Right(_Nil) = 0; 
	}
    ++_Nilrefs;
    _Head = _Buynode(_Nil, _Red), _Size = 0;
    _Lmost() = _Head, _Rmost() = _Head; 
}

void mlMapWCS2Int::_Lrotate(_Nodeptr _X){

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

void mlMapWCS2Int::_Rrotate(_Nodeptr _X){

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

mlMapWCS2Int::iterator mlMapWCS2Int::_Insert(_Nodeptr _X, _Nodeptr _Y, const value_type& _V)
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

mlMapWCS2Int::iterator mlMapWCS2Int::_erase_(iterator _P){
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

void mlMapWCS2Int::_Erase(_Nodeptr _X){
    for (_Nodeptr _Y = _X; _Y != _Nil; _X = _Y){
		_Erase(_Right(_Y));
		_Y = _Left(_Y);
		_Destval(&_Value(_X));
		_Freenode(_X); 
	}
}

mlMapWCS2Int::iterator mlMapWCS2Int::_erase_(iterator _F, iterator _L){
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

mlMapWCS2Int::size_type mlMapWCS2Int::_count_(const _K& _Kv) const{
	_Paircc _Ans = equal_range(_Kv);
    size_type _N = 0;
//    _Distance(_Ans.first, _Ans.second, _N);
	for (; _Ans.first != _Ans.second; ++_Ans.first) ++_N; 
    return (_N); 
}

mlMapWCS2Int::size_type mlMapWCS2Int::_erase_(const _K& _X){
    _Pairii _P = equal_range(_X);
    size_type _N = 0;
//    _Distance(_P.first, _P.second, _N);
	for (; _P.first != _P.second; ++_P.first) ++_N; 
    _erase_(_P.first, _P.second);
    return (_N); 
}

}
