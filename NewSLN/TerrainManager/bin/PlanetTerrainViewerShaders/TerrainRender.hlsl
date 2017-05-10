////////////////////////
// Глобальные буфферы //
////////////////////////
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

// Буффер с матрицами трансформации
cbuffer MatrixBuffer : register(b1)
{
	matrix viewMatrix;
	matrix projectionMatrix;
}


// Сэмплер для выборки из карты высот
SamplerState HeightTextureSampler
{
    Filter   = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

// Текстура с картой высот
Texture2D InputHeightTexture		: register(t0);

// Формат входных данных для вершинного шейдера.
// Вершинный буфер, на базе которого запускается вершинный шейдер, строится заранее (возможно кэшируется для различных размеров карты высот,
// поскольку он одинаковый для карт высот с одинаковыми размерами)
struct VSInput
{
	uint uiQuadId : QUAD_ID; 	// идентификатор четырехугольника, на базе которого сроится пара треугольников, тип DXGI_FORMAT_R32_UINT
};

struct GSInput
{
	uint uiVertex0;
	uint uiVertex1;
	uint uiVertex2;
	uint uiVertex3;
};

////////////////////////////////////////////////////////////////////////////////
// Вершинный шейдер - вычисляет индексы вершин четырехугольника
////////////////////////////////////////////////////////////////////////////////
GSInput TerrainVertexShader(VSInput input)
{
	GSInput output;

	uint iy = (input.uiQuadId / (nCountX - 1));
	uint ix = input.uiQuadId - iy * (nCountX - 1);

	output.uiVertex0 = iy*nCountX + ix;
	output.uiVertex1 = iy*nCountX + ix + 1;
	output.uiVertex2 = (iy + 1)*nCountX + ix;
	output.uiVertex3 = (iy + 1)*nCountX + ix + 1;
	
	return output;
}

// Формат входных данных пиксельного шейдера
struct PSInput
{
	float4 position : SV_POSITION;
    float2 tex 		: TEXCOORD0;
	float3 normal 	: NORMAL;
	float3 binormal : BINORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// получить точку на поверхности эллипсоида WGS-84
////////////////////////////////////////////////////////////////////////////////
double3 GetWGS84SurfacePoint(float longitude, float lattitude)
{
	double Rmin = 6356752.3142;
	double Rmax = 6378137;

	double cosB = cos(lattitude);
	double sinB = sin(lattitude);

	double cosA = cos(longitude);
	double sinA = sin(longitude);

	double R = sqrt(Rmax*Rmax*Rmin*Rmin / (Rmin*Rmin*cosB*cosB + Rmax*Rmax*sinB*sinB));

	return double3(
		R*cosA*cosB,
		R*sinA*cosB,
		R*sinB
		);
}

////////////////////////////////////////////////////////////////////////////////
// нормаль к эллипсоиду в точке на поверхности
////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
// получить высоту вершины по индексам вдоль осей x и y
////////////////////////////////////////////////////////////////////////////////
float GetVertexHeight(uint ix, uint iy)
{
	float2 texCoord;

	float fx = ix;
	float fy = iy;

	texCoord.x = fx / nCountX;
	texCoord.y = fy / nCountY;

	float4 TexColor = InputHeightTexture.SampleLevel(HeightTextureSampler, texCoord, 0);

	return TexColor.r;
}

////////////////////////////////////////////////////////////////////////////////
// получить позицию вершины на эллипсоиде WGS-84 с учетом карты высот
////////////////////////////////////////////////////////////////////////////////
float3 GetWGS84VertexPos(uint ix, uint iy, float height, double3 vMiddlePoint, double3 vMiddleNormal, double3 vEast, double3 vNorth)
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

	/*double3 vDelta = vVertex - vMiddlePoint;

	double xCoord = dot(vDelta, vNorth);
	double yCoord = dot(vDelta, vMiddleNormal);
	double zCoord = dot(vDelta, vEast);

	return float3(xCoord, yCoord, zCoord);*/

	// global coords
	return vVertex;
}

////////////////////////////////////////////////////////////////////////////////
// Функция расчета вершины на поверхности Земли в зависимости от индекса
////////////////////////////////////////////////////////////////////////////////
PSInput ComputeWGS84Vertex(uint vertexId)
{
	PSInput output;

	uint iy = vertexId / nCountX;
	uint ix = vertexId - iy * nCountX;

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

	// Calculate the position of the vertex against the view, and projection matrices.
	output.position = mul(float4(vVertexPos, 1), viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.normal = normal;
	output.tex = texcoord;
	output.binormal = binormal;
}

////////////////////////////////////////////////////////////////////////////////
// Геометрический шейдер
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(4)]
void TerrainGeometryShader(point GSInput input, inout TriangleStream<PSInput> triStream)
{
	PSInput v0, v1, v2, v3;

	v0 = ComputeWGS84Vertex(input.uiVertex0);
	v1 = ComputeWGS84Vertex(input.uiVertex1);
	v2 = ComputeWGS84Vertex(input.uiVertex2);
	v3 = ComputeWGS84Vertex(input.uiVertex3);

	triStream.Append(v2);
	triStream.Append(v0);
	triStream.Append(v3);
	triStream.Append(v1);
}

//////////////////////////////////
// Пиксельный шейдер
//////////////////////////////////

// Текстура Земли
Texture2D terrainTexture;

// Сэмплер для выборки из текстуры Земли
SamplerState TerrainTextureSampler
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

// Буффер, в котором хранятся данные источника освещения
cbuffer LightBuffer
{
    float4	diffuseColor;
    float3	lightDirection;
	uint	mode;
};

//
// Примитивный пиксельный шейдер с расчетом диффузного освещения
//

float4 TerrainPixelShader(PSInput input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	float4 ambientColor;


	ambientColor = float4(0.2, 0.2, 0.2, 0.2);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = terrainTexture.Sample(TerrainTextureSampler, input.tex);

	float3 N = input.normal;

	// Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(N, lightDir));

    // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
    color = saturate(diffuseColor * lightIntensity + ambientColor);

	if (mode == 0)
	{
		color = color * textureColor;
	}
	else if (mode == 1)
	{
		color = float4(0.5*N + 0.5, 1);
	}

    return color;
}

///////////////////////////////////////////////////////////////////
//	Техника
///////////////////////////////////////////////////////////////////
technique11 RenderTerrainGS
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, TerrainVertexShader()));
		SetGeometryShader(CompileShader(gs_5_0, TerrainGeometryShader()));
		SetPixelShader(CompileShader(ps_5_0, TerrainPixelShader()));
	}
}
