
Texture2D diffuseTexture;
SamplerState sampl
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_INPUT
{
    float4 fPosition : POSITION;
    float2 fTexture : TEXCOORD;
};

struct PS_INPUT
{
    float4 fPosition : SV_POSITION;
    float2 fTexture : TEXCOORD0;
};

// Vertex Shader
PS_INPUT VS( VS_INPUT input )
{	
    return input;
}

// Pixel Shader
float4 PS( PS_INPUT input) : SV_Target
{
    return diffuseTexture.Sample( sampl, input.Tex );
}