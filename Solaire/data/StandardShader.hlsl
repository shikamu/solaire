float3 LightPos;
float3 EyePos;

float4x4 matWorldViewProj;
float4x4 matInvTransWorld;
float4x4 matWorld; 


struct VS_OUTPUT 
{
   float4 Position :        POSITION0;
   float2 Texcoord :        TEXCOORD0;
   float3 ViewDirection :   TEXCOORD1;
   float3 LightDirection :  TEXCOORD2;
   float3 Normal :          TEXCOORD3;
};

VS_OUTPUT vs_main( 
   in float4 Position : POSITION0,
   in float2 Texcoord : TEXCOORD0,
   in float3 Normal :   NORMAL0 )
{
   VS_OUTPUT Output;

   Output.Position         = mul( Position, matWorldViewProj );
   Output.Texcoord         = Texcoord;
   
   float3 Pos = mul( Position, matWorld );
   
   Output.ViewDirection    = normalize(EyePos - Pos);
   Output.LightDirection   = normalize(LightPos);
   Output.Normal           = normalize(mul( Normal, matWorld) );
   return( Output );
   
}

sampler2D tex0 : register(s0);
sampler2D tex1 : register(s1);

float4 ps_main( float2 Texcoord : TEXCOORD0,
				float3 ViewDirection :   TEXCOORD1,
				float3 LightDirection:   TEXCOORD2,
				float3 Normal :          TEXCOORD3 ) : COLOR0
{  
    float4 fvAmbient = float4(0.8f, 0.8f, 0.8f, 1.0f);
	float4 fvSpecular = float4(0.8f, 0.8f, 0.8f, 1.0f);
	float4 fvDiffuse = float4(0.5f, 0.5f, 0.5f, 1.0f);
	
	
	
   float Alpha = tex2D( tex1, Texcoord ).r;
   float3 fvLightDirection = normalize( LightDirection );
   float3 fvNormal         = normalize( Normal );
   float  fNDotL           = dot( fvNormal, fvLightDirection ); 
   
   float3 fvReflection     = normalize( ( ( 2.0f * fvNormal ) * ( fNDotL ) ) - fvLightDirection ); 
   float3 fvViewDirection  = normalize( ViewDirection );
   float  fRDotV           = max( 0.0f, dot( fvReflection, fvViewDirection ) );
   
   float4 fvBaseColor      = tex2D( tex0, Texcoord );
   
   float4 fvTotalAmbient   = fvAmbient * fvBaseColor; 
   float4 fvTotalDiffuse   = fvDiffuse * fNDotL * fvBaseColor; 
   float4 fvTotalSpecular  = fvSpecular * pow( fRDotV, 200.0f );
   
   float4 Output = ( saturate( fvTotalAmbient + fvTotalDiffuse + fvTotalSpecular ) );
   
   
   Output.a = 0.5 + 0.5*Alpha;
   return Output; 
}
