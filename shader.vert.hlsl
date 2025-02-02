ByteAddressBuffer dataBuffer : register(t0, space0);
Texture2D<float4> Texture : register(t0, space2);
cbuffer Index : register(b0, space1)
{
    int bIndex;
};

cbuffer worldData : register(b1, space1)
{
    int cameraX;
    int cameraY;
    int cameraWidth;
    int cameraHeight;
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

#define getData(startIndex, index) dataBuffer.Load(startIndex+(index*4))

Output main(uint index : SV_VertexID)
{
    uint bufferIndex = dataBuffer.Load(bIndex*4);
    ObjData objectData;
    objectData.bodyX        =   asint(getData(bufferIndex, 0));
    objectData.bodyY        =   asint(getData(bufferIndex, 1));
    objectData.bodyWidth    =   asint(getData(bufferIndex, 2));
    objectData.bodyHeight   =   asint(getData(bufferIndex, 3));
    objectData.texX         = asfloat(getData(bufferIndex, 4));
    objectData.texY         = asfloat(getData(bufferIndex, 5));
    objectData.texWidth     = asfloat(getData(bufferIndex, 6));
    objectData.texHeight    = asfloat(getData(bufferIndex, 7));

    Output output;

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
    float4x4 Translation = float4x4(
        float4(1.0f, 0.0f, 0.0f, 0.0f),
        float4(0.0f, 1.0f, 0.0f, 0.0f),
        float4(0.0f, 0.0f, 1.0f, 0.0f),
        float4(positionX, positionY, 1.0f, 1.0f)
    );

    uint width, height;
    Texture.GetDimensions(width, height);

    /*if(index == 0)
        output.TexCoord = float2(objectData.texX / width, objectData.texY / height);
    if(index == 1)
        output.TexCoord = float2((objectData.texX + objectData.texWidth) / width, objectData.texY / height);
    if(index == 2)
        output.TexCoord = float2((objectData.texX + objectData.texWidth) / width, (objectData.texY + objectData.texHeight)  / height);
    else
        output.TexCoord = float2(objectData.texX / width, (objectData.texY + objectData.texHeight) / height);*/

    if(indexData[index] == 0)
        output.TexCoord = float2(objectData.texX, objectData.texY);
    else if(indexData[index] == 1)
        output.TexCoord = float2(objectData.texWidth, objectData.texY);
    else if(indexData[index] == 2)
        output.TexCoord = float2(objectData.texWidth, objectData.texHeight);
    else
        output.TexCoord = float2(objectData.texX, objectData.texHeight);

    output.Position = mul(float4(verticesData[indexData[index]], 0.0f, 1.0f), mul(Scale, Translation));
    return output;

}