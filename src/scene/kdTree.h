#pragma once

// Note: you can put kd-tree here

// struct kdNode{
// 	SplitNode;
// 	LeafNode;
// };
#include <vector>
#include <glm/vec3.hpp>
#include "ray.h"
#include "scene.h"
#include "bbox.h"

class kdTree;
class SplitNode;
class LeafNode;


class Node{


};

class SplitNode : public Node
{
public: 
	int axis;
	int position;
	Node left;
	Node right;

    SplitNode( int _axis,
    	       int _pos,
    	       Node _l,
    	       Node _r)
    	     : axis(_axis), position(_pos), left(_l), right(_r)  {}

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

	~SplitNode();

};
class LeafNode : public Node
{
public:

	std::vector<Geometry*> objList;
	LeafNode(std::vector<Geometry*> _obj) : objList(_obj) {}

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
	~LeafNode();
};

struct Plane{
	int axis; //0 = x, 1 = y, 2 = z
	int position;
	int leftCount;
	int rightCount;
    BoundingBox* leftBBox; 
    BoundingBox* rightBBox; 
};

class kdTree
{
public:
    int depth;
    Node root;

    kdTree() : depth(0) {}
	//   use beginObjects() and scene->bounds() for initial call
	Node buildTree(std::vector<Geometry*> objList, 
		           BoundingBox* bbox, int depthLimit, int leafSize) {

		if (objList.size() <= leafSize || ++depth == depthLimit){ 
			return LeafNode(objList);	
		}
		std::vector<Geometry*> leftList;
		std::vector<Geometry*> rightList;
		
		Plane bestPlane = findBestSplitPlane(objList, bbox);

		for (std::vector<Geometry*>::iterator t = objList.begin(); 
			 t != objList.end(); ++t){

			 Geometry* obj = *t; 
			 BoundingBox obj_bbox = obj->getBoundingBox();
			 double min = obj_bbox.getMin()[bestPlane.axis];
             double max = obj_bbox.getMax()[bestPlane.axis];
             double N = length(obj->getNormal()); //not sure what N is supposed to be
        
			if (min < bestPlane.position)
				leftList.push_back(obj);
			if (max > bestPlane.position)
				rightList.push_back(obj);
			if ( bestPlane.position == max &&
				 bestPlane.position == min && N < 0) 
				leftList.push_back(obj);
			else if (bestPlane.position == max &&
					 bestPlane.position == min && N >= 0) 
				rightList.push_back(obj); 
		}
		

		if (rightList.empty() || leftList.empty()) 
			return LeafNode(objList);
		
		else return SplitNode(bestPlane.axis, bestPlane.position,
		buildTree(leftList, bestPlane.leftBBox, depth, leafSize),
		buildTree(rightList, bestPlane.rightBBox, depth, leafSize)); 
	}

	Plane findBestSplitPlane(std::vector<Geometry*> objList, BoundingBox* bbox){
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
		Plane tmp;

		return tmp;		
	}

};