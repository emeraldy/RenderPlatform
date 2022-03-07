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
    output.pos = float4(input.pos, 1.0f);
    output.color = input.color;

    return output;
}
