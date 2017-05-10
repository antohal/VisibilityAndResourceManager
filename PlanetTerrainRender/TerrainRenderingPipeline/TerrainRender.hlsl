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
// Вершинный шейдер
////////////////////////////////////////////////////////////////////////////////
GSInput TerrainVertexShader(VSInput input)
{
	GSInput output;

	
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
// Геометрический шейдер
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(4)]
void TerrainGeometryShader(point GSInput input, inout TriangleStream<PSInput> triStream)
{
	
}

//////////////////////////////////
// Пиксельный шейдер
//////////////////////////////////

// Текстура Земли
Texture2D terrainTexture;

// Сэмплер для выборки из текстуры Земли
SamplerState HeightTextureSampler
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

float4 TerrainPixelShader(PSInput input) : SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	float4 ambientColor;


	ambientColor = float4(0.2, 0.2, 0.2, 0.2);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);

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
