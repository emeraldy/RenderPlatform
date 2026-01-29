cbuffer VSConstants
{
    float4x4 mvp;
};

struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 color : COLOR;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 clipPos = mul(float4(input.pos, 1.0f), mvp);
    output.pos = clipPos;
    output.color = input.color;

    return output;
}
