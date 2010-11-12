matrix World;
matrix View;
matrix Projection;

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
    float4 Normal : NORMAL0;
};

PS_INPUT VS( float4 Pos : POSITION, float4 Color : COLOR, float4 Normal : NORMAL )
{
    PS_INPUT psInput;
    Pos = mul( Pos, World );
    Pos = mul( Pos, View );

    psInput.Pos = mul( Pos, Projection );
    psInput.Normal = Normal;
	psInput.Color = Color;
 
    return psInput;
}
 
float4 PS( PS_INPUT psInput ) : SV_Target
{
	float dProd = .5 + dot( psInput.Normal.xyz, float3(0,0,1) );
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
