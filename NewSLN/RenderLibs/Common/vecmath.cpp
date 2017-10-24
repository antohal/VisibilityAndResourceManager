#include "vecmath.h"
//#include <pthread.h>

using namespace vm;

//*********************************************************************//
//
//      Gaussian Solution of System of Linear Equations.
//
//      Решение систем линейных уравнений методом Гаусса.
//
//*************************** 24.04.2007 ******************************//

SLU_Gauss::SLU_Gauss ( unsigned int m, const double * const * u ) : n ( m ), cond ( 1e32 )
{
	if ( n < 1 )
	{
		m_pA = 0;
		return;
	}
	m_pA = new double* [n];
	for ( unsigned int i = 0; i < n; ++i )
		m_pA[i] = new double [n];

	unsigned int i, j, k;
	for ( i = 0; i < n; ++i )
	{
		for ( j = 0; j < n; ++j )
			m_pA[i][j] = u[i][j];
	}


	const unsigned int n1 = n - 1;
	ipvt = new unsigned int[n];
	ipvt[n1] = 1;
	if ( n == 1 )
	{
		if ( m_pA[0][0] != 0 ) cond = 1.;
		return;
	}
// Вычисляем 1-норму матрицы A
	double anorm = 0.;
	for ( j = 0; j < n; ++j )
	{
		double t = 0.;
		for ( i = 0; i < n; ++i ) t += fabs ( m_pA[i][j] );
		if ( anorm < t ) anorm = t;
	}
// Гауссово исключение с частичным выбором ведущего элемента
	for ( k = 0; k < n1; ++k )
	{
		unsigned int m = k;
		const unsigned int kp1 = k + 1;
		for ( i = kp1; i < n; ++i )
		{
			if ( fabs ( m_pA[i][k] ) > fabs ( m_pA[m][k] ) ) m = i;
		}
		ipvt[k] = m;
		if ( m != k ) ipvt[n1] = 1 - ipvt[n1];
		double t = m_pA[m][k];
		m_pA[m][k] = m_pA[k][k];
		m_pA[k][k] = t;
		if ( t )
		{
			t = -1. / t;
			for ( i = kp1; i < n; ++i ) m_pA[i][k] = m_pA[i][k] * t;
			for ( j = kp1; j < n; ++j )
			{
				double t = m_pA[m][j];
				m_pA[m][j] = m_pA[k][j];
				m_pA[k][j] = t;
				if ( t )
				{
					for ( i = kp1; i < n; ++i ) m_pA[i][j] += m_pA[i][k] * t;
				}
			}
		}
		else
		{
			return;
		}
	}
	if ( ! m_pA[n1][n1] ) return; // Эта строка добавлена 12.05.2004
// Вычисление обусловленности
	double * work = new double[n];
	for ( k = 0; k < n; ++k )
	{
		double t = 0.;
		for ( i = 0; i + 1 < k; ++i )
		{
			t += m_pA[i][k] * work[i];
		}
		double ek = t < 0 ? -1. : 1.;
		work[k] = - ( ek + t ) / m_pA[k][k];
	}
	for ( k = n1; k-- > 0; )
	{
		double t = 0.;
		const unsigned int kp1 = k + 1;
		for ( i = kp1; i < n; ++i )
		{
			t += m_pA[i][k] * work[k];
		}
		work[k] = t;
		const unsigned int m = ipvt[k];
		if ( m != k )
		{
			t = work[m];
			work[m] = work[k];
			work[k] = t;
		}
	}
	double ynorm = 0.;
	for ( k = 0; k < n; ++k ) ynorm += fabs ( work[k] );
	cond = 1.; // для того, чтобы сработал solve
	solve ( work, work );
	double znorm = 0.;
	for ( k = 0; k < n; ++k ) znorm += fabs ( work[k] );
	delete[] work;
	cond = anorm * znorm / ynorm;
	if ( cond < 1. ) cond = 1.;
}

bool SLU_Gauss::solve ( const double * b, double * x ) const // b[n], x[n]
{
	if ( cond + 1. == cond ) return false;
	if ( b != x )
	{
		for ( unsigned int i = 0; i < n; ++i ) x[i] = b[i];
	}
// Прямой ход
	const unsigned int n1 = n - 1;
	for ( unsigned int k = 0; k < n1; ++k )
	{
		const unsigned int m = ipvt[k];
		const double t = x[m];
		x[m] = x[k];
		x[k] = t;
		for ( unsigned int i = k + 1; i < n; ++i )
		{
			x[i] += m_pA[i][k] * t;
		}
	}
// Обратная подстановка
	for ( unsigned int k1 = n - 1; k1-- > 0; )
	{
		const unsigned int k = k1 + 1;
		x[k] /= m_pA[k][k];
		const double t = - x[k];
		for ( unsigned int i = 0; i <= k1; ++i )
		{
			x[i] += m_pA[i][k] * t;
		}
	}
	x[0] /= m_pA[0][0];
	return true;
}

bool SLU_Gauss::solve ( const double * const * u, const double * b, double * x ) const
{
	double * y = new double[4*n];
	if ( ! solve ( b, y ) )
	{
		delete[] y;
		return false;
	}
	double d0 = 0.;
	double * r = y + n;
	unsigned int i, j;
	for ( i = 0; i < n; ++i )
	{
		r[i] = b[i];
		for ( j = 0; j < n; ++j )
		{
			r[i] -= u[i][j] * y[j];
		}
		d0 += r[i] * r[i];
	}
	double * e = r + n;
	double * z = e + n;
	for (;;)
	{
		solve ( r, e );
		for ( i = 0; i < n; ++i )
		{
			z[i] = y[i];
			y[i] += e[i];
		}
		double d1 = 0.;
		for ( i = 0; i < n; ++i )
		{
			r[i] = b[i];
			for ( j = 0; j < n; ++j )
			{
				r[i] -= u[i][j] * y[j];
			}
			d1 += r[i] * r[i];
		}
		if ( 4. * d1 >= d0 )
		{
			if ( d1 < d0 )
			{
				for ( i = 0; i < n; ++i ) x[i] = y[i];
			}
			else
			{
				for ( i = 0; i < n; ++i ) x[i] = z[i];
			}
			break;
		}
		d0 = d1;
	}
	delete[] y;
	return true;
}

double SLU_Gauss::determinant () const
{
	if ( cond + 1. == cond ) return 0.;
	double d = ipvt[n-1] == 1 ? 1. : -1.;
	for ( unsigned int i = 0; i < n; ++i ) d *= m_pA[i][i];
	return d;
}

void SLU_Gauss::destroy()
{
	if(m_pA)
	{
		for(unsigned int i = 0; i < n; ++i)
		{
			double * pA = m_pA[i];

			delete [] pA;
		}

		delete[] m_pA;

//		delete[] ipvt;
	}
	m_pA = 0;
}


SLU_Gauss::~SLU_Gauss()
{
	destroy();
}


//void DynamicMatrix::multiply (const double* in_vec, double* out_vec)
//{
//	ThreadStruct data;
//
//	data._mat = this;
//	data._i1 = 0;
//	data._i2 = _N/2;
//	data._inVec = in_vec;
//	data._outVec = out_vec;
//
//	//pthread_t idProcessThread;
//	//pthread_create(&idProcessThread, NULL, &DynamicMatrix::ProcessThread, &data);
//
//	ThreadStruct mainThreadData = data;
//	mainThreadData._i1 = 0;//_N/2;
//	mainThreadData._i2 = _N;
//	ProcessThread(&mainThreadData);
//
//	//void* valuePtr = NULL;
//	//pthread_join(idProcessThread, &valuePtr);
//}
//
//void* DynamicMatrix::ProcessThread (void* pData)
//{
//	ThreadStruct* data = static_cast<ThreadStruct*>(pData);
//
//	for (size_t i = data->_i1; i < data->_i2; i++)
//	{
//		data->_outVec[i] = 0;
//		for (size_t j = 0; j < data->_mat->_N; j++)
//		{
//			data->_outVec[i] += data->_mat->at(i,j)*data->_inVec[j];
//		}
//	}
//
//	return NULL;
//}
