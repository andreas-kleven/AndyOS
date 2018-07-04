#include "KDTree.h"
#include "GEngine.h"
#include "limits.h"

void SortTriangles(Triangle* arr[], int axis, int left, int right) {
	int i = left, j = right;
	Triangle* tmp;
	float pivot = arr[(left + right) / 2]->Center()[axis];

	//Partition
	while (i <= j)
	{
		while (arr[i]->Center()[axis] < pivot)
			i++;

		while (arr[j]->Center()[axis] > pivot)
			j--;

		if (i <= j) {
			tmp = arr[i];
			arr[i] = arr[j];
			arr[j] = tmp;
			i++;
			j--;
		}
	}

	//Recursion
	if (left < j)
		SortTriangles(arr, axis, left, j);

	if (i < right)
		SortTriangles(arr, axis, i, right);
}

KDNode::KDNode(int depth)
{
	this->depth = depth;
	this->left = 0;
	this->right = 0;
	this->bounds = Box();
	this->triangles = 0;
	this->triangle_count = 0;
}

KDNode::~KDNode()
{
	if (this->triangles)
	{
		delete[] this->triangles;
	}
}

//https://blog.frogslayer.com/kd-trees-for-faster-ray-tracing-with-triangles/
KDNode* KDNode::Build(Triangle* tri[], int count, int depth)
{
	if (depth > 8)
		return 0;

	if (count == 0)
		return 0;

	int right = count - 1;

	Triangle** tris = new Triangle*[count];
	memcpy(tris, tri, count * sizeof(Triangle*));

	Box bounds = tri[0]->BoundingBox();
	for (int i = 1; i < count; i++)
		bounds.Expand(tris[i]->BoundingBox());

	KDNode* node = new KDNode(depth);
	node->bounds = bounds;

	int axis = bounds.LongestAxis();

	if (count > 1)
		SortTriangles(tris, axis, 0, right);

	float best = FLT_MAX;
	float bestAlpha = 0;
	int bestIndex = 0;

	float first = tris[0]->Center()[axis];
	float last = tris[right]->Center()[axis];
	float distance = last - first;

	for (int i = 1; i < count - 1; i++)
	{
		float alpha = (tris[i]->Center()[axis] - first) / distance;

		float costL = alpha * i;
		float costR = (1 - alpha) * (right - i);
		float cost = costL + costR;

		if (cost < best)
		{
			best = cost;
			bestAlpha = alpha;
			bestIndex = i;
		}
	}

	node->left = Build(tris, bestIndex + 1, depth + 1);
	node->right = Build(tris + bestIndex + 1, right - bestIndex, depth + 1);

	if (node->left || node->right)
		return node;

	node->triangles = tris;
	node->triangle_count = count;

	return node;
}

KDTree::KDTree()
{
}

KDTree::~KDTree()
{
	if (root)
		delete root;
}

void KDTree::Build(Model3D* model)
{
	debug_print("Building k-d tree. %i triangles\n", model->triangles.Count());

	Triangle** tris = new Triangle*[model->triangles.Count()];

	for (int i = 0; i < model->triangles.Count(); i++)
		tris[i] = &model->triangle_buffer[i];

	root = KDNode::Build(tris, model->triangles.Count(), 0);
}
