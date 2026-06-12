float3 EyePos;

float4x4 matWorldViewProj;
float4x4 matWorld; 


struct VS_OUTPUT 
{
   float4 Position :        POSITION0;
   float3 ViewDirection :   TEXCOORD1;
   float3 Normal :          TEXCOORD2;
};

VS_OUTPUT vs_main( 
   in float4 Position : POSITION0,
   in float3 Normal :   NORMAL0 )
{
   VS_OUTPUT Output;

   Output.Position         = mul( Position, matWorldViewProj );
   
   float3 Pos = mul( Position, matWorld );
   
   Output.ViewDirection    = normalize(EyePos - Pos);
   Output.Normal           = normalize(mul( Normal, matWorld) );
   return( Output );
   
}

float3 Colour1;
float3 Colour2;

float4 ps_main( float3 ViewDirection :   TEXCOORD1,
				float3 Normal :          TEXCOORD2 ) : COLOR0
{  
	
   float3 fvNormal         = normalize( Normal ); 
   float3 fvViewDirection  = normalize( ViewDirection );

   float NDotV = pow(max(0.0f, dot(fvNormal, fvViewDirection)), 0.5f);
   float4 Output = NDotV * float4(Colour1, 1.0f) + 
				   (1.0f - NDotV) * float4(Colour2, 1.0f);
   
   Output.a = NDotV * 0.3f + 0.7f;
   return Output; 
}
