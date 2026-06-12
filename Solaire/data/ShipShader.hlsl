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
   float3 OutPos : 			TEXCOORD4;
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
   Output.OutPos		   = mul(Position, matInvTransWorld); 
   return( Output );

}


float Remaining;
float Time;
float3 Direction; 
float3 MaskColour; 

float4 CalculateBaseColour(float2 Tex)
{	
	return float4(float3(float3(0.3f, 0.3f, 0.3f) * (Tex.y < 0.25f) + 
				 float3(0.9f, 0.8f, 0.5f) * (Tex.y >= 0.25f && Tex.y < 0.5f) + 
				 float3(0.0f, 0.0f, 0.0f) * (Tex.y >= 0.5f && Tex.y < 0.75f) + 
				 MaskColour * (Tex.y >= 0.75f)), 1.0f);
}

float4 ps_main( float2 Texcoord : TEXCOORD0,
				float3 ViewDirection :   TEXCOORD1,
				float3 LightDirection:   TEXCOORD2,
				float3 InNormal :          TEXCOORD3,
				float3 OutPos : 		 TEXCOORD4 ) : COLOR0
{  
    float4 fvAmbient = float4(0.8f, 0.8f, 0.8f, 1.0f);
	float4 fvSpecular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 fvDiffuse = float4(0.8f, 0.8f, 0.8f, 1.0f);
	

   float3 LightDir		   = normalize( LightDirection );
   float3 Normal		   = normalize( InNormal );
   float  fNDotL           = dot( Normal, LightDir ); 
   
   float3 fvReflection     = normalize( ( ( 2.0f * Normal ) * ( fNDotL ) ) - LightDir ); 
   float  fRDotV           = max( 0.0f, dot( fvReflection, normalize( ViewDirection ) ) );
   
   float4 fvBaseColor      = CalculateBaseColour(Texcoord);
   
   float4 fvTotalAmbient   = fvAmbient * fvBaseColor; 
   float4 fvTotalDiffuse   = fvDiffuse * fNDotL * fvBaseColor; 
   float4 fvTotalSpecular  = fvSpecular * pow( fRDotV, 40.0f ) * (Texcoord.x >= 0.5f);// * sin(OutPos.x);
   float4 Output = ( saturate( fvTotalAmbient + fvTotalDiffuse + fvTotalSpecular ) );
   
   
  //float ImpactFactor = smoothstep(ImpactDotPos, ImpactDotPos + 0.1f,  1.0f - Time) - smoothstep(ImpactDotPos - 0.1f, ImpactDotPos, 1.0f - Time);


   float ImpactFactor =  pow(max(0.0f, dot(normalize(Direction), normalize(OutPos))), 2) * Time; 
   Output.rgb = Output.rgb * (1.0f - ImpactFactor) + float3(1.0f - Remaining, Remaining, 0.0f) * ImpactFactor; 

   //float OutlineFactor = pow(max( 0.0f, dot( Normal, normalize( ViewDirection ) ) ), 0.4f);
   
   //Output.rgb = float3(0.0f, 0.6f, 1.0f) * (1.0f - OutlineFactor) + Output.rgb * OutlineFactor; 
   
   //Output.rgb = float3(0.0f, 0.6f, 1.0f) * OutlineFactor + Output.rgb * (1.0f - OutlineFactor);   
   
   Output.a = 1.0f;
   return Output; 
}
