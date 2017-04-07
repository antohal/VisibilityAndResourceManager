/////////////
// GLOBALS //
/////////////
cbuffer HeightfieldSettings  : register(b0)
{
	float fMinHeight;								// минимальная высота (соответствующая значению 0 в данных)
	float fMaxHeight;								// максимальная высота (соответствующая значению 255 в данных)
	float fSizeX;									// размер по X
	float fSizeY;									// размер по Y
	
	uint nCountX;									// количество точек по X
	uint nCountY;									// количество точек по Y
	
	float fTemp1;
	float fTemp2;
};

//////////////////////
// INPUT DATA TYPES //
//////////////////////

struct HeightBufferItem
{
    uint	heights;
};

ByteAddressBuffer 	InputHeightBuffer 		: register(t0);
RWByteAddressBuffer OutVertexBuffer 		: register(u0);
RWByteAddressBuffer OutIndexBuffer 			: register(u1);

// получить индекс вершины
uint GetVertexId(uint ix, uint iy)
{
	return ix + iy * nCountX;
}

// получить высоту вершины по индексам вдоль осей x и y
uint GetVertexHeight(uint ix, uint iy)
{
	uint ivtx = GetVertexId(ix, iy);
	
	uint addr = ivtx / 4;
	uint heights = asuint( InputHeightBuffer.Load(addr * 4 ) );

	uint aHeights[4];
	
	aHeights[0] = (heights) & 0xFF;
	aHeights[1] = (heights >> 8) & 0xFF;
	aHeights[2] = (heights >> 16) & 0xFF;
	aHeights[3] = (heights >> 24) & 0xFF;
	
	return aHeights[ivtx % 4];
}

// получить позицию вершины
float3 GetVertexPos(uint ix, uint iy, uint height)
{
	float dx = fSizeX / (nCountX - 1);
	float dy = fSizeY / (nCountY - 1);
	
	return float3(	ix * dx - fSizeX * 0.5, 
					fMinHeight + (fMaxHeight - fMinHeight) * (height / 255.f),
					iy * dy - fSizeX * 0.5
					);
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
