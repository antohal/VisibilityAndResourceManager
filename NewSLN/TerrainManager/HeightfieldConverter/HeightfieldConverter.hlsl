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

	float	fNormalDivisionAngleCos;	//  осинус минимального угла разделени€ нормалей

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

	float	fTemp3;
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
	return normalize(cross(v2 - v0, v1 - v0));
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

void CopyQuad(out QuadGeometry a, in QuadGeometry b)
{
	int i = 0;

	for (i = 0; i < 2; i++)
	{
		for (int j = 0; j < 3; j++)
			a.t[i].v[j] = b.t[i].v[j];

		a.t[i].n = b.t[i].n;
	}

	for (i = 0; i < 4; i++)
	{
		a.vertex[i] = b.vertex[i];
		a.tex[i] = b.tex[i];
	}
}

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

void ComputeNeighbourQuadGeom(int iQuadX, int iQuadY, out QuadGeometry neighbourQuadGeom)
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

void ComputeVertexNormalAndTangent(in float3 neigbours[4], inout Vertex v)
{
	float3 middleNormal = float3(0, 0, 0);

	bool smoothNormal = true;

	Triangle neighbourTriangles[4];

	neighbourTriangles[0] = MakeTriangle(v.pos, neigbours[1], neigbours[0]);
	neighbourTriangles[1] = MakeTriangle(v.pos, neigbours[2], neigbours[1]);
	neighbourTriangles[2] = MakeTriangle(v.pos, neigbours[3], neigbours[2]);
	neighbourTriangles[3] = MakeTriangle(v.pos, neigbours[0], neigbours[3]);

	for (int i = 0; i < 4; i++)
	{
		if (dot(v.normal, neighbourTriangles[i].n) < 0)
			neighbourTriangles[i].n = -neighbourTriangles[i].n;

		if (dot(v.normal, neighbourTriangles[i].n) < fNormalDivisionAngleCos)
			smoothNormal = false;

		middleNormal += neighbourTriangles[i].n;
	}

	middleNormal *= 0.25f;

	middleNormal = normalize(middleNormal);

	if (smoothNormal)
		v.normal = middleNormal;

	v.tangent = -cross(float3(1, 0, 0), v.normal);
}

void ComputeQuadOutputData(int iQuadX, int iQuadY, out OutputQuadData data)
{
	QuadGeometry thisQuad;
	ComputeQuadGeometry(iQuadX, iQuadY, InputHeightTexture, thisQuad, fLongitudeCoeff, fLattitudeCoeff, fMinLattitude, fMaxLattitude, fMinLongitude, fMaxLongitude);

	QuadGeometry neighbourQuads[4];
	for (int i = 0; i < 4; i++)
		CopyQuad(neighbourQuads[i], thisQuad);

//	if (iQuadX > 0 && iQuadX < (int)nCountX - 2 &&
//		iQuadY > 0 && iQuadY < (int)nCountY - 2 )
//	{
		ComputeNeighbourQuadGeom(iQuadX + 1, iQuadY, neighbourQuads[0]);
		ComputeNeighbourQuadGeom(iQuadX, iQuadY + 1, neighbourQuads[1]);
		ComputeNeighbourQuadGeom(iQuadX - 1, iQuadY, neighbourQuads[2]);
		ComputeNeighbourQuadGeom(iQuadX, iQuadY - 1, neighbourQuads[3]);
//	}


	Vertex v[6];

	/*if ((iQuadX == 0) || (iQuadY == 0) || (iQuadX == ((int)nCountX - 2)) || (iQuadY == ((int)nCountY - 2)))
	{
		v[0].normal = thisQuad.t[0].n;
		v[0].tex = thisQuad.tex[0];
		v[0].pos = thisQuad.vertex[0];
		v[0].tangent = normalize(-cross(float3(1, 0, 0), v[0].normal));

		v[1].normal = thisQuad.t[0].n;
		v[1].tex = thisQuad.tex[1];
		v[1].pos = thisQuad.vertex[1];
		v[1].tangent = normalize(-cross(float3(1, 0, 0), v[1].normal));

		v[2].normal = thisQuad.t[0].n;
		v[2].tex = thisQuad.tex[2];
		v[2].pos = thisQuad.vertex[2];
		v[2].tangent = normalize(-cross(float3(1, 0, 0), v[2].normal));

		v[3].normal = thisQuad.t[1].n;
		v[3].tex = thisQuad.tex[0];
		v[3].pos = thisQuad.vertex[0];
		v[3].tangent = normalize(-cross(float3(1, 0, 0), v[3].normal));

		v[4].normal = thisQuad.t[1].n;
		v[4].tex = thisQuad.tex[2];
		v[4].pos = thisQuad.vertex[2];
		v[4].tangent = normalize(-cross(float3(1, 0, 0), v[4].normal));

		v[5].normal = thisQuad.t[1].n;
		v[5].tex = thisQuad.tex[3];
		v[5].pos = thisQuad.vertex[3];
		v[5].tangent = normalize(-cross(float3(1, 0, 0), v[5].normal));
	}
	else
	{*/
		float3 neighbourPoints[4];

		//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 0  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		v[0].normal = thisQuad.t[0].n;
		v[0].tex = thisQuad.tex[0];
		v[0].pos = thisQuad.vertex[0];

		neighbourPoints[0] = neighbourQuads[0].vertex[0];
		neighbourPoints[1] = thisQuad.vertex[3];
		neighbourPoints[2] = thisQuad.vertex[1];
		neighbourPoints[3] = neighbourQuads[3].vertex[0];

		ComputeVertexNormalAndTangent(neighbourPoints, v[0]);

		//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 0  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



		//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 1  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		v[1].normal = thisQuad.t[0].n;
		v[1].tex = thisQuad.tex[1];
		v[1].pos = thisQuad.vertex[1];

		neighbourPoints[0] = thisQuad.vertex[0];
		neighbourPoints[1] = thisQuad.vertex[2];
		neighbourPoints[2] = neighbourQuads[2].vertex[1];
		neighbourPoints[3] = neighbourQuads[3].vertex[1];

		ComputeVertexNormalAndTangent(neighbourPoints, v[1]);

		//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 1  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


		//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 2  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		v[2].normal = thisQuad.t[0].n;
		v[2].tex = thisQuad.tex[2];
		v[2].pos = thisQuad.vertex[2];

		neighbourPoints[0] = thisQuad.vertex[3];
		neighbourPoints[1] = neighbourQuads[1].vertex[2];
		neighbourPoints[2] = neighbourQuads[2].vertex[2];
		neighbourPoints[3] = thisQuad.vertex[1];


		ComputeVertexNormalAndTangent(neighbourPoints, v[2]);

		//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 3  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		v[3].normal = thisQuad.t[1].n;
		v[3].tex = thisQuad.tex[0];
		v[3].pos = thisQuad.vertex[0];

		neighbourPoints[0] = neighbourQuads[0].vertex[0];
		neighbourPoints[1] = thisQuad.vertex[3];
		neighbourPoints[2] = thisQuad.vertex[1];
		neighbourPoints[3] = neighbourQuads[3].vertex[0];

		ComputeVertexNormalAndTangent(neighbourPoints, v[3]);

		//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 3 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 4  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		v[4].normal = thisQuad.t[1].n;
		v[4].tex = thisQuad.tex[2];
		v[4].pos = thisQuad.vertex[2];

		neighbourPoints[0] = thisQuad.vertex[3];
		neighbourPoints[1] = neighbourQuads[1].vertex[2];
		neighbourPoints[2] = neighbourQuads[2].vertex[2];
		neighbourPoints[3] = thisQuad.vertex[1];


		ComputeVertexNormalAndTangent(neighbourPoints, v[4]);

		//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 4 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 5  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		v[5].normal = thisQuad.t[1].n;
		v[5].tex = thisQuad.tex[3];
		v[5].pos = thisQuad.vertex[3];

		neighbourPoints[0] = neighbourQuads[0].vertex[3];
		neighbourPoints[1] = neighbourQuads[1].vertex[3];
		neighbourPoints[2] = thisQuad.vertex[2];
		neighbourPoints[3] = thisQuad.vertex[0];

		ComputeVertexNormalAndTangent(neighbourPoints, v[5]);

		//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 5 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//}
	


	for (int iVertex = 0; iVertex < 6; iVertex++)
	{
		data.vertices[iVertex] = v[iVertex];
//		data.indices[iVertex] = iVertex;
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
