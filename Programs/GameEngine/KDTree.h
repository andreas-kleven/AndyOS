#pragma once
#include "Box.h"
#include "Model3D.h"

struct KDNode
{
	int depth;
	Box bounds;
	KDNode* left;
	KDNode* right;
	Triangle** triangles;
	int triangle_count;

	KDNode(int depth);
	~KDNode();

	static KDNode* Build(Triangle* tris[], int right, int depth);
};

class KDTree
{
public:
	KDTree();
	~KDTree();

	void Build(Model3D* model);

	KDNode* root;
};