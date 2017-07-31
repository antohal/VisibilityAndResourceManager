//
//	Ўейдер дл€ расчета триангул€ции по карте высот
//

cbuffer HeightfieldSettings  : register(b0)
{
	float	fMinLattitude;				// минимальна€ широта
	float	fMaxLattitude;				// максимальна€ широта

	float	fMinLongitude;				// минимальна€ долгота
	float	fMaxLongitude;				// максимальна€ долгота

	uint	nCountX;					// количество точек по X
	uint	nCountY;					// количество точек по Y


	float	fLongitudeCoeff;			// максимальна€ текстурна€ координата по долготе
	float	fLattitudeCoeff;			// максимальна€ текстурна€ координата по широте

	float	fWorldScale;				// ћасштаб мира
	float	fHeightScale;				// ћасштаб высоты

	float	fNormalDivisionAngleCos1;	//  осинус минимального угла разделени€ нормалей
	float	fNormalDivisionAngleCos2;

	float	fNorthBlockLongCoeff;
	float	fNorthBlockLatCoeff;

	float	fEastBlockLongCoeff;
	float	fEastBlockLatCoeff;

	float	fSouthBlockLongCoeff;
	float	fSouthBlockLatCoeff;

	float	fWestBlockLongCoeff;
	float	fWestBlockLatCoeff;

	
	float	fNorthMinLat;
	float	fNorthMaxLat;

	float	fNorthMinLong;
	float	fNorthMaxLong;


	float	fEastMinLat;
	float	fEastMaxLat;

	float	fEastMinLong;
	float	fEastMaxLong;


	float	fSouthMinLat;
	float	fSouthMaxLat;

	float	fSouthMinLong;
	float	fSouthMaxLong;


	float	fWestMinLat;
	float	fWestMaxLat;

	float	fWestMinLong;
	float	fWestMaxLong;
};


SamplerState HeightTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Mirror;
	AddressV = Mirror;
};

Texture2D				InputHeightTexture				: register(t0);

Texture2D				NorthNeighbourTexture			: register(t1);		// север
Texture2D				EastNeighbourTexture			: register(t2);
Texture2D				SouthNeighbourTexture			: register(t3);
Texture2D				WestNeighbourTexture			: register(t4);


RWByteAddressBuffer 	OutVertexBuffer 		: register(u0);
RWByteAddressBuffer 	OutIndexBuffer 			: register(u1);

// получить точку на поверхности эллипсоида WGS-84
double3 GetWGS84SurfacePoint(float longitude, float lattitude)
{
	double Rmin = 6356752.3142;
	double Rmax = 6378137;

	double cosB = cos(lattitude);
	double sinB = sin(lattitude);

	double cosA = cos(longitude);
	double sinA = sin(longitude);

	double R = sqrt(Rmax*Rmax*Rmin*Rmin / (Rmin*Rmin*cosB*cosB + Rmax*Rmax*sinB*sinB));

	return double3(R*sinA*cosB, R*sinB, -R*cosA*cosB);
}

// нормаль к эллипсоиду в точке на поверхности
double3 GetWGS84SurfaceNormal(double3 in_vSurfacePoint)
{
	const double Rmin = 6356752.3142;
	const double Rmax = 6378137;

	double3 vUnnormalizedNormal = double3(
		2 * in_vSurfacePoint.x / (Rmax*Rmax),
		2 * in_vSurfacePoint.y / (Rmin*Rmin),
		2 * in_vSurfacePoint.z / (Rmax*Rmax)
		);

	return normalize(vUnnormalizedNormal);
}

float2 CalcTexcoords(uint ix, uint iy, float longCoeff, float latCoeff)
{
	return float2(
		longCoeff *(float)iy / (nCountY - 1),
		latCoeff * (1 - (float)ix / (nCountX - 1))
		);
}

// получить высоту вершины по индексам вдоль осей x и y
float GetVertexHeight(uint ix, uint iy, in Texture2D tex, float longCoeff, float latCoeff)
{
	float2 texCoord;

	float fx = ix;
	float fy = iy;

	texCoord = CalcTexcoords(ix, iy, longCoeff, latCoeff);

	float4 TexColor = tex.SampleLevel(HeightTextureSampler, texCoord, 0);

	return TexColor.r;
}

// получить позицию вершины
float3 GetVertexPos(uint ix, uint iy, in Texture2D tex, float longCoeff, float latCoeff, float minLat, float maxLat, float minLong, float maxLong)
{
	float height = GetVertexHeight(ix, iy, tex, longCoeff, latCoeff);

	float fLongitudeAmpl = maxLong - minLong;
	float fLattitudeAmpl = maxLat - minLat;

	float dlong = fLongitudeAmpl / (nCountY - 1);
	float dlat = fLattitudeAmpl / (nCountX - 1);

	float longitude = minLong + iy * dlong;
	float lattitude = minLat + ix * dlat;

	double3 vSurfacePoint = fWorldScale * GetWGS84SurfacePoint(longitude, lattitude);
	double3 vSurfaceNormal = GetWGS84SurfaceNormal(vSurfacePoint);

	double scaledHeight = fWorldScale * height * fHeightScale;

	double3 vVertex = vSurfacePoint + scaledHeight * vSurfaceNormal;

	// global coords
	return float3((float)vVertex.x, (float)vVertex.y, (float)vVertex.z);
}

struct Triangle
{
	float3	v[3];
	float3	n;
};


float3 ComputeTripleNormal(in float3 v0, in float3 v1, in float3 v2)
{
	float3 n = normalize(cross(v1 - v0, v2 - v0));
	if (dot(n, v0) < 0)
		n = -n;

	return n;
}


float3 ComputeTriangleNormal(in Triangle t)
{
	return ComputeTripleNormal(t.v[0], t.v[1], t.v[2]);
}

Triangle MakeTriangle(float3 v0, float3 v1, float3 v2)
{
	Triangle t;

	t.v[0] = v0; t.v[1] = v1; t.v[2] = v2;
	t.n = ComputeTriangleNormal(t);

	return t;
}

struct QuadGeometry
{
	Triangle t[2];
	float3	 vertex[4];
	float2	 tex[4];
};

void ComputeQuadGeometry(in int iQuadX, in int iQuadY, in Texture2D tex, out QuadGeometry geom, float longCoeff, float latCoeff, float minLat, float maxLat, float minLong, float maxLong)
{
	uint	ix[4], iy[4];
	float3	v[4];
	float2	texCoords[4];
	
	ix[1] = ix[2] = iQuadX;
	ix[0] = ix[3] = iQuadX + 1;

	iy[0] = iy[1] = iQuadY;
	iy[2] = iy[3] = iQuadY + 1;

	for (int i = 0; i < 4; i++)
	{
		v[i] = GetVertexPos(ix[i], iy[i], tex, longCoeff, latCoeff, minLat, maxLat, minLong, maxLong);
		texCoords[i] = CalcTexcoords(ix[i], iy[i], longCoeff, latCoeff);

		geom.vertex[i] = v[i];
		geom.tex[i] = texCoords[i];
	}

	geom.t[0].v[0] = v[0];
	geom.t[0].v[1] = v[1];
	geom.t[0].v[2] = v[2];

	geom.t[0].n = ComputeTriangleNormal(geom.t[0]);

	geom.t[1].v[0] = v[0];
	geom.t[1].v[1] = v[2];
	geom.t[1].v[2] = v[3];

	geom.t[1].n = ComputeTriangleNormal(geom.t[1]);
}

void ComputeNeighbourQuadGeom(int iQuadX, int iQuadY, inout QuadGeometry neighbourQuadGeom)
{

	if (iQuadX >= 0 && iQuadX < (int)nCountX - 1 &&
		iQuadY >= 0 && iQuadY < (int)nCountY - 1)
	{
		ComputeQuadGeometry(iQuadX, iQuadY, InputHeightTexture, neighbourQuadGeom, fLongitudeCoeff, fLattitudeCoeff, fMinLattitude, fMaxLattitude, fMinLongitude, fMaxLongitude);
	}
	else
	{
		
		if (iQuadX < 0)
			ComputeQuadGeometry((int)nCountX - 2, iQuadY, SouthNeighbourTexture, neighbourQuadGeom, fSouthBlockLongCoeff, fSouthBlockLatCoeff, fSouthMinLat, fSouthMaxLat, fSouthMinLong, fSouthMaxLong);

		if (iQuadY < 0)
			ComputeQuadGeometry(iQuadX, (int)nCountY - 2, WestNeighbourTexture, neighbourQuadGeom, fWestBlockLongCoeff, fWestBlockLatCoeff, fWestMinLat, fWestMaxLat, fWestMinLong, fWestMaxLong);

		if (iQuadX >= (int)nCountX - 1)
			ComputeQuadGeometry(0, iQuadY, NorthNeighbourTexture, neighbourQuadGeom, fNorthBlockLongCoeff, fNorthBlockLatCoeff, fNorthMinLat, fNorthMaxLat, fNorthMinLong, fNorthMaxLong);

		if (iQuadY >= (int)nCountX - 1)
			ComputeQuadGeometry(iQuadX, 0, EastNeighbourTexture, neighbourQuadGeom, fEastBlockLongCoeff, fEastBlockLatCoeff, fEastMinLat, fEastMaxLat, fEastMinLong, fEastMaxLong);
			
	}

}

struct Vertex
{
	float3	pos;
	float3	normal;
	float2	tex;
	float3	tangent;
};

struct OutputQuadData
{
	Vertex	vertices[6];
	uint	indices[6];
};

void ComputeVertexNormalAndTangent(inout Triangle neighbourTriangles[6], inout Vertex v)
{
	float3 middleNormal = float3(0, 0, 0);

	//bool smoothNormal = true;

	for (int i = 0; i < 6; i++)
	{
		//if (dot(v.normal, neighbourTriangles[i].n) < 0)
		//	neighbourTriangles[i].n = -neighbourTriangles[i].n;

//		if (dot(v.normal, neighbourTriangles[i].n) < fNormalDivisionAngleCos)
//			smoothNormal = false;

		middleNormal += neighbourTriangles[i].n;
	}

	middleNormal = normalize(middleNormal);

	//if (smoothNormal)
	//	v.normal = middleNormal;

	float cosAngle = dot(v.normal, middleNormal);

	if (cosAngle > fNormalDivisionAngleCos1)
		v.normal = middleNormal;
	else if (cosAngle >= fNormalDivisionAngleCos2 && cosAngle <= fNormalDivisionAngleCos1)
		v.normal = lerp(v.normal, middleNormal, (cosAngle - fNormalDivisionAngleCos2) / (fNormalDivisionAngleCos1 - fNormalDivisionAngleCos2));

	v.tangent = -cross(float3(1, 0, 0), v.normal);
}

void ComputeQuadOutputData(int iQuadX, int iQuadY, out OutputQuadData data)
{
	QuadGeometry thisQuad;
	ComputeQuadGeometry(iQuadX, iQuadY, InputHeightTexture, thisQuad, fLongitudeCoeff, fLattitudeCoeff, fMinLattitude, fMaxLattitude, fMinLongitude, fMaxLongitude);

	QuadGeometry neighbourQuads[8];
	for (int i = 0; i < 8; i++)
		neighbourQuads[i] = thisQuad;

	ComputeNeighbourQuadGeom(iQuadX + 1, iQuadY,		neighbourQuads[0]);
	ComputeNeighbourQuadGeom(iQuadX + 1, iQuadY + 1,	neighbourQuads[1]);
	ComputeNeighbourQuadGeom(iQuadX,	 iQuadY + 1,	neighbourQuads[2]);
	ComputeNeighbourQuadGeom(iQuadX - 1, iQuadY + 1,	neighbourQuads[3]);

	ComputeNeighbourQuadGeom(iQuadX - 1, iQuadY,		neighbourQuads[4]);
	ComputeNeighbourQuadGeom(iQuadX - 1, iQuadY - 1,	neighbourQuads[5]);
	ComputeNeighbourQuadGeom(iQuadX,     iQuadY - 1,	neighbourQuads[6]);
	ComputeNeighbourQuadGeom(iQuadX + 1, iQuadY - 1,	neighbourQuads[7]);


	Vertex v[6];

	Triangle neighbourTriangles[6];

	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 0  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[0].normal = thisQuad.t[0].n;
	v[0].tex = thisQuad.tex[0];
	v[0].pos = thisQuad.vertex[0];

	neighbourTriangles[0] = neighbourQuads[7].t[0];
	neighbourTriangles[1] = neighbourQuads[7].t[1];
	neighbourTriangles[2] = neighbourQuads[0].t[0];
	neighbourTriangles[3] = thisQuad.t[1];
	neighbourTriangles[4] = thisQuad.t[0];
	neighbourTriangles[5] = neighbourQuads[6].t[1];

	ComputeVertexNormalAndTangent(neighbourTriangles, v[0]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 0  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 1  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[1].normal = thisQuad.t[0].n;
	v[1].tex = thisQuad.tex[1];
	v[1].pos = thisQuad.vertex[1];

	neighbourTriangles[0] = neighbourQuads[6].t[0];
	neighbourTriangles[1] = neighbourQuads[6].t[1];
	neighbourTriangles[2] = thisQuad.t[0];
	neighbourTriangles[3] = neighbourQuads[4].t[1];
	neighbourTriangles[4] = neighbourQuads[4].t[0];
	neighbourTriangles[5] = neighbourQuads[5].t[1];

	ComputeVertexNormalAndTangent(neighbourTriangles, v[1]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 1  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 2  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[2].normal = thisQuad.t[0].n;
	v[2].tex = thisQuad.tex[2];
	v[2].pos = thisQuad.vertex[2];

	neighbourTriangles[0] = thisQuad.t[0];
	neighbourTriangles[1] = thisQuad.t[1];
	neighbourTriangles[2] = neighbourQuads[2].t[0];
	neighbourTriangles[3] = neighbourQuads[3].t[1];
	neighbourTriangles[4] = neighbourQuads[3].t[0];
	neighbourTriangles[5] = neighbourQuads[4].t[1];


	ComputeVertexNormalAndTangent(neighbourTriangles, v[2]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 3  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[3].normal = thisQuad.t[1].n;
	v[3].tex = thisQuad.tex[0];
	v[3].pos = thisQuad.vertex[0];

	neighbourTriangles[0] = neighbourQuads[7].t[0];
	neighbourTriangles[1] = neighbourQuads[7].t[1];
	neighbourTriangles[2] = neighbourQuads[0].t[0];
	neighbourTriangles[3] = thisQuad.t[1];
	neighbourTriangles[4] = thisQuad.t[0];
	neighbourTriangles[5] = neighbourQuads[6].t[1];

	ComputeVertexNormalAndTangent(neighbourTriangles, v[3]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 3 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 4  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[4].normal = thisQuad.t[1].n;
	v[4].tex = thisQuad.tex[2];
	v[4].pos = thisQuad.vertex[2];

	neighbourTriangles[0] = thisQuad.t[0];
	neighbourTriangles[1] = thisQuad.t[1];
	neighbourTriangles[2] = neighbourQuads[2].t[0];
	neighbourTriangles[3] = neighbourQuads[3].t[1];
	neighbourTriangles[4] = neighbourQuads[3].t[0];
	neighbourTriangles[5] = neighbourQuads[4].t[1];


	ComputeVertexNormalAndTangent(neighbourTriangles, v[4]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 4 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 5  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[5].normal = thisQuad.t[1].n;
	v[5].tex = thisQuad.tex[3];
	v[5].pos = thisQuad.vertex[3];

	neighbourTriangles[0] = neighbourQuads[0].t[0];
	neighbourTriangles[1] = neighbourQuads[0].t[1];
	neighbourTriangles[2] = neighbourQuads[1].t[0];
	neighbourTriangles[3] = neighbourQuads[2].t[1];
	neighbourTriangles[4] = neighbourQuads[2].t[0];
	neighbourTriangles[5] = thisQuad.t[1];

	ComputeVertexNormalAndTangent(neighbourTriangles, v[5]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 5 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	for (int iVertex = 0; iVertex < 6; iVertex++)
	{
		data.vertices[iVertex] = v[iVertex];
	}

	data.indices[0] = 0;
	data.indices[1] = 2;
	data.indices[2] = 1;

	data.indices[3] = 3;
	data.indices[4] = 5;
	data.indices[5] = 4;
}

[numthreads(1, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
	int iQuadX = DTid.x;
	int iQuadY = DTid.y;

	OutputQuadData outData;
	ComputeQuadOutputData(iQuadX, iQuadY, outData);

	uint nStartIndex = (iQuadY * (nCountX - 1) + iQuadX) * 6;
	uint VERTEX_SIZE = 44;

	Vertex outVertex;

	for (int i = 0; i < 6; i++)
	{
		uint nVertexStartIndex = nStartIndex + i;

		OutIndexBuffer.Store(nVertexStartIndex * 4, nStartIndex + outData.indices[i]);

		outVertex = outData.vertices[i];

		OutVertexBuffer.Store(nVertexStartIndex * VERTEX_SIZE,		asuint(outVertex.pos.x));
		OutVertexBuffer.Store(nVertexStartIndex * VERTEX_SIZE + 4,	asuint(outVertex.pos.y));
		OutVertexBuffer.Store(nVertexStartIndex * VERTEX_SIZE + 8,	asuint(outVertex.pos.z));

		OutVertexBuffer.Store(nVertexStartIndex * VERTEX_SIZE + 12,	asuint(outVertex.normal.x));
		OutVertexBuffer.Store(nVertexStartIndex * VERTEX_SIZE + 16,	asuint(outVertex.normal.y));
		OutVertexBuffer.Store(nVertexStartIndex * VERTEX_SIZE + 20,	asuint(outVertex.normal.z));

		OutVertexBuffer.Store(nVertexStartIndex * VERTEX_SIZE + 24,	asuint(outVertex.tex.x));
		OutVertexBuffer.Store(nVertexStartIndex * VERTEX_SIZE + 28,	asuint(outVertex.tex.y));

		OutVertexBuffer.Store(nVertexStartIndex * VERTEX_SIZE + 32,	asuint(outVertex.tangent.x));
		OutVertexBuffer.Store(nVertexStartIndex * VERTEX_SIZE + 36,	asuint(outVertex.tangent.y));
		OutVertexBuffer.Store(nVertexStartIndex * VERTEX_SIZE + 40,	asuint(outVertex.tangent.z));
	}
}
