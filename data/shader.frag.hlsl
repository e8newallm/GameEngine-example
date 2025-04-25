Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

static float2 texData[4] =
{
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(0.0f, 1.0f)
};


float4 main(float2 TexCoord : TEXCOORD0) : SV_Target0
{
    return Texture.Sample(Sampler, TexCoord);
}