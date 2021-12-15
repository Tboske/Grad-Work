// ==================================================================================
//	        Global variables
// ----------------------------------------------------------------------------------
//  Sets culling state
RasterizerState gRasterizerState
{
	CullMode = front;
};

float4x4	gWorldViewProj	: worldViewProjection;
float4x4	gWorldMatrix	: worldMatrix;
float4x4	gViewInverse	: viewInverseMatrix;

float3		gColor;
float3		gLightDirection = float3(0.577f, -0.577f, 0.577f);
float		gPi = 3.1415927f;
float		gLightIntensity = 7.f;



// ==================================================================================
//	        Texture samplers
// ----------------------------------------------------------------------------------

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	AddressU = Border;
	AddressV = Clamp;
	BorderColor = float4(0.f, 0.f, 1.f, 1.f);
};


// ==================================================================================
//	        Vertex shader output/input structure
// ----------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 Position : POSITION;
	float3 Normal	: NORMAL;
};

struct VS_OUTPUT
{
	float4 Position		: SV_POSITION;
	float3 Normal		: NORMAL;
};


// ==================================================================================
//	        Computes transform and lightning
// ----------------------------------------------------------------------------------

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	// transform the position with worldViewProjmatrix
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	output.Normal = mul(input.Normal, (float3x3)gWorldMatrix);

	return output;
}

// ==================================================================================
//	        Pixel Shader Functions
// ----------------------------------------------------------------------------------

float4 GetObservedArea(float3 normal)
{
	float observedArea = dot(-normal, gLightDirection);
	// if observedArea is positive, the bool is true, and the observedarea will stay the same
	observedArea = observedArea * (observedArea > 0.f);

	return float4(observedArea, observedArea, observedArea, 1.f);
}

float4 Lambert(float3 color, float reflectance)
{
	return float4(color * (reflectance / gPi), 1.f);
}

// ==================================================================================
//	        Pixel Shader output structure
// ----------------------------------------------------------------------------------

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	// move to [-1,1] range
	input.Normal *= 2;
	input.Normal.x -= 1;
	input.Normal.y -= 1;
	input.Normal.z -= 1;

	const float4 observedArea = GetObservedArea(input.Normal);


    return float4(gColor, 1.0f);// * observedArea;
}


// ==================================================================================
//	        Renders scene to render target
// ----------------------------------------------------------------------------------

technique11 Default
{
	pass P0
	{
		SetRasterizerState(gRasterizerState);
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));
	}
}



