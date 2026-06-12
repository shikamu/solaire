float4x4 matWorldViewProj;
float4x4 matInvWorld;

struct VS_OUTPUT 
{
   float4 Position :        POSITION0;
   float2 Texcoord :        TEXCOORD0;
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
   
   
   Output.Normal           = normalize(mul( Normal, matInvWorld ));
      
   return( Output );
   
}

sampler2D tex0 : register(s0);
sampler2D tex1 : register(s1);

float4 ps_main( float2 Texcoord : TEXCOORD0,
				float3 Normal :          TEXCOORD3 ) : COLOR0
{  

	
   
   
   float4 Output      = tex2D( tex0, Texcoord );
   Output.r = 1.0f;
   Output.a = 0.5f;
   
   return Output; 
}
