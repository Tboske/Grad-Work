// ==================================================================================
//	        Global variables
// ----------------------------------------------------------------------------------
//  Sets culling state

float4x4    gWorldViewProj : worldViewProjection;
float4x4    gWorldMatrix   : worldMatrix;
float4x4    gViewInverse   : viewInverseMatrix;

float3      gColor;     


// ==================================================================================
//	        Vertex shader output/input structure
// ----------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION;
    uint CubeID      : CUBEID;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
};

struct GS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
};


// ==================================================================================
//	        Computes transform and lightning
// ----------------------------------------------------------------------------------

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

	// transform the position with worldViewProjmatrix
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);

    return output;
}

//VS_INPUT MCVS(VS_INPUT input)
//{
//    return input;
//}

// ==================================================================================
//	        Geometry functions
// ----------------------------------------------------------------------------------



// ==================================================================================
//	        Geometry shader
// ----------------------------------------------------------------------------------
//[maxvertexcount(4)]
//void GS(point VS_INPUT sprite[1], inout TriangleStream<GS_OUTPUT> triStream)
//{
//    GS_OUTPUT t;





//}


// ==================================================================================
//	        Pixel Shader output structure
// ----------------------------------------------------------------------------------

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    return float4(gColor, 1.f);
}

//float4 MCPS(VS_OUTPUT input) : SV_TARGET
//{
//    return float4(gColor, 1.f);
//}

// ==================================================================================
//	        Renders scene to render target
// ----------------------------------------------------------------------------------

technique11 Default
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
    //pass P0
    //{
    //    SetVertexShader(CompileShader(vs_4_0, MCVS()));
    //    SetGeometryShader(NULL);
    //    SetPixelShader(CompileShader(ps_4_0, MCPS()));
    //}
}



