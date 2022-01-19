// ==================================================================================
//	        Global variables
// ----------------------------------------------------------------------------------
//  Sets culling state

RasterizerState gRasterizerState
{
    CullMode = front;
};

float4x4    gWorldViewProj : worldViewProjection;
float4x4    gWorldMatrix   : worldMatrix;
float4x4    gViewInverse   : viewInverseMatrix;

float3      gLightDirection = float3(0.577f, -0.577f, 0.577f);
const float       gPi = 3.1415927f;
float       gLightIntensity = 7.f;

float3      gColor;     


Texture2D<int> gTriangulationLUT    : CUBEIDLUT;

const static float3 tVertexOffset[12] =
{
    // z     y     x                    
    { 0.5f, 0.f, 0.f}, // 0     
    { 1.f, 0.5f, 0.f}, // 1     
    { 0.5f, 1.f, 0.f}, // 2     
    { 0.f, 0.5f, 0.f}, // 3     
    { 0.5f, 0.f, 1.f}, // 4     
    { 1.f, 0.5f, 1.f}, // 5     
    { 0.5f, 1.f, 1.f}, // 6     
    { 0.f, 0.5f, 1.f}, // 7     
    { 0.f, 0.f, 0.5f}, // 8     
    { 1.f, 0.f, 0.5f}, // 9     
    { 1.f, 1.f, 0.5f}, // 10    
    { 0.f, 1.f, 0.5f} // 11    
};

// ==================================================================================
//	        shader output/input structures
// ----------------------------------------------------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION;
    uint CubeID      : CUBEID;
};                                  

struct GS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Normal   : NORMAL;
};


// ==================================================================================
//	        Vertex Shader
// ----------------------------------------------------------------------------------

VS_INPUT MCVS(VS_INPUT input)
{
    return input;
}

// ==================================================================================
//	        Geometry shader
// ----------------------------------------------------------------------------------

// the maximum triangles for just 1 cube is 5, so this means we only have a maximum of 15 vertices
[maxvertexcount(15)]
void MCGS(point VS_INPUT input[1], inout TriangleStream<GS_OUTPUT> triStream)
{
    // we increment by 3 because we want to always add 3 vertices (or 1 triangle) at a time
    for (int i = 0; gTriangulationLUT.Load(int3(i, input[0].CubeID, 0)) != -1; i += 3)
    {
        GS_OUTPUT vert1;
        GS_OUTPUT vert2;
        GS_OUTPUT vert3;

        const float3 pos1 = input[0].Position + tVertexOffset[gTriangulationLUT.Load(int3(i    , input[0].CubeID, 0))];
        const float3 pos2 = input[0].Position + tVertexOffset[gTriangulationLUT.Load(int3(i + 1, input[0].CubeID, 0))];
        const float3 pos3 = input[0].Position + tVertexOffset[gTriangulationLUT.Load(int3(i + 2, input[0].CubeID, 0))];

        // calculate the normal of this triangle, and apply it to all 3 vertices (or this triangle)
        float3 norm = cross(pos2 - pos1, pos3 - pos1);
        norm = mul(norm, (float3x3) gWorldMatrix);
        normalize(norm);

        vert1.Position = mul(float4(pos1, 1.f), gWorldViewProj);
        vert2.Position = mul(float4(pos2, 1.f), gWorldViewProj);
        vert3.Position = mul(float4(pos3, 1.f), gWorldViewProj);

        vert1.Normal = -norm;
        vert2.Normal = -norm;
        vert3.Normal = -norm;
        // add triangle vertices to trianglestream
        triStream.Append(vert1);
        triStream.Append(vert2);
        triStream.Append(vert3);
        // restart the strip, because we are using a triangle list topology
        triStream.RestartStrip();
    }
}


// ==================================================================================
//	        Pixel Shader 
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

float4 MCPS(GS_OUTPUT input) : SV_TARGET
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
        SetVertexShader(CompileShader(vs_4_0, MCVS()));
        SetGeometryShader(CompileShader(gs_4_0, MCGS()));
        SetPixelShader(CompileShader(ps_4_0, MCPS()));
    }
}



