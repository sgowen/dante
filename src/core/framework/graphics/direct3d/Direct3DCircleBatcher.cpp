//
//  Direct3DCircleBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Direct3DCircleBatcher.h"

#include "Circle.h"
#include "Vector2D.h"
#include "Direct3DProgramInput.h"
#include "macros.h"
#include "Direct3DManager.h"
#include "GpuProgramWrapper.h"
#include "Direct3DGeometryGpuProgramWrapper.h"
#include "Color.h"

#define DEGREE_SPACING 6

Direct3DCircleBatcher::Direct3DCircleBatcher() : CircleBatcher()
{
	// Empty
}

void Direct3DCircleBatcher::renderCircle(Circle &circle, Color &c, GpuProgramWrapper &gpuProgramWrapper)
{
	m_iNumPoints = 0;
	D3DManager->getColorVertices().clear();

	for (int i = 0; i <= 360; i += DEGREE_SPACING)
	{
		float rad = DEGREES_TO_RADIANS(i);
		float cos = cosf(rad);
		float sin = sinf(rad);

		addVertexCoordinate(cos * circle.getRadius() + circle.getCenter().getX(), sin * circle.getRadius() + circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);

		addVertexCoordinate(circle.getCenter().getX(), circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);
	}

	endBatch(gpuProgramWrapper);
}

void Direct3DCircleBatcher::renderPartialCircle(Circle &circle, int arcDegrees, Color &c, GpuProgramWrapper &gpuProgramWrapper)
{
	m_iNumPoints = 0;
	D3DManager->getColorVertices().clear();

	for (int i = 90; i < (450 - arcDegrees); i += DEGREE_SPACING)
	{
		float rad = DEGREES_TO_RADIANS(i);
		float cos = cosf(rad);
		float sin = sinf(rad);

		addVertexCoordinate(cos * circle.getRadius() + circle.getCenter().getX(), sin * circle.getRadius() + circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);

		addVertexCoordinate(circle.getCenter().getX(), circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);
	}

	float rad = DEGREES_TO_RADIANS(450 - arcDegrees);
	float cos = cosf(rad);
	float sin = sinf(rad);

	addVertexCoordinate(cos * circle.getRadius() + circle.getCenter().getX(), sin * circle.getRadius() + circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);

	addVertexCoordinate(circle.getCenter().getX(), circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);

	endBatch(gpuProgramWrapper);
}

void Direct3DCircleBatcher::endBatch(GpuProgramWrapper &gpuProgramWrapper)
{
	// set the primitive topology
	ID3D11DeviceContext* d3dContext = Direct3DManager::getD3dContext();
	d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	gpuProgramWrapper.bind();

	d3dContext->Draw(m_iNumPoints, 0);

	gpuProgramWrapper.unbind();
}

void Direct3DCircleBatcher::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a)
{
	D3DManager->addVertexCoordinate(x, y, z, r, g, b, a);

	m_iNumPoints++;
}
