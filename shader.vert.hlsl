//UNIFORMS

cbuffer worldData : register(b0, space1)
{
    int cameraX;
    int cameraY;
    int cameraWidth;
    int cameraHeight;
    int resolutionWidth;
    int resolutionHeight;
};

cbuffer objectData : register(b2, space1)
{
    int bodyX;
    int bodyY;
    int bodyWidth;
    int bodyHeight;
};

//STATICS

static float2 verticesData[4] =
{
    float2(-1.0f, 1.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, -1.0f),
    float2(-1.0, -1.0f)
};

static float2 texData[4] =
{
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(0.0f, 1.0f)
};

static uint indexData[6] =
{
    0, 1, 2, 0, 2, 3
};

struct Output
{
    float2 TexCoord : TEXCOORD0;
    float4 Position : SV_Position;
};

Output main(uint index : SV_VertexID)
{
    float xScale = ((float)bodyWidth / (float)cameraWidth);
    float yScale = ((float)bodyHeight / (float)cameraHeight);
    float4x4 Scale = float4x4(
        float4(xScale, 0.0f, 0.0f, 0.0f),
        float4(0.0f, yScale, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(0.0f, 0.0f, 0.0f, 1.0f)
    );

    float left = bodyX + (bodyWidth / 2.0f);
    float top  = bodyY + (bodyHeight / 2.0f);

    float positionX = (((float)(left - cameraX) * 2.0f) / (float)cameraWidth) - 1.0f;
    float positionY = (((float)(-top - cameraY) * 2.0f) / (float)cameraHeight) + 1.0f;
    float4x4 Translation = float4x4(
        float4(1.0f, 0.0f, 0.0f, 0.0f),
        float4(0.0f, 1.0f, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(positionX, positionY, 1.0f, 1.0f)
    );

    Output output;
    output.TexCoord = texData[indexData[index]];
    output.Position = mul(float4(verticesData[indexData[index]], 0.0f, 1.0f), mul(Scale, Translation));
    return output;

}