/////////////
// GLOBALS //
/////////////
cbuffer HeightfieldSettings  : register(b0)
{
	float	fMinLattitude;				// минимальная широта
	float	fMaxLattitude;				// максимальная широта

	float	fMinLongitude;				// минимальная долгота
	float	fMaxLongitude;				// максимальная долгота

	uint	nCountX;					// количество точек по X
	uint	nCountY;					// количество точек по Y
	
	float	fWorldScale;				// Масштаб мира
	float	fHeightScale;				// Масштаб высоты
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
double3 GetWGS84SurfacePoint(float longitude, float lattitude)
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
float GetVertexHeight(uint ix, uint iy)
{
	float2 texCoord;
	
	float fx = ix;
	float fy = iy;
	
	texCoord.x = fx/nCountX;
	texCoord.y = fy/nCountY;
	
	float4 TexColor = InputHeightTexture.SampleLevel(HeightTextureSampler, texCoord, 0);
	
	return TexColor.r;
}

// получить позицию вершины
float3 GetVertexPos(uint ix, uint iy, float height, double3 vMiddlePoint, double3 vMiddleNormal, double3 vEast, double3 vNorth)
{
	float fLongitudeAmpl = fMaxLongitude - fMinLongitude;
	float fLattitudeAmpl = fMaxLattitude - fMinLattitude;

	float dlong = fLongitudeAmpl / (nCountX - 1);
	float dlat = fLattitudeAmpl / (nCountY - 1);

	float longitude = fMinLongitude + ix * dlong;
	float lattitude = fMinLattitude + iy * dlat;

	double3 vSurfacePoint = fWorldScale * GetWGS84SurfacePoint(longitude, lattitude);
	double3 vSurfaceNormal = GetWGS84SurfaceNormal(vSurfacePoint);

	double scaledHeight = fWorldScale * height * fHeightScale;
	
	double3 vVertex = vSurfacePoint + scaledHeight * vSurfaceNormal;

	// local coords

	double3 vDelta = vVertex - vMiddlePoint;

	double xCoord = dot(vDelta, vNorth);
	double yCoord = dot(vDelta, vMiddleNormal);
	double zCoord = dot(vDelta, vEast);

	return float3(xCoord, yCoord, zCoord);

	// global coords
	//return vVertex;
}

[numthreads(1, 1, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID )
{
	uint ix = DTid.x;
	uint iy = DTid.y;

	float middleLattitude = (fMinLattitude + fMaxLattitude)*0.5;
	float middleLongitude = (fMinLongitude + fMaxLongitude)*0.5;

	double3 vMiddlePoint = fWorldScale * GetWGS84SurfacePoint(middleLongitude, middleLattitude);
	double3 vMiddleNormal = GetWGS84SurfaceNormal(vMiddlePoint);
	double3 vEast = normalize(cross(vMiddleNormal, double3(0, 0, 1)));
	double3 vNorth = normalize(cross(vMiddleNormal, vEast));


	// INDICES
	
	if (ix < nCountX - 1 && iy < nCountY - 1)
	{
		uint nStartIndex = 6 * (ix + iy * (nCountX - 1));
	
		uint i0 = iy*nCountX + ix;
		uint i1 = iy*nCountX + ix + 1;
		uint i2 = (iy + 1)*nCountX + ix;
		uint i3 = (iy + 1)*nCountX + ix + 1;

		OutIndexBuffer.Store(nStartIndex 		* 4, i0);
		OutIndexBuffer.Store((nStartIndex + 1) 	* 4, i1);
		OutIndexBuffer.Store((nStartIndex + 2) 	* 4, i3);

		OutIndexBuffer.Store((nStartIndex + 3) 	* 4, i0);
		OutIndexBuffer.Store((nStartIndex + 4) 	* 4, i3);
		OutIndexBuffer.Store((nStartIndex + 5) 	* 4, i2);
	}
	
	// VERTICES
	
	float thisVertexHeight = GetVertexHeight(ix, iy);
	float3 vVertexPos = GetVertexPos(ix, iy, thisVertexHeight, vMiddlePoint, vMiddleNormal, vEast, vNorth);
	
	float3 vLeftVertex = vVertexPos;
	float3 vRightVertex = vVertexPos;
	float3 vUpperVertex = vVertexPos;
	float3 vLowerVertex = vVertexPos;
	
	if (ix >= 1)
	{
		float leftVertexHeight = GetVertexHeight(ix - 1, iy);
		vLeftVertex = GetVertexPos(ix - 1, iy, leftVertexHeight, vMiddlePoint, vMiddleNormal, vEast, vNorth);
	}
	
	if (ix < nCountX - 1)
	{
		float rightVertexHeight = GetVertexHeight(ix + 1, iy);
		vRightVertex = GetVertexPos(ix + 1, iy, rightVertexHeight, vMiddlePoint, vMiddleNormal, vEast, vNorth);
	}

	if (iy >= 1)
	{
		float upperVertexHeight = GetVertexHeight(ix, iy - 1);
		vUpperVertex = GetVertexPos(ix, iy - 1, upperVertexHeight, vMiddlePoint, vMiddleNormal, vEast, vNorth);
	}
	
	if (iy < nCountY - 1)
	{
		float lowerVertexHeight = GetVertexHeight(ix, iy + 1);
		vLowerVertex = GetVertexPos(ix, iy + 1, lowerVertexHeight, vMiddlePoint, vMiddleNormal, vEast, vNorth);
	}

	float2 texcoord = float2((float)ix / (nCountX - 1), (float)iy / (nCountY - 1));
	
	float3 nur = cross(vRightVertex - vVertexPos, vUpperVertex - vVertexPos);
	float3 nrd = cross(vLowerVertex - vVertexPos, vRightVertex - vVertexPos);
	float3 nld = cross(vLeftVertex - vVertexPos, vLowerVertex - vVertexPos);
	float3 nul = cross(vUpperVertex - vVertexPos, vLeftVertex - vVertexPos);
	
	float3 normal = -normalize(nur + nrd + nld + nul);
	
	float3 binormal = -normalize(cross(float3(1, 0, 0), normal));
	
	
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
