#pragma once

// Note: you can put kd-tree here

// struct kdNode{
// 	SplitNode;
// 	LeafNode;
// };
class kdTree
{
public:


	//This one is a bit weird. It is returning a node type, might need to uncomment and implement the struct.
	void buildTree(std::vector<Geometry*> objList, BoundingBox* bbox, int depthLimit, int leafSize){
		/*if (objList.size <= leafSize or ++depth == depthLimit){ 
			return LeafNode(objList)
		}
		bestPlane ← findBestSplitPlane(objList, boundingBox);
		for (each obj in objList){
			if (obj.boundingBoxMinOnBestAxis < bestPlane.position)
				addToLeftList;
			if (obj.boundingBoxMaxOnBestAxis > bestPlane.position)
				addToRightList;
			if (bestPlane.position == obj.bBMaxOnAxis &&
				bestPosition == obj.bBMinOnAxis && obj.N < 0) 
				addToLeftList;
			else if (bestPlane.position == obj.bBMaxOnAxis &&
					obj.bBMinOnAxis && obj.N >= 0) 
				addToRightList;
		}
		if (rightList.isEmpty || leftList.isEmpty) 
			return	LeafNode(objList);
		else return SplitNode(bestPlane.position, bestPlane.axis,
		buildTree(leftList, bestPlane.leftBBox, depth, leafSize),
		buildTree(rightList, bestPlane.rightBBox, depth, leafSize));*/
	}

	int findBestSplitPlane(std::vector<Geometry*> objList, BoundingBox* bbox){
		/*for each axis
			for each object:
			SplitPlane p1.position = obj.bBMinOnAxis
			SplitPlane p2.position = obj.bBMaxOnAxis
			candidateList.pushback(p1)
			candidateList.pushback(p2)
		for each plane in candidateList:
			plane.leftCount = countLeftObjects()
			plane.leftBBoxArea = calculateLeftBBox()
			plane.rightCount = countRightObjects()
			plane.rightBBoxArea = calculateRightBBox()
		for each plane in candidateList:
			SAM = (plane.leftCount * plane.leftBBoxArea + plane.rightCount
				* plane.rightBBoxArea)/boundingBox
			if (SAM < minSam) then
				minSam = SAM
				bestPlane = plane
		return bestPlane;*/
		return 0;		
	}

};

class SplitNode : public kdTree
{
public: 
	char axis;
	int position;
	kdTree* left;
	kdTree* right;

	bool findIntersection(ray& r, isect& i, int t_min, int t_max){

		/*//The check for ray is parallel sounds like r.getDirection() cross/dot axis == 1/0 i forget which gives parallel, but yeah.
		if(ray is nearly parallel){

			//Skip this intersection??
			calculate as near parallel()
		}else{
			// Idk what it means by beyond and before...
			if(splitPlane.position is beyond ray.position){
				if(left->findIntersection(r, i, t_min, t_max))
					return true;
			}
			else if(splitPlane.position is before ray.position){
				if(right->findIntersection(r, i, t_min, t_max))
					return true;
			}
			else{
				if (left→findIntersection(r, i, t_min, t_max)) return true;
				if (right→findIntersection(r, i, t_min, t_max)) return true;
			}
			return false;
		}*/
		return false;	
	}

	//Initiate the Stuff in here, though I forget if C++ yells at you for putting this after the function. might need to have this before findIntersection
	SplitNode();
	~SplitNode();

};
class LeafNode : public kdTree
{
public:

	// beginObjects() from the scene.h to populate this objList 
	std::vector<Geometry*> objList;

	bool findIntersection(ray& r, isect& i, int t_min, int t_max){
		/*//For each loop, needs obj type.
		for(<t> obj: objList){
			isect c_i
			if(obj.intersect(r, c_i) && c_i.t >= t_min && c_i.t <= t_max ){
				i = c_i;
			}
		}*/
		return false;	
	}
	// Initate the objList in here, same as the splitnode if it yells.
	LeafNode();
	~LeafNode();
};
