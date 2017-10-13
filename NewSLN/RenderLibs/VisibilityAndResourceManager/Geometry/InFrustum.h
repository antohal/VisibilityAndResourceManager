#pragma once

//in_btCrossPlaneFlag - от 0 до 6 бита. Стоит 1, если пересекает плоскость. Если 0 - не пересекает, можно не проверять.
//используется когда включающий AABB проверен, и этот флаг от него. Все нули - весь внутри, но это надо проверять выше.
inline bool IsAABBInFrustum( const CBoundBox<float>& in_BoundBox, const CFrustum<float>& in_pFrustum, BYTE& io_btCrossPlaneFlag )
{
//	_mm_empty();

	Vector3D<float> vHalfSizes = in_BoundBox.GetHalfSize();
	assert_debug( vHalfSizes.x >= 0 && vHalfSizes.y >= 0 && vHalfSizes.z >= 0, L"Illegal halfsizes" );

	Vector3D<float> vCenter = in_BoundBox.vMin + vHalfSizes; //это быстрее

	BYTE btCrossBitMask = 1;

	for( BYTE btPlane = 0; btPlane < 6; btPlane ++ )
	{
		if( (io_btCrossPlaneFlag & btCrossBitMask) > 0 )
		{ //значит надо проверить
			const Plane<float>& plane = in_pFrustum[btPlane];

			(*(DWORD *)&(vHalfSizes.x)) |= ( (*(DWORD *)&(plane.N.x)) & 0x80000000 );
			(*(DWORD *)&(vHalfSizes.y)) |= ( (*(DWORD *)&(plane.N.y)) & 0x80000000 );
			(*(DWORD *)&(vHalfSizes.z)) |= ( (*(DWORD *)&(plane.N.z)) & 0x80000000 );

			Vector3D<float> vertex1 = vCenter - vHalfSizes;
			bool bResExit = dot(vertex1, plane.N) >= plane.D;

#ifdef _DEBUG

			// determine bb's vertex to test
			// dot prod >= 0 --> -; < 0 --> +
			int line1_sgn = ((plane.N.x < 0.f) << 1) - 1;
			int line2_sgn = ((plane.N.y < 0.f) << 1) - 1;
			int line3_sgn = ((plane.N.z < 0.f) << 1) - 1;

			assert_debug( abs( line1_sgn ) == 1, L"" );
			assert_debug( abs( line2_sgn ) == 1, L"" );
			assert_debug( abs( line3_sgn ) == 1, L"" );
			__assume( abs( line1_sgn ) == 1 );
			__assume( abs( line2_sgn ) == 1 );
			__assume( abs( line3_sgn ) == 1 );

			// calculate this vertex in world space
			Vector3D<float> vertex2 = vCenter + Vector3D<float>(in_BoundBox.GetHalfSize().x * line1_sgn, in_BoundBox.GetHalfSize().y * line2_sgn, in_BoundBox.GetHalfSize().z * line3_sgn);
			bool bResExit2 = dot(vertex2, plane.N) >= plane.D;
			assert_debug( bResExit == bResExit2, L"" );

#endif //_DEBUG

			// is it out?
			if( bResExit )
				return false;

			Vector3D<float> vertex_out = vCenter + vHalfSizes;
			bool bResOutExit = dot(vertex_out, plane.N) >= plane.D; //bResOutExit == 1 - значит этот AABB тоже пересекается с плоскостью
			if( !bResOutExit )
			{
				io_btCrossPlaneFlag &= ~btCrossBitMask;
			}

			(*(DWORD *)&(vHalfSizes.x)) &= 0x7FFFFFFF;
			(*(DWORD *)&(vHalfSizes.y)) &= 0x7FFFFFFF;
			(*(DWORD *)&(vHalfSizes.z)) &= 0x7FFFFFFF;
		}

		btCrossBitMask <<= 1;
	}

	return true;
}

inline bool IsAABBInFrustum( const CBoundBox<float>& in_BoundBox, const CFrustum<float>& in_pFrustum )
{
//	_mm_empty();

	Vector3D<float> vHalfSizes = in_BoundBox.GetHalfSize();
	assert_debug( vHalfSizes.x > 0.f && vHalfSizes.y > 0.f && vHalfSizes.z > 0.f, L"" );

	Vector3D<float> vCenter = in_BoundBox.vMin + vHalfSizes; //это быстрее

	for( BYTE btPlane = 0; btPlane < 6; btPlane ++ )
	{
		const Plane<float>& plane = in_pFrustum[btPlane];

		(*(DWORD *)&(vHalfSizes.x)) |= ( (*(DWORD *)&(plane.N.x)) & 0x80000000 );
		(*(DWORD *)&(vHalfSizes.y)) |= ( (*(DWORD *)&(plane.N.y)) & 0x80000000 );
		(*(DWORD *)&(vHalfSizes.z)) |= ( (*(DWORD *)&(plane.N.z)) & 0x80000000 );

		Vector3D<float> vertex1 = vCenter - vHalfSizes;
		bool bResExit = dot(vertex1, plane.N) >= plane.D;

#ifdef _DEBUG

		// determine bb's vertex to test
		// dot prod >= 0 --> -; < 0 --> +
		int line1_sgn = ((plane.N.x < 0.f) << 1) - 1;
		int line2_sgn = ((plane.N.y < 0.f) << 1) - 1;
		int line3_sgn = ((plane.N.z < 0.f) << 1) - 1;

		assert_debug( abs( line1_sgn ) == 1, L"" );
		assert_debug( abs( line2_sgn ) == 1, L"" );
		assert_debug( abs( line3_sgn ) == 1, L"" );
		__assume( abs( line1_sgn ) == 1 );
		__assume( abs( line2_sgn ) == 1 );
		__assume( abs( line3_sgn ) == 1 );

		// calculate this vertex in world space
		Vector3D<float> vertex2 = vCenter + Vector3D<float>(in_BoundBox.GetHalfSize().x * line1_sgn, in_BoundBox.GetHalfSize().y * line2_sgn, in_BoundBox.GetHalfSize().z * line3_sgn);
		bool bResExit2 = dot(vertex2, plane.N) >= plane.D;
		assert_debug( bResExit == bResExit2, L"" );

#endif //_DEBUG

		// is it out?
		if( bResExit )
			return false;

		(*(DWORD *)&(vHalfSizes.x)) &= 0x7FFFFFFF;
		(*(DWORD *)&(vHalfSizes.y)) &= 0x7FFFFFFF;
		(*(DWORD *)&(vHalfSizes.z)) &= 0x7FFFFFFF;
	}

	return true;
}

inline bool IsOBBInFrustum( const Vector3D<float>& in_vCenter, const Vector3D<float>& in_vHalfSize, const Matrix3x3<float>& in_vOrient, const CFrustum<float>& in_pFrustum )
{
	in_vOrient.AssertOrthogonal();
//	assert_andrey( in_vHalfSize.x > 0 && in_vHalfSize.y > 0 && in_vHalfSize.z > 0 );

	Matrix3x3<float> mRot;
	mRot.line1 = in_vOrient.line1 * in_vHalfSize.x;
	mRot.line2 = in_vOrient.line2 * in_vHalfSize.y;
	mRot.line3 = in_vOrient.line3 * in_vHalfSize.z;

	Matrix3x3<float> mRotTemp;

	for ( int iPlane = 0; iPlane < 6; iPlane ++ )
	{
		const Plane<float>& plane = in_pFrustum[iPlane];

		mRotTemp = mRot;

		float fDirX = dot(in_vOrient.line1, plane.N);
		DWORD dwFlagX = ( (*(DWORD *)&(fDirX)) & 0x80000000 );
		(*(DWORD *)&(mRotTemp.line1.x)) ^= dwFlagX;
		(*(DWORD *)&(mRotTemp.line1.y)) ^= dwFlagX;
		(*(DWORD *)&(mRotTemp.line1.z)) ^= dwFlagX;

		float fDirY = dot(in_vOrient.line2, plane.N);
		DWORD dwFlagY = ( (*(DWORD *)&(fDirY)) & 0x80000000 );
		(*(DWORD *)&(mRotTemp.line2.x)) ^= dwFlagY;
		(*(DWORD *)&(mRotTemp.line2.y)) ^= dwFlagY;
		(*(DWORD *)&(mRotTemp.line2.z)) ^= dwFlagY;

		float fDirZ = dot(in_vOrient.line3, plane.N);
		DWORD dwFlagZ = ( (*(DWORD *)&(fDirZ)) & 0x80000000 );
		(*(DWORD *)&(mRotTemp.line3.x)) ^= dwFlagZ;
		(*(DWORD *)&(mRotTemp.line3.y)) ^= dwFlagZ;
		(*(DWORD *)&(mRotTemp.line3.z)) ^= dwFlagZ;

		Vector3D<float> vertex = in_vCenter - mRotTemp.line1 - mRotTemp.line2 - mRotTemp.line3;

		bool bResExit = dot(vertex, plane.N) >= plane.D;

#ifdef _DEBUG
		// determine bb's vertex to test
		// dot prod >= 0 --> -; < 0 --> +
		int line1_sgn = ((dot(in_vOrient.line1, plane.N) < 0.f) << 1) - 1;
		int line2_sgn = ((dot(in_vOrient.line2, plane.N) < 0.f) << 1) - 1;
		int line3_sgn = ((dot(in_vOrient.line3, plane.N) < 0.f) << 1) - 1;

		assert_debug( abs( line1_sgn ) == 1, L"" );
		assert_debug( abs( line2_sgn ) == 1, L"" );
		assert_debug( abs( line3_sgn ) == 1, L"" );
		__assume( abs( line1_sgn ) == 1 );
		__assume( abs( line2_sgn ) == 1 );
		__assume( abs( line3_sgn ) == 1 );

		// calculate this vertex in world space
		Vector3D<float> vertex2 = in_vCenter + 
			in_vOrient.line1 * (in_vHalfSize.x * line1_sgn) +
			in_vOrient.line2 * (in_vHalfSize.y * line2_sgn) +
			in_vOrient.line3 * (in_vHalfSize.z * line3_sgn);

		bool bResExit2 = dot(vertex2, plane.N) >= plane.D;
		assert_debug( bResExit == bResExit2, L"" );

#endif //_DEBUG

		// is it out?
		if( bResExit )
			return false;
	}

	return true;
}

inline bool IsOBBInFrustum( const Vector3D<float>& in_vCenter, const Vector3D<float>& in_vHalfSize, const Matrix3x3<float>& in_vOrient, 
					const CFrustum<float>& in_pFrustum, BYTE in_btCrossPlaneFlag )
{
	assert_debug( in_btCrossPlaneFlag, L"" ); //Все нули - весь внутри, но это надо проверять выше.
	in_vOrient.AssertOrthogonal();
	assert_debug( in_vHalfSize.x >= 0 && in_vHalfSize.y >= 0 && in_vHalfSize.z >= 0, L"" );

	Matrix3x3<float> mRot;
	mRot.line1 = in_vOrient.line1 * in_vHalfSize.x;
	mRot.line2 = in_vOrient.line2 * in_vHalfSize.y;
	mRot.line3 = in_vOrient.line3 * in_vHalfSize.z;

	Matrix3x3<float> mRotTemp;

	BYTE btCrossBitMask = 1;

	// left right far low high near
	for ( int iPlane = 0; iPlane < 6; iPlane ++ )
	{
		if( (in_btCrossPlaneFlag & btCrossBitMask) > 0 )
		{ //значит надо проверить
			const Plane<float>& plane = in_pFrustum[iPlane];

			mRotTemp = mRot;

			float fDirX = dot(in_vOrient.line1, plane.N);
			DWORD dwFlagX = ( (*(DWORD *)&(fDirX)) & 0x80000000 );
			(*(DWORD *)&(mRotTemp.line1.x)) ^= dwFlagX;
			(*(DWORD *)&(mRotTemp.line1.y)) ^= dwFlagX;
			(*(DWORD *)&(mRotTemp.line1.z)) ^= dwFlagX;

			float fDirY = dot(in_vOrient.line2, plane.N);
			DWORD dwFlagY = ( (*(DWORD *)&(fDirY)) & 0x80000000 );
			(*(DWORD *)&(mRotTemp.line2.x)) ^= dwFlagY;
			(*(DWORD *)&(mRotTemp.line2.y)) ^= dwFlagY;
			(*(DWORD *)&(mRotTemp.line2.z)) ^= dwFlagY;

			float fDirZ = dot(in_vOrient.line3, plane.N);
			DWORD dwFlagZ = ( (*(DWORD *)&(fDirZ)) & 0x80000000 );
			(*(DWORD *)&(mRotTemp.line3.x)) ^= dwFlagZ;
			(*(DWORD *)&(mRotTemp.line3.y)) ^= dwFlagZ;
			(*(DWORD *)&(mRotTemp.line3.z)) ^= dwFlagZ;

			Vector3D<float> vertex = in_vCenter - mRotTemp.line1 - mRotTemp.line2 - mRotTemp.line3;

			bool bResExit = dot(vertex, plane.N) >= plane.D;

#ifdef _DEBUG
			// determine bb's vertex to test
			// dot prod >= 0 --> -; < 0 --> +
			int line1_sgn = ((dot(in_vOrient.line1, plane.N) < 0.f) << 1) - 1;
			int line2_sgn = ((dot(in_vOrient.line2, plane.N) < 0.f) << 1) - 1;
			int line3_sgn = ((dot(in_vOrient.line3, plane.N) < 0.f) << 1) - 1;

			assert_debug( abs( line1_sgn ) == 1, L"" );
			assert_debug( abs( line2_sgn ) == 1, L"" );
			assert_debug( abs( line3_sgn ) == 1, L"" );
			__assume( abs( line1_sgn ) == 1 );
			__assume( abs( line2_sgn ) == 1 );
			__assume( abs( line3_sgn ) == 1 );

			// calculate this vertex in world space
			Vector3D<float> vertex2 = in_vCenter + 
				in_vOrient.line1 * (in_vHalfSize.x * line1_sgn) +
				in_vOrient.line2 * (in_vHalfSize.y * line2_sgn) +
				in_vOrient.line3 * (in_vHalfSize.z * line3_sgn);

			bool bResExit2 = dot(vertex2, plane.N) >= plane.D;
			assert_debug( bResExit == bResExit2, L"" );

#endif //_DEBUG

			// is it out?
			if( bResExit )
				return false;
		}

		btCrossBitMask <<= 1;
	}

	return true;
}

template<typename TYPE>
inline bool IsTriangleInFrustum(const Vector3D<TYPE>& in_vPoint1, const Vector3D<TYPE>& in_vPoint2, const Vector3D<TYPE>& in_vPoint3, 
						 const CFrustum<TYPE>& in_pFrustum, BYTE in_btCrossPlaneFlag, const TYPE& in_fEpsilon)
{
	assert_debug( in_btCrossPlaneFlag, L"" ); //Все нули - весь внутри, но это надо проверять выше.

	BYTE btCrossBitMask = 1;
	for ( int iPlane = 0; iPlane < 6; iPlane ++ )
	{
		if( (in_btCrossPlaneFlag & btCrossBitMask) > 0 )
		{
			const Plane<TYPE>& plane = in_pFrustum[iPlane];
			TYPE fPD = plane.D + in_fEpsilon;

			TYPE fDP1 = dot(plane.N, in_vPoint1);
			if( fDP1 < fPD )
				continue;

			TYPE fDP2 = dot(plane.N, in_vPoint2);
			if( fDP2 < fPD )
				continue;

			TYPE fDP3 = dot(plane.N, in_vPoint3);
			if( fDP3 < fPD )
				continue;

			return false;
		}
		btCrossBitMask <<= 1;
	}
	return true;
}

template<typename TYPE>
inline bool IsSphereInFrustrum( const Vector3D<TYPE>& in_vCenter, const TYPE& in_fRadius, const CFrustum<TYPE>& in_pFrustum, BYTE in_btCullMask)
{
	assert_debug( in_fRadius >= TYPE(0), L"" );
	for( BYTE btPlane = 0, btPlaneMask = 1; btPlane < 6; btPlane ++, btPlaneMask <<= 1 )
	{
		if( !(in_btCullMask & btPlaneMask) )
			continue;
		const Plane<TYPE>& plane = in_pFrustum[btPlane];
		if (dot(in_vCenter, plane.N) - in_fRadius >= plane.D)
			return false;
	}
	return true;	// "Внутри" всех плоскостей.
}

template<typename TYPE>
inline bool IsPointInFrustum( const Vector3D<TYPE>& in_vPoint, const CFrustum<TYPE>& in_pFrustum )
{
	for ( int iPlane = 0; iPlane < 6; iPlane ++ )
	{
		const Plane<TYPE>& plane = in_pFrustum[iPlane];

		TYPE fDP = dot(plane.N, in_vPoint);
		if( fDP < plane.D )
			continue;

		return false;
	}
	return true;
}

template <typename TYPE>
inline bool UpdateFrustumPlanes(const Matrix3x3<TYPE>& in_mOrientation, const Vector3D<TYPE>& in_vPlace, const TYPE& in_fFarPlane,
	const TYPE& in_fNearPlane, const TYPE& in_fTanW, const TYPE& in_fTanH, const Matrix4x4<TYPE>& in_mViewProjection, CBoundBox<TYPE>& out_BoundBox, CFrustum<TYPE>& out_Frustum)
{
	TYPE half_w = in_fFarPlane * in_fTanW;
	TYPE half_h = in_fFarPlane * in_fTanH;

	Vector3D<TYPE> cen = in_vPlace + in_fFarPlane * in_mOrientation.forward();
	Vector3D<TYPE> tl = cen + in_mOrientation.left() * half_w + in_mOrientation.up() * half_h;
	Vector3D<TYPE> tr = cen - in_mOrientation.left() * half_w + in_mOrientation.up() * half_h;
	Vector3D<TYPE> bl = cen + in_mOrientation.left() * half_w - in_mOrientation.up() * half_h;
	Vector3D<TYPE> br = cen - in_mOrientation.left() * half_w - in_mOrientation.up() * half_h;

	out_BoundBox.Init();
	out_BoundBox.Update(in_vPlace);
	out_BoundBox.Update(tl);
	out_BoundBox.Update(tr);
	out_BoundBox.Update(bl);
	out_BoundBox.Update(br);

	out_Frustum = CFrustum<TYPE>(in_mViewProjection);

	return true;
}

template<typename TYPE>
inline bool UpdateFrustumPlanes(const Matrix3x3<TYPE>& in_mOrientation, const Vector3D<TYPE>& in_vPlace, const Vector3f in_avViewSpaceFrustumPoints[8],
	const TYPE& in_fFarPlane, const TYPE& in_fNearPlane, CBoundBox<TYPE>& out_BoundBox, CFrustum<TYPE>& out_Frustum)
{
	Vector3f vWorldSpaceFrustumPoints[8];
	for (int i = 0; i < 8; i++)
	{
		vWorldSpaceFrustumPoints[i] = in_vPlace + in_avViewSpaceFrustumPoints[i].x * in_mOrientation.left() + 
			in_avViewSpaceFrustumPoints[i].y * in_mOrientation.up() + in_avViewSpaceFrustumPoints[i].z * in_mOrientation.forward();
	}

//	Vector3D<TYPE> vPlaceViewSpace = (in_avViewSpaceFrustumPoints[0] + in_avViewSpaceFrustumPoints[1] + in_avViewSpaceFrustumPoints[2] + in_avViewSpaceFrustumPoints[3]);
//	vPlaceViewSpace *= 0.25;


	Vector3D<TYPE> tl = vWorldSpaceFrustumPoints[4];
	Vector3D<TYPE> tr = vWorldSpaceFrustumPoints[7];
	Vector3D<TYPE> bl = vWorldSpaceFrustumPoints[5];
	Vector3D<TYPE> br = vWorldSpaceFrustumPoints[6];

	Vector3D<TYPE> ntl = vWorldSpaceFrustumPoints[0];
	Vector3D<TYPE> ntr = vWorldSpaceFrustumPoints[3];
	Vector3D<TYPE> nbl = vWorldSpaceFrustumPoints[1];
	Vector3D<TYPE> nbr = vWorldSpaceFrustumPoints[2];


	out_BoundBox.Init();
	out_BoundBox.Update(tl);
	out_BoundBox.Update(tr);
	out_BoundBox.Update(bl);
	out_BoundBox.Update(br);
	out_BoundBox.Update(ntl);
	out_BoundBox.Update(ntr);
	out_BoundBox.Update(nbl);
	out_BoundBox.Update(nbr);


	out_Frustum[0].Create(ntl, tl, bl, POINTS3);	// left
	out_Frustum[1].Create(ntr, br, tr, POINTS3);	// right
	out_Frustum[2].Create(in_vPlace + in_mOrientation.forward() * in_fFarPlane,
		in_mOrientation.up(), -in_mOrientation.left(), POINTS1_VECTORS2);				// far
	out_Frustum[3].Create(nbl, bl, br, POINTS3);	// low
	out_Frustum[4].Create(ntr, tr, tl, POINTS3);	// high
	out_Frustum[5].Create(in_vPlace + in_mOrientation.forward() * in_fNearPlane,
		-in_mOrientation.left(), in_mOrientation.up(), POINTS1_VECTORS2);				// near


	//out_Frustum[0].Create(vWorldSpaceFrustumPoints[3], vWorldSpaceFrustumPoints[7], vWorldSpaceFrustumPoints[6], POINTS3); // left
	//out_Frustum[1].Create(vWorldSpaceFrustumPoints[1], vWorldSpaceFrustumPoints[5], vWorldSpaceFrustumPoints[4], POINTS3); // right
	//out_Frustum[2].Create(vWorldSpaceFrustumPoints[7], vWorldSpaceFrustumPoints[4], vWorldSpaceFrustumPoints[5], POINTS3); // far

	//out_Frustum[3].Create(vWorldSpaceFrustumPoints[5], vWorldSpaceFrustumPoints[1], vWorldSpaceFrustumPoints[2], POINTS3); // low
	//out_Frustum[4].Create(vWorldSpaceFrustumPoints[0], vWorldSpaceFrustumPoints[4], vWorldSpaceFrustumPoints[7], POINTS3); // high
	//out_Frustum[5].Create(vWorldSpaceFrustumPoints[1], vWorldSpaceFrustumPoints[0], vWorldSpaceFrustumPoints[3], POINTS3); // near

	return true;
}


template<typename TYPE>
inline bool UpdateFrustumPlanes(const Matrix3x3<TYPE>& in_mOrientation, const Vector3D<TYPE>& in_vPlace, const TYPE& in_fFarPlane, 
						 const TYPE& in_fNearPlane, const TYPE& in_fTanW, const TYPE& in_fTanH, 
						 CBoundBox<TYPE>& out_BoundBox, CFrustum<TYPE>& out_Frustum)
{
	TYPE half_w = in_fFarPlane * in_fTanW;
	TYPE half_h = in_fFarPlane * in_fTanH;

	Vector3D<TYPE> cen = in_vPlace + in_fFarPlane * in_mOrientation.forward();
	Vector3D<TYPE> tl = cen + in_mOrientation.left() * half_w + in_mOrientation.up() * half_h;
	Vector3D<TYPE> tr = cen - in_mOrientation.left() * half_w + in_mOrientation.up() * half_h;
	Vector3D<TYPE> bl = cen + in_mOrientation.left() * half_w - in_mOrientation.up() * half_h;
	Vector3D<TYPE> br = cen - in_mOrientation.left() * half_w - in_mOrientation.up() * half_h;

	out_BoundBox.Init();
	out_BoundBox.Update(in_vPlace);
	out_BoundBox.Update(tl);
	out_BoundBox.Update(tr);
	out_BoundBox.Update(bl);
	out_BoundBox.Update(br);

	out_Frustum[0].Create( in_vPlace, tl, bl, POINTS3 );	// left
	out_Frustum[1].Create( in_vPlace, br, tr, POINTS3 );	// right
	out_Frustum[2].Create( in_vPlace + in_mOrientation.forward() * in_fFarPlane, 
		in_mOrientation.up(), -in_mOrientation.left(), POINTS1_VECTORS2 );				// far
	out_Frustum[3].Create( in_vPlace, bl, br, POINTS3 );	// low
	out_Frustum[4].Create( in_vPlace, tr, tl, POINTS3 );	// high
	out_Frustum[5].Create( in_vPlace + in_mOrientation.forward() * in_fNearPlane, 
		-in_mOrientation.left(), in_mOrientation.up(), POINTS1_VECTORS2 );				// near

	return true;

}


template<typename TYPE>
inline bool UpdateFrustumPlanes(const Matrix3x3<TYPE>& in_mOrientation, const Vector3D<TYPE>& in_vPlace, const Vector3D<TYPE>& in_vHalfSizes, 
						 CBoundBox<TYPE>& out_BoundBox, CFrustum<TYPE>& out_Frustum)
{

	Vector3D<TYPE> v1 = in_vPlace + in_mOrientation.forward()*in_vHalfSizes.x + in_mOrientation.left()*in_vHalfSizes.y + in_mOrientation.up()*in_vHalfSizes.z;
	Vector3D<TYPE> v2 = in_vPlace + in_mOrientation.forward()*in_vHalfSizes.x + in_mOrientation.left()*in_vHalfSizes.y - in_mOrientation.up()*in_vHalfSizes.z;
	Vector3D<TYPE> v3 = in_vPlace + in_mOrientation.forward()*in_vHalfSizes.x - in_mOrientation.left()*in_vHalfSizes.y - in_mOrientation.up()*in_vHalfSizes.z;
	Vector3D<TYPE> v4 = in_vPlace + in_mOrientation.forward()*in_vHalfSizes.x - in_mOrientation.left()*in_vHalfSizes.y + in_mOrientation.up()*in_vHalfSizes.z;
	
	Vector3D<TYPE> v5 = in_vPlace - in_mOrientation.forward()*in_vHalfSizes.x + in_mOrientation.left()*in_vHalfSizes.y + in_mOrientation.up()*in_vHalfSizes.z;
	Vector3D<TYPE> v6 = in_vPlace - in_mOrientation.forward()*in_vHalfSizes.x + in_mOrientation.left()*in_vHalfSizes.y - in_mOrientation.up()*in_vHalfSizes.z;
	Vector3D<TYPE> v7 = in_vPlace - in_mOrientation.forward()*in_vHalfSizes.x - in_mOrientation.left()*in_vHalfSizes.y - in_mOrientation.up()*in_vHalfSizes.z;
	Vector3D<TYPE> v8 = in_vPlace - in_mOrientation.forward()*in_vHalfSizes.x - in_mOrientation.left()*in_vHalfSizes.y + in_mOrientation.up()*in_vHalfSizes.z;


	out_BoundBox.Init();
	out_BoundBox.Update(v1);
	out_BoundBox.Update(v2);
	out_BoundBox.Update(v3);
	out_BoundBox.Update(v4);
	out_BoundBox.Update(v5);
	out_BoundBox.Update(v6);
	out_BoundBox.Update(v7);
	out_BoundBox.Update(v8);


	out_Frustum[0].Create( v6, v5, v2, POINTS3 );	// 1. left
	out_Frustum[1].Create( v7, v3, v8, POINTS3 );	// 2. right
	out_Frustum[2].Create( v3, v2, v4, POINTS3 );	// 3. far
	out_Frustum[3].Create( v7, v6, v3, POINTS3 );	// 4. bottom
	out_Frustum[4].Create( v8, v4, v5, POINTS3 );	// 5. top
	out_Frustum[5].Create( v7, v8, v6, POINTS3 );	// 6. near

	return true;
}

template<typename TYPE>
inline bool ComputeFrustumCorners(const Matrix3x3<TYPE>& in_mOrientation, const Vector3D<TYPE>& in_vPlace, 
						   const TYPE& in_fFarPlane, const TYPE& in_fNearPlane,
						   const TYPE& in_fTanW, const TYPE& in_fTanH, Vector3D<TYPE> out_vVertex[8])
{
	TYPE fDistance[2] = {in_fFarPlane, in_fNearPlane};
	for ( BYTE dwIndex = 0; dwIndex < 2; dwIndex++)
	{
		BYTE k = dwIndex*4;
		TYPE fHalfWidth = fDistance[dwIndex]*in_fTanW/2;
		TYPE fHalfHeight = fDistance[dwIndex]*in_fTanH/2;
		out_vVertex[k]	 = Vector3D<TYPE>(fDistance[dwIndex], fHalfWidth, fHalfHeight);
		out_vVertex[k+1] = Vector3D<TYPE>(fDistance[dwIndex], -fHalfWidth, fHalfHeight);
		out_vVertex[k+2] = Vector3D<TYPE>(fDistance[dwIndex], -fHalfWidth, -fHalfHeight);
		out_vVertex[k+3] = Vector3D<TYPE>(fDistance[dwIndex], fHalfWidth, -fHalfHeight);
	}
	for ( BYTE dwVertex = 0; dwVertex < 8; dwVertex++)
	{
		out_vVertex[dwVertex] *= in_mMatrix;
		out_vVertex[dwVertex] += in_vPlace;
	}
	return true;
}