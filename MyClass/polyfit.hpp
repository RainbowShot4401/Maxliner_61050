#pragma once

#ifdef BOOST_UBLAS_TYPE_CHECK
#	undef BOOST_UBLAS_TYPE_CHECK
#endif
#define BOOST_UBLAS_TYPE_CHECK 0
#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include < boost/assert.hpp >
//# include <lu.hpp>
//#include <matrix.hpp>
#include <vector>
#include <stdexcept>

/*
	Finds the coefficients of a polynomial p(x) of degree n that fits the data, 
	p(x(i)) to y(i), in a least squares sense. The result p is a row vector of 
	length n+1 containing the polynomial coefficients in incremental powers.

	param:
		oX				x axis values
		oY				y axis values
		nDegree			polynomial degree including the constant

	return:
		coefficients of a polynomial starting at the constant coefficient and
		ending with the coefficient of power to nDegree. C++0x-compatible 
		compilers make returning locally created vectors very efficient.

*/
template<typename T>
std::vector<T> polyfit( const std::vector<T>& oX, const std::vector<T>& oY, int nDegree )
{
	using namespace boost::numeric::ublas;

	if ( oX.size() != oY.size() )
		throw std::invalid_argument( "X and Y vector sizes do not match" );

	// more intuative this way
	nDegree++;
	
	size_t nCount =  oX.size();
	matrix<T> oXMatrix( nCount, nDegree );
	matrix<T> oYMatrix( nCount, 1 );
	
	// copy y matrix
	for ( size_t i = 0; i < nCount; i++ )
	{
		oYMatrix(i, 0) = oY[i];
	}

	// create the X matrix
	for ( size_t nRow = 0; nRow < nCount; nRow++ )
	{
		T nVal = 1.0f;
		for ( int nCol = 0; nCol < nDegree; nCol++ )
		{
			oXMatrix(nRow, nCol) = nVal;
			nVal *= oX[nRow];
		}
	}

	// transpose X matrix
	matrix<T> oXtMatrix( trans(oXMatrix) );
	// multiply transposed X matrix with X matrix
	matrix<T> oXtXMatrix( prec_prod(oXtMatrix, oXMatrix) );
	// multiply transposed X matrix with Y matrix
	matrix<T> oXtYMatrix( prec_prod(oXtMatrix, oYMatrix) );

	// lu decomposition
	permutation_matrix<int> pert(oXtXMatrix.size1());
	const std::size_t singular = lu_factorize(oXtXMatrix, pert);
	// must be singular
	BOOST_ASSERT( singular == 0 );

	// backsubstitution
	lu_substitute(oXtXMatrix, pert, oXtYMatrix);

	// copy the result to coeff
	return std::vector<T>( oXtYMatrix.data().begin(), oXtYMatrix.data().end() );
}

/*
	Calculates the value of a polynomial of degree n evaluated at x. The input 
	argument pCoeff is a vector of length n+1 whose elements are the coefficients 
	in incremental powers of the polynomial to be evaluated.

	param:
		oCoeff			polynomial coefficients generated by polyfit() function
		oX				x axis values

	return:
		Fitted Y values. C++0x-compatible compilers make returning locally 
		created vectors very efficient.
*/
template<typename T>
std::vector<T> polyval( const std::vector<T>& oCoeff, const std::vector<T>& oX )
{
	size_t nCount =  oX.size();
	size_t nDegree = oCoeff.size();
	std::vector<T>	oY( nCount );

	for ( size_t i = 0; i < nCount; i++ )
	{
		T nY = 0;
		T nXT = 1;
		T nX = oX[i];
		for ( size_t j = 0; j < nDegree; j++ )
		{
			// multiply current x by a coefficient
			nY += oCoeff[j] * nXT;
			// power up the X
			nXT *= nX;
		}
		oY[i] = nY;
	}

	return oY;
}
