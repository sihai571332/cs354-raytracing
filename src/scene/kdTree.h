#pragma once

// Note: you can put kd-tree here

// struct kdNode{
// 	SplitNode;
// 	LeafNode;
// };

/*#include <vector>
#include <glm/vec3.hpp>
#include "ray.h"
#include "scene.h"
#include "bbox.h"
*/
//class KdTree;
class SplitNode;
class LeafNode;


class Node{
public:
    bool findIntersection(ray& r, isect& i, int t_min, int t_max);
    //Node();
    //virtual ~Node();
};

class SplitNode : public Node
{
public: 
    int axis;
    double position;
    Node left;
    Node right;

    SplitNode( int _axis,
    	       int _pos,
    	       Node _l,
    	       Node _r)
    	     : axis(_axis), position(_pos), left(_l), right(_r)  {}

    bool findIntersection(ray& r, isect& i, int t_min, int t_max){

        //fill in tmin and tmax here??    
	    if(r.getDirection()[axis] < 1e-6 && r.getDirection()[axis] > -1e-6 ){
            //calculate as near parallel()
            //add ray epsilon
        }
        else{
            // Not sure what ray position means here
            if(position > r.getPosition()[axis]){
                if(left.findIntersection(r, i, t_min, t_max))
                    return true;
            }
	    else if(position < r.getPosition()[axis]){
                 if(right.findIntersection(r, i, t_min, t_max))
                     return true;
	    }
            else{
                 if (left.findIntersection(r, i, t_min, t_max)) return true;
                 if (right.findIntersection(r, i, t_min, t_max)) return true;
            }
            return false;
         }
         return false;	
    }

    ~SplitNode() {};

};
class LeafNode : public Node
{
public:

    std::vector<Geometry*> objList;
    LeafNode(std::vector<Geometry*> _obj) : objList(_obj) {}

    bool findIntersection(ray& r, isect& i, int t_min, int t_max){

        for(std::vector<Geometry*>::iterator t = objList.begin(); 
            t != objList.end(); ++t){

            Geometry* obj = *t;
            isect curr;
            //Set t_min and t_max somewhere
	    if(obj->intersect(r, curr) && curr.t >= t_min && curr.t <= t_max ){
                i = curr;
                return true;
            }
        }
        return false;	
    }
    ~LeafNode() {};
};

struct Plane{
    int axis; //0 = x, 1 = y, 2 = z
    double position;
    int leftCount; 
    int rightCount;
    double leftBBoxArea;
    double rightBBoxArea;
    BoundingBox leftBBox; 
    BoundingBox rightBBox; 
};

template<typename T>
class KdTree
{ 
public:
    int depth;
    Node root;

    KdTree() : depth(0) {}
    KdTree(std::vector<Geometry*>& objList, 
        BoundingBox bbox, int depthLimit, int leafSize) {
        depth = 0;
        root = buildTree(objList, bbox, depthLimit, leafSize);
    }
    ~KdTree() {};

    //   use beginObjects() and scene->bounds() for initial call
    Node buildTree(std::vector<Geometry*> objList, 
                   BoundingBox bbox, int depthLimit, int leafSize) {

        if (objList.size() <= leafSize || ++depth == depthLimit){ 
            return LeafNode(objList);	
        }
        std::vector<Geometry*> leftList;
        std::vector<Geometry*> rightList;
		
        Plane bestPlane = findBestSplitPlane(objList, bbox);

        //printf("Axis: %d\n", bestPlane.axis );

        for (std::vector<Geometry*>::iterator t = objList.begin(); 
	        t != objList.end(); ++t){

	        Geometry* obj = *t; 
	        BoundingBox obj_bbox = obj->getBoundingBox();
	        double min = obj_bbox.getMin()[bestPlane.axis];
            double max = obj_bbox.getMax()[bestPlane.axis];
            double N = length(obj->getNormal());
            //not sure what N is supposed to be
        
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

    Plane findBestSplitPlane(std::vector<Geometry*> objList, BoundingBox bbox){

        std::vector<Plane> candidates;
        Plane bestPlane;
        Plane plane;

        for (int axis = 0 ; axis < 3; axis++) {
            for (std::vector<Geometry*>::iterator t = objList.begin(); 
                t != objList.end(); ++t){

                Geometry* obj = *t; 
                BoundingBox obj_bbox = obj->getBoundingBox();	
                Plane p1;
                Plane p2;
             
                p1.position = obj_bbox.getMin()[axis];
                p1.axis = axis;
                p1.leftBBox = BoundingBox(bbox.getMin(), bbox.getMax());
	            p1.leftBBox.setMax(axis, p1.position);
	            p1.rightBBox = BoundingBox(bbox.getMin(), bbox.getMax());
	            p1.rightBBox.setMin(axis, p1.position);


                p2.position = obj_bbox.getMax()[axis];
                p2.axis = axis;
                p2.leftBBox = BoundingBox(bbox.getMin(), bbox.getMax());
                p2.leftBBox.setMax(axis, p2.position);
                p2.rightBBox = BoundingBox(bbox.getMin(), bbox.getMax());
                p2.rightBBox.setMin(axis, p2.position);

                candidates.push_back(p1);
                candidates.push_back(p2);
            }
        }    
      /*  for (std::vector<Plane>::iterator v = candidates.begin();
             v!= candidates.end(); ++v) {

            plane = *v;

            plane.leftCount = countLeft(objList, plane);
            plane.leftBBoxArea = plane.leftBBox.area();
            plane.rightCount = countRight(objList, plane);
	        plane.rightBBoxArea = plane.rightBBox.area();
	    } */
        double minSam = 1e308;  	
	    for (std::vector<Plane>::iterator q = candidates.begin();
             q!= candidates.end(); ++q) {

            plane = *q;
            plane.leftCount = countLeft(objList, plane);
            plane.leftBBoxArea = plane.leftBBox.area();
            plane.rightCount = countRight(objList, plane);
            plane.rightBBoxArea = plane.rightBBox.area();
            //Why divide by "bounding box" ?
            //Surface area of node or objects in node??
            double SAM = (plane.leftCount * plane.leftBBoxArea + plane.rightCount
                         * plane.rightBBoxArea); 

            
            if (SAM < minSam){
                minSam = SAM;
                bestPlane = plane;
                //printf("SAM: %f\n", SAM );
                //printf("lc: %d\n", plane.leftCount );
                //printf("la: %f\n", plane.leftBBoxArea);
                

            }
        }		
	    return bestPlane;
    }
    int countLeft(std::vector<Geometry*> objList, Plane& plane){

        int count = 0;
        for (std::vector<Geometry*>::iterator t = objList.begin(); 
             t != objList.end(); ++t){

            Geometry* obj = *t;
            BoundingBox obj_bbox = obj->getBoundingBox();
            double min = obj_bbox.getMin()[plane.axis];

            if(min <  plane.position) count++;


        }	
        return count;

    }
    int countRight(std::vector<Geometry*> objList, Plane& plane){

        int count = 0;
        for (std::vector<Geometry*>::iterator t = objList.begin(); 
             t != objList.end(); ++t){

            Geometry* obj = *t;
            BoundingBox obj_bbox = obj->getBoundingBox();
            double max = obj_bbox.getMax()[plane.axis];

            if(max >  plane.position) count++;


        }	
        return count;
    }


};
