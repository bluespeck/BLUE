matrix World;
matrix View;
matrix Projection;

struct VS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
    float4 vecNormal : NORMAL0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
    float4 vecNormal : NORMAL0;
};

VS_INPUT VS( float4 Pos : POSITION, float4 Color : COLOR, float4 vecNormal : NORMAL )
{
    PS_INPUT psInput;
    
	float4x4 mWVP = mul( mul(World, View), Projection);
	
	//Pos = mul( Pos, World );
    //Pos = mul( Pos, View );
	
    psInput.Pos = mul (Pos, mWVP);
    psInput.vecNormal = vecNormal;//normalize(mul(vecNormal, mWVP));
	psInput.Color = Color;
 
    return psInput;
}
 
float4 PS( PS_INPUT psInput ) : SV_Target
{
	float dProd = dot( psInput.vecNormal.xyz, float3(-1,-1,1) );
    psInput.Color.rgb = psInput.Color.rgb * dProd;
    return psInput.Color;
}
 
technique10 BasicRender
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
