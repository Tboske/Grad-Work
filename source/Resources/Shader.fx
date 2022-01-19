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
const float		gPi = 3.1415927f;
float		gLightIntensity = 7.f;


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


float map(float value, float min1, float max1, float min2, float max2)
{
    // normalize the value / a.k.a get a percent
    const float norm = (value - min1) / (max1 - min1);

    return norm * (max2 - min2) + min2;
}

float4 GetObservedArea(float3 normal)
{
    float observedArea = dot(-normal, gLightDirection);
	// if observedArea is positive, the bool is true, and the observedarea will stay the same
    //observedArea = observedArea * (observedArea > 0.f);

    observedArea = map(observedArea, -1.f, 1.f, 0.f, 1.f);

    return float4(observedArea, observedArea, observedArea, 1.f);
}

// ==================================================================================
//	        Pixel Shader 
// ----------------------------------------------------------------------------------

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	const float4 observedArea = GetObservedArea(input.Normal);


    return float4(gColor, 1.0f) * observedArea;
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



