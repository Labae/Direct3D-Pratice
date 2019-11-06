cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatirx;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

PixelInputType BumpMapVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatirx);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.normal);

	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.normal);

	return output;
}