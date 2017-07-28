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
	float	fTemp3;
};


SamplerState HeightTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Clamp;
	AddressV = Clamp;
};

Texture2D				InputHeightTexture		: register(t0);

//Texture2D				RNeighbourTexture		: register(t1);
//Texture2D				RBNeighbourTexture		: register(t2);
//Texture2D				BNeighbourTexture		: register(t3);
//Texture2D				LBNeighbourTexture		: register(t4);
//Texture2D				LNeighbourTexture		: register(t5);
//Texture2D				LTNeighbourTexture		: register(t6);
//Texture2D				TNeighbourTexture		: register(t7);
//Texture2D				RTNeighbourTexture		: register(t8);


Texture2D				LNeighbourTexture		: register(t1);
Texture2D				LTNeighbourTexture		: register(t2);
Texture2D				TNeighbourTexture		: register(t3);
Texture2D				RTNeighbourTexture		: register(t4);
Texture2D				RNeighbourTexture		: register(t5);
Texture2D				RBNeighbourTexture		: register(t6);
Texture2D				BNeighbourTexture		: register(t7);
Texture2D				LBNeighbourTexture		: register(t8);


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

float2 CalcTexcoords(uint ix, uint iy)
{
	return float2(
		fLongitudeCoeff *(float)iy / (nCountY - 1),
		fLattitudeCoeff * (1 - (float)ix / (nCountX - 1))
		);
}

// получить высоту вершины по индексам вдоль осей x и y
float GetVertexHeight(uint ix, uint iy, in Texture2D tex)
{
	float2 texCoord;

	float fx = ix;
	float fy = iy;

	texCoord = CalcTexcoords(ix, iy);

	float4 TexColor = tex.SampleLevel(HeightTextureSampler, texCoord, 0);

	return TexColor.r;
}

// получить позицию вершины
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
	return -normalize(cross(v2 - v0, v1 - v0));
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

void ComputeQuadGeometry(in uint iQuadX, in uint iQuadY, in Texture2D tex, out QuadGeometry geom)
{
	uint	ix[4], iy[4];
	float3	v[4];
	float2	texCoords[4];

	ix[1] = ix[0] = iQuadX;
	ix[2] = ix[3] = iQuadX + 1;

	iy[1] = iy[2] = iQuadY;
	iy[0] = iy[3] = iQuadY + 1;

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


void ComputeLeftNeighbourQuadGeometry(uint iQuadX, uint iQuadY, out QuadGeometry leftQuadGeom)
{
	if (iQuadX > 0)
	{
		ComputeQuadGeometry(iQuadX - 1, iQuadY, InputHeightTexture, leftQuadGeom);

	}
	else
	{
		ComputeQuadGeometry(nCountX - 1, iQuadY, LNeighbourTexture, leftQuadGeom);
	}
}

void ComputeLowerLeftNeighbourQuadGeometry(uint iQuadX, uint iQuadY, out QuadGeometry lowerLeftQuadGeom)
{
	if (iQuadY < nCountY - 1 && iQuadX > 0)
	{
		ComputeQuadGeometry(iQuadX - 1, iQuadY + 1, InputHeightTexture, lowerLeftQuadGeom);

	}
	else
	{
		if (iQuadX > 0)
		{
			ComputeQuadGeometry(iQuadX - 1, 0, BNeighbourTexture, lowerLeftQuadGeom);
		}
		else if (iQuadY < nCountY - 1)
		{
			ComputeQuadGeometry(nCountX - 1, iQuadY, LNeighbourTexture, lowerLeftQuadGeom);
		}
		/*else
		{
			ComputeQuadGeometry(nCountX - 1, 0, LBNeighbourTexture, lowerLeftQuadGeom);
		}*/
	}
}

void ComputeRightNeighbourQuadGeometry(uint iQuadX, uint iQuadY, out QuadGeometry rightQuadGeom)
{
	if (iQuadX < nCountX - 1)
	{
		ComputeQuadGeometry(iQuadX + 1, iQuadY, InputHeightTexture, rightQuadGeom);

	}
	else
	{
		
		ComputeQuadGeometry(0, iQuadY, RNeighbourTexture, rightQuadGeom);
	}
}

void ComputeUpperRightNeighbourQuadGeometry(uint iQuadX, uint iQuadY, out QuadGeometry upperRightQuadGeom)
{
	if (iQuadX < nCountX - 1 > 0 && iQuadY > 0)
	{
		ComputeQuadGeometry(iQuadX + 1, iQuadY - 1, InputHeightTexture, upperRightQuadGeom);

	}
	else
	{
		if (iQuadY > 0)
		{
			ComputeQuadGeometry(0, iQuadY - 1, RNeighbourTexture, upperRightQuadGeom);
		}
		else if (iQuadX < nCountX - 1 > 0)
		{
			ComputeQuadGeometry(iQuadX + 1, nCountY - 1, TNeighbourTexture, upperRightQuadGeom);
		}
	}
}

void ComputeUpperNeighbourQuadGeometry(uint iQuadX, uint iQuadY, out QuadGeometry upperQuadGeom)
{
	if (iQuadY > 0)
	{
		ComputeQuadGeometry(iQuadX, iQuadY - 1, InputHeightTexture, upperQuadGeom);

	}
	else
	{
		
		ComputeQuadGeometry(iQuadX, nCountY - 1, TNeighbourTexture, upperQuadGeom);
	}
}

void ComputeUpperLeftNeighbourQuadGeometry(uint iQuadX, uint iQuadY, out QuadGeometry upperLeftQuadGeom)
{
	if (iQuadX > 0 && iQuadY > 0)
	{
		ComputeQuadGeometry(iQuadX - 1, iQuadY - 1, InputHeightTexture, upperLeftQuadGeom);

	}
	else
	{
		if (iQuadX > 0)
		{
			ComputeQuadGeometry(iQuadX - 1, nCountY - 1, TNeighbourTexture, upperLeftQuadGeom);
		}
		else if (iQuadY > 0)
		{
			ComputeQuadGeometry(nCountX - 1, iQuadY - 1, LNeighbourTexture, upperLeftQuadGeom);
		}
	}
}

void ComputeLowerNeighbourQuadGeometry(uint iQuadX, uint iQuadY, out QuadGeometry lowerQuadGeom)
{
	if (iQuadY < nCountY - 1)
	{
		ComputeQuadGeometry(iQuadX, iQuadY + 1, InputHeightTexture, lowerQuadGeom);

	}
	else
	{
		ComputeQuadGeometry(iQuadX, 0, BNeighbourTexture, lowerQuadGeom);

	}
}

void ComputeLowerRightNeighbourQuadGeometry(uint iQuadX, uint iQuadY, out QuadGeometry lowerRightQuadGeom)
{
	if (iQuadY < nCountY - 1 && iQuadX < nCountX - 1)
	{
		ComputeQuadGeometry(iQuadX + 1, iQuadY + 1, InputHeightTexture, lowerRightQuadGeom);

	}
	else
	{
		if (iQuadY < nCountY - 1)
		{
			ComputeQuadGeometry(0, iQuadY + 1, RNeighbourTexture, lowerRightQuadGeom);
		}
		else if (iQuadX < nCountX - 1)
		{
			ComputeQuadGeometry(iQuadX + 1, 0, BNeighbourTexture, lowerRightQuadGeom);
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

void ComputeQuadOutputData(uint iQuadX, uint iQuadY, out OutputQuadData data)
{
	QuadGeometry thisQuad;
	ComputeQuadGeometry(iQuadX, iQuadY, InputHeightTexture, thisQuad);

	QuadGeometry leftNeighbourQuad = thisQuad;
	ComputeLeftNeighbourQuadGeometry(iQuadX, iQuadY, leftNeighbourQuad);

	QuadGeometry lowerLeftNeighbourQuad = thisQuad;
	ComputeLowerLeftNeighbourQuadGeometry(iQuadX, iQuadY, lowerLeftNeighbourQuad);

	QuadGeometry rightNeighbourQuad = thisQuad;
	ComputeRightNeighbourQuadGeometry(iQuadX, iQuadY, rightNeighbourQuad);

	QuadGeometry upperRightNeighbourQuad = thisQuad;
	ComputeUpperRightNeighbourQuadGeometry(iQuadX, iQuadY, upperRightNeighbourQuad);

	QuadGeometry upperNeighbourQuad = thisQuad;
	ComputeUpperNeighbourQuadGeometry(iQuadX, iQuadY, upperNeighbourQuad);

	QuadGeometry upperLeftNeighbourQuad = thisQuad;
	ComputeUpperLeftNeighbourQuadGeometry(iQuadX, iQuadY, upperLeftNeighbourQuad);

	QuadGeometry lowerNeighbourQuad = thisQuad;
	ComputeLowerNeighbourQuadGeometry(iQuadX, iQuadY, lowerNeighbourQuad);

	QuadGeometry lowerRightNeighbourQuad = thisQuad;
	ComputeLowerRightNeighbourQuadGeometry(iQuadX, iQuadY, lowerRightNeighbourQuad);


	Triangle neigbours[6];
	Vertex v[6];

	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 0  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[0].normal = thisQuad.t[0].n;
	v[0].tex = thisQuad.tex[0];
	v[0].pos = thisQuad.vertex[0];

	neigbours[0] = thisQuad.t[0];
	neigbours[1] = thisQuad.t[1];
	neigbours[2] = lowerNeighbourQuad.t[0];
	neigbours[3] = lowerLeftNeighbourQuad.t[1];
	neigbours[4] = lowerLeftNeighbourQuad.t[0];
	neigbours[5] = leftNeighbourQuad.t[1];

	ComputeVertexNormalAndTangent(neigbours, v[0]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 0  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 1  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[1].normal = thisQuad.t[0].n;
	v[1].tex = thisQuad.tex[1];
	v[1].pos = thisQuad.vertex[1];

	neigbours[0] = upperNeighbourQuad.t[0];
	neigbours[1] = upperNeighbourQuad.t[1];
	neigbours[2] = thisQuad.t[0];
	neigbours[3] = leftNeighbourQuad.t[1];
	neigbours[4] = leftNeighbourQuad.t[0];
	neigbours[5] = upperLeftNeighbourQuad.t[1];

	ComputeVertexNormalAndTangent(neigbours, v[1]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 1  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 2  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[2].normal = thisQuad.t[0].n;
	v[2].tex = thisQuad.tex[2];
	v[2].pos = thisQuad.vertex[2];

	neigbours[0] = upperRightNeighbourQuad.t[0];
	neigbours[1] = upperRightNeighbourQuad.t[1];
	neigbours[2] = rightNeighbourQuad.t[0];
	neigbours[3] = thisQuad.t[1];
	neigbours[4] = thisQuad.t[0];
	neigbours[5] = upperNeighbourQuad.t[1];

	ComputeVertexNormalAndTangent(neigbours, v[2]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 3  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[3].normal = thisQuad.t[1].n;
	v[3].tex = thisQuad.tex[0];
	v[3].pos = thisQuad.vertex[0];

	neigbours[0] = thisQuad.t[0];
	neigbours[1] = thisQuad.t[1];
	neigbours[2] = lowerNeighbourQuad.t[0];
	neigbours[3] = lowerLeftNeighbourQuad.t[1];
	neigbours[4] = lowerLeftNeighbourQuad.t[0];
	neigbours[5] = leftNeighbourQuad.t[1];

	ComputeVertexNormalAndTangent(neigbours, v[3]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 3 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 4  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[4].normal = thisQuad.t[1].n;
	v[4].tex = thisQuad.tex[2];
	v[4].pos = thisQuad.vertex[2];

	neigbours[0] = upperRightNeighbourQuad.t[0];
	neigbours[1] = upperRightNeighbourQuad.t[1];
	neigbours[2] = rightNeighbourQuad.t[0];
	neigbours[3] = thisQuad.t[1];
	neigbours[4] = thisQuad.t[0];
	neigbours[5] = upperNeighbourQuad.t[1];

	ComputeVertexNormalAndTangent(neigbours, v[4]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 4 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//@{ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ vertex 5  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	v[5].normal = thisQuad.t[1].n;
	v[5].tex = thisQuad.tex[3];
	v[5].pos = thisQuad.vertex[3];

	neigbours[0] = rightNeighbourQuad.t[0];
	neigbours[1] = rightNeighbourQuad.t[1];
	neigbours[2] = lowerRightNeighbourQuad.t[0];
	neigbours[3] = lowerNeighbourQuad.t[1];
	neigbours[4] = lowerNeighbourQuad.t[0];
	neigbours[5] = thisQuad.t[1];

	ComputeVertexNormalAndTangent(neigbours, v[5]);

	//@} ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end: vertex 5 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	for (int iVertex = 0; iVertex < 6; iVertex++)
	{
		data.vertices[iVertex] = v[iVertex];
		data.indices[iVertex] = iVertex;
	}


	/*for (int iTriangle = 0; iTriangle < 2; iTriangle++)
	{
		float3 triangleNormal = thisQuad.t[iTriangle].n;
		float3 triangleTangent = - cross(float3(1, 0, 0), triangleNormal);

		for (int iVertex = 0; iVertex < 3; iVertex++)
		{
			uint ivtx = iVertex + iTriangle * 3;

			data.vertices[ivtx].pos = thisQuad.t[iTriangle].v[iVertex];
			data.vertices[ivtx].normal = triangleNormal;
			data.vertices[ivtx].tex = thisQuad.t[iTriangle].tex[iVertex];
			data.vertices[ivtx].tangent = triangleTangent;

			data.indices[ivtx] = ivtx;
		}
	}*/
}

[numthreads(1, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
	uint iQuadX = DTid.x;
	uint iQuadY = DTid.y;

	OutputQuadData outData;
	ComputeQuadOutputData(iQuadX, iQuadY, outData);

	uint nStartIndex = (iQuadY * (nCountX - 1) + iQuadX) * 6;
	uint VERTEX_SIZE = 44;

	Vertex outVertex;

	for (int i = 0; i < 6; i++)
	{
		uint nVertexStartIndex = nStartIndex + i;

		OutIndexBuffer.Store(nVertexStartIndex * 4, nVertexStartIndex);

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
