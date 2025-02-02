ByteAddressBuffer dataBuffer : register(t0, space0);

cbuffer worldData : register(b0, space1)
{
    int cameraX;
    int cameraY;
    int cameraWidth;
    int cameraHeight;
    int resolutionWidth;
    int resolutionHeight;
};

struct ObjData
{
    int bodyX;
    int bodyY;
    int bodyWidth;
    int bodyHeight;

    float texX;
    float texY;
    float texWidth;
    float texHeight;
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

#define getIndex(startIndex, index) (startIndex+index) * 4

Output main(uint index : SV_VertexID)
{
    uint bufferIndex = dataBuffer.Load(SV_DrawID);
    ObjData objectData;
    objectData.bodyX        =   asint(dataBuffer.Load(getIndex(bufferIndex, 0)));
    objectData.bodyY        =   asint(dataBuffer.Load(getIndex(bufferIndex, 1)));
    objectData.bodyWidth    =   asint(dataBuffer.Load(getIndex(bufferIndex, 2)));
    objectData.bodyHeight   =   asint(dataBuffer.Load(getIndex(bufferIndex, 3)));
    objectData.texX         = asfloat(dataBuffer.Load(getIndex(bufferIndex, 4)));
    objectData.texY         = asfloat(dataBuffer.Load(getIndex(bufferIndex, 5)));
    objectData.texWidth     = asfloat(dataBuffer.Load(getIndex(bufferIndex, 6)));
    objectData.texHeight    = asfloat(dataBuffer.Load(getIndex(bufferIndex, 7)));


    float xScale = ((float)objectData.bodyWidth / (float)cameraWidth);
    float yScale = ((float)objectData.bodyHeight / (float)cameraHeight);
    float4x4 Scale = float4x4(
        float4(xScale, 0.0f, 0.0f, 0.0f),
        float4(0.0f, yScale, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(0.0f, 0.0f, 0.0f, 1.0f)
    );

    float left = objectData.bodyX + (objectData.bodyWidth / 2.0f);
    float top  = objectData.bodyY + (objectData.bodyHeight / 2.0f);

    float positionX = (((float)(left - cameraX) * 2.0f) / (float)cameraWidth) - 1.0f;
    float positionY = (((float)(-top - cameraY) * 2.0f) / (float)cameraHeight) + 1.0f;

    if(SV_DrawID == 0)
        positionX *= 2.0f;

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