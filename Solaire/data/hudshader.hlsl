float Shield;
float Armour; 
float4x4 matProj; 

struct VS_OUTPUT 
{
   float4 Position :        POSITION0;
   float2 Texcoord :        TEXCOORD0;
};

float2 Rotate(float2 inTex, float Rotation)
{
   float2 Temp = inTex;
   
   Temp.x = Temp.x * 2.0 - 1.0;
   Temp.y = Temp.y * 2.0 - 1.0;
   
   float2 TempTex = Temp; 
   
   Temp.x = TempTex.x * cos(Rotation * 3.1415) - TempTex.y * sin(Rotation * 3.1415);
   Temp.y = TempTex.x * sin(Rotation * 3.1415) + TempTex.y * cos(Rotation * 3.1415);
   
   Temp.x = Temp.x * 0.5 + 0.5;
   Temp.y = Temp.y * 0.5 + 0.5;

   return Temp;
}

float4 EvaluateBar(float4 inColour, float2 inCoord, float StatValue)
{
	float BlendValue = (dot(normalize(inCoord - 0.5), float2(0, 1)) + 1.0) * 0.5;

	BlendValue = smoothstep(StatValue, StatValue + 0.2 * (1 - abs(BlendValue - 0.5) * 2.0), BlendValue); 

	return float4 (inColour.rgb * (1 - BlendValue), inColour.a); 
}

VS_OUTPUT vs_main( 
   in float4 Position : POSITION0,
   in float2 Texcoord : TEXCOORD0)
{
   VS_OUTPUT Output;

   float4 Pos = Position;
   Pos.z += 5;
   Pos.y -= 2;
   Pos.x -= 5;
   Output.Position         = mul(Pos, matProj);
 

   Output.Texcoord         = Texcoord;
      
   return( Output );
   
}

sampler2D tex0 : register(s0);
sampler2D tex1 : register(s1);
sampler2D tex2 : register(s2);


float4 ps_main( float2 Texcoord : TEXCOORD0) : COLOR0
{  
   float2 Tex = Texcoord; 
   Tex.xy = Tex.yx;
   
    
   float4 BaseColour = tex2D(tex0, Tex );
   float4 BarColour = EvaluateBar(tex2D(tex1, Tex), Tex, Shield) + 
						EvaluateBar(tex2D(tex2, Tex ), Tex, Armour);

   float4 Output = BaseColour * (1-BarColour.a) + BarColour;


   return Output; 
}
