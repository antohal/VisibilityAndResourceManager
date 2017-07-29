//
//	������ ��� ������� ������������ �� ����� �����
//

cbuffer HeightfieldSettings  : register(b0)
{
	float	fMinLattitude;				// ����������� ������
	float	fMaxLattitude;				// ������������ ������

	float	fMinLongitude;				// ����������� �������
	float	fMaxLongitude;				// ������������ �������

	uint	nCountX;					// ���������� ����� �� X
	uint	nCountY;					// ���������� ����� �� Y


	float	fLongitudeCoeff;			// ������������ ���������� ���������� �� �������
	float	fLattitudeCoeff;			// ������������ ���������� ���������� �� ������

	float	fWorldScale;				// ������� ����
	float	fHeightScale;				// ������� ������

	float	fNormalDivisionAngleCos;	// ������� ������������ ���� ���������� ��������
	float	fTemp3;
};


SamplerState HeightTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
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

float2 CalcTexcoords(uint ix, uint iy)
{
	return float2(
		fLongitudeCoeff *(float)iy / (nCountY - 1),
		fLattitudeCoeff * (1 - (float)ix / (nCountX - 1))
		);
}

// �������� ������ ������� �� �������� ����� ���� x � y
float GetVertexHeight(uint ix, uint iy, in Texture2D tex)
{
	float2 texCoord;

	float fx = ix;
	float fy = iy;

	texCoord = CalcTexcoords(ix, iy);

	float4 TexColor = tex.SampleLevel(HeightTextureSampler, texCoord, 0);

	return TexColor.r;
}

// �������� ������� �������
float3 GetVertexPos(uint ix, uint iy, in Texture2D tex)
{
	float height = GetVertexHeight(ix, iy, tex);

	float fLongitudeAmpl = fMaxLongitude - fMinLongitude;
	float fLattitudeAmpl = fMaxLattitude - fMinLattitude;

	float dlong = fLongitudeAmpl / (nCountY - 1);
	float dlat = fLattitudeAmpl / (nCountX - 1);

	float longitude = fMinLongitude + iy * dlong;
	float lattitude = fMinLattitude + ix * dlat;

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
	return normalize(cross(v1 - v0, v2 - v0));
}


float3 ComputeTriangleNormal(in Triangle t)
{
	return ComputeTripleNormal(t.v[0], t.v[1], t.v[2]);
}

struct QuadGeometry
{
	Triangle t[2];
	float3	 vertex[4];
	float2	 tex[4];
};

void CopyQuad(out QuadGeometry a, in QuadGeometry b)
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 3; j++)
			a.t[i].v[j] = b.t[i].v[j];

		a.t[i].n = b.t[i].n;
	}

	for (int i = 0; i < 4; i++)
	{
		a.vertex[i] = b.vertex[i];
		a.tex[i] = b.tex[i];
	}
}

void ComputeQuadGeometry(in int iQuadX, in int iQuadY, in Texture2D tex, out QuadGeometry geom)
{
	uint	ix[4], iy[4];
	float3	v[4];
	float2	texCoords[4];
	
	ix[1] = ix[2] = iQuadX + 1;
	ix[0] = ix[3] = iQuadX;

	iy[0] = iy[1] = iQuadY;
	iy[2] = iy[3] = iQuadY + 1;

	for (int i = 0; i < 4; i++)
	{
		v[i] = GetVertexPos(ix[i], iy[i], tex);
		texCoords[i] = CalcTexcoords(ix[i], iy[i]);

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

	if (iQuadX >= 0 && iQuadX <= nCountX - 1 &&
		iQuadY >= 0 && iQuadY <= nCountY - 1)
	{
		ComputeQuadGeometry(iQuadX, iQuadY, InputHeightTexture, neighbourQuadGeom);
	}
	else
	{



	}

	//else
	//{

	//	if (iQuadX < 0)
	//	{
	//		if (iQuadY >= 0 && iQuadY <= nCountY - 1)
	//		{
	//			//ComputeQuadGeometry(nCountX - 1, iQuadY, SouthNeighbourTexture, neighbourQuadGeom);
	//			ComputeQuadGeometry(nCountX - 1, iQuadY, NorthNeighbourTexture, neighbourQuadGeom);
	//		}
	//		else if (iQuadY < 0)
	//		{
	//			ComputeQuadGeometry(nCountX - 1, nCountY - 1, SouthWestNeighbourTexture, neighbourQuadGeom);
	//		}
	//		else if (iQuadY > nCountY - 1)
	//		{
	//			ComputeQuadGeometry(nCountX - 1, 0, SouthEastNeighbourTexture, neighbourQuadGeom);
	//		}
	//	}
	//	else if (iQuadX > nCountX - 1)
	//	{
	//		if (iQuadY >= 0 && iQuadY <= nCountY - 1)
	//		{
	//			ComputeQuadGeometry(0, iQuadY, NorthNeighbourTexture, neighbourQuadGeom);
	//		}
	//		else if (iQuadY < 0)
	//		{
	//			ComputeQuadGeometry(0, nCountY - 1, NorthWestNeighbourTexture, neighbourQuadGeom);
	//		}
	//		else if (iQuadY > nCountY - 1)
	//		{
	//			ComputeQuadGeometry(0, 0, NorthEastNeighbourTexture, neighbourQuadGeom);
	//		}
	//	}
	//	else if (iQuadY < 0)
	//	{
	//		ComputeQuadGeometry(iQuadX, nCountY - 1, WestNeighbourTexture, neighbourQuadGeom);
	//	}
	//	else if (iQuadY > nCountY - 1)
	//	{
	//		ComputeQuadGeometry(iQuadX, 0, EastNeighbourTexture, neighbourQuadGeom);
	//	}
	//}
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

void ComputeVertexNormalAndTangent(in Triangle neigbours[6], inout Vertex v)
{
	float3 middleNormal = float3(0, 0, 0);

	bool smoothNormal = true;

	for (int i = 0; i < 6; i++)
	{
		if (dot(v.normal, neigbours[i].n) < fNormalDivisionAngleCos)
			smoothNormal = false;

		middleNormal += neigbours[i].n;
	}

	middleNormal = normalize(middleNormal);

	//if (smoothNormal)
		v.normal = middleNormal;

	v.tangent = -cross(float3(1, 0, 0), v.normal);
}

void ComputeQuadOutputData(int iQuadX, int iQuadY, out OutputQuadData data)
{
	QuadGeometry thisQuad;
	ComputeQuadGeometry(iQuadX, iQuadY, InputHeightTexture, thisQuad);

	QuadGeometry neighbourQuads[8];
	for (int i = 0; i < 8; i++)
		CopyQuad(neighbourQuads[i], thisQuad);

	ComputeNeighbourQuadGeom(iQuadX - 1, iQuadY    , neighbourQuads[0]);
	ComputeNeighbourQuadGeom(iQuadX - 1, iQuadY + 1, neighbourQuads[1]);
	ComputeNeighbourQuadGeom(iQuadX    , iQuadY + 1, neighbourQuads[2]);
	ComputeNeighbourQuadGeom(iQuadX + 1, iQuadY + 1, neighbourQuads[3]);
	ComputeNeighbourQuadGeom(iQuadX + 1, iQuadY    , neighbourQuads[4]);
	ComputeNeighbourQuadGeom(iQuadX + 1, iQuadY - 1, neighbourQuads[5]);
	ComputeNeighbourQuadGeom(iQuadX    , iQuadY - 1, neighbourQuads[6]);
	ComputeNeighbourQuadGeom(iQuadX - 1, iQuadY - 1, neighbourQuads[7]);

	Triangle neigbours[6];
	Vertex v[6];

	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 0  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[0].normal = thisQuad.t[0].n;
	v[0].tex = thisQuad.tex[0];
	v[0].pos = thisQuad.vertex[0];

	neigbours[0] = neighbourQuads[7].t[0];
	neigbours[1] = neighbourQuads[7].t[1];
	neigbours[2] = neighbourQuads[0].t[0];
	neigbours[3] = thisQuad.t[1];
	neigbours[4] = thisQuad.t[0];
	neigbours[5] = neighbourQuads[6].t[1];

	ComputeVertexNormalAndTangent(neigbours, v[0]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 0  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 1  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[1].normal = thisQuad.t[0].n;
	v[1].tex = thisQuad.tex[1];
	v[1].pos = thisQuad.vertex[1];

	neigbours[0] = neighbourQuads[6].t[0];
	neigbours[1] = neighbourQuads[6].t[1];
	neigbours[2] = thisQuad.t[0];
	neigbours[3] = neighbourQuads[4].t[1];
	neigbours[4] = neighbourQuads[4].t[0];
	neigbours[5] = neighbourQuads[5].t[1];

	ComputeVertexNormalAndTangent(neigbours, v[1]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 1  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 2  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[2].normal = thisQuad.t[0].n;
	v[2].tex = thisQuad.tex[2];
	v[2].pos = thisQuad.vertex[2];

	neigbours[0] = thisQuad.t[0];
	neigbours[1] = thisQuad.t[1];
	neigbours[2] = neighbourQuads[2].t[0];
	neigbours[3] = neighbourQuads[3].t[1];
	neigbours[4] = neighbourQuads[3].t[0];
	neigbours[5] = neighbourQuads[4].t[1];

	ComputeVertexNormalAndTangent(neigbours, v[2]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 3  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[3].normal = thisQuad.t[1].n;
	v[3].tex = thisQuad.tex[0];
	v[3].pos = thisQuad.vertex[0];

	neigbours[0] = neighbourQuads[7].t[0];
	neigbours[1] = neighbourQuads[7].t[1];
	neigbours[2] = neighbourQuads[0].t[0];
	neigbours[3] = thisQuad.t[1];
	neigbours[4] = thisQuad.t[0];
	neigbours[5] = neighbourQuads[6].t[1];

	ComputeVertexNormalAndTangent(neigbours, v[3]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 3 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 4  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[4].normal = thisQuad.t[1].n;
	v[4].tex = thisQuad.tex[2];
	v[4].pos = thisQuad.vertex[2];

	neigbours[0] = thisQuad.t[0];
	neigbours[1] = thisQuad.t[1];
	neigbours[2] = neighbourQuads[2].t[0];
	neigbours[3] = neighbourQuads[3].t[1];
	neigbours[4] = neighbourQuads[3].t[0];
	neigbours[5] = neighbourQuads[4].t[1];

	ComputeVertexNormalAndTangent(neigbours, v[4]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 4 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 5  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[5].normal = thisQuad.t[1].n;
	v[5].tex = thisQuad.tex[3];
	v[5].pos = thisQuad.vertex[3];

	neigbours[0] = neighbourQuads[0].t[0];
	neigbours[1] = neighbourQuads[0].t[1];
	neigbours[2] = neighbourQuads[1].t[0];
	neigbours[3] = neighbourQuads[2].t[1];
	neigbours[4] = neighbourQuads[2].t[0];
	neigbours[5] = thisQuad.t[1];

	ComputeVertexNormalAndTangent(neigbours, v[5]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 5 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	for (int iVertex = 0; iVertex < 6; iVertex++)
	{
		data.vertices[iVertex] = v[iVertex];
//		data.indices[iVertex] = iVertex;
	}

	data.indices[0] = 0;
	data.indices[1] = 1;
	data.indices[2] = 2;

	data.indices[3] = 3;
	data.indices[4] = 4;
	data.indices[5] = 5;
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
