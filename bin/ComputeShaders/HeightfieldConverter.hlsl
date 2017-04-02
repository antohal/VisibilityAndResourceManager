//--------------------------------------------------------------------------------------
// File: BasicCompute11.hlsl
//
// This file contains the Compute Shader to perform array A + array B
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


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

///////////////////////
// OUTPUT DATA TYPES //
///////////////////////

/*
struct VertexType
{
    float3 position;
    float2 tex;
	float3 normal;
	float3 binormal;
};*/

ByteAddressBuffer 	InputHeightBuffer 		: register(t0);
RWByteAddressBuffer OutVertexBuffer 		: register(u0);
RWByteAddressBuffer OutIndexBuffer 			: register(u1);

[numthreads(1, 1, 1)]
void CSMain( uint3 DTid : SV_DispatchThreadID )
{
	uint ix = DTid.x;
	uint iy = DTid.y;
	
	uint ivtx = ix + iy * nCountX;
	
	//uint addr = ix * iy * 4;
	//uint height = InputHeightBuffer.Load();
	
	
	// INDICES
	
	if (ix < nCountX - 1 && iy < nCountY - 1)
	{
		uint nStartIndex = 6 * (ix + iy * (nCountX - 1));
	
		uint i0 = iy*nCountX + ix;
		uint i1 = iy*nCountX + ix + 1;
		uint i2 = (iy + 1)*nCountX + ix;
		uint i3 = (iy + 1)*nCountX + ix + 1;

//		OutIndexBuffer.Store(nStartIndex 		* 4, ivtx);
//		OutIndexBuffer.Store((nStartIndex + 1) 	* 4, ix);
//		OutIndexBuffer.Store((nStartIndex + 2) 	* 4, iy);
		
		OutIndexBuffer.Store(nStartIndex 		* 4, i3);
		OutIndexBuffer.Store((nStartIndex + 1) 	* 4, i1);
		OutIndexBuffer.Store((nStartIndex + 2) 	* 4, i0);

		OutIndexBuffer.Store((nStartIndex + 3) 	* 4, i2);
		OutIndexBuffer.Store((nStartIndex + 4) 	* 4, i3);
		OutIndexBuffer.Store((nStartIndex + 5) 	* 4, i0);
	}
	
	float dx = fSizeX / (nCountX - 1);
	float dy = fSizeY / (nCountY - 1);
	
	// VERTICES
			
	float posx = ix * dx - fSizeX * 0.5;
	float posy = 0; //_heightfield.fMinHeight + (_heightfield.fMaxHeight - _heightfield.fMinHeight) * ((float)_heightfield.vecData[i] / 255.f);
	float posz = iy * dy - fSizeX * 0.5;

	float tu = (float)ix / (nCountX - 1);
	float tv = (float)iy / (nCountY - 1);
	
	float nx = 0;
	float ny = 1;
	float nz = 0;
	
	float bnx = 1;
	float bny = 0;
	float bnz = 0;
	
	
	uint VERTEX_SIZE = 44;
	
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE, 		asuint(posx));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 4, 	asuint(posy));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 8, 	asuint(posz));

	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 12, 	asuint(tu));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 16, 	asuint(tv));
	
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 20,	asuint(nx));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 24, 	asuint(ny));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 28, 	asuint(nz));

	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 32,	asuint(bnx));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 36, 	asuint(bny));
	OutVertexBuffer.Store(ivtx * VERTEX_SIZE + 40, 	asuint(bnz));
}
