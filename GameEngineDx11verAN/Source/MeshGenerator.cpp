#include "MeshGenerator.h"

std::vector<Vertex> CreateGrid(int width, int depth)
{
	std::vector<Vertex> vertices;
	float spacing = 1.0f;

	for (int z = 0; z < depth; ++z)
	{
		for (int x = 0; x < width; ++x)
		{
			Vertex v;
			v.position = { x * spacing, 0.0f, z * spacing };
			v.normal = { 0.0f, 1.0f, 0.0f };
			v.tex = { x / (float)(width - 1), z / (float)(depth - 1) };
			vertices.push_back(v);
		}
	}
	return vertices;
}
