//--------------------------------------------------------------------------------------
// File: Tutorial022.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	float4 WorldMatrix;
	float4 ViewMatrix;
	float4 info;
};

struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	float offset = 1.0/4.0;

	PS_INPUT output = (PS_INPUT)0;
	input.Pos.x += WorldMatrix.x;
	input.Pos.y += WorldMatrix.y;


	output.Pos = input.Pos;
	output.Pos.x *= WorldMatrix.w;
	output.Pos.y *= WorldMatrix.w;

	
	output.Pos.z = 0;
	//output.Pos.x += WorldMatrix.x;
	//output.Pos.y += WorldMatrix.y;
	
	// animate
	output.Tex = input.Tex * offset;
	//output.Tex += float2((1 / 7) * (cb_a % 7), (1 / 7) * floor(cb_a / 7));
	//output.Tex.x += float(offset * (info.x % 4)); 
	output.Tex.x += offset * info.x;
	output.Tex.y += offset * info.y;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{

	float2 texture_coordinates = input.Tex;
	float4 color = txDiffuse.Sample(samLinear, texture_coordinates);
	/*float avg = color.r + color.g + color.b;
	avg /= 3;
	color.r = avg;
	color.g = avg;
	color.b = avg;
	if (color.r == 1 && color.g == 1 && color.b == 1){
		color.a = 0;
	}*/
	//return float4(1, 0, 0, 1);
	return color;
}