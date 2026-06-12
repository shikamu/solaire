float4x4 matWorldViewProj;

struct VS_OUTPUT 
{
   float4 Position :        POSITION0;
};

VS_OUTPUT vs_main( 
   in float4 Position : POSITION0 )
{
   VS_OUTPUT Output;

   Output.Position         = mul( Position, matWorldViewProj );
   
   return( Output );
}

float Time;
float4 ps_main( ) : COLOR0
{  
   float4 Output = float4(0.0f, 0.0f, 0.0f, 0.0f);
   Output.r = 0.0f;
   Output.g = 1.0f;
   Output.b = 0.0f;
   Output.a = Time;
   return Output;  
}
