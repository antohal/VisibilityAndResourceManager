// Сингулярное разложение матрицы
template<typename Real, int nRow, int nCol>
inline bool SVD(const Matrix<Real, nRow, nCol>& in_A, Matrix<Real, nRow, nRow>& U,
                Matrix<Real, nRow, nCol>& W, Matrix<Real, nCol, nCol>& V)
{
    if(nRow == 0)
        return false;

    if(nCol > nRow)
    {
        Matrix<Real, nCol, nRow> C(transposed(in_A)), B;
        if(!SVD(C, V, B, U))
            return false;

        V = transposed(V);
        U = transposed(U);
        W = transposed(B);

        return true;
    }

    if(nCol < 1 || nRow < 1)
        return false;

    W = in_A;

    for(unsigned int i = 0; i < nCol; ++i)
    {
        if(i + 2 > nRow)
            break;

        const int i1 = i + 1;
        Real s(0);
        for(unsigned int k = i; k < nRow; ++k)
        {
            s += W[k][i] * W[k][i];
        }

        if(s > 0)
        {
            s = sqrt(s);
            const Real f = W[i][i];
            Real g = Real(1.) / ( f < 0 ? f - s : f + s );
            W[i][i] = g < 0 ? s : -s;

            for( unsigned int k = i1; k < nRow; ++k )
            {
                W[k][i] *= g;
            }

            g = Real(1.) + Real(fabs(f)) / s;

            for(unsigned int j = i1; j < nCol; ++j)
            {
                Real s = W[i][j];
                for(unsigned int k = i1; k < nRow; ++k)
                {
                    s += W[k][j] * W[k][i];
                }

                const Real h = g * s;
                W[i][j] -= h;

                for(unsigned int k = i1; k < nRow; ++k)
                {
                    W[k][j] -= h * W[k][i];
                }
            }
        }

        const unsigned int i2 = i + 2;

        if(i2 >= nCol)
            continue;
        s = 0;

        for(unsigned int k = i1; k < nCol; ++k)
        {
            s += W[i][k] * W[i][k];
        }

        if(s > 0)
        {
            s = sqrt ( s );
            const Real f = W[i][i1];
            Real g = Real(1.) / ( f < 0 ? f - s : f + s );

            W[i][i1] = g < 0 ? s : -s;

            for(unsigned int k = i2; k < nCol; ++k)
            {
                W[i][k] *= g;
            }

            g = Real(1.) + Real(fabs(f)) / s;

            for(unsigned int j = i1; j < nRow; ++j)
            {
                double s = W[j][i1];
                for(unsigned int k = i2; k < nCol; ++k)
                {
                    s += W[j][k] * W[i][k];
                }

                const double h = g * s;
                W[j][i1] -= h;

                for(unsigned int k = i2; k < nCol; ++k)
                {
                    W[j][k] -= h * W[i][k];
                }
            }
        }
    }

    U.Fill(0);
    // Накопление левосторонних преобразований
    for(unsigned int i = nRow; --i < nRow;)
    {
        U[i][i] = Real(1);
        if(i >= nCol || i >= nRow - 1)
            continue;

        if(W[i][i])
        {
            Real s(1);
            const unsigned int i1 = i + 1;
            for(unsigned int k = i1; k < nRow; ++k)
            {
                s += W[k][i] * W[k][i];
            }
            const Real g = Real(2.) / s;
            for(unsigned int j = i; j < nRow; ++j )
            {
                Real s = U[i][j];
                for ( unsigned int k = i1; k < nRow; ++k )
                {
                    s += U[k][j] * W[k][i];
                }
                const Real h = g * s;
                U[i][j] -= h;
                for ( unsigned int k = i1; k < nRow; ++k )
                {
                    U[k][j] -= h * W[k][i];
                }
            }
            for (  unsigned int k = i1; k < nRow; ++k ) W[k][i] = Real(0.);
        }
    }

    V.Fill(0);
    // Накопление правосторонних преобразований
    for ( unsigned int i = nCol; --i < nCol; )
    {
        V[i][i] = 1;
        if ( i + 2 >= nCol ) continue;
        const int i1 = i + 1;
        const int i2 = i + 2;
        if( W[i][i1] )
        {
            Real s(1);
            for ( unsigned int k = i2; k < nCol; ++k )
            {
                s += W[i][k] * W[i][k];
            }
            const Real g = Real(2.) / s;
            for ( unsigned int j = i1; j < nCol; ++j )
            {
                Real s = V[j][i1];
                for ( unsigned int k = i2; k < nCol; ++k )
                {
                    s += V[j][k] * W[i][k];
                }
                const Real h = g * s;
                V[j][i1] -= h;
                for ( unsigned int k = i2; k < nCol; ++k )
                {
                    V[j][k] -= h * W[i][k];
                }
            }
            for ( unsigned int k = i2; k < nCol; ++k ) W[i][k] = 0.;
        }
    }

    // Вычисляем матричную норму и переносим наддиагональные элементы в массив e
    Real norm = 0;
    Real * e = new Real[nCol];
    for ( unsigned int i = nCol; --i < nCol; )
    {
        Real sum = Real(fabs(W[i][i]));
        if ( i > 0 )
        {
            sum += Real(fabs(e[i] = W[i-1][i]));
            W[i-1][i] = 0.;
        }
        else
        {
            e[i] = 0.;
        }
        if ( norm < sum ) norm = sum;
    }

// Получение диагональной формы
    for ( unsigned int k = nCol; --k < nCol; )
    {
        unsigned int n = 0;
m0:
        unsigned int l = k;
        for ( ; ; --l )
        {
            if ( l == 0 ) goto m2;
            const int l1 = l - 1;
            if ( fabs ( e[l] ) + norm == norm ) goto m2;
            if ( fabs ( W[l1][l1] ) + norm == norm ) goto m1;
        }
m1:
        {
            Real c = 0;
            Real s = 1;
            const unsigned int l1 = l - 1;
            for ( unsigned int i = l; i <= k; ++i )
            {
                const Real f = s * e[i];
                e[i] *= c;
                if ( Real(fabs ( f )) + norm == norm ) goto m2;
                const Real g = W[i][i];
                const Real h = W[i][i] = sqrt ( f * f + g * g );
                c =   g / h;
                s = - f / h;
                for ( unsigned int j = 0; j < nRow; ++j )
                {
                    const Real y = U[j][l1];
                    const Real z = U[j][i ];
                    U[j][l1] = y * c + z * s;
                    U[j][i ] = z * c - y * s;
                }
            }
        }
m2:
        {
            Real z = W[k][k];
            if ( l < k )
            {
                if ( ++n == 30 ) return false; // Проверка сходимости
                Real x = W[l][l];
                Real y = W[k-1][k-1];
                Real g = e[k-1];
                Real h = e[k];
                Real f = ( ( y - z ) * ( y + z ) + ( g - h ) * ( g + h ) ) / ( 2. * h * y );
                g = Real(sqrt ( f * f + 1 ));
                f = ( ( x - z ) * ( x + z ) + h * ( y / ( f < 0 ? f - g : f + g ) - h ) ) / x;
                Real c = 1;
                Real s = 1;
                for ( unsigned int i = l + 1; i <= k; ++i )
                {
                    const int i1 = i - 1;
                    g = e[i];
                    y = W[i][i];
                    h = s * g;
                    g = c * g;
                    e[i1] = z = Real(sqrt ( f * f + h * h ));
                    c = f / z;
                    s = h / z;
                    f = x * c + g * s;
                    g = g * c - x * s;
                    h = y * s;
                    y = y * c;
                    for ( unsigned int j = 0; j < nCol; ++j )
                    {
                        x = V[i1][j];
                        z = V[i ][j];
                        V[i1][j] = x * c + z * s;
                        V[i ][j] = z * c - x * s;
                    }
                    W[i1][i1] = z = Real(sqrt ( f * f + h * h ));
                    if ( z )
                    {
                        c = f / z;
                        s = h / z;
                    }
                    f = c * g + s * y;
                    x = c * y - s * g;
                    for ( unsigned int j = 0; j < nRow; ++j )
                    {
                        y = U[j][i1];
                        z = U[j][i ];
                        U[j][i1] = y * c + z * s;
                        U[j][i ] = z * c - y * s;
                    }
                }
                e[l] = 0;
                e[k] = f;
                W[k][k] = x;
                goto m0;
            }
            if ( z < 0 )
            {
                W[k][k] = -z;
                for ( unsigned int j = 0; j < nCol; ++j ) V[k][j] = - V[k][j];
            }
        }
    }
    delete[] e;
    return true;

}


template <typename MatrixTypeA, typename MatrixTypeB>
inline bool inverse(const MatrixTypeA& in_A, MatrixTypeB& out_B, size_t M)
{
    if(M == 0)
        return false;

    double ** a;
    a = new double* [M];
    for(size_t i = 0; i < M; ++i)
    {
        a[i] = new double [M];
    }

    for(size_t i = 0; i < M; ++i)
        for(size_t j = 0; j < M; ++j)
            a[i][j] = in_A.at(i, j);

    SLU_Gauss slu(static_cast<int>(M), a);

    double* pC = new double [M*M];
    for(int i = static_cast<int>(M); --i >= 0;)
        pC[i] = 0.;

    for(int i = static_cast<int>(M); --i >= 0;)
    {
        double* pRow = new double [M];

        pC[i] = 1.;
        if(!slu.solve(pC, pRow))
        {
            slu.destroy();

            for(size_t i = 0; i < M; ++i)
                delete [] a[i];

            delete[] a;

            return false;
        }

        if(i)
            pC[i] = 0.;

        for(size_t j = 0; j < M; ++j)
            out_B.at(j, i) = pRow[j];

		delete [] pRow;
    }

    slu.destroy();

	delete [] pC;

    for(size_t i = 0; i < M; ++i)
        delete [] a[i];

    delete[] a;

    return true;
}

template <typename Real, int M, int N>
Matrix<Real, N, M>	pseudo_inverse(const Matrix<Real, M, N>& in_A)
{
    if(M == 0 || N == 0)
        return transposed(in_A);

    if(N > M)
        return transposed(pseudo_inverse(transposed(in_A)));

    Matrix<Real, M, M> U;
    Matrix<Real, M, N> S;
    Matrix<Real, N, N> V, VT;

    bool bRes = SVD(in_A, U, S, VT);
//	assert_dev(bRes, "SVD returned unsuccess");

    if(!bRes)
    {
        //cout << "SVD returned unsuccess" <<endl;
    }

    V = transposed(VT);
    Vector<Real, N> vDiag;
    bRes = S.DiagN(vDiag);
    assert_dev(bRes, L"Diag returned unsuccess");

    Matrix<Real, M, N> U2;
    for(size_t i = 0; i < M; ++i)
        for(size_t j = 0; j < N; ++j)
            U2[i][j] = U[i][j];

    const Real cfEps(0.00001);

    Matrix<Real, N, N> mS;
    mS.Zero();
    for(size_t i = 0; i < N; ++i)
    {
        if(vDiag[i] > cfEps)
            mS[i][i] = Real(1)/vDiag[i];
        else
            mS[i][i] = 0;
    }

    return mul(mul(V, mS), transposed(U2));
}


// SVD TEST
/*		Matrix<float, 4, 3> mMatr(
			Vector3f(1, 2, 3),
			Vector3f(2, 1, 2),
			Vector3f(1, 0, 1),
			Vector3f(1, 3, 4) );*/


/*Matrix<float, 4, 4> U;
Matrix<float, 4, 3> W;
Matrix<float, 3, 3> VT;
bool bRes = SVD(mMatr, U, W, VT);

Matrix<float, 4, 4> UUT = mul(U, transposed(U));
Matrix<float, 3, 3> VVT = mul(VT, transposed(VT));

Matrix<float, 4, 3> UWVT = mul(mul(U, W), VT);*/

//PINV test
/*Matrix<float, 4, 3> A(
		  Vector3f(1, 2, 3),
		  Vector3f(2, 1, 2),
		  Vector3f(1, 0, 1),
		Vector3f(1, 3, 4) );


Matrix<float, 3, 4> X = pseudo_inverse(A);
Matrix<float, 4, 3> Atest = mul(mul(A,X),A);

Matrix<float, 3, 4> Atest2 = mul(mul(X,A),X);*/

//INVERSE TEST
/*Matrix<float, 4, 4> mA(
	Vector4f(1, 2, 3, 2),
	Vector4f(2, 1, 2, 1),
	Vector4f(1, 0, 1, 0),
	Vector4f(1, 3, 4, 1) );

Matrix<float, 4, 4> mA_1;
bool bRes = inverse(mA, mA_1);

Matrix<float, 4, 4> mTest = mul(mA, mA_1);*/
