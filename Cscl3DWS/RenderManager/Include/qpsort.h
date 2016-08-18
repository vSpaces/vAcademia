#pragma once

namespace natura3d
{
	namespace sorting
	{
		// TEMPLATE FUNCTION sort
		template<class _RI> inline
			void qpsort(_RI* _F, _RI* _L)
			{
				for(_RI* it=_F; it!=_L; it++)
					for(_RI* it2=it+1; it2!=_L; it2++)
					{
						//if( *(*it) < *(*it2))
						if( (*it)->compare_visible_depth((*it2)) < 0)
						{
							_RI	data = *it;
							*it = *it2;
							*it2 = data;
						}
					}
			}

		// TEMPLATE FUNCTION sort
		template<class _RI, class _less_compare_function>
			void qpfuncsort(_RI* _F, _RI* _L, _less_compare_function cf)
			{
				for(_RI* it=_F; it!=_L; it++)
					for(_RI* it2=it+1; it2!=_L; it2++)
					{
						if( cf(it, it2))
						{
							_RI	data = *it;
							*it = *it2;
							*it2 = data;
						}
					}
			}

		// TEMPLATE FUNCTION sort WITH 1 PARAM
		template<class _RI, class _less_compare_function>
			void qpfuncsort_1param(_RI* _F, _RI* _L, _less_compare_function cf, void* aparam)
			{
				for(_RI* it=_F; it!=_L; it++)
					for(_RI* it2=it+1; it2!=_L; it2++)
					{
						if( cf(it, it2, aparam))
						{
							_RI	data = *it;
							*it = *it2;
							*it2 = data;
						}
					}
			}


		template<class _Ty> inline
			_Ty *_Val_type(const _Ty *)
			{return ((_Ty *)0); }

		// TEMPLATE FUNCTION sort
		const int _SORT_MAX = 16;
		template<class _BI, class _Ty> inline
			void _Unguarded_insert(_BI _L, _Ty _V)
			{for (_BI _M = _L; _V < *--_M; _L = _M)
				*_L = *_M;
			*_L = _V; }

				// TEMPLATE FUNCTION iter_swap
		template<class _FI1, class _FI2> inline
			void iter_swap(_FI1 _X, _FI2 _Y)
			{_Iter_swap(_X, _Y, _Val_type(_X)); }
		template<class _FI1, class _FI2, class _Ty> inline
			void _Iter_swap(_FI1 _X, _FI2 _Y, _Ty *)
			{_Ty _Tmp = *_X;
			*_X = *_Y, *_Y = _Tmp; }

		// TEMPLATE FUNCTION _Median
		template<class _Ty, class _less_compare_function> inline
			_Ty _Median(_Ty _X, _Ty _Y, _Ty _Z, _less_compare_function cmp)
				{if (cmp(_X,_Y))
					return (cmp(_Y, _Z) ? _Y : cmp(_X, _Z) ? _Z : _X);
				else
					return (cmp(_X, _Z) ? _X : cmp(_Y, _Z) ? _Z : _Y); }

				// TEMPLATE FUNCTION copy_backward
		template<class _BI1, class _BI2> inline
			_BI2 copy_backward(_BI1 _F, _BI1 _L, _BI2 _X)
			{while (_F != _L)
				*--_X = *--_L;
			return (_X); }

		template<class _RI, class _less_compare_function> inline
		void funcsort(_RI _F, _RI _L, _less_compare_function cmp)
		{
			func_Sort_0(_F, _L, _Val_type(_F), cmp); 
		}

		template<class _RI, class _Ty, class _less_compare_function> inline
		void func_Sort_0(_RI _F, _RI _L, _Ty *, _less_compare_function cmp)
		{
			if (_L - _F <= _SORT_MAX)
				func_Insertion_sort(_F, _L, cmp);
			else
			{
				func_Sort(_F, _L, (_Ty *)0, cmp);
				func_Insertion_sort(_F, _F + _SORT_MAX, cmp);
				for (_F += _SORT_MAX; _F != _L; ++_F)
					_Unguarded_insert(_F, _Ty(*_F)); 
			}
		}

		template<class _RI, class _Ty, class _less_compare_function> inline
		void func_Sort(_RI _F, _RI _L, _Ty *, _less_compare_function cmp)
		{
			for (; _SORT_MAX < _L - _F; )
			{
				_RI _M = func_Unguarded_partition(_F, _L, _Median(_Ty(*_F),
					_Ty(*(_F + (_L - _F) / 2)), _Ty(*(_L - 1)), cmp));
				if (_L - _M <= _M - _F)
					func_Sort(_M, _L, _Val_type(_F), cmp), _L = _M;
				else
					func_Sort(_F, _M, _Val_type(_F), cmp), _F = _M; 
			}
		}

		template<class _RI, class _Ty> inline
		_RI func_Unguarded_partition(_RI _F, _RI _L, _Ty _Piv)
		{
			for (; ; ++_F)
			{
				for (; *_F < _Piv; ++_F)
					;
				for (; _Piv < *--_L; )
					;
				if (_L <= _F)
					return (_F);
				iter_swap(_F, _L); 
			}
		}

		template<class _RI, class _less_compare_function> inline
		void func_Insertion_sort(_RI _F, _RI _L, _less_compare_function cmp)
		{
			func_Insertion_sort_1(_F, _L, _Val_type(_F), cmp); 
		}


		template<class _BI, class _Ty, class _less_compare_function> inline
		void func_Insertion_sort_1(_BI _F, _BI _L, _Ty *, _less_compare_function cmp)
		{
			if (_F != _L)
				for (_BI _M = _F; ++_M != _L; )
				{
					_Ty _V = *_M;
					if (!(cmp(_M, _F)))
						_Unguarded_insert(_M, _V);
					else
					{
						copy_backward(_F, _M, _M + 1);
						*_F = _V; 
					}
				}
		}
	}
}