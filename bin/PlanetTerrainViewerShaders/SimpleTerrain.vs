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

	double4 camPos;
	double4 axisX;
	double4 axisY;
	double4 axisZ;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 binormal : BINORMAL;
};


struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 binormal : BINORMAL;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, viewMatrix);

   /*
   double3 vPos = input.position.xyz;
   double3 vDelta = vPos - camPos.xyz;

   double dfX = dot(axisX.xyz, vDelta);
   double dfY = dot(axisY.xyz, vDelta);
   double dfZ = dot(axisZ.xyz, vDelta);

   output.position.x = (float)dfX;
   output.position.y = (float)dfY;
   output.position.z = (float)dfZ;
   output.position.w = 1;
   */

    output.position = mul(output.position, projectionMatrix);
    

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    
    // Normalize the normal vector.
    output.normal = input.normal;

	output.binormal = input.binormal;

    return output;
}
