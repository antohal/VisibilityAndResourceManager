////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix viewMatrix;
	matrix projectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};


struct PixelInputType
{
    float4 position : SV_POSITION;
	float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
	float3 tangent : TANGENT;
	float4 color : COLOR0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(float4(input.position, 1.0f), viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    
    // Normalize the normal vector.
    output.normal = input.normal;

	output.tangent = input.tangent;

	output.color = float4(1,1,1,1);

	if (input.tex.x == 0 || input.tex.y == 0 || input.tex.x == 1 || input.tex.y == 1)
		output.color = float4(1,0,0,1);

    return output;
}
