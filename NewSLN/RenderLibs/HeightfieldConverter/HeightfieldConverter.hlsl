//
//	������ ��� ������� ������������ �� ����� �����
//

cbuffer HeightfieldSettings  : register(b0)
{
	float	fMinLattitude;				// ����������� ������
	float	fMaxLattitude;				// ������������ ������

	float	fMinLongitude;				// ����������� �������
	float	fMaxLongitude;				// ������������ �������

	uint	nCountLat;					// ���������� ����� �� X
	uint	nCountLong;					// ���������� ����� �� Y
	
	float	fLongitudeCoeff;			// ������������ ���������� ���������� �� �������
	float	fLattitudeCoeff;			// ������������ ���������� ���������� �� ������

	float	fWorldScale;				// ������� ����
	float	fHeightScale;				// ������� ������

	float	fNormalDivisionAngleCos1;	// ������� ������������ ���� ���������� ��������
	float	fNormalDivisionAngleCos2;	// ����� ����� 2�� ���������� ���������� ������������

	// Cut coeffs
	float	fNorthBlockLongCoeff;
	float	fNorthBlockLatCoeff;

	float	fNorthEastBlockLongCoeff;
	float	fNorthEastBlockLatCoeff;

	float	fEastBlockLongCoeff;
	float	fEastBlockLatCoeff;

	float	fSouthEastBlockLongCoeff;
	float	fSouthEastBlockLatCoeff;

	float	fSouthBlockLongCoeff;
	float	fSouthBlockLatCoeff;

	float	fSouthWestBlockLongCoeff;
	float	fSouthWestBlockLatCoeff;

	float	fWestBlockLongCoeff;
	float	fWestBlockLatCoeff;

	float	fNorthWestBlockLongCoeff;
	float	fNorthWestBlockLatCoeff;

	// 0
	float	fNorthMinLat;
	float	fNorthMaxLat;

	float	fNorthMinLong;
	float	fNorthMaxLong;

	// 1
	float	fNorthEastMinLat;
	float	fNorthEastMaxLat;

	float	fNorthEastMinLong;
	float	fNorthEastMaxLong;

	//2

	float	fEastMinLat;
	float	fEastMaxLat;

	float	fEastMinLong;
	float	fEastMaxLong;

	//3
	float	fSouthEastMinLat;
	float	fSouthEastMaxLat;

	float	fSouthEastMinLong;
	float	fSouthEastMaxLong;

	//4
	float	fSouthMinLat;
	float	fSouthMaxLat;

	float	fSouthMinLong;
	float	fSouthMaxLong;

	//5
	float	fSouthWestMinLat;
	float	fSouthWestMaxLat;

	float	fSouthWestMinLong;
	float	fSouthWestMaxLong;

	//6
	float	fWestMinLat;
	float	fWestMaxLat;

	float	fWestMinLong;
	float	fWestMaxLong;

	//7
	float	fNorthWestMinLat;
	float	fNorthWestMaxLat;

	float	fNorthWestMinLong;
	float	fNorthWestMaxLong;
};


SamplerState HeightTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Mirror;
	AddressV = Mirror;
};

Texture2D				InputHeightTexture				: register(t0);

Texture2D				NorthNeighbourTexture			: register(t1);		// �����
Texture2D				NorthEastNeighbourTexture		: register(t2);
Texture2D				EastNeighbourTexture			: register(t3);
Texture2D				SouthEastNeighbourTexture		: register(t4);
Texture2D				SouthNeighbourTexture			: register(t5);
Texture2D				SouthWestNeighbourTexture		: register(t6);
Texture2D				WestNeighbourTexture			: register(t7);
Texture2D				NorthWestNeighbourTexture		: register(t8);


RWByteAddressBuffer 	OutVertexBuffer 		: register(u0);
RWByteAddressBuffer 	OutIndexBuffer 			: register(u1);

// �������� ����� �� ����������� ���������� WGS-84
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

// ������� � ���������� � ����� �� �����������
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

float2 CalcTexcoords(uint iLong, uint iLat, float longCoeff, float latCoeff)
{
	float u = (float) iLong / (nCountLong - 1) ;
	float v = (float) iLat / (nCountLat - 1) ;

	return float2(
		longCoeff *u,
		latCoeff * (1 - v)
	);
}

float2 CalcTexcoordsHF(uint iLong, uint iLat, float longCoeff, float latCoeff, float Width, float Height)
{
	float longA = longCoeff * (float)iLong / (nCountLong - 1);
	float latA = latCoeff * (float)iLat / (nCountLat - 1);

	float u = lerp(0.5 / Width, 1 - 0.5 / Width, longA);
	float v = lerp(0.5 / Height, 1 - 0.5 / Height, latA);

	return float2(
		u,
		(1 - v)
	);
}

// �������� ������ ������� �� �������� ����� ���� x � y
float GetVertexHeight(uint iLong, uint iLat, in Texture2D tex, float longCoeff, float latCoeff)
{
	float2 texCoord;

	float Width, Height, NoL;
	tex.GetDimensions(0, Width, Height, NoL);

	texCoord = CalcTexcoordsHF(iLong, iLat, longCoeff, latCoeff, Width, Height);

	float4 TexColor = tex.SampleLevel(HeightTextureSampler, texCoord, 0);

	return TexColor.r;
}

// �������� ������� �������
float3 GetVertexPos(uint iLong, uint iLat, in Texture2D tex, float longCoeff, float latCoeff, float minLat, float maxLat, float minLong, float maxLong)
{
	float height = GetVertexHeight(iLong, iLat, tex, longCoeff, latCoeff);

	float fLongitudeAmpl = maxLong - minLong;
	float fLattitudeAmpl = maxLat - minLat;

	float dlong = fLongitudeAmpl / (nCountLong - 1);
	float dlat = fLattitudeAmpl / (nCountLat - 1);

	float longitude = minLong + iLong * dlong;
	float lattitude = minLat + iLat * dlat;

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

void ComputeQuadGeometry(in int iQuadLat, in int iQuadLong, in Texture2D tex, out QuadGeometry geom, float longCoeff, float latCoeff, float minLat, float maxLat, float minLong, float maxLong)
{
	uint	iLat[4], iLong[4];
	float3	v[4];
	float2	texCoords[4];
	
	iLat[1] = iLat[2] = iQuadLat;
	iLat[0] = iLat[3] = iQuadLat + 1;

	iLong[0] = iLong[1] = iQuadLong;
	iLong[2] = iLong[3] = iQuadLong + 1;

	for (int i = 0; i < 4; i++)
	{
		v[i] = GetVertexPos(iLong[i], iLat[i], tex, longCoeff, latCoeff, minLat, maxLat, minLong, maxLong);
		texCoords[i] = CalcTexcoords(iLong[i], iLat[i], longCoeff, latCoeff);
	}

	geom.vertex[0] = v[0];
	geom.tex[0] = texCoords[0];

	geom.vertex[1] = v[1];
	geom.tex[1] = texCoords[1];

	geom.vertex[2] = v[2];
	geom.tex[2] = texCoords[2];

	geom.vertex[3] = v[3];
	geom.tex[3] = texCoords[3];


	geom.t[0].v[0] = v[0];
	geom.t[0].v[1] = v[1];
	geom.t[0].v[2] = v[2];

	geom.t[0].n = ComputeTriangleNormal(geom.t[0]);

	geom.t[1].v[0] = v[0];
	geom.t[1].v[1] = v[2];
	geom.t[1].v[2] = v[3];

	geom.t[1].n = ComputeTriangleNormal(geom.t[1]);
}

void ComputeNeighbourQuadGeom(int iQuadLat, int iQuadLong, inout QuadGeometry neighbourQuadGeom)
{

	if (iQuadLat >= 0 && iQuadLat < (int)nCountLat - 1 &&
		iQuadLong >= 0 && iQuadLong < (int)nCountLong - 1)
	{
		ComputeQuadGeometry(iQuadLat, iQuadLong, InputHeightTexture, neighbourQuadGeom, fLongitudeCoeff, fLattitudeCoeff, fMinLattitude, fMaxLattitude, fMinLongitude, fMaxLongitude);
	}
	else
	{
	
		if (iQuadLat >= 0 && iQuadLat < (int)nCountLong - 1)
		{
			// left
			if (iQuadLong < 0)
				ComputeQuadGeometry(iQuadLat, (int)nCountLong - 2, WestNeighbourTexture, neighbourQuadGeom, fWestBlockLongCoeff, fWestBlockLatCoeff, fWestMinLat, fWestMaxLat, fWestMinLong, fWestMaxLong);

			// right
			if (iQuadLong >= (int)nCountLong - 1)
				ComputeQuadGeometry(iQuadLat, 0, EastNeighbourTexture, neighbourQuadGeom, fEastBlockLongCoeff, fEastBlockLatCoeff, fEastMinLat, fEastMaxLat, fEastMinLong, fEastMaxLong);
		}
		
		else if (iQuadLong >= 0 && iQuadLong < (int)nCountLong - 1)
		{
			// top
			if (iQuadLat >= (int)nCountLat - 1)
				ComputeQuadGeometry(0, iQuadLong, NorthNeighbourTexture, neighbourQuadGeom, fNorthBlockLongCoeff, fNorthBlockLatCoeff, fNorthMinLat, fNorthMaxLat, fNorthMinLong, fNorthMaxLong);

			// bottom
			if (iQuadLat < 0)
				ComputeQuadGeometry((int)nCountLat - 2, iQuadLong, SouthNeighbourTexture, neighbourQuadGeom, fSouthBlockLongCoeff, fSouthBlockLatCoeff, fSouthMinLat, fSouthMaxLat, fSouthMinLong, fSouthMaxLong);
		}

		else if ((iQuadLat < 0) && (iQuadLong < 0))
		{
			// left bottom
			ComputeQuadGeometry((int)nCountLat - 2, (int)nCountLong - 2, SouthWestNeighbourTexture, neighbourQuadGeom, fSouthWestBlockLongCoeff, fSouthWestBlockLatCoeff, fSouthWestMinLat, fSouthWestMaxLat, fSouthWestMinLong, fSouthWestMaxLong);
		}
		
		else if ((iQuadLat < 0) && (iQuadLong >= (int)nCountLong - 1))
		{
			// right bottom
			ComputeQuadGeometry((int)nCountLat - 2, 0, SouthEastNeighbourTexture, neighbourQuadGeom, fSouthEastBlockLongCoeff, fSouthEastBlockLatCoeff, fSouthEastMinLat, fSouthEastMaxLat, fSouthEastMinLong, fSouthEastMaxLong);
		}
		
		else if ((iQuadLat >= (int)nCountLat - 1) && (iQuadLong < 0))
		{
			// left top
			ComputeQuadGeometry(0, (int)nCountLong - 2, NorthWestNeighbourTexture, neighbourQuadGeom, fNorthWestBlockLongCoeff, fNorthWestBlockLatCoeff, fNorthWestMinLat, fNorthWestMaxLat, fNorthWestMinLong, fNorthWestMaxLong);
		}

		else if ((iQuadLat >= (int)nCountLat - 1) && (iQuadLong >= (int)nCountLong - 1))
		{
			// right top
			ComputeQuadGeometry(0, 0, NorthEastNeighbourTexture, neighbourQuadGeom, fNorthEastBlockLongCoeff, fNorthEastBlockLatCoeff, fNorthEastMinLat, fNorthEastMaxLat, fNorthEastMinLong, fNorthEastMaxLong);
		}

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

	for (int i = 0; i < 6; i++)
	{
		middleNormal += neighbourTriangles[i].n;
	}

	middleNormal = normalize(middleNormal);

	float cosAngle = dot(v.normal, middleNormal);

	if (cosAngle > fNormalDivisionAngleCos1)
	{
		v.normal = middleNormal;
	}
	else if (cosAngle >= fNormalDivisionAngleCos2 && cosAngle <= fNormalDivisionAngleCos1)
	{
		v.normal = lerp(v.normal, middleNormal, (cosAngle - fNormalDivisionAngleCos2) / (fNormalDivisionAngleCos1 - fNormalDivisionAngleCos2));
	}

	v.tangent = -cross(float3(1, 0, 0), v.normal);
}

void ComputeQuadOutputData(int iQuadLat, int iQuadLong, out OutputQuadData data)
{
	QuadGeometry thisQuad;
	ComputeQuadGeometry(iQuadLat, iQuadLong, InputHeightTexture, thisQuad, fLongitudeCoeff, fLattitudeCoeff, fMinLattitude, fMaxLattitude, fMinLongitude, fMaxLongitude);

	QuadGeometry neighbourQuads[8];
	for (int i = 0; i < 8; i++)
		neighbourQuads[i] = thisQuad;

	
	ComputeNeighbourQuadGeom(iQuadLat + 1, iQuadLong,		neighbourQuads[0]);
	ComputeNeighbourQuadGeom(iQuadLat + 1, iQuadLong + 1,	neighbourQuads[1]);
	ComputeNeighbourQuadGeom(iQuadLat,	 iQuadLong + 1,	neighbourQuads[2]);
	ComputeNeighbourQuadGeom(iQuadLat - 1, iQuadLong + 1,	neighbourQuads[3]);

	ComputeNeighbourQuadGeom(iQuadLat - 1, iQuadLong,		neighbourQuads[4]);
	ComputeNeighbourQuadGeom(iQuadLat - 1, iQuadLong - 1,	neighbourQuads[5]);
	ComputeNeighbourQuadGeom(iQuadLat,     iQuadLong - 1,	neighbourQuads[6]);
	ComputeNeighbourQuadGeom(iQuadLat + 1, iQuadLong - 1,	neighbourQuads[7]);

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

	data.vertices[0] = v[0];
	data.vertices[1] = v[1];
	data.vertices[2] = v[2];
	data.vertices[3] = v[3];
	data.vertices[4] = v[4];
	data.vertices[5] = v[5];


	data.indices[0] = 0;
	data.indices[1] = 2;
	data.indices[2] = 1;

	data.indices[3] = 3;
	data.indices[4] = 5;
	data.indices[5] = 4;
}

[numthreads(1, 1, 1)]
void HeightfieldConverterCS(uint3 DTid : SV_DispatchThreadID)
{
	int iQuadLong = DTid.x;
	int iQuadLat = DTid.y;
	
	OutputQuadData outData;
	ComputeQuadOutputData(iQuadLat, iQuadLong, outData);

	uint nStartIndex = (iQuadLong * (nCountLat - 1) + iQuadLat) * 6;
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


technique11 HeightfieldConverter
{
	pass P0
	{
		SetComputeShader(CompileShader(cs_5_0, HeightfieldConverterCS()));
	}
}
