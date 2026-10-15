cbuffer Uniforms : register(b0, space3)
{
    float4 color;
}
struct Input
{
    float4 color : TEXCOORD0;
};

struct Output
{
	float4 Color : SV_Target0;
};

Output main(Input input)
{
	Output output;
    output.Color = input.color;

	return output;
}
