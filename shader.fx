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

	PS_INPUT output = (PS_INPUT)0;
	if (input.Pos.z >= 50) {
		input.Pos.x += WorldMatrix.x/4;
		input.Pos.y += WorldMatrix.y/8;
	}
	else {
		input.Pos.x += WorldMatrix.x;
		input.Pos.y += WorldMatrix.y;
	}
	


	output.Pos = input.Pos;
	output.Pos.x *= WorldMatrix.w;
	output.Pos.y *= WorldMatrix.w;

	
	output.Pos.z = 0;
	//output.Pos.x += WorldMatrix.x;
	//output.Pos.y += WorldMatrix.y;
	
	
	output.Tex = input.Tex;

	/*if (cb_e == 99 && cb_f == 0){
		output.Tex.x *= -1;
	}*/
	return output;

}

PS_INPUT VS_screen(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	float4 pos = input.Pos;
	output.Pos = pos;
	output.Tex = input.Tex;
	//lighing:
	//also turn the light normals in case of a rotation:




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

float4 PS_screen(PS_INPUT input) : SV_Target
{
	float4 texture_color = txDiffuse.SampleLevel(samLinear, input.Tex, 0);
	texture_color.a = 1;
	int posx = 1280 - input.Pos.x;
	int posy = 1780- input.Pos.y;
	int radius = 400;
	//float x = asin(y / radius);
	//(x - posx)^2 + (y-posy)^2 = radius^2

	if (!(pow((posx - input.Pos.x), 2) + pow((posy - input.Pos.y), 2) <= pow(radius, 2))) {
		//texture_color.rgb = 0;
	}
	return texture_color;
}