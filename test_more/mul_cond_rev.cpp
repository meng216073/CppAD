// $Id$
/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2003-15 Bradley M. Bell

CppAD is distributed under multiple licenses. This distribution is under
the terms of the
                    Eclipse Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */
/*
Test of multi-level conditional expressions reverse mode
*/

# include <cppad/cppad.hpp>

bool mul_cond_rev(void)
{
	bool ok = true;
	using CppAD::vector;
	using CppAD::NearEqual;
	double eps = 10. * std::numeric_limits<double>::epsilon();
	//
	typedef CppAD::AD<double>   a1double;
	typedef CppAD::AD<a1double> a2double;
	//
	a1double a1zero = 0.0;
	a2double a2zero = a1zero;
	a1double a1one  = 1.0;
	a2double a2one  = a1one;
	//
	// --------------------------------------------------------------------
	// create a1f = f(x)
	size_t n = 1;
# if CPPAD_USE_CPLUSPLUS_2011
	size_t m = 5;
# else
	size_t m = 4;
# endif
	vector<a2double> a2x(n), a2y(m);
	a2x[0] = a2double( 5.0 );
	Independent(a2x);
	//
	size_t i = 0;
	a2double a2tmp;
	// div
	a2y[i++]  = CondExpGt(a2x[0], a2zero, a2one / a2x[0], a2zero);
	// abs
	a2y[i++]  = CondExpGt(a2x[0], a2zero, abs( a2y[0] ), a2zero);
	// add
	a2y[i++]  = CondExpGt(a2x[0], a2zero, a2y[0] + a2y[0], a2zero);
	// acos
	a2tmp     = a2one / (a2x[0] + a2double(0.5));
	a2y[i++]  = CondExpGt(a2x[0], a2zero, acos(a2tmp), a2zero);
# if CPPAD_USE_CPLUSPLUS_2011
	// acosh
	a2y[i++]  = CondExpGt(a2x[0], a2zero, acosh( a2x[0] ), a2zero);
# endif
	//
	CppAD::ADFun<a1double> a1f;
	a1f.Dependent(a2x, a2y);
	// --------------------------------------------------------------------
	// create h = f(x)
	vector<a1double> a1x(n), a1y(m);
	a1x[0] = 5.0;
	//
	Independent(a1x);
	i = 0;
	a1double a1tmp;
	// div
	a1y[i++]  = CondExpGt(a1x[0], a1zero, a1one / a1x[0], a1zero);
	// abs
	a1y[i++]  = CondExpGt(a1x[0], a1zero, abs( a1y[0] ), a1zero);
	// add
	a1y[i++]  = CondExpGt(a1x[0], a1zero, a1y[0] + a1y[0], a1zero);
	// acos
	a1tmp     = a1one / (a1x[0] + a1double(0.5));
	a1y[i++]  = CondExpGt(a1x[0], a1zero, acos(a1tmp), a1zero);
# if CPPAD_USE_CPLUSPLUS_2011
	// acosh
	a1y[i++]  = CondExpGt(a1x[0], a1zero, acosh( a1x[0] ), a1zero);
# endif
	//
	CppAD::ADFun<double> h;
	h.Dependent(a1x, a1y);
	// --------------------------------------------------------------------
	// create g = f'(x)
	vector<a1double> a1dy(m), a1w(m);
	a1x[0] = 2.0;
	for(i = 0; i < m; i++)
		a1w[i] = 0.0;
	//
	Independent(a1x);
	a1f.Forward(0, a1x);
	//
	for(i = 0; i < m; i++)
	{	a1w[i] = 1.0;
		vector<a1double> dyi_dx = a1f.Reverse(1, a1w);
		a1dy[i] = dyi_dx[0];
		a1w[i] = 0.0;
	}
	CppAD::ADFun<double> g; // g uses reverse mode derivatives
	g.Dependent(a1x, a1dy);
	// --------------------------------------------------------------------
	// check case where x[0] > 0
	vector<double> x(1), dx(1), dg(m), dh(m);
	x[0]  = 2.0;
	dx[0] = 1.0;
	h.Forward(0, x);
	dh   = h.Forward(1, dx); // dh uses forward mode derivatives
	dg   = g.Forward(0, x);
	for(i = 0; i < m; i++)
		ok  &= NearEqual(dg[i], dh[i], eps, eps);
	// --------------------------------------------------------------------
	// check case where x[0] = 0
	x[0] = 0.0;
	dg   = g.Forward(0, x);
	h.Forward(0, x);
	dh   = h.Forward(1, dx);
	for(i = 0; i < m; i++)
	{	ok  &= dg[i] == 0.0;
		ok  &= dh[i] == 0.0;
	}
	// --------------------------------------------------------------------
	return ok;
}
