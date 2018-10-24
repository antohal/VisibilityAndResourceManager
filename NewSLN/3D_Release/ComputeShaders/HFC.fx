//
//	Ўейдер дл€ расчета триангул€ции по карте высот
//

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
