////////////////////////
// ���������� ������� //
////////////////////////
cbuffer HeightfieldSettings  : register(b0)
{
	float	fMinLattitude;				// ����������� ������
	float	fMaxLattitude;				// ������������ ������

	float	fMinLongitude;				// ����������� �������
	float	fMaxLongitude;				// ������������ �������

	uint	nCountX;					// ���������� ����� �� X
	uint	nCountY;					// ���������� ����� �� Y
	
	float	fWorldScale;				// ������� ����
	float	fHeightScale;				// ������� ������
};


// ������� ��� ������� �� ����� �����
SamplerState HeightTextureSampler
{
    Filter   = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

// �������� � ������ �����
Texture2D InputHeightTexture		: register(t0);

// ������ ������� ������ ��� ���������� �������.
// ��������� �����, �� ���� �������� ����������� ��������� ������, �������� ������� (�������� ���������� ��� ��������� �������� ����� �����,
// ��������� �� ���������� ��� ���� ����� � ����������� ���������)
struct VSInput
{
	uint uiQuadId : QUAD_ID; 	// ������������� ����������������, �� ���� �������� ������� ���� �������������, ��� DXGI_FORMAT_R32_UINT
};

struct GSInput
{
	uint uiVertex0;
	uint uiVertex1;
	uint uiVertex2;
	uint uiVertex3;
};

////////////////////////////////////////////////////////////////////////////////
// ��������� ������
////////////////////////////////////////////////////////////////////////////////
GSInput TerrainVertexShader(VSInput input)
{
	GSInput output;

	
	return output;
}

// ������ ������� ������ ����������� �������
struct PSInput
{
	float4 position : SV_POSITION;
    float2 tex 		: TEXCOORD0;
	float3 normal 	: NORMAL;
	float3 binormal : BINORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// �������������� ������
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(4)]
void TerrainGeometryShader(point GSInput input, inout TriangleStream<PSInput> triStream)
{
	
}

//////////////////////////////////
// ���������� ������
//////////////////////////////////

// �������� �����
Texture2D terrainTexture;

// ������� ��� ������� �� �������� �����
SamplerState HeightTextureSampler
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

// ������, � ������� �������� ������ ��������� ���������
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
