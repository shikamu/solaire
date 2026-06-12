float4x4 matProj; 
float3 TargetDirection;

struct VS_OUTPUT 
{
   float4 Position :        POSITION0;
   float2 Texcoord :        TEXCOORD0;
};

VS_OUTPUT vs_main( 
   in float4 Position : POSITION0,
   in float2 Texcoord : TEXCOORD0)
{
   VS_OUTPUT Output;

   float4 Pos = Position;
   Pos.x *= 0.1f;
   Pos.y *= 0.1f;

   float3 Direction;
   Direction.x = min(0.8f, max(-0.8f, TargetDirection.x));
   Direction.y = min(0.2f, max(-0.8f, TargetDirection.y));
   Direction.z = max(0.7f, TargetDirection.z);

   Pos.z += Direction.z *2.0f;
   Pos.y += Direction.y *2.0f + 0.6f;
   Pos.x += Direction.x *2.0f;

   Output.Position         = mul(Pos, matProj);
 

   Output.Texcoord         = Texcoord;
      
   return( Output );
   
}

sampler2D tex0 : register(s0);
float Visibility; 


float4 ps_main( float2 Texcoord : TEXCOORD0) : COLOR0
{  

   float4 Output = tex2D(tex0, Texcoord );

   Output.a *= Visibility;
   return Output; 
}
