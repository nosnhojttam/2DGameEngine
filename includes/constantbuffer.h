#pragma once
#ifndef CONSTANT_BUFFER
#define CONSTANT_BUFFER

struct VS_CONSTANT_BUFFER
{

	XMFLOAT4 WorldMatrix;
	XMFLOAT4 ViewMatrix;
	XMFLOAT4 info;
	XMFLOAT4 animation;
};

#endif
