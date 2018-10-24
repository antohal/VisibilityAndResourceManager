float2 g_RND[64] = {
			float2(2.0*0.001000-1.0,2.0*0.563000-1.0),
			float2(2.0*0.193000-1.0,2.0*0.808000-1.0),
			float2(2.0*0.585000-1.0,2.0*0.479000-1.0),
			float2(2.0*0.350000-1.0,2.0*0.895000-1.0),
			float2(2.0*0.822000-1.0,2.0*0.746000-1.0),
			float2(2.0*0.174000-1.0,2.0*0.858000-1.0),
			float2(2.0*0.710000-1.0,2.0*0.513000-1.0),
			float2(2.0*0.303000-1.0,2.0*0.014000-1.0),
			float2(2.0*0.091000-1.0,2.0*0.364000-1.0),
			float2(2.0*0.147000-1.0,2.0*0.165000-1.0),
			float2(2.0*0.988000-1.0,2.0*0.445000-1.0),
			float2(2.0*0.119000-1.0,2.0*0.004000-1.0),
			float2(2.0*0.008000-1.0,2.0*0.377000-1.0),
			float2(2.0*0.531000-1.0,2.0*0.571000-1.0),
			float2(2.0*0.601000-1.0,2.0*0.607000-1.0),
			float2(2.0*0.166000-1.0,2.0*0.663000-1.0),
			float2(2.0*0.450000-1.0,2.0*0.352000-1.0),
			float2(2.0*0.057000-1.0,2.0*0.607000-1.0),
			float2(2.0*0.783000-1.0,2.0*0.802000-1.0),
			float2(2.0*0.519000-1.0,2.0*0.301000-1.0),
			float2(2.0*0.875000-1.0,2.0*0.726000-1.0),
			float2(2.0*0.955000-1.0,2.0*0.925000-1.0),
			float2(2.0*0.539000-1.0,2.0*0.142000-1.0),
			float2(2.0*0.462000-1.0,2.0*0.235000-1.0),
			float2(2.0*0.862000-1.0,2.0*0.209000-1.0),
			float2(2.0*0.779000-1.0,2.0*0.843000-1.0),
			float2(2.0*0.996000-1.0,2.0*0.999000-1.0),
			float2(2.0*0.611000-1.0,2.0*0.392000-1.0),
			float2(2.0*0.266000-1.0,2.0*0.297000-1.0),
			float2(2.0*0.840000-1.0,2.0*0.023000-1.0),
			float2(2.0*0.375000-1.0,2.0*0.092000-1.0),
			float2(2.0*0.677000-1.0,2.0*0.056000-1.0),
			float2(2.0*0.008000-1.0,2.0*0.918000-1.0),
			float2(2.0*0.275000-1.0,2.0*0.272000-1.0),
			float2(2.0*0.587000-1.0,2.0*0.691000-1.0),
			float2(2.0*0.837000-1.0,2.0*0.726000-1.0),
			float2(2.0*0.484000-1.0,2.0*0.205000-1.0),
			float2(2.0*0.743000-1.0,2.0*0.468000-1.0),
			float2(2.0*0.457000-1.0,2.0*0.949000-1.0),
			float2(2.0*0.744000-1.0,2.0*0.108000-1.0),
			float2(2.0*0.599000-1.0,2.0*0.385000-1.0),
			float2(2.0*0.735000-1.0,2.0*0.608000-1.0),
			float2(2.0*0.572000-1.0,2.0*0.361000-1.0),
			float2(2.0*0.151000-1.0,2.0*0.225000-1.0),
			float2(2.0*0.425000-1.0,2.0*0.802000-1.0),
			float2(2.0*0.517000-1.0,2.0*0.989000-1.0),
			float2(2.0*0.751000-1.0,2.0*0.345000-1.0),
			float2(2.0*0.168000-1.0,2.0*0.657000-1.0),
			float2(2.0*0.491000-1.0,2.0*0.063000-1.0),
			float2(2.0*0.699000-1.0,2.0*0.504000-1.0),
			float2(2.0*0.147000-1.0,2.0*0.949000-1.0),
			float2(2.0*0.141000-1.0,2.0*0.905000-1.0),
			float2(2.0*0.692000-1.0,2.0*0.303000-1.0),
			float2(2.0*0.426000-1.0,2.0*0.070000-1.0),
			float2(2.0*0.966000-1.0,2.0*0.683000-1.0),
			float2(2.0*0.153000-1.0,2.0*0.877000-1.0),
			float2(2.0*0.821000-1.0,2.0*0.582000-1.0),
			float2(2.0*0.191000-1.0,2.0*0.177000-1.0),
			float2(2.0*0.817000-1.0,2.0*0.475000-1.0),
			float2(2.0*0.155000-1.0,2.0*0.503000-1.0),
			float2(2.0*0.732000-1.0,2.0*0.405000-1.0),
			float2(2.0*0.279000-1.0,2.0*0.568000-1.0),
			float2(2.0*0.682000-1.0,2.0*0.755000-1.0),
			float2(2.0*0.721000-1.0,2.0*0.475000-1.0)
		};

	Texture2D   AmbientTexture          = NULL;
	Texture2D   DiffuseTexture          = NULL;
	Texture2D   SpecularTexture         = NULL;
	Texture2D   GlossinessTexture       = NULL;
	Texture2D   SpecularLevelTexture    = NULL;
	Texture2D   SelfIlluminationTexture = NULL;
	Texture2D   OpacitiTexture          = NULL;
	Texture2D   FilterColorTexture      = NULL;
	Texture2D   BumpTexture             = NULL;
	TextureCube ReflectionTexture       = NULL;
	Texture2D   RefractionTexture       = NULL;
	Texture2D   DisplacementTexture     = NULL;

	Texture2D   BackBufferTexture1                    = NULL;
	Texture2D   BackBufferTexture2                    = NULL;
//	Texture2DMS <float4,4>  DepthBufferTexture        = NULL;
	Texture2DMS <float2,4>  DepthBufferTexture        = NULL;
	Texture2DMS <float,4>   TempBufferTexture         = NULL;
	Texture2DMS <float,4>   ZBufferTexture            = NULL;
	Texture2DMS <uint,4>    StencilBufferTexture      = NULL;
	Texture2DMS <float,4>   MaskBufferTexture         = NULL;
//	Texture2D MaskBufferTexture                       = NULL;
//	Texture2DMS <float,4>   ZBufferTexture            = NULL;
//	Texture2D    ZBufferTexture                       = NULL;
//	Texture2DMS <float,4>   DepthBufferTexture        = NULL;
	Texture2D   NormalBufferTexture                   = NULL;
	//Texture2D   DepthMaskTexture                    = NULL;
	//Texture2D   AmbientOcclusionTexture             = NULL;

	Texture2D MapTexture       = NULL;
	Texture2D GroundTexture    = NULL;
	Texture2D SkyBoxTexture    = NULL;
	Texture2D BillBoardTexture = NULL;

	TextureCube EnvMap       = NULL;
	TextureCube MoonCube  	 = NULL;

RasterizerState rsCullNone 
{ 
	CullMode = None; 
};

RasterizerState rsCullCW 
{ 
	CullMode = FRONT; 
};

RasterizerState rsCullCCW
{ 
	CullMode = BACK; 
};


RasterizerState rsBillboard 
{ 
	CullMode = NONE; 
	FillMode = SOLID;
};

BlendState bsBillboard 
{ 
	AlphaToCoverageEnable = FALSE;

	BlendEnable[0] = TRUE;
	BlendEnable[1] = TRUE;
	BlendEnable[2] = TRUE;
	BlendEnable[3] = TRUE;
	BlendEnable[4] = TRUE;
	BlendEnable[5] = TRUE;
	BlendEnable[6] = TRUE;
	BlendEnable[7] = TRUE;

	SrcBlend       = SRC_ALPHA;
	DestBlend      = INV_SRC_ALPHA; 
};

BlendState bsBillboardText 
{ 
	AlphaToCoverageEnable = FALSE;

	BlendEnable[0] = FALSE;
	BlendEnable[1] = FALSE;
	BlendEnable[2] = FALSE;
	BlendEnable[3] = FALSE;
	BlendEnable[4] = FALSE;
	BlendEnable[5] = FALSE;
	BlendEnable[6] = FALSE;
	BlendEnable[7] = FALSE;

	SrcBlend       = ONE;
	DestBlend      = ZERO; 
};

BlendState bsSSAO 
{ 
	AlphaToCoverageEnable = FALSE;

	BlendEnable[0] = TRUE;
	BlendEnable[1] = FALSE;
	BlendEnable[2] = FALSE;
	BlendEnable[3] = FALSE;
	BlendEnable[4] = FALSE;
	BlendEnable[5] = FALSE;
	BlendEnable[6] = FALSE;
	BlendEnable[7] = FALSE;

	SrcBlend       = SRC_ALPHA;
	DestBlend      = INV_SRC_ALPHA; 
};

DepthStencilState dssNoZWrite 
{ 
	DepthEnable    = TRUE;
	DepthWriteMask = ZERO;
};

DepthStencilState dssZWrite   
{ 
	DepthEnable    = TRUE;
	DepthWriteMask = ALL; 

	DepthFunc      = LESS_EQUAL;
};

cbuffer CB_Scene
{
	matrix matViewProj;
	matrix matViewProjI;
	matrix matViewProjIT;
	float4 g_CameraPosition;
	float4 g_CameraDirection;
	float4 g_LightDirection <string group="Освещение";          string name="Направление источника";>                           = float4(0.35, -0.75, -0.50, 1.0);
	float4 g_SunDirection                                                                                                       = float4(0,-1,0,1);
	float4 g_MoonDirection                                                                                                      = float4(0,-1,0,1);
	float4 g_LightAmbient   <string group="Освещение";          string name="Цвет фонового освещения"; int color=0;>            = float4(0.25,  0.25,  0.25, 1.0);
	float4 g_LightDiffuse   <string group="Освещение";          string name="Цвет источника света";    int color=0;>            = float4(1.75,  1.75,  1.75, 1.0);
	float4 g_HiliteColor    <string group="Выделение объектов"; string name="Цвет выделения";          int color=0;>            = 1.0;
	float3 g_3DPoint                                                                                                            = float3(0.0,0.0,0.0);
	float  g_HiliteProgress <string group="Выделение объектов"; string name="Скорость мигания";float min=0;float max=20;>       = 1.0;
	float  g_HilitePower    <string group="Выделение объектов"; string name="Интенсивность мигания";float min=0;float max=1;>   = 1.0;
	float  g_Time;
	float  g_Lightintensity <string group="Освещение";          string name="Интенсивность источника";float min=0;float max=3;> = 1.0;
};

cbuffer CB_Mesh
{
	matrix matWorld;
	matrix matWorldInverceTranspose;
	float4 g_ObjectPickColor;
	float3 BBMin;
	float3 BBMax;
	float3 BBExtMin;
	float3 BBExtMax;
	float  LODLevel;
	bool   IsHilite;
};

cbuffer CB_Material
{
	float4 mat_AmbientColor;
	float4 mat_DiffuseColor;
	float4 mat_SpecularColor;

	float  mat_SelfIllumination = 0.0;
	float  mat_SpecularLevel    = 1.0;
	float  mat_SpecularPower    = 64;
	float  mat_Opaciti;

	float  mat_AmbientMapAmount;
	float  mat_DiffuseMapAmount;
	float  mat_SpecularColorMapAmount;
	float  mat_SpecularLevelMapAmount;
	float  mat_GlossinessMapAmount;
	float  mat_SelfIlluminationMapAmount;
	float  mat_OpacityMapAmount;
	float  mat_FilterColorMapAmount;
	float  mat_BumpMapAmount;
	float  mat_ReflectionMapAmount;
	float  mat_RefractionMapAmount;
	float  mat_DisplacementMapAmount;

	float FxOpaciti;

	bool   IsUseAmbientMap;
	bool   IsUseDiffuseMap;
	bool   IsUseSpecularColorMap;
	bool   IsUseSpecularLevelMap;
	bool   IsUseGlossinessMap;
	bool   IsUseSelfIlluminationMap;
	bool   IsUseOpacitiMap;
	bool   IsUseFilterColorMap;
	bool   IsUseBumpMap;
	bool   IsUseReflectionMap;
	bool   IsUseRefractionMap;
	bool   IsUseDisplacementMap;	

	int    mat_RequestLoadStatus;
};

cbuffer CB_MaterialManager_SetRenderMode
{
	float  g_Opaciti = 1.0;
	int    g_RenderMode = 1;
};

int    g_RenderType = 0;

float  g_NormalDirection = 1;

float  g_SkyBoxSize    <string group="Небо";string name="Радиус"; >            = 430000.0;
float  g_SkyBoxScale   <string group="Небо";string name="SkyBoxSphereSize"; >     = 100000.0;
float  g_SkyBoxGround  <string group="Небо";string name="Нулевой уровень"; >   =  15000.0;
float  g_SunSize       <string group="Небо";string name="Размер солнца"; >     =  0.9925f;
float  g_SunBrightness <string group="Небо";string name="Яркость солнца"; >    =     35.f;

float  g_FogPower      <string group="Туман"; string name="Плотность";>          = 3.0;
float4 g_FogColor      <string group="Туман"; string name="Цвет";  int color=0;> = float4(1.0,1.0,1.0,1.0);
bool   g_bIsFogEnabled <string group="Туман"; string name="Использовать туман";> = true;

bool   g_bIsAtmoEnabled<string group="Атмосфера"; string name="Отображать атмосферу";> = true;

bool   g_IsTesselationEnabled <string group="Тесселяция"; string name="Включить тесселяцию";> = true;
bool   g_IsCompositeMaterial  = false;

int    g_DebugDepthMode = 1;
float  g_DebugMaxDepth  = 430000.0;

float  g_BumpZ      <string group="Стандартный материал"; string name="Смещение бампа";float min=0;float max=500;>          = 150.0;

int g_RenderDirection = 1;

SamplerState sam_Point
{
    Filter   = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState sam_Linear
{
    Filter   = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState sam_Anisotropic
{
    Filter   = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_IN
{
	float3 Position : POSITION;
        float3 Normal   : NORMAL;
        float2 TexCoord : TEXCOORD;
        float3 Tangent  : TANGENT;
//        float3 BiNormal : BINORMAL;
};

struct VS_OUT_SOLID
{
	float4 Position   : SV_POSITION; 
	float2 TexCoord   : TEXCOORD0;
	float3 P          : TEXCOORD1;
	float3 N          : TEXCOORD2;
	float3 T          : TEXCOORD3;
	float3 B          : TEXCOORD4;
};

struct VS_OUT_TRANSPARENT
{
	float4 Position   : SV_POSITION; 
	float2 TexCoord   : TEXCOORD0;
	float3 P          : TEXCOORD1;
	float3 N          : TEXCOORD2;
};

struct VS_OUT_WIRE
{
	float4 Position   : SV_POSITION; 
	float2 TexCoord   : TEXCOORD0;
	float3 P          : TEXCOORD1;
	float3 N          : TEXCOORD2;
};

struct VS_OUT_BACKBUFFER
{
	float4 Position   : SV_POSITION; 
	float2 TexCoord   : TEXCOORD0;
};

struct VS_OUT_MAP
{
    	float4 Position   : SV_POSITION; 
    	float2 TexCoord   : TEXCOORD0;
    	float3 P          : TEXCOORD1;
};

struct VS_OUT_GROUND
{
    	float4 Position   : SV_POSITION; 
    	float2 TexCoord   : TEXCOORD0;
	float3 P          : TEXCOORD1;
    	float3 N          : TEXCOORD2;
};

struct VS_OUT_SKYBOX
{
    	float4 Position   : SV_POSITION; 
    	float2 TexCoord   : TEXCOORD0;
	float  P          : TEXCOORD1;
};

struct VS_OUT_BILLBOARD 
{
   	float4 Position	: SV_POSITION;
   	float2 TexCoord	: TEXCOORD0;
};

struct VS_OUT_PICK
{
    float4 Position : SV_POSITION; 
};

struct VS_OUT_FIRE
{
	float4 Position   : SV_POSITION; 
	float3 P          : TEXCOORD0;
	float3 Pos        : TEXCOORD1;
	float3 X          : TEXCOORD2;
	float3 Y          : TEXCOORD3;
	float3 Z          : TEXCOORD4;
	float3 O          : TEXCOORD5;
	float  Size       : TEXCOORD6;
	float  Scale      : TEXCOORD7;
};

struct PS_Color_Z
{
    float4 Color	: SV_Target0;
    float  Z  	 	: SV_Depth0;
};

struct PS_Color_Depth
{
    float4 Color	: SV_Target0;
    float2 Depth	: SV_Target1;
//    float Normal	: SV_Target2;
};

struct PS_Color_Depth_Z
{
    float4 Color	: SV_Target0;
    float2 Depth	: SV_Target1;
    float  Z  	 	: SV_Depth0;
};

struct PS_Color_Depth_Mask
{
    float4 Color	: SV_Target0;
    float2 Depth	: SV_Target1;
    float  Mask		: SV_Target2;
};

struct PS_Color_Depth_Z_Mask
{
    float4 Color	: SV_Target0;
    float2 Depth	: SV_Target1;
    float  Mask		: SV_Target2;
    float  Z  	 	: SV_Depth0;
};

struct SpecularParams
{
	float fLevel;
	float fPower;
};


float g_fEpsilon = 0.00001;

bool IsEqual( float val1, float val2 )
{
	return ( abs( val1 - val2 ) < g_fEpsilon );
}

bool IsEqual( float val1, float val2, float fEpsilon )
{
	return ( abs( val1 - val2 ) < fEpsilon );
}

bool IsZeroLength( float3 unnormVec )
{
	return ( length( unnormVec ) < g_fEpsilon );
}

bool IsZeroLength( float3 unnormVec, float fEpsilon )
{
	return ( length( unnormVec ) < fEpsilon );
}

bool IsCollinear( float3 normVec1, float3 normVec2 )
{
	return ( abs( dot( normVec1, normVec2 ) ) > 1.0f - g_fEpsilon );
}

bool IsCollinear( float3 normVec1, float3 normVec2, float fEpsilon )
{
	return ( abs( dot( normVec1, normVec2 ) ) > 1.0f - fEpsilon );
}

bool IsPerpendicular( float3 normVec1, float3 normVec2 )
{
	return ( abs( dot( normVec1, normVec2 ) ) < g_fEpsilon );
}

bool IsPerpendicular( float3 normVec1, float3 normVec2, float fEpsilon )
{
	return ( abs( dot( normVec1, normVec2 ) ) < fEpsilon );
}



float DiffusePower(float3 N)
{
//	return max(dot(-normalize(g_SunDirection.xyz),N),0) + 0.25*max(dot(normalize(g_SunDirection.xyz),N),0);
	return g_Lightintensity*(max(dot(-normalize(g_LightDirection.xyz),N),0) + 0.25*max(dot(normalize(g_LightDirection.xyz),N),0));
}

float SpecularPower(float3 N, float3 ViewDirection)
{
	float3 LR1 = normalize(reflect( g_LightDirection.xyz,N));
//	float3 LR1 = normalize(reflect( g_SunDirection.xyz,N));
	//float3 LR2 = normalize(reflect(-g_LightDirection.xyz,N));

//	return pow(max(dot(LR,-ViewDirection.xyz),0),1+min(mat_SpecularPower,0.5)*100);

	return pow(max(dot(LR1,-ViewDirection.xyz),0),1+mat_SpecularPower*100);// + pow(max(dot(LR2,-ViewDirection.xyz),0),1+mat_SpecularPower*100);
}

float SpclrPower(float3 N, float3 ViewDirection, float specPower)
{
	float3 LR1 = normalize( reflect( g_LightDirection.xyz, N ) );

	return pow( max( dot( LR1, -ViewDirection ), 0 ), 1 + specPower * 100 );
}

float SpecularPwr(float3 N, float3 ViewDirection,float Power)
{
	float3 LR1 = normalize(reflect( g_LightDirection.xyz,N));

	return pow(max(dot(LR1,-ViewDirection.xyz),0),Power);
}

float GetCameraDepth(float3 Pos)
{
	float A = g_CameraDirection.x;
	float B = g_CameraDirection.y;
	float C = g_CameraDirection.z;
	float D = - (g_CameraPosition.x*A + g_CameraPosition.y*B + g_CameraPosition.z*C);

	return abs(A*Pos.x + B*Pos.y + C*Pos.z + D)/length(g_CameraDirection);///sqrt(A*A+B*B+C*C);
}

float Freshnel(float3 ViewDirection,float3 N, float Amount, float Power)
{
	return lerp(pow(1-saturate(dot(ViewDirection,-N)),Power),1,Amount);
}

float3x3 CalculateNewBasis(float3x3 TangentToWorld, float3 locNorm)
{
    float3 newNormal  = mul(locNorm, TangentToWorld);
    newNormal  = normalize(newNormal);

    float3x3 newNormToWorld;
    newNormToWorld[2] = newNormal;
    //newNormToWorld[0] = normalize( cross( newNormal, TangentToWorld[1] ) );
    //newNormToWorld[1] = cross( newNormToWorld[0], newNormToWorld[2] );
    newNormToWorld[0] = normalize( cross( TangentToWorld[1], newNormal ) );
    newNormToWorld[1] = cross( newNormToWorld[2], newNormToWorld[0] );

    return newNormToWorld;
}

float3 Chessboard(float2 TC_Detailed)
{
	return  ( float3( abs(TC_Detailed.x) % 1 , abs(TC_Detailed.y) % 1, 0.0) );
}

float CalculateDetailedCoefficent(float3 absWorldPos, float fDetailProjCoeff, float fFullDetailCoeff)
{
	float fDetailBlendCoeff = 0.0;

	float3 ddxAbsWP = ddx( absWorldPos );
	float3 ddyAbsWP = ddy( absWorldPos );
	
	//float fProjCoeff = length( length( ddy( absWorldPos ) ), length( ddx( absWorldPos ) ) );
	float fProjCoeff = sqrt(ddxAbsWP.x*ddxAbsWP.x + ddxAbsWP.y*ddxAbsWP.y + ddxAbsWP.z*ddxAbsWP.z + ddyAbsWP.x*ddyAbsWP.x + ddyAbsWP.y*ddyAbsWP.y + ddyAbsWP.z*ddyAbsWP.z); 
	if( ( fProjCoeff <= fDetailProjCoeff ) )
	{
		if( fProjCoeff < fDetailProjCoeff * fFullDetailCoeff )
			fDetailBlendCoeff = 1.0;
		else
			fDetailBlendCoeff = ( fDetailProjCoeff - fProjCoeff ) / ( fDetailProjCoeff * (1.0 - fFullDetailCoeff) );
	}
	return fDetailBlendCoeff;
}

//// Параметры определяющие ветер
	float  velositywind                  <string group="Ветер ";       string name=" скорость ветра";>      = 5;                        /// скорость ветра  
	float4 wind                          <string group="Ветер ";       string name=" направление ветра";> = float4( 1, 1, 0, 0);        /// направление ветра  
////
/////////////
Texture2D Noise;                          // Noize4096.jpg
Texture2D MaskaTerrain;                   // white.jpg
Texture3D NoiseMove;
//---------------------------------------------------------------------------------------------------------------
RasterizerState rsTesselation 
{ 
	CullMode = NONE; 
	FillMode = WIREFRAME;
};
struct HS1
{
	float3 Pos      :   POSITION;
	float3 Normal   :   NORMAL;
	float2 Tex      :   TEXCOORD;
	float3 Tan      :   TANGENT;
};


///
struct PatchTessTri
{
	float EdgeTess[3]   : SV_TessFactor;
	float InsideTess    : SV_InsideTessFactor;
};

struct DomainOut
{
	//float4 PosH : SV_POSITION;
	float3 Pos      :   POSITION;
	float3 Normal   :   NORMAL;
	float2 Tex      :   TEXCOORD;
	float3 Tan      :   TANGENT;
};

struct HullOut
{
	float3 Pos      :   POSITION;
	float3 Normal   :   NORMAL;
	float2 Tex      :   TEXCOORD;
	float3 Tan      :   TANGENT;
};
float StepTesselation          /*         <string group="Трава";        string name="Степень тесселяции"; float min=0; float max=64;> */ =  4;
float RadiusTesselationMax      /*        <string group="Трава";        string name="Расстояние на котором степень тесселяции максимальна"; > */ =  10000;
float RadiusTesselationMin       /*       <string group="Трава";        string name="Расстояние на котором производится тесселяция"; > */ =  20000;
bool TurnGrassOtcechenie       /*      <string group="Трава";  string name="Включить отсещение на границе";>  */ = false;
//float NoteTesselation                   <string group="Тесселяция";        string name="Количество узлов"; float min=0; float max=15;> =  5;
float4   g_densitycolorbase       <string group="Плотность травы";      string name="базовый цвет";int color = 0;> = float4( 0, 0.5, 0, 0);
float    powHue                   <string group="Плотность травы";      string name="powHue"; float min=0; float max=5;>  = 1;
float    powSaturation            <string group="Плотность травы";      string name="powSaturation"; float min=0; float max=5;>  = 2;
float    powBright                <string group="Плотность травы";      string name="powBright"; float min=0; float max=5;>  = 3;
float    angleHuemaxDelta         <string group="Плотность травы";      string name="angleHuemaxDelta"; float min=0; float max=360;>  =  40;
float    angleSaturationDelta     <string group="Плотность травы";      string name="angleSaturationDelta"; float min=0; float max = 1;>  = 0.5;	
float    angleBrightDelta         <string group="Плотность травы";      string name="angleBrightDelta"; float min=0; float max = 1;>  = 0.5;		
float    SampleLevel              <string group="Плотность травы";      string name="Уровень сглаживания"; float min=0; float max = 10;>  = 0;
float    radiusgrass1             <string group="Плотность травы";      string name="радиус полной альфа";>          = 1000; 
float    radiusgrass2             <string group="Плотность травы";      string name="радиус уменьшения альфа";>                 = 2000; 
float    radiusgrassdensity1      <string group="Плотность травы";      string name="радиус полной прорисовки";>          = 1000; 
float    radiusgrassdensity2      <string group="Плотность травы";      string name="радиус прорисовки";>                 = 2000; 
float    g_powremovalgrass        /*<string group="Плотность травы";    string name="степень удаления от расстояния"; float min=0; float max = 10;>*/  = 2;
float AngleBlackOut               <string group="Трава";  string name="угол затемения"; float min=0; float max=1.57;> = 0.5;
float diaposon1 <string group="Трава диапазоны";  string name="диапазон 1"; float min=0; float max=1;> = 0.99825;
float LevelNoizeTree                         /*     <string group="Деревья";        string name="Сосна"; float min=0; float max=1;> */  =  0;
///
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;//ANISOTROPIC;//
        AddressU = Wrap; 
        AddressV = Wrap; 
        AddressW = Wrap; 
	BorderColor = float4(0,0,0,0);
};
BlendState AlphaBlendingOn
{
	BlendEnable[0] = TRUE;
        DestBlend = INV_SRC_ALPHA;
        SrcBlend = SRC_ALPHA;
	AlphaToCoverageEnable = TRUE;//FALSE;//TRUE;
};
DepthStencilState DisableDepth
{
        DepthEnable = FALSE;
        DepthWriteMask = ZERO;
};
DepthStencilState EnableDepth
{
        DepthEnable = TRUE;
        DepthWriteMask = ALL;
};
struct VS1
{
	float3 Pos      :   POSITION;
	float3 Normal   :   NORMAL;
	float2 Tex      :   TEXCOORD;
	float3 Tan      :   TANGENT;
};
struct GS1
{
	float3 Pos      :   POSITION;
	float3 Normal   :   NORMAL;
	float2 Tex      :   TEXCOORD;
	float3 Tan      :   TANGENT;
};

struct VS2
{
	float3 Pos      :   POSITION;
	float3 Normal   :   NORMAL;
	float2 Tex      :   TEXCOORD;
	float3 Tan      :   TANGENT;
};
struct GS2
{
	float3 Pos      :   POSITION;
	float3 Normal   :   NORMAL;
	float2 Tex      :   TEXCOORD;
	float3 Tan      :   TANGENT;
};
struct VS3
{
	float3 Pos      :   POSITION;
	float3 Normal   :   NORMAL;
	float2 Tex      :   TEXCOORD;
	float3 Tan      :   TANGENT;
};
struct VS4
{
	float3 Pos      :   POSITION;
	float3 Normal   :   NORMAL;
	float2 Tex      :   TEXCOORD;
	float3 Tan      :   TANGENT;
};
struct GS3
{
	float3 Pos      :   POSITION;
	float3 Normal   :   NORMAL;
	float2 Tex      :   TEXCOORD;
	float3 Tan      :   TANGENT;
};
struct GS4
{
	float3 Pos      :   POSITION;
	float3 Normal   :   NORMAL;
	float2 Tex      :   TEXCOORD;
	float3 Tan      :   TANGENT;
};
struct PS_INPUT
{
	float4 Pos      :   SV_POSITION;
	float3 Tex      :   TEXCOORD0;
	float3 Normal	:   TEXCOORD2;
	float3 P        :   TEXCOORD3; 
	float  alpha    :   TEXCOORD4;
};
///
struct PS_Tree
{
	float4 Pos      :   SV_POSITION;
	float3 Tex      :   TEXCOORD0;
	float3 Normal	:   TEXCOORD2;
	float3 P        :   TEXCOORD3; 
	float2  alpha    :   TEXCOORD4;
};
///
struct PS_GRASS
{
	float4 Pos      :   SV_POSITION;
	float3 Tex      :   TEXCOORD0;
	float3 Normal	:   TEXCOORD2;
	float3 P        :   TEXCOORD3; 
	float3  alpha    :   TEXCOORD4;
};
///

float Arctangets(float x, float y)
{
	float fi;
        if ( x == 0 && y > 0 )
	{
		fi = 1.57079;
        }
	if ( x == 0 && y < 0 )
	{ 
		fi = -1.57079;
        }
	if ( x !=0 )
	{
		fi=atan(y/x);
		if ( x < 0 ) 
		{ 
			fi = fi+3.14159;
		}
		
	}
        return fi;
}
////
bool RayIntersectSegment(float3 point2, float3 pointray, float3 vVertex0pos, float3 vVertex1pos)
{
	float det = -(pointray.x-point2.x)*(vVertex1pos.y-vVertex0pos.y)+(vVertex1pos.x-vVertex0pos.x)*(pointray.y-point2.y);
	float det1 = -(vVertex0pos.x-point2.x)*(vVertex1pos.y-vVertex0pos.y)+(vVertex1pos.x-vVertex0pos.x)*(vVertex0pos.y-point2.y);
	float det2 = (pointray.x-point2.x)*(vVertex0pos.y-point2.y)-(vVertex0pos.x-point2.x)*(pointray.y-point2.y);
	float tray = det1/det;
	float tsegment = det2/det;
	if ( (tray>=0) && (tsegment<=1) && (tsegment>=0) )
	{
		if ( (point2.z+tray*(pointray.z-point2.z))-(vVertex0pos.z+tsegment*(vVertex1pos.z-vVertex0pos.z))<0.001   )
		{
			return true;
		}
		else {return false;}
	}
	
	else return false;
}
///
bool PointInTriangle(float3 point2, float3 vVertex0pos, float3 vVertex1pos, float3 vVertex2pos)
{
	float3 pointside = (vVertex0pos+vVertex1pos)/2;
	if  ( RayIntersectSegment(point2,pointside,vVertex1pos,vVertex2pos) || RayIntersectSegment(point2,pointside,vVertex0pos,vVertex2pos) )
		return false;
	else return true;
}
///
float3 NormalTriangle(float3 pos1, float3 pos2, float3 pos3)
{
	float3 vectorside=pos3-pos1;
	float3 vectorside2=pos2-pos1;
	float3 Normal=cross(vectorside,vectorside2);
	Normal=normalize(Normal);
	if (Normal.z<0)
	{
		Normal = -Normal;
	}
	return Normal;
}
///
float4 CoefficientPlaneTriangle(float3 pos1, float3 pos2, float3 pos3)
{
	float3 Normal = NormalTriangle(pos1,pos2,pos3);
	float A = Normal.x;
	float B = Normal.y;
	float C = Normal.z;
	float D = -(A*pos1.x+B*pos1.y+C*pos1.z);
	return float4(A,B,C,D);
}
////
float3 OffsetInTriangle(float3 pos, float3 pos1, float3 pos2, float3 pos3, float r, float rand1, float rand2)
{
	
	while ( 1 > 0 )
	{
		pos.x = pos.x + r*sin(6.283*rand1);
		pos.z = pos.z + r*sin(6.283*rand2);
		pos.y = 0;
		if  ( PointInTriangle( pos, pos1, pos2 , pos3) )
		{
			break;
		}
		else
		{
			r = r/3;
		}
	}
	return pos;
	

}

////
////
float2 DefineMaxandMin(float a, float b, float c)
{
	float max = 1;
	float min = 0;
	////
	if ( a >= b && a >= c )
	{
		max = a;
	} 
	else
	{
		if ( b >= c  )
		{
			max = b;
		} 
		else
		{
			max = c;
		}
	}
	////
	if ( a <= b && a <= c )
	{
		min = a;
	} 
	else
	{
		if ( b <= c  )
		{
			min = b;
		} 
		else
		{
			min = c;
		}
	} 
	///
	return float2(max,min);
}
////
float3 ConversionRGBtoHSB(float4 color)
{
	float max,min = 0;
	float3 HSB = float3(0,0,0);
	float2 maxmin = DefineMaxandMin(color.r,color.g,color.b);
	max = maxmin.x;
	min = maxmin.y;
	//// Define Hue
	if ( max == min)
	{
		HSB.x = 0;
	}
	else
	{
		if ( max == color.r && color.g >= color.b )
		{
			HSB.x = 60*(color.g-color.b)/(max-min);
		}
		if ( max == color.r && color.g <= color.b )
		{
			HSB.x = 60*(color.g-color.b)/(max-min) + 360;
		}
		if ( max == color.g )
		{
			HSB.x = 60*(color.b-color.r)/(max-min) + 120;
		}
		if ( max == color.b )
		{
			HSB.x = 60*(color.r-color.g)/(max-min) +240;
		}
		////
	}
	/// Define Saturation
	if ( max == 0 )
	{
		HSB.y = 0;
	}
	else
	{
		HSB.y = 1 - (min/max);
	}
	///
	
	/// Define  Brightness
	HSB.z = max;
	///

	return HSB;
}
/////
float DefineDensity(float4 terrain)
{	
	float  anglediaposon = angleHuemaxDelta;
	float3 HSBterrain = ConversionRGBtoHSB( terrain );
	float3 HSBbase = ConversionRGBtoHSB( g_densitycolorbase );	
	float density = 0;
	float angle = abs( HSBterrain.x - HSBbase.x );
	////
	if ( (angle > angleHuemaxDelta) || (abs(HSBterrain.y - HSBbase.y) > angleSaturationDelta) || (abs(HSBterrain.z - HSBbase.z) > angleBrightDelta) )
	{ 
		return density;
	}
	////	
	float kH = saturate( -(angle/anglediaposon) + 1 );
	float DifferenceSaturation = abs( HSBterrain.y - HSBbase.y );
	float kS = saturate( -DifferenceSaturation + 1 );
	float DifferenceBright = abs( HSBterrain.z - HSBbase.z );
	float kB = saturate( -DifferenceBright + 1 );
	density = pow(kH,powHue)*pow(kS,powSaturation)*pow(kB,powBright);
	return density;
}
///
float DensityDistance(float3 position, float layer)
{
	
	float powdis = g_powremovalgrass;
	float r1 =  radiusgrassdensity1;
	float r2 =  radiusgrassdensity2; 
	float dis = distance( g_CameraPosition , position );
	float density = 1;
	float z = (dis-r2)/(r2-r1);
	if ( (dis > r1) && (dis < r2) )
	{
		density = z*z; 
	}

	if ( layer > saturate(diaposon1) )
	{ density = 1; }

	if ( dis >= r2 )
	{ density = 0;}
	
	
	return density;
}
/////
float Mycos( float2 vec1, float2 vec2)
{
	float angle = 0;
	if ( vec2.y >= 0 )
	{
		angle = acos(dot(vec1,vec2));	
	}
	else
	{
		angle = 3.14159 + acos(dot(-vec1,vec2));
	}
	return angle;
}

float g_NumberBlockTrunk = 5;
float b                         /*     <string group="Деревья";        string name="Сосна"; float min=0; float max=30;> */  =  3;
float IndicatorDownloadTexture      /*  <string group="Деревья";      string name="индикатор загрузки текстуры";float min=0; float max=1; >*/    = 0;
struct DefineTreeCrown
{
	float Pos                   : POSITION; 
	float HeightTreeBranches1   : POSITION1;
	float HeightTreeBranches2   : POSITION2;
	float AngleBrunch           : POSITION3;
	float AngleSlopeBrunch      : POSITION4;
	float AngleSlopeBrunch2     : POSITION5;
	
	
};
///
float PI = 3.141593;
struct Tree
{
	float Height;                     //                               : POSITION;
	float RadiusTrunk                                                  : POSITION1; 
	float RadiusTip                                                    : POSITION2;
	float LenghtBranches1                                              : POSITION3;
	float AngleBrunch                                                  : POSITION4;
	float AngleSlopeBrunch                                             : POSITION5;
	float AngleSlopeBrunch2                                            : POSITION6;
	float LenghtBranches2                                              : POSITION7;
	float TypeCrown                                                    : POSITION8;
	float OffsetHeight                                                 : POSITION9;
	float PercentBaldTrunk                                             : POSITION10;
	float ScatterRadius                                                : POSITION11;
	float ScatterRadiusTip                                             : POSITION12;
	float AngleTrunk1                                                  : POSITION13;
	float AngleTrunk2                                                  : POSITION14;
	float AngleTrunk3                                                  : POSITION15;
	float NumberBranches                                               : POSITION16;
	float RadiusBrunch                                                 : POSITION17;
	float RadiusBrunchTip                                              : POSITION18;
	float DeltaAngleBrunch                                             : POSITION19;
	float WidthBranches2                                               : POSITION20;
	float PowDecreaseBranch                                            : POSITION21;
	float BendingStiffness                                             : POSITION22; 
	float DeltaBendingStiffness                                        : POSITION23; 
	float SwivelStiffness                                              : POSITION24; 
	float DeltaSwivelStiffness                                         : POSITION25;
	float OffsetBranch2Alpha1                                          : POSITION26;
	float OffsetBranch2Alpha2                                          : POSITION27;
	float DeltaAngleSlopeBrunch                                        : POSITION28;  
	float DeltaAngleSlopeBrunch2                                       : POSITION29;
	float Deltapercentbaldtrunk                                        : POSITION30;
	float IndicatorSlopeTrunk                                          : POSITION31;  
	float AngleTrunk4                                                  : POSITION32;
	float AngleTrunk5                                                  : POSITION33;
	float DiaposonTextureBranches1                                     : POSITION34; 
	float DiaposonTextureBranches2                                     : POSITION35; 
	float DiaposonTextureBranches3                                     : POSITION36; 
	float NumberBranches2                                              : POSITION37;	
	float HeightBase                                                   : POSITION38; 
	/////
	float Radius                                                       : POSITION39;
	float NumberTrunk                                                  : POSITION40;    
	float RadiusBase                                                   : POSITION41;   
	float RadiusTipBase                                                : POSITION42;   
	float alphaOY                                                      : POSITION43;
	float CountLeaves                                                  : POSITION44;
	float AngleLeavesOnBranch                                          : POSITION45;
	float DeltaAngleLeavesOnBranch                                     : POSITION46;
	float RandomPositionOnBranch                                       : POSITION47;
	float StepDecreaseLenghtLeaves                                     : POSITION48;
	float TexTrunku                                                    : POSITION49;
	float TexTrunkv                                                    : POSITION50;
	int   TypeTree                                                     : POSITION51;
	float TurnForceGravity                                             : POSITION52;
	float ResizeSprite                                                 : POSITION53;
	float AngleLightSprite1                                            : POSITION54;
	float AngleLightSprite2                                            : POSITION55;
	float LenghtBranchesOnTip                                          : POSITION56; 
	float ResizeNormalHorizSprite                                      : POSITION57; 
	float ResizeHorizSprite                                            : POSITION58; 
	float BezvetochnayaModel                                           : POSITION59;    
	float VidBush                                                      : POSITION60;  
	float PercentTrunkUnderGroud                                       : POSITION61;
	float ResizeHeightSprite                                           : POSITION62;
	float OffsetSprite                                                 : POSITION63;
	float HeightEdge                                                   : POSITION64;
	bool  TypeBushCenter;
};  
struct Plane
{
	float3 normal1;
	float3 normal2;
};
///
struct Brunch
{
	float3 pos1;
	float3 pos2;
	float3 pos3;
	float3 pos4;
	float3 posnach;
	float3 normal;
};
////
///
float randomsign(float noize)
{
	if ( noize > 0.5 )
	{ return 1; }
	else
	{ return -1;}
}
float3 VectorDirectVector(float3 vec, float3 direct, float alpha )
{
	vec = normalize(vec);
	direct = normalize(direct);
	float beta = PI - acos(dot(vec,direct));
	float gama = abs(PI - alpha - beta);
	float a = 0;
	if ( gama != 0 )
	{
		a = (sin(alpha))/sin(gama);
	}
	float3 avector = vec + abs(a)*direct;
	avector = normalize(avector);
	if ( velositywind == 0 )
	{ avector = vec; }
	//avector = vec;
	return avector;
}
////
float3 RandomBinormal(float3 Normal, float rand1, float rand2)
{
	float3 Binormal;
	if (Normal.z!=0)
	{
		Binormal.x = -1+rand1*2;
		Binormal.y = -1+rand2*2;
		Binormal.z = -(Normal.x*Binormal.x+Normal.y*Binormal.y)/Normal.z;
	}
	if (Normal.z==0)
	{
		if (Normal.x!=0)
		{
			Binormal.z = -1+rand1*2;
			Binormal.y = -1+rand2*2;
			Binormal.x = -(Normal.y*Binormal.y)/Normal.x;
		}
		if (Normal.x==0)
		{
			Binormal.z = -1+rand1*2;
			Binormal.x = -1+rand1*2;
			Binormal.y = 0;
		}
	}

	Binormal = normalize(Binormal);
	return Binormal;
}
float3 RandomBinormalAndAngleOY(float3 Normal, float rand1, float rand2, float alpha)
{
	float3 binormal =  RandomBinormal(Normal,rand1,rand2);
	float3 vreper = cross(binormal,Normal);
	float3 vbinormal = binormal;
	binormal.y = 0;
	/*
	for ( int i=0; i<=10; i++)
	{
		if ( acos(abs(dot(binormal,float3(0,1,0)))) < alpha )
		{
			binormal = VectorDirectVector(binormal,float3(0.001,-1,0),(0.09));
			
		}
		else
		{
			i = 30;
		}
	}
	*/
	binormal = VectorDirectVector(binormal,float3(0,1,0),alpha*rand1);
	//binormal = float3(1,0,1);
	return binormal;
}
///
////



////
float3 BinormalOXZ(float3 vec, float noize)
{
	float a,b = 1;
	if ( vec.z != 0 )
	{
		a = noize;
		b = -a*vec.x/vec.z;
	}
	else
	{
		a = 0;
		b = noize;
	}
	float3 binormal = normalize(float3(a,0,b));
	return binormal;
}
float3 Binormal(float3 Normal)
{
	float3 Binormal;
	if (Normal.z!=0)
	{
		Binormal.x = -1;
		Binormal.y = -1;
		Binormal.z = -(Normal.x*Binormal.x+Normal.y*Binormal.y)/Normal.z;
	}
	if (Normal.z==0)
	{
		if (Normal.x!=0)
		{
			Binormal.z = -1;
			Binormal.y = -1;
			Binormal.x = -(Normal.y*Binormal.y)/Normal.x;
		}
		if (Normal.x==0)
		{
			Binormal.z = -1;
			Binormal.x = -1;
			Binormal.y = 0;
		}
	}

	Binormal = normalize(Binormal);
	return Binormal;
}
////
float3 GeometryTrunk(float3 pos, float radius, float HeightTree, float alpha)
{
	float3 vec,posend = float3(0,0,0); 
	vec.x = radius*sin(alpha);
	vec.y = HeightTree;
	vec.z = radius*cos(alpha);
	posend = pos + vec;
	return posend;
}
///
float3 GeometryTrunkSlope(float3 pos, float radius, float HeightTree, float alpha, float OffsetTreeTunk, float4 noize)
{
	float3 vec,posend = float3(0,0,0); 
	float2 randomvec = normalize(float2(sin(2*PI*noize.x),sin(2*PI*noize.y)));
	pos.xz = pos.xz + OffsetTreeTunk*randomvec;
	vec.x = radius*sin(alpha);
	vec.y = HeightTree;
	vec.z = radius*cos(alpha);
	posend = pos + vec;
	return posend;
}
///
float3 GeometryTrunkSlope2(float3 pos, float radius,float radiustip, float HeightTree, float alpha,  float3 dir)
{
	float3 vec,posend = float3(0,0,0); 
	float3 poscenter = pos + dir*HeightTree;
	float radius2 = 0;
	if ( HeightTree == 0 )
	{ radius2 = radius;}
	else
	{ radius2 = radiustip;}
	vec.x = radius2*sin(alpha);
	vec.y = 0;
	vec.z = radius2*cos(alpha);

	posend = poscenter + vec;

	return posend;
}
///
float3 GeometryTreeBrunch(float3 pos, float radius, float alpha)
{
	float3 vec,posend = float3(0,0,0);
	vec.x = radius*sin(alpha);
	vec.y = 0;
	vec.z = radius*cos(alpha);
	posend = pos + radius*vec;

	return posend;
}
///
float3 GeometryTreeBrunchEnd(float3 pos, float3 dir, float radius, float LenghtBrunch, float alpha)
{
	float3 vec,posend = float3(0,0,0);
	float3 vbinormal = Binormal(dir);
	float3 vreper = normalize(cross(vbinormal,dir));
	float3 vecbrunch = vbinormal*sin(alpha)+ vreper*cos(alpha);
	vecbrunch = vecbrunch*radius;
	float3 posendbrunch = pos + dir*LenghtBrunch;
	vec.x = radius*sin(alpha);
	vec.y = 0;
	vec.z = radius*cos(alpha);
//	posend = posendbrunch + radius*vec;
	posend = posendbrunch + radius*vecbrunch;

	return posend;
}
///
///
float3 DirectBrunch(float4 density, float density2, Tree outTree, float LevelNoize, float BaseAngle)
{
	float3 directbrunch = float3(1,0,1); 
	
	float AngleBrunch = outTree.AngleBrunch;
	
	if ( outTree.TypeCrown >= 4.5 )
	{ AngleBrunch =  BaseAngle; }

	float angle = (AngleBrunch + sin(2*PI*density2)*outTree.DeltaAngleBrunch)*(1+LevelNoize*density.z);

	directbrunch.y = cos(angle);
	directbrunch.z = randomsign(density.x)*sqrt(1-density.z)*sin(angle);
	directbrunch.x = randomsign(density.y)*sqrt(density.z)*sin(angle);
	
	directbrunch = normalize(directbrunch);

	return directbrunch;

}
////

////
float3 DirectTrunk(float4 density, float density2, float angle,float LevelNoize)
{
	float3 directbrunch = float3(1,0,1); 
	angle = angle*(1+LevelNoize*density.z);
	directbrunch.y = cos(angle);
	directbrunch.z = randomsign(density.x)*sqrt(1-density.z)*sin(angle);
	directbrunch.x = randomsign(density.y)*sqrt(density.z)*sin(angle);
	directbrunch = normalize(directbrunch);

	return directbrunch;

}
///
float2 DefineTexcoordTree(float2 pos, float number, float i, float radius)
{
	float alpha = (2*PI)/number;
	float2 vec = float2(cos(i*alpha),sin(i*alpha));
	float2 posend = pos + radius*vec;
	return posend;
}
///
float3 SlopeLine(float3 vector1, float alpha, float delta, float noize)
{
	float beta = alpha + delta*sin(2*PI*noize);
	float3 vec1 = normalize( cross(vector1,float3(0,1,0)) );
	float3 vec2 = normalize( cross(vector1, vec1) );
	if ( vec2.y < 0 )
	{ vec2 = - vec2; }
	if ( vector1.x == 0 && vector1.y == 1 && vector1.z == 0 )
	{ 
		//vec1 = normalize(float3(sin(2*PI*noize),sin(PI*noize),cos(2*PI*noize))); 
		vec2 = normalize(float3(sin(2*PI*noize),0,cos(2*PI*noize))); 
	}
	float3 vectorend = float3(0,0,0);
	vectorend = cos(beta)*vector1 + sin(beta)*vec2;
	return vectorend;

}
////
float3 SlopeLineGravity(float3 vector1, float alpha, float delta, float noize, float proportion, float indicator)
{
	float beta = 0;

	if ( indicator < 0.5 )
	{ beta = (alpha + delta*sin(2*PI*noize))*1; }
	else
	{ beta = (alpha + delta*sin(2*PI*noize))*proportion; }

	float3 vec1 = normalize( cross(vector1,float3(0,1,0)) );
	float3 vec2 = normalize( cross(vector1, vec1) );
	if ( vec2.y < 0 )
	{ vec2 = - vec2; }
	if ( vector1.x == 0 && vector1.y == 1 && vector1.z == 0 )
	{ 
		//vec1 = normalize(float3(sin(2*PI*noize),sin(PI*noize),cos(2*PI*noize))); 
		vec2 = normalize(float3(sin(2*PI*noize),0,cos(2*PI*noize))); 
	}
	float3 vectorend = float3(0,0,0);
	vectorend = cos(beta)*vector1 + sin(beta)*vec2;
	return vectorend;

}
////
float3 DefinePositionBrunch2(float3 pos0, float3 dir, float height, float noize)
{
	float3 pos1 = pos0 + dir*height;
	float3 pos = pos0*noize + (1-noize)*pos1;
	return pos;
}
////////
float3 DefinePositionBrunchOnSlopeTrunk2(float3 pos0, float3 pos1,float3 pos2,float3 pos3,float3 pos4, float3 dir1, float3 dir2, float3 dir3,float3 dir4, float3 dir5, float3 position, float height)
{
	float lenght = 0;
	float3 posbrunch = float3(0,0,0);
	float lenghtblock = 1/g_NumberBlockTrunk;
	if ( (position.y > pos0.y) && ( position.y < (pos0.y + height*lenghtblock) )	)
	{
		lenght = position.y - pos0.y;
		posbrunch = pos0 + dir1*lenght;
	}
	if ( ( position.y > (pos0.y + height*lenghtblock) ) && ( position.y < (pos0.y + height*2*lenghtblock) )	)
	{
		lenght = position.y - pos0.y - height*lenghtblock;
		posbrunch = pos1 + dir2*lenght;
	}
	if ( ( position.y > (pos0.y + height*2*lenghtblock) ) && ( position.y < (pos0.y + height+3*lenghtblock) ) )	
	{
		lenght = position.y - pos0.y - height*2*lenghtblock;
		posbrunch = pos2 + dir3*lenght;
	}
	if ( ( position.y > (pos0.y + height*3*lenghtblock) ) && ( position.y < (pos0.y + height+4*lenghtblock) ) )	
	{
		lenght = position.y - pos0.y - height*3*lenghtblock;
		posbrunch = pos3 + dir4*lenght;
	}
	if ( ( position.y > (pos0.y + height*4*lenghtblock) ) && ( position.y < (pos0.y + height+5*lenghtblock) ) )	
	{
		lenght = position.y - pos0.y - height*4*lenghtblock;
		posbrunch = pos4 + dir5*lenght;
	}
	return posbrunch;
}
////
////////
float4 DefinePositionBrunchOnSlopeTrunk3(float3 pos0, float3 pos1,float3 pos2,float3 pos3,float3 pos4, float3 dir1, float3 dir2, float3 dir3,float3 dir4, float3 dir5, float3 position, float height)
{
	float lenght = 0;
	float3 posbrunch = float3(0,0,0);
	float  proportion = 1;
	float lenghtblock = 1/g_NumberBlockTrunk;
	if ( (position.y > pos0.y) && ( position.y < (pos0.y + height*lenghtblock) )	)
	{
		lenght = position.y - pos0.y;
		posbrunch = pos0 + dir1*lenght;
		proportion = lenght/height;
	}
	if ( ( position.y > (pos0.y + height*lenghtblock) ) && ( position.y < (pos0.y + height*2*lenghtblock) )	)
	{
		lenght = position.y - pos0.y - height*lenghtblock;
		posbrunch = pos1 + dir2*lenght;
		proportion = (lenght/height) + 0.2;
	}
	if ( ( position.y > (pos0.y + height*2*lenghtblock) ) && ( position.y < (pos0.y + height+3*lenghtblock) ) )	
	{
		lenght = position.y - pos0.y - height*2*lenghtblock;
		posbrunch = pos2 + dir3*lenght;
		proportion = (lenght/height) + 0.4;
	}
	if ( ( position.y > (pos0.y + height*3*lenghtblock) ) && ( position.y < (pos0.y + height+4*lenghtblock) ) )	
	{
		lenght = position.y - pos0.y - height*3*lenghtblock;
		posbrunch = pos3 + dir4*lenght;
		proportion = (lenght/height) + 0.6;
	}
	if ( ( position.y > (pos0.y + height*4*lenghtblock) ) && ( position.y < (pos0.y + height+5*lenghtblock) ) )	
	{
		lenght = position.y - pos0.y - height*4*lenghtblock;
		posbrunch = pos4 + dir5*lenght;
		proportion = (lenght/height) + 0.8;
	}
	return float4(posbrunch.x,posbrunch.y,posbrunch.z,proportion);
}
////

////
float LenghtSheraCrown(float R, float L, float3 dir,float ind, float3 center, float3 pos)
{
	float alpha = 0;//acos(abs(dot(dir,float3(0,1,0))));
	float beta = 0;
	float gama = 0 ;
	if ( ind > 0 && dir.y > 0 )
	{ acos(-abs(dot(dir,float3(0,1,0)))); }
	if ( ind > 0 && dir.y < 0 )
	{ acos(abs(dot(dir,float3(0,1,0)))); }
	if ( ind < 0 && dir.y > 0 )
	{ acos(abs(dot(dir,float3(0,1,0)))); }
	if ( ind < 0 && dir.y < 0 )
	{ acos(-abs(dot(dir,float3(0,1,0)))); }
	beta = asin( ((L/R)*sin(alpha)) );
	gama = abs(PI - alpha - beta);
	if ( gama > PI*0.5 )
	{
		alpha = PI - alpha;
		beta = asin( ((L/R)*sin(alpha)) );
		gama = abs(PI - alpha - beta);
	}
	float lenght = sqrt( R*R + L*L - 2*R*L*cos(gama) );
	if ( lenght > R )
	{
		lenght = sqrt( R*R + L*L + 2*R*L*cos(gama) );
	}
	float3 posend = pos + lenght*dir;
	float dis = distance(posend,center);
	float3 vec = posend - pos;
	for ( int i = 0; i<10; i++)
	{
	if ( dis > R )
	{
		lenght = lenght *0.9;
		posend = pos + lenght*dir;
		dis = distance(posend,center);
	}
	}
	return lenght;
}
///
DefineTreeCrown DefineCrown(float3 root, float HeightTree, float4 noize, float3 dir, Tree outTree , float count)
{
	DefineTreeCrown Tree ;
	float pos,coeff, anglebrunch, RadiusCrown , angleslopebrunch, angleslopebrunch2,OffsetRadiusCenter,RadiusCenter= 1;
	float percentbald = abs( outTree.PercentBaldTrunk + outTree.Deltapercentbaldtrunk*sin(2*PI*noize.z) );
	float LenghtBrunch = outTree.LenghtBranches1;
	float LenghtBrunch2 = outTree.LenghtBranches2;
	
	pos = root.y + percentbald*HeightTree + noize.g*HeightTree*(1-percentbald);
//	pos = root.y + percentbald*HeightTree + (count/outTree.NumberBranches)*HeightTree*(1-percentbald);
	//float a = 1-((pos-root.y)/HeightTree);
	//float a = 1 - noize.g*(1-percentbald);
	float a = 1 - noize.g;   ////!!!!
//	a = 1 - (count/outTree.NumberBranches);

	if ( outTree.PowDecreaseBranch < 0)
	{
		a = 1 - a;
		outTree.PowDecreaseBranch = abs((outTree.PowDecreaseBranch));
	}

	if ( outTree.TypeTree == 1 )   /// = 1 для сосны
	{
		//float b = 0.8;
		pos = root.y + percentbald*HeightTree + ((count+b)/outTree.NumberBranches)*HeightTree*(1-percentbald);
		a = 1 - (( count+b)/outTree.NumberBranches);
		//if ( noize.z < 0.8)
		//{pos = root.y + noize.g*HeightTree*b*(1-percentbald) + percentbald*HeightTree;}
		//else 
		//{pos = root.y + noize.g*HeightTree*(1-b)*(1-percentbald) + percentbald*HeightTree+ HeightTree*b*(1-percentbald);}
		//a = 1- ( (pos - root.y - percentbald*HeightTree)/( HeightTree - percentbald*HeightTree) );
	}	

	if ( outTree.TypeCrown < 0.5 )
	{ 
		coeff = pow(a,outTree.PowDecreaseBranch)*LenghtBrunch;
		LenghtBrunch2 = LenghtBrunch2*(1-((pos-root)/HeightTree));
		anglebrunch = outTree.AngleBrunch;
		angleslopebrunch = outTree.AngleSlopeBrunch;
		angleslopebrunch2 = outTree.AngleSlopeBrunch2;
	}
	if ( outTree.TypeCrown >= 0.5 && outTree.TypeCrown < 1.5 )
	{ 
		coeff = LenghtBrunch; 
		anglebrunch = outTree.AngleBrunch + PI*0.5;
		angleslopebrunch = outTree.AngleSlopeBrunch;
		angleslopebrunch2 = outTree.AngleSlopeBrunch2;
	}
	if ( outTree.TypeCrown >= 1.5 && outTree.TypeCrown < 2.5 )
	{ 
		coeff = pow(a,outTree.PowDecreaseBranch)*LenghtBrunch; 
		LenghtBrunch2 = LenghtBrunch2*(1-((pos-root)/HeightTree));
		anglebrunch = outTree.AngleBrunch + PI*0.5;
		angleslopebrunch = outTree.AngleSlopeBrunch;
		angleslopebrunch2 = outTree.AngleSlopeBrunch2;
	}
	if ( outTree.TypeCrown >= 2.5 && outTree.TypeCrown < 3.5)
	{
		coeff = LenghtBrunch;
		anglebrunch = outTree.AngleBrunch;
		angleslopebrunch = outTree.AngleSlopeBrunch;
		angleslopebrunch2 = outTree.AngleSlopeBrunch2;
	}	
	if ( outTree.TypeCrown >= 3.5 && outTree.TypeCrown <= 4.5)
	{
		
		RadiusCrown = HeightTree*(1-percentbald)*0.5;
		RadiusCenter = RadiusCrown + percentbald*HeightTree+root.y;
		OffsetRadiusCenter = abs(RadiusCenter - pos); 
		float3 center = float3(root.x,(percentbald*HeightTree+root.y+RadiusCrown),root.z);
		float3 posb = float3(root.x,pos,root.z);
		coeff = sqrt(RadiusCrown*RadiusCrown - OffsetRadiusCenter*OffsetRadiusCenter);
		//coeff = LenghtSheraCrown(RadiusCrown,OffsetRadiusCenter,dir,(pos-RadiusCenter),center,posb);
		anglebrunch = PI*0.5;
		angleslopebrunch = 0;
		angleslopebrunch2 = 0;
		//anglebrunch = outTree.AngleBrunch;
		LenghtBrunch2 = coeff;
	}	
	////
	if ( outTree.TypeCrown >= 4.5)
	{
		anglebrunch = outTree.AngleBrunch*a;	
		angleslopebrunch = angleslopebrunch*a;
		angleslopebrunch2 = angleslopebrunch2*a;
		coeff = outTree.LenghtBranches1*pow(a,outTree.PowDecreaseBranch) + outTree.LenghtBranchesOnTip*(1-pow(a,outTree.PowDecreaseBranch));
		LenghtBrunch2 = pow(a,outTree.PowDecreaseBranch)*LenghtBrunch2;
	}	
	/////
	
	Tree.Pos = pos;
	Tree.HeightTreeBranches1 = coeff;
	Tree.AngleBrunch = anglebrunch;
	Tree.AngleSlopeBrunch = angleslopebrunch;
	Tree.AngleSlopeBrunch2 = angleslopebrunch2;
	Tree.HeightTreeBranches2 = LenghtBrunch2;

	return Tree;
}
////
PS_Tree DefineTreeVertex(float4 Position, float3 Texcoord, float3 Normal, float3 P, float2 alpha)
{
	PS_Tree vertex;

	vertex.Pos = Position;
	vertex.Tex = Texcoord;
	vertex.Normal = Normal;
	vertex.P = P;
	vertex.alpha = alpha;

	return vertex;
}
////

float SpriteDefineDirection(float3  vTreePos)
{
	float3 DirectCameraObject = normalize((vTreePos - g_CameraPosition));
	//float3 binormal = normalize( cross( float3(0,1,0), g_CameraDirection ) );
	float3 binormal = normalize( cross( float3(0,1,0), DirectCameraObject ) );	
	binormal.y = 0;
	float  angle = Mycos(float2(1,0),binormal.xz);
	return angle; 

}
///

float3 AxisTrunk(float3 vPos,float3 dir, float Height )
{
	
        float3 vertexPos = float3(0,0,0);                          
       	vertexPos = vPos + dir*Height;
	return vertexPos;

}
///

float3 KarkasTrunk(float3 vTreePos, float Radius )
{
	float3 vertexPos;
	vertexPos = vTreePos;
	float angle  = SpriteDefineDirection(vTreePos );
	float width  = Radius;
	///
	vertexPos.x = vertexPos.x + width*cos(angle);           // на камеру направление
	vertexPos.z = vertexPos.z + width*sin(angle);
	
	return vertexPos;

}
////
float3 TurnInPlane(float3 dir1, float3 dir2, float3 direct, float alpha)
{
	float3 dir3 = normalize(cross(dir1,dir2));
	if ( dot(dir3,direct) < 0 )
	{ dir3 = - dir3; }
	float3 vec = dir2*cos(alpha) + dir3*sin(alpha);
	vec = normalize(vec);
	return vec;

}

float2 AngleMaxTurnBranches(float3 dir, float3 dir2, Tree outTree, float4 noize )
{
	float3 normal = normalize(cross(dir,dir2));
	float3 wind1 = normalize(float3(wind.x,wind.y,wind.z));
	float n = dot(normal,wind1);
	//n = 1;
	float F = 1.14*velositywind*velositywind*n*n;
	float F2 = 1.14*velositywind*velositywind*(1 - n*n);
	float alpha,beta = 0;

	float BendingStiffness = abs(outTree.BendingStiffness + outTree.DeltaBendingStiffness*sin(2*PI*noize.x));
	float SwivelStiffness = outTree.SwivelStiffness + outTree.DeltaSwivelStiffness*sin(2*PI*noize.y);

	alpha = asin(saturate(F/BendingStiffness));
	beta = asin(saturate(F2/SwivelStiffness));

	if( velositywind == 0 ) 
	{
		alpha = 0;
		beta = 0;
	}
	if ( beta > alpha )
	{ beta = alpha; }

	return float2(alpha,beta);
}
///
Plane DefineVectorBendingWind2(float3 dir, float3 dir2,float alpha, float beta)
{
	Plane list;
	float3 vwind = normalize(float3(wind.x,wind.y,wind.z));
	
	float3 dirtime = VectorDirectVector(dir,vwind,alpha);
	//dir2 = float3(1,0,1);
	float3 dir2time = dir2;
	float3 a = cross(dirtime,vwind);
	float anglewindanddir2 = acos(abs(dot(vwind,dir2)));
	if (  (dot(a,a) != 0 ) && (velositywind != 0) )   
	{ dir2time = dir2; }
	
	dir2time = TurnInPlane( dirtime, dir2time, vwind , beta );
	if ( dot(float3(wind.x,wind.y,wind.z),float3(wind.x,wind.y,wind.z)) == 0 )
	{ 	
		dirtime = dir;
		dir2time = dir2;
	}	
	
	list.normal1 = dirtime;
	list.normal2 = dir2time;
	//SlopeLine(float3 vector1, float alpha, float delta, float noize)
	return list;
}
///
Brunch DefineBrunch(float3 posnach, float3 normal,float3 binormal, float height, float width, float alpha )
{
	Brunch plane;

	float3 normal2 = SlopeLine(normal,alpha,0,0);
	float3 posend = posnach + normal2*height;

	plane.pos1 = posnach - binormal*width*0.5;
	plane.pos2 = posnach + binormal*width*0.5;
	plane.pos3 = posend - binormal*width*0.5;
	plane.pos4 = posend + binormal*width*0.5;  
	plane.posnach = posend;
	plane.normal = normal2;

	return plane;  	
}
///
float2 XTexTreeCoordinate(float layer)
{
	float2 tex = float2(0,1);
	if ( IndicatorDownloadTexture > 1.5 )
	{
		if ( layer < 1.9 )
		{ tex = float2(0,0.2); }
		if ( layer >=2 && layer < 3 )
		{ tex = float2(0.2,0.4); }
		if ( layer >=3 && layer < 4 )
		{ tex = float2(0.4,0.6); }
		if ( layer >=4 && layer < 5 )
		{ tex = float2(0.6,0.8); }
		if ( layer >=5.1 )
		{ tex = float2(0.8,0.99); }
	}

	return tex;
}
///
float2 DefineRadiusTrunkBlock(float layer, float radius, float radiustip)
{
	float radiusblock = radius;
	float radiusblocktip = radiustip;
	float NumberBlock = 3;
	float a = 1/g_NumberBlockTrunk;
	if ( layer == 0 )
	{
		radiusblock    = radius;
		radiusblocktip = radius*(1-a)+radiustip*a;
	}
	if ( layer == -1 )
	{
		radiusblock = radius*(1-a)+radiustip*a;
		radiusblocktip = radius*(1-2*a)+radiustip*2*a;
	}
	if ( layer == -2 )
	{
		radiusblock = radius*(1-2*a)+radiustip*2*a;
		radiusblocktip = radius*(1-3*a)+radiustip*3*a;
	}	
	if ( layer == -3 )
	{
		radiusblock = radius*(1-3*a)+radiustip*3*a;
		radiusblocktip = radius*(1-4*a)+radiustip*4*a;
	}	
	if ( layer == -4 )
	{
		radiusblock = radius*(1-4*a)+radiustip*4*a;
		radiusblocktip = radiustip;
	}	
	
	return float2(radiusblock,radiusblocktip);
}
///
float DefineTextureBranches(float noize, float diaposon1, float diaposon2, float diaposon3 )
{
	float layer = 2;
	if ( noize < saturate(diaposon1) )
	{ layer = 2.5; }
	if ( noize >= saturate(diaposon1) &&  noize < saturate(diaposon1+diaposon2) )
	{ layer = 3.5; }
	if ( noize >= saturate(diaposon1+diaposon2) &&  noize < saturate(diaposon1+diaposon2+diaposon3) )
	{ layer = 4.5; }
	////
	if ( diaposon1 == 0 && diaposon2 == 0 )
	{ layer = 4.5; }
	if ( diaposon1 == 0 && diaposon3 == 0 )
	{ layer = 3.5; }
	if ( diaposon3 == 0 && diaposon2 == 0 )
	{ layer = 2.5; }
	return layer;
}
/////
float3 DefineDirectTrunkBlock(float IndicatorSlopeTrunk,float alpha,float3 dirprevblock, float4 noize, float LevelNoize )
{
	float3 dir = float3(0,1,0);
	if ( IndicatorSlopeTrunk < 0.5)
	{ 
		dir = DirectTrunk(noize,noize.y,alpha,LevelNoize );
	}
	else
	{
		dir = SlopeLine(dirprevblock, alpha, 0, noize.x);
	}
	return dir;
}
////
float3 NormalDependCameraOnlyTree(float3 pos1, float3 pos2, float3 pos3, float3 pos_camera, float l_AngleBlackOut)
{
	
	
	float alphamax = l_AngleBlackOut;
	float alphamin,alpha1,alpha2,alpha3,alpha = 0;
	float3 normal = float3(0,0,0);
	normal = normalize( cross( (pos3 - pos1), (pos2-pos1) ) );
	if ( dot( (pos1-pos_camera),normal) > 0 )
	{
		normal = -normal;	
	}
	///
	alpha1 = acos( abs( dot(normal,normalize( cross( (pos_camera-pos1),(pos_camera-pos2) ) ) ) ) );
	alpha2 = acos( abs( dot(normal,normalize( cross( (pos_camera-pos1),(pos_camera-pos3) ) ) ) ) );
	alpha3 = acos( abs( dot(normal,normalize( cross( (pos_camera-pos3),(pos_camera-pos2) ) ) ) ) );
	if ( (alpha1 <= alpha2) && (alpha1 <= alpha3) )
	{
		alphamin = alpha1;
	}
	else
	{
		if (  (alpha2 <= alpha3) )
		{
			alphamin = alpha2;
		}
		else
		{
			alphamin = alpha3;
		}
	}
	
	alpha = alphamin;
	if ( alpha < alphamax && alpha > 0 )
	{
		normal.xyz = normal.xyz*pow( (alpha/alphamax), 1);
	}
	///
	
	return normal;
	
}
////
float3 DefineDirectLeaves(float3 dir, int i, float4 noize, float alpha, float alphaleaves, float deltaalphaleaves, float2 tex )
{
	
	//float2 TexCoord = DefineTexcoordTree(float2(noize.z,noize.z),10,i,0.1);
	float2 TexCoord = DefineTexcoordTree(tex,10,i,0.1);
	float4 density = Noise.SampleLevel( samLinear,TexCoord, 0);
	float3 dirleaves = dir;
	//float3 dirleaves = float3(0,0,0);
	//float3 dirleaves2 = RandomBinormal(dir,noize.x,noize.y);
	float3 dirleaves2 = RandomBinormalAndAngleOY( dir, noize.x,noize.y, alpha);
	//alphaleaves = 1;
	alphaleaves = alphaleaves*( 1 + sin(2*PI*density.x)*deltaalphaleaves);
	if ( alphaleaves < 0 )
	{ alphaleaves = 0; }
	if ( alphaleaves > 1.56 )
	{ alphaleaves = 1.56 ; }

	float beta = PI - alphaleaves;
	float3 normal1 = dirleaves2*cos(alphaleaves) + dirleaves*sin(alphaleaves);	
	float3 normal2 = dirleaves2*cos(beta) + dirleaves*sin(beta);	
	float3 outdir = dir;
	if ( i == 0 )
	{ dirleaves = dir; }
	if ( i == 1 || i == 3 || i == 5 || i == 7 || i == 9 || i == 11)
	{ outdir = normal1; }
	if ( i == 2 || i == 4  || i == 6 || i == 8 || i == 10 || i == 12 )
	{ outdir = normal2; }
	return outdir;

}
////
float3 DefinePosituonLeaves(float3 pos, float3 dir, float lenght, int i, float4 noize, float count, float RandomPositionOnBranch, float2 tex)
{

	//outTree.RandomPositionOnBranch 
	float3 posleaves = float3(0,0,0);
	float a = 0.5;
	//float2 TexCoord = DefineTexcoordTree(float2(noize.x,noize.y),10,i,0.1);
	float2 TexCoord = DefineTexcoordTree(tex,10,i,0.1);
	float4 noizeposition = Noise.SampleLevel( samLinear,TexCoord, 0);
	//float count  = 6;
	if ( RandomPositionOnBranch < 0.5 )
	{ noizeposition.x = 1; } 
	if ( count >= 1 && count < 3 )
	{ a = 1;}
	if ( count >= 3 && count < 5 )
	{ a = 0.5;}
	if ( count >= 5 && count < 7 )
	{ a = 0.333;}
	if ( count >= 7 && count < 9 )
	{ a = 0.25;}
	if ( count >= 9 && count < 11 )
	{ a = 0.2;}
	if ( count >= 11 )
	{ a = 0.166;}
	
	if ( i == 0 )
	{ posleaves = pos + dir*lenght*0.9; }
	if ( i == 1 || i == 2 )
	{
		posleaves = pos + dir*lenght*a*noizeposition.x;
	}
	
	if ( i == 3 || i == 4 )
	{
		posleaves = pos + dir*lenght*a*(1+noizeposition.x);
	}
	
	if ( i == 5 || i == 6 )
	{
		posleaves = pos + dir*lenght*a*(2+noizeposition.x);
	}
	if ( i == 7 || i == 8 )
	{
		posleaves = pos + dir*lenght*a*(3+noizeposition.x);
	}
	if ( i == 9 || i == 10 )
	{
		posleaves = pos + dir*lenght*a*(4+noizeposition.x);
	}
	if ( i == 11 || i == 12 )
	{
		posleaves = pos + dir*lenght*a*(5+noizeposition.x);
	}
	return posleaves;
}
///
float CoeffDecreaseLenghtLeaves(float3 posleaves, float3 posbrunch, float lenght, float layer, float StepDecreaseLenghtLeaves)
{
	float d = abs(distance(posleaves, posbrunch));
	float c = 0;
	float i = 1;
	if ( layer == 2.1 )
	{ c = saturate(d/lenght); }
	if ( layer == 2.2 )
	{ c = saturate((d/lenght) + 0.333); }
	if ( layer > 2.21)
	{ c = saturate((d/lenght) + 0.666);}
	
	float power = abs(StepDecreaseLenghtLeaves);
	
	if ( StepDecreaseLenghtLeaves == 0)
	{i = 1;}

	float x = pow((-c+1),power);
	if ( StepDecreaseLenghtLeaves > 0 )
	{ i = x; }

	if ( StepDecreaseLenghtLeaves < 0 )
	{ 
		i = pow((1-abs(c-1)),power);
		//i = -x + 1;
		
	}
	

	return i;
}
////
float2 TexOYTrunk(float layer)
{
	float2 tex = float2(0,1);
	if ( layer == 0 )
	{ tex = float2(0,0.2); }
	if ( layer == -1 )
	{ tex = float2(0.2,0.4); }
	if ( layer == -2 )
	{ tex = float2(0.4,0.6); }
	if ( layer == -3 )
	{ tex = float2(0.6,0.8); }
	if ( layer == -4 )
	{ tex = float2(0.8,1); }

	return tex;
}

////
float3 NormalDependCameraTree(float3 pos1, float3 pos2, float3 pos3, float3 pos_camera)
{
	
	float3 normal = float3(0,0,0);
	normal = -normalize( cross( (pos3 - pos1), (pos2-pos1) ) );
	if ( dot( (pos1-pos_camera),normal) > 0 )
	{
		normal = -normal;	
	}
		
	return normal;
	
}
////
////
float3 NormalLeaves(float3 pos1, float3 pos2, float3 pos3, float3 pos_camera)
{
	
	float3 normal = float3(0,0,0);
	normal =  normalize( cross( (pos3 - pos1), (pos2-pos1) ) );
	if ( dot( (pos1-pos_camera),normal) > 0 )
	{
		normal = -normal;	
	}
		
	return normal;
	
}
float3 NormalSpriteTree(float3 pos1, float3 pos2, float3 pos3, float3 pos_camera, Tree outTree)
{
	
	float3 normal = float3(0,0,0);
	float alpha1 = outTree.AngleLightSprite1;
	float alpha2 = outTree.AngleLightSprite2;
	float coeff = 0.5;
	normal = -normalize( cross( (pos3 - pos1), (pos2-pos1) ) );
	if ( dot( (pos1-pos_camera),normal) > 0 )
	{
		normal = -normal;	
	}
	
	float3 LightDir = normalize(g_SunDirection.xyz);
	
	coeff = (dot(normal,LightDir) + 1)*0.5;
	float3 BinormalLightDir = Binormal(LightDir);
	float beta = alpha2*coeff + (1-coeff)*alpha1; 	
	normal = -( LightDir*cos(beta) + BinormalLightDir*sin(beta) );
	
	return -LightDir;
	
}
float MaxAngleOffset(float velositywind, float stiffness, float Width, float Tip )
{
	//float teta = asin( saturate( (0.7*velositywind*velositywind*(Width+Tip)/stiffness) ) );
	float teta = asin( saturate( (0.7*velositywind*velositywind/stiffness) ) );
	return teta;
}
////
////
float SquareTriangleGeron(float3 pos1, float3 pos2, float3 pos3)
{
	float a = distance(pos1,pos2);
	float b = distance(pos1,pos3);
	float c = distance(pos3,pos2);
	float p = (a + b + c)*0.5;
	float S = sqrt(p*(p-a)*(p-b)*(p-c));
	///
	
	return S;
}
///
float Lenghtvector(float3 vec)
{
	float lenght;
	lenght = sqrt(vec.x*vec.x+vec.y*vec.y+vec.z*vec.z);
	return lenght;
}
///////// вычисляет угол, в основании которого находится первая вершина
float AngleVector(float3 vVertex0pos, float3 vVertex1pos, float3 vVertex2pos)
{
	float3 vector1 = vVertex1pos-vVertex0pos;
	float3 vector2 = vVertex2pos-vVertex0pos;
	float scalar = vector1.x*vector2.x+vector1.y*vector2.y+vector1.z*vector2.z;
	float angle = acos(scalar/(Lenghtvector(vector1)*Lenghtvector(vector2)));
	return angle;
}
/////////// определяет лежит ли точка внутри треугольника заданного тремя вершинами, методом проверки углов 
bool PointInTriangle2(float3 vpoint, float3 vVertex0pos, float3 vVertex1pos, float3 vVertex2pos)
{  
	int flag = 0;
	if (AngleVector(vVertex0pos,vpoint,vVertex2pos)<=AngleVector(vVertex0pos,vVertex1pos,vVertex2pos))
	{flag = flag+1;}
	if (AngleVector(vVertex0pos,vpoint,vVertex1pos)<=AngleVector(vVertex0pos,vVertex1pos,vVertex2pos))
	{flag = flag+1;}
	if (AngleVector(vVertex1pos,vpoint,vVertex2pos)<=AngleVector(vVertex1pos,vVertex2pos,vVertex0pos))
	{flag = flag+1;}
	if (AngleVector(vVertex1pos,vpoint,vVertex0pos)<=AngleVector(vVertex1pos,vVertex2pos,vVertex0pos))
	{flag = flag+1;}
	if (AngleVector(vVertex2pos,vpoint,vVertex1pos)<=AngleVector(vVertex2pos,vVertex1pos,vVertex0pos))
	{flag = flag+1;}
	if (AngleVector(vVertex2pos,vpoint,vVertex0pos)<=AngleVector(vVertex2pos,vVertex1pos,vVertex0pos))
	{flag = flag+1;}
	if ( flag == 6)
		return true;
	else return false;
}
///
float3 PositionRandomInTiangle4(float3 pos, float3 pos1, float3 pos2, float3 pos3, float4 noize)
{
	float3 vec1 = pos3 - pos1;
	float3 vec2 = pos2 - pos1;
	float3 vpos = pos;
	float a = noize.z;
	float b = (1 - a)*noize.y;
	vpos = pos1 + a*vec1 + b*vec2;	

	return vpos;
}
float2 TexRandomInTiangle4(float2 tex1, float2 tex2, float2 tex3, float4 noize)
{
	float2 vec1 = tex3 - tex1;
	float2 vec2 = tex2 - tex1;
	float2 vtex = tex1;
	float a = noize.z;
	float b = (1 - a)*noize.y;
	vtex = tex1 + a*vec1 + b*vec2;	

	return vtex;
}






cbuffer HeightfieldSettings  : register(b0)
{
	float	fMinLattitude;				// минимальная широта
	float	fMaxLattitude;				// максимальная широта

	float	fMinLongitude;				// минимальная долгота
	float	fMaxLongitude;				// максимальная долгота

	uint	nCountLat;					// количество точек по широте
	uint	nCountLong;					// количество точек по долготе
	
	float	fLongitudeCoeff;			// максимальная текстурная координата по долготе
	float	fLattitudeCoeff;			// максимальная текстурная координата по широте

	float	fWorldScale;				// Масштаб мира
	float	fHeightScale;				// Масштаб высоты

	float	fNormalDivisionAngleCos1;	// Косинус минимального угла разделения нормалей
	float	fNormalDivisionAngleCos2;	// Между этими 2мя значениями происходит интерполяция

	// Cut coeffs
	float	fNorthBlockLongCoeff;
	float	fNorthBlockLatCoeff;

	float	fNorthEastBlockLongCoeff;
	float	fNorthEastBlockLatCoeff;

	float	fEastBlockLongCoeff;
	float	fEastBlockLatCoeff;

	float	fSouthEastBlockLongCoeff;
	float	fSouthEastBlockLatCoeff;

	float	fSouthBlockLongCoeff;
	float	fSouthBlockLatCoeff;

	float	fSouthWestBlockLongCoeff;
	float	fSouthWestBlockLatCoeff;

	float	fWestBlockLongCoeff;
	float	fWestBlockLatCoeff;

	float	fNorthWestBlockLongCoeff;
	float	fNorthWestBlockLatCoeff;

	// 0
	float	fNorthMinLat;
	float	fNorthMaxLat;

	float	fNorthMinLong;
	float	fNorthMaxLong;

	// 1
	float	fNorthEastMinLat;
	float	fNorthEastMaxLat;

	float	fNorthEastMinLong;
	float	fNorthEastMaxLong;

	//2

	float	fEastMinLat;
	float	fEastMaxLat;

	float	fEastMinLong;
	float	fEastMaxLong;

	//3
	float	fSouthEastMinLat;
	float	fSouthEastMaxLat;

	float	fSouthEastMinLong;
	float	fSouthEastMaxLong;

	//4
	float	fSouthMinLat;
	float	fSouthMaxLat;

	float	fSouthMinLong;
	float	fSouthMaxLong;

	//5
	float	fSouthWestMinLat;
	float	fSouthWestMaxLat;

	float	fSouthWestMinLong;
	float	fSouthWestMaxLong;

	//6
	float	fWestMinLat;
	float	fWestMaxLat;

	float	fWestMinLong;
	float	fWestMaxLong;

	//7
	float	fNorthWestMinLat;
	float	fNorthWestMaxLat;

	float	fNorthWestMinLong;
	float	fNorthWestMaxLong;
};



Texture2D				InputHeightTexture				: register(t0);

Texture2D				NorthNeighbourTexture			: register(t1);		// север
Texture2D				NorthEastNeighbourTexture		: register(t2);
Texture2D				EastNeighbourTexture			: register(t3);
Texture2D				SouthEastNeighbourTexture		: register(t4);
Texture2D				SouthNeighbourTexture			: register(t5);
Texture2D				SouthWestNeighbourTexture		: register(t6);
Texture2D				WestNeighbourTexture			: register(t7);
Texture2D				NorthWestNeighbourTexture		: register(t8);


RWByteAddressBuffer 	OutVertexBuffer 		: register(u0);
RWByteAddressBuffer 	OutIndexBuffer 			: register(u1);


struct Vertex
{
	float3	pos;
	float3	normal;
	float2	tex;
	float3	tangent;
};

struct GeoCoord
{
	float lat;
	float long;
};

struct GeoIndex
{
	int lat;
	int long;
};

GeoIndex MakeGeoIndex( int lat, int long )
{
	GeoIndex geoIndex;
	geoIndex.lat  = lat;
	geoIndex.long = long;
	return geoIndex;
}

SamplerState HeightTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Mirror;
	AddressV = Mirror;
};

double lerp_double(double a, double b, double k)
{
	return a + (b - a)*k;
}

#define ONE_UNIT_IN_RAD 0.00017453292519943295

void sincos_double(double in_x, out double sine, out double cosine)
{
	double x = in_x;
	
	double deg = x / ONE_UNIT_IN_RAD;
	double nDeg1 = floor(deg);
	double nDeg2 = ceil(deg);
	double alpha = (deg - nDeg1);
	
	double nRad1 = ONE_UNIT_IN_RAD * nDeg1;
	double nRad2 = ONE_UNIT_IN_RAD * nDeg2;
	
	float sRad1, cRad1;
	float sRad2, cRad2;
	
	sincos(nRad1, sRad1, cRad1);
	sincos(nRad2, sRad2, cRad2);
	
	sine = lerp_double(sRad1, sRad2, alpha);
	cosine = lerp_double(cRad1, cRad2, alpha);
	
	double len = sqrt(sine*sine + cosine*cosine);
	sine = sine / len;
	cosine = cosine / len;
}

// получить точку на поверхности эллипсоида WGS-84
double3 GetWGS84SurfacePoint(float longitude, float lattitude, float H)
{
	//double Rmin = 6356752.3142;
	//double Rmax = 6378137;

	//double cosB = cos(lattitude);
	//double sinB = sin(lattitude);

	//double cosA = cos(longitude);
	//double sinA = sin(longitude);

	//double R = sqrt(Rmax*Rmax*Rmin*Rmin / (Rmin*Rmin*cosB*cosB + Rmax*Rmax*sinB*sinB));
	//return double3(R*sinA*cosB, R*sinB, -R*cosA*cosB);

	
	////////////////////////////////////////
	// ГОСТ Р 51794-2001
	
	double cosB, sinB;
	sincos_double(lattitude, sinB, cosB);
	
	double cosA, sinA;
	sincos_double(longitude, sinA, cosA);


	double a 		= 6378137.0; 		// большая полуось WGS-84
	double e2 		= 0.006694379993;	// квадрат эксцентриситета WGS-84
	
	// радиус кривизны первого вертикала
	double N = a / sqrt(1 - e2 * sinB * sinB);
	
	double X = (N + H) * cosB * cosA;
	double Y = (N + H) * cosB * sinA;
	double Z = ((1 - e2)*N + H) * sinB;
	
	return double3(Y, Z, -X);
}

double3 GetWGS84SurfaceNormalBL(float longitude, float lattitude)
{
	/*double3 vPos1 = GetWGS84SurfacePoint(longitude, lattitude, 100.f);
	double3 vPos0 = GetWGS84SurfacePoint(longitude, lattitude, 0.f);
	
	return normalize(vPos1 - vPos0);*/
	
	double cosB, sinB;
	sincos_double(lattitude, sinB, cosB);
	
	double cosA, sinA;
	sincos_double(longitude, sinA, cosA);

	double NX = cosB*cosA;
	double NY = cosB*sinA;
	double NZ = sinB;
	
	return double3(NY, NZ, -NX);
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

float2 CalcTexcoords(uint iLong, uint iLat, float longCoeff, float latCoeff)
{
	float u = (float) iLong / (nCountLong - 1) ;
	float v = (float) iLat / (nCountLat - 1) ;

	return float2(
		longCoeff *u,
		latCoeff * (1 - v)
	);
}

float2 CalcTexcoordsHF(uint iLong, uint iLat, float longCoeff, float latCoeff, float Width, float Height)
{
	float longA = longCoeff * (float)iLong / (nCountLong - 1);
	float latA = latCoeff * (float)iLat / (nCountLat - 1);

	float u = lerp(0.5 / Width, 1 - 0.5 / Width, longA);
	float v = lerp(0.5 / Height, 1 - 0.5 / Height, latA);

	return float2(
		u,
		(1 - v)
	);
}

uint uintRound( float val )
{
	return uint( round( val ) + 0.1 );//Страховка на случай представления целого числа во float в виде "n.9999999999"
}
int intRound( float val )
{
	return int( round( val ) + 0.1 );//Страховка на случай представления целого числа во float в виде "n.9999999999"
}

//Функция, обратная функции CalcTexcoords
GeoIndex TexcoordsToVertexIndex( float2 texCoord, float longCoeff, float latCoeff )
{
	float u =     texCoord.x / longCoeff;
	float v = 1 - texCoord.y / latCoeff;
	GeoIndex vertexIndex;
	vertexIndex.lat  = intRound( v * ( nCountLat  - 1 ) );
	vertexIndex.long = intRound( u * ( nCountLong - 1 ) );
	return vertexIndex;
}

// получить высоту вершины по индексам вдоль осей x и y
float GetVertexHeight(uint iLong, uint iLat, in Texture2D tex, float longCoeff, float latCoeff)
{
	float2 texCoord;

	float Width, Height, NoL;
	tex.GetDimensions(0, Width, Height, NoL);

	texCoord = CalcTexcoordsHF(iLong, iLat, longCoeff, latCoeff, Width, Height);

	float4 TexColor = tex.SampleLevel(HeightTextureSampler, texCoord, 0);

	return TexColor.r;
}

// получить позицию вершины
float3 GetVertexPos(uint iLong, uint iLat, in Texture2D tex, float longCoeff, float latCoeff, float minLat, float maxLat, float minLong, float maxLong)
{
	float height = GetVertexHeight(iLong, iLat, tex, longCoeff, latCoeff);

	float fLongitudeAmpl = maxLong - minLong;
	float fLattitudeAmpl = maxLat - minLat;

	float dlong = fLongitudeAmpl / (nCountLong - 1);
	float dlat = fLattitudeAmpl / (nCountLat - 1);

	float longitude = minLong + iLong * dlong;
	float lattitude = minLat + iLat * dlat;

	double3 vSurfacePoint = fWorldScale * GetWGS84SurfacePoint(longitude, lattitude, 0);
	double3 vSurfaceNormal = GetWGS84SurfaceNormalBL(longitude, lattitude);

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
	float3 n = normalize(cross(v1 - v0, v2 - v0));
	if (dot(n, v0) < 0)
		n = -n;

	return n;
}


float3 ComputeTriangleNormal(in Triangle t)
{
	return ComputeTripleNormal(t.v[0], t.v[1], t.v[2]);
}

Triangle MakeTriangle(float3 v0, float3 v1, float3 v2)
{
	Triangle t;

	t.v[0] = v0; t.v[1] = v1; t.v[2] = v2;
	t.n = ComputeTriangleNormal(t);

	return t;
}

struct QuadGeometry
{
	Triangle t[2];
	float3	 vertex[4];
	float2	 tex[4];
};

void ComputeQuadGeometry(in int iQuadLat, in int iQuadLong, in Texture2D tex, out QuadGeometry geom, float longCoeff, float latCoeff, float minLat, float maxLat, float minLong, float maxLong)
{
	uint	iLat[4], iLong[4];
	float3	v[4];
	float2	texCoords[4];
	
	iLat[1] = iLat[2] = iQuadLat;
	iLat[0] = iLat[3] = iQuadLat + 1;

	iLong[0] = iLong[1] = iQuadLong;
	iLong[2] = iLong[3] = iQuadLong + 1;

	for (int i = 0; i < 4; i++)
	{
		v[i] = GetVertexPos(iLong[i], iLat[i], tex, longCoeff, latCoeff, minLat, maxLat, minLong, maxLong);
		texCoords[i] = CalcTexcoords(iLong[i], iLat[i], longCoeff, latCoeff);
	}

	geom.vertex[0] = v[0];
	geom.tex[0] = texCoords[0];

	geom.vertex[1] = v[1];
	geom.tex[1] = texCoords[1];

	geom.vertex[2] = v[2];
	geom.tex[2] = texCoords[2];

	geom.vertex[3] = v[3];
	geom.tex[3] = texCoords[3];


	geom.t[0].v[0] = v[0];
	geom.t[0].v[1] = v[1];
	geom.t[0].v[2] = v[2];

	geom.t[0].n = ComputeTriangleNormal(geom.t[0]);

	geom.t[1].v[0] = v[0];
	geom.t[1].v[1] = v[2];
	geom.t[1].v[2] = v[3];

	geom.t[1].n = ComputeTriangleNormal(geom.t[1]);
}






