#include "BVHierarchy.h"

namespace BVHierarchy
{
	bool compareX(GameObject* a, GameObject* b)
	{
		return a->transform.Position.x < b->transform.Position.x;
	}

	bool compareY(GameObject* a, GameObject* b)
	{
		return a->transform.Position.y < b->transform.Position.y;
	}

	bool compareZ(GameObject* a, GameObject* b)
	{
		return a->transform.Position.z < b->transform.Position.z;
	}

	char FindLargestAxis(GameObject* objects[], int numObjects) //decides the split plane
	{
		float minX = FLT_MAX, maxX = FLT_MIN;
		float minY = FLT_MAX, maxY = FLT_MIN;
		float minZ = FLT_MAX, maxZ = FLT_MIN;
		for (int i = 0; i < numObjects; ++i)
		{
			if (objects[i]->transform.Position.x < minX) minX = objects[i]->transform.Position.x;
			if (objects[i]->transform.Position.y < minY) minY = objects[i]->transform.Position.y;
			if (objects[i]->transform.Position.z < minZ) minZ = objects[i]->transform.Position.z;

			if (objects[i]->transform.Position.x > maxX) maxX = objects[i]->transform.Position.x;
			if (objects[i]->transform.Position.y > maxY) maxY = objects[i]->transform.Position.y;
			if (objects[i]->transform.Position.z > maxZ) maxZ = objects[i]->transform.Position.z;
		}

		float xSpread = abs(maxX - minX);
		float ySpread = abs(maxY - minY);
		float zSpread = abs(maxZ - minZ);
		if (xSpread > ySpread && xSpread > zSpread)
			return 'x';
		else if (ySpread > xSpread && ySpread > zSpread)
			return 'y';
		else return 'z';
	}
	//AABB Method
	Collision::AABB ComputeBoundingVolume(GameObject* objects[], int numObjects)
	{
		assert(numObjects > 0);
		glm::vec3 Min = objects[0]->transform.Position + objects[0]->aabbBV.m_Min;
		glm::vec3 Max = objects[0]->transform.Position + objects[0]->aabbBV.m_Max;
		for (int i = 1; i < numObjects; ++i)
		{
			if (Min.x > objects[i]->transform.Position.x + objects[i]->aabbBV.m_Min.x) 
				Min.x = objects[i]->transform.Position.x + objects[i]->aabbBV.m_Min.x;
			if (Max.x < objects[i]->transform.Position.x + objects[i]->aabbBV.m_Max.x) 
				Max.x = objects[i]->transform.Position.x + objects[i]->aabbBV.m_Max.x;
			if (Min.y > objects[i]->transform.Position.y + objects[i]->aabbBV.m_Min.y) 
				Min.y = objects[i]->transform.Position.y + objects[i]->aabbBV.m_Min.y;
			if (Max.y < objects[i]->transform.Position.y + objects[i]->aabbBV.m_Max.y)
				Max.y = objects[i]->transform.Position.y + objects[i]->aabbBV.m_Max.y;
			if (Min.z > objects[i]->transform.Position.z + objects[i]->aabbBV.m_Min.z) 
				Min.z = objects[i]->transform.Position.z + objects[i]->aabbBV.m_Min.z;
			if (Max.z < objects[i]->transform.Position.z + objects[i]->aabbBV.m_Max.z)
				Max.z = objects[i]->transform.Position.z + objects[i]->aabbBV.m_Max.z;
		}
		return Collision::AABB(Min, Max);
	}

	void TopDownBVTree(Node** tree, GameObject* objects[], int numObjects, int depth)
	{
		assert(numObjects > 0);
		const int MIN_OBJECTS_PER_LEAF = 1;
		Node* pNode = new Node;
		*tree = pNode;
		// Compute a bounding volume for object[split], ..., object[numObjects - 1]
		pNode->BV_AABB = ComputeBoundingVolume(&objects[0], numObjects);

		if (numObjects <= MIN_OBJECTS_PER_LEAF) {
			pNode->type = Node::Type::LEAF;
			pNode->numObjects = numObjects;
			pNode->data = objects[0]; // Pointer to first object in leaf
			pNode->treeDepth = depth;
		}
		else {
			pNode->type = Node::Type::INTERNAL;
			pNode->treeDepth = depth;
			// Based on some partitioning strategy, arrange objects into
			// two partitions: object[0..k], and object[k+1..numObjects-1]
			int k = PartitionObjects(objects, numObjects);
			// Recursively construct left and right subtree from subarrays and
			// point the left and right fields of the current node at the subtrees
			TopDownBVTree(&(pNode->lChild), objects, k, depth + 1);
			TopDownBVTree(&(pNode->rChild), &objects[k], numObjects - k, depth + 1);
		}
	}

	//Chosen Heuristic: minimise total volume occupied by child nodes (volume proportional to SA)
	float GetHeuristicCost(GameObject* objects[], // the array of objects
		int split, // the index to split into left & right
		int numObjects) // the total number of objects
	{
		/*Collision::AABB parentAABB = ComputeBoundingVolume(&objects[0], numObjects);
		float parentSurfaceArea = parentAABB.GetSurfaceArea();*/
		Collision::AABB leftAABB = ComputeBoundingVolume(&objects[0], split);
		float leftSurfaceArea = leftAABB.GetSurfaceArea();
		Collision::AABB rightAABB = ComputeBoundingVolume(&objects[split], numObjects - split);
		float rightSurfaceArea = rightAABB.GetSurfaceArea();
		return (split * leftSurfaceArea) +
			((numObjects - split) * rightSurfaceArea);
	}

	int PartitionObjects(GameObject* objects[], int numObjects)
	{
		float split = 0;
		float minCost = FLT_MAX;
		for (int i = 1; i < numObjects; ++i)
		{
			//possible split combinations
			//index 0 and 1 to numObjects - 1;
			//index 0 to 1 AND 2 to numObjects - 1;
			//index 0 to 2, 3 to numObjects - 1;
			//Divide the space into two subsets
			float cost = GetHeuristicCost(objects, i, numObjects);
			if (cost < minCost)
			{
				minCost = cost; //take the lowest cost one
				split = i; //Find the best split point
			}
				
		}
		return split;
	}

	//void RenderTopDownBVTree(Node** tree)
	//{
	//	
	//}

}

