//UNIFORMS

cbuffer data : register(b0, space1)
{
  int4 camera;
  int2 resolution;
};

//STATICS

static float2 verticesData[4] =
{
    float2(0.0f, 1.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 0.0f)
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
    Output output;
    output.TexCoord = texData[indexData[index]];

    float2 transVert = verticesData[indexData[index]];
    transVert.x += ((float)camera.x / (float)camera.z);
    transVert.y -= ((float)camera.y / (float)camera.w);
    output.Position = float4(transVert.x, transVert.y, 0.0f, 1.0f);
    return output;
}

