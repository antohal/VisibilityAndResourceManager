/////////////
// GLOBALS //
/////////////
cbuffer HeightfieldSettings  : register(b0)
{
	float fMinLattitude;				// минимальная широта
	float fMaxLattitude;				// максимальная широта

	float fMinLongitude;				// минимальная долгота
	float fMaxLongitude;				// максимальная долгота

	float fMinHeight;					// минимальная высота (соответствующая значению 0 в данных)
	float fMaxHeight;					// максимальная высота (соответствующая значению 255 в данных)

	uint nCountX;						// количество точек по X
	uint nCountY;						// количество точек по Y
	
	uint nChannel;						// номер канала в текстуре
	
	float fWorldScale;
	
	uint vObjectCenterX0, vObjectCenterX1;
	uint vObjectCenterY0, vObjectCenterY1;
	uint vObjectCenterZ0, vObjectCenterZ1;

	uint vObjectXAxisX0, vObjectXAxisX1;
	uint vObjectXAxisY0, vObjectXAxisY1;
	uint vObjectXAxisZ0, vObjectXAxisZ1;

	uint vObjectYAxisX0, vObjectYAxisX1;
	uint vObjectYAxisY0, vObjectYAxisY1;
	uint vObjectYAxisZ0, vObjectYAxisZ1;

	uint vObjectZAxisX0, vObjectZAxisX1;
	uint vObjectZAxisY0, vObjectZAxisY1;
	uint vObjectZAxisZ0, vObjectZAxisZ1;

	/*double3	vObjectCenter;
	double3 vObjectXAxis;
	double3 vObjectYAxis;
	double3 vObjectZAxis;*/

	float temp1;
	float temp2;
};


SamplerState HeightTextureSampler
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

Texture2D				InputHeightTexture		: register(t0);
RWByteAddressBuffer 	OutVertexBuffer 		: register(u0);
RWByteAddressBuffer 	OutIndexBuffer 			: register(u1);

// получить индекс вершины
uint GetVertexId(uint ix, uint iy)
{
	return ix + iy * nCountX;
}

// получить точку на поверхности эллипсоида WGS-84
double3 GetWGS84SurfacePoint(double longitude, double lattitude)
{
	double Rmin = 6356752.3142;
	double Rmax = 6378137;

	double cosB = cos(lattitude);
	double sinB = sin(lattitude);

	double cosA = cos(longitude);
	double sinA = sin(longitude);

	double R = sqrt( Rmax*Rmax*Rmin*Rmin / (Rmin*Rmin*cosB*cosB + Rmax*Rmax*sinB*sinB) );

	return double3(
			R*cosA*cosB,
			R*sinA*cosB,
			R*sinB
		);
}

// нормаль к эллипсоиду в точке на поверхности
double3 GetWGS84SurfaceNormal(double3 in_vSurfacePoint)
{
	const double Rmin = 6356752.3142;
	const double Rmax = 6378137;

	double3 vUnnormalizedNormal = double3(
		2 * in_vSurfacePoint.x / (Rmax*Rmax),
		2 * in_vSurfacePoint.y / (Rmax*Rmax),
		2 * in_vSurfacePoint.z / (Rmin*Rmin)
	);

	return normalize(vUnnormalizedNormal);
}

// получить высоту вершины по индексам вдоль осей x и y
uint GetVertexHeight(uint ix, uint iy)
{
	/*uint ivtx = GetVertexId(ix, iy);
	
	uint addr = ivtx / 4;
	uint heights = asuint( InputHeightBuffer.Load(addr * 4 ) );

	uint aHeights[4];
	
	aHeights[0] = (heights) & 0xFF;
	aHeights[1] = (heights >> 8) & 0xFF;
	aHeights[2] = (heights >> 16) & 0xFF;
	aHeights[3] = (heights >> 24) & 0xFF;
	
	return aHeights[ivtx % 4];*/
	
	float2 texCoord;
	
	float fx = ix;
	float fy = iy;
	
	texCoord.x = fx/nCountX;
	texCoord.y = fy/nCountY;
	
	float4 TexColor = InputHeightTexture.SampleLevel(HeightTextureSampler, texCoord, 0);
	
	return TexColor.r * 255;
}

// получить позицию вершины
float3 GetVertexPos(uint ix, uint iy, uint height)
{
	double3	vObjectCenter = double3(asdouble(vObjectCenterX0, vObjectCenterX1), asdouble(vObjectCenterY0, vObjectCenterY1), asdouble(vObjectCenterZ0, vObjectCenterZ1));
	double3 vObjectXAxis = double3(asdouble(vObjectXAxisX0, vObjectXAxisX1), asdouble(vObjectXAxisY0, vObjectXAxisY1), asdouble(vObjectXAxisZ0, vObjectXAxisZ1));
	double3 vObjectYAxis = double3(asdouble(vObjectYAxisX0, vObjectYAxisX1), asdouble(vObjectYAxisY0, vObjectYAxisY1), asdouble(vObjectYAxisZ0, vObjectYAxisZ1));
	double3 vObjectZAxis = double3(asdouble(vObjectZAxisX0, vObjectZAxisX1), asdouble(vObjectZAxisY0, vObjectZAxisY1), asdouble(vObjectZAxisZ0, vObjectZAxisZ1));
	

	float fLongitudeAmpl = fMaxLongitude - fMinLongitude;
	float fLattitudeAmpl = fMaxLattitude - fMinLattitude;

	float dlong = fLongitudeAmpl / (nCountX - 1);
	float dlat = fLattitudeAmpl / (nCountY - 1);

	float longitude = fMinLongitude + ix * dlong;
	float lattitude = fMinLattitude + iy * dlat;
	
	double3 vSurfacePoint = GetWGS84SurfacePoint(longitude, lattitude);
	double3 vSurfaceNormal = GetWGS84SurfaceNormal(vSurfacePoint);

	double3 vVertex = vSurfacePoint + vSurfaceNormal*(fMinHeight + (fMaxHeight - fMinHeight) * (height / 255.f));

	double3 vDelta = fWorldScale*vVertex - vObjectCenter;

	double xCoord = dot(vDelta, vObjectXAxis);
	double yCoord = dot(vDelta, vObjectYAxis);
	double zCoord = dot(vDelta, vObjectZAxis);

	//return float3(	ix * dx - fSizeX * 0.5, 
	//				fMinHeight + (fMaxHeight - fMinHeight) * (height / 255.f),
	//				iy * dy - fSizeX * 0.5
	//				);

	return fWorldScale * float3(vSurfacePoint.x, vSurfacePoint.y, vSurfacePoint.z);// float3(xCoord, yCoord, zCoord);
}

[numthreads(1, 1, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID )
{
	uint ix = DTid.x;
	uint iy = DTid.y;


	// INDICES
	
	if (ix < nCountX - 1 && iy < nCountY - 1)
	{
		uint nStartIndex = 6 * (ix + iy * (nCountX - 1));
	
		uint i0 = iy*nCountX + ix;
		uint i1 = iy*nCountX + ix + 1;
		uint i2 = (iy + 1)*nCountX + ix;
		uint i3 = (iy + 1)*nCountX + ix + 1;

		OutIndexBuffer.Store(nStartIndex 		* 4, i3);
		OutIndexBuffer.Store((nStartIndex + 1) 	* 4, i1);
		OutIndexBuffer.Store((nStartIndex + 2) 	* 4, i0);

		OutIndexBuffer.Store((nStartIndex + 3) 	* 4, i2);
		OutIndexBuffer.Store((nStartIndex + 4) 	* 4, i3);
		OutIndexBuffer.Store((nStartIndex + 5) 	* 4, i0);
	}
	
	// VERTICES
	
	uint thisVertexHeight = GetVertexHeight(ix, iy);
	float3 vVertexPos = GetVertexPos(ix, iy, thisVertexHeight);
	
	float3 vLeftVertex = vVertexPos;
	float3 vRightVertex = vVertexPos;
	float3 vUpperVertex = vVertexPos;
	float3 vLowerVertex = vVertexPos;
	
	if (ix > 1)
	{
		uint leftVertexHeight = GetVertexHeight(ix - 1, iy);
		vLeftVertex = GetVertexPos(ix - 1, iy, leftVertexHeight);
	}
	
	if (ix < nCountX - 1)
	{
		uint rightVertexHeight = GetVertexHeight(ix + 1, iy);
		vRightVertex = GetVertexPos(ix + 1, iy, rightVertexHeight);
	}

	if (iy > 1)
	{
		uint upperVertexHeight = GetVertexHeight(ix, iy - 1);
		vUpperVertex = GetVertexPos(ix, iy - 1, upperVertexHeight);
	}
	
	if (iy < nCountY - 1)
	{
		uint lowerVertexHeight = GetVertexHeight(ix, iy + 1);
		vLowerVertex = GetVertexPos(ix, iy + 1, lowerVertexHeight);
	}

	float2 texcoord = float2((float)ix / (nCountX - 1), (float)iy / (nCountY - 1));
	
	float3 nur = cross(vRightVertex - vVertexPos, vUpperVertex - vVertexPos);
	float3 nrd = cross(vLowerVertex - vVertexPos, vRightVertex - vVertexPos);
	float3 nld = cross(vLeftVertex - vVertexPos, vLowerVertex - vVertexPos);
	float3 nul = cross(vUpperVertex - vVertexPos, vLeftVertex - vVertexPos);
	
	float3 normal = normalize(nur + nrd + nld + nul);
	
	float3 binormal = normalize(cross(float3(1, 0, 0), normal));
	
	
	// STORE IN VERTEX BUFFER
	
	uint ivtx = GetVertexId(ix, iy);
	
	uint VERTEX_SIZE = 44;
	
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE, 		asuint(vVertexPos.x));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 4, 	asuint(vVertexPos.y));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 8, 	asuint(vVertexPos.z));

	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 12, 	asuint(texcoord.x));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 16, 	asuint(texcoord.y));
	
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 20,	asuint(normal.x));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 24, 	asuint(normal.y));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 28, 	asuint(normal.z));

	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 32,	asuint(binormal.x));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 36, 	asuint(binormal.y));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 40, 	asuint(binormal.z));
}
