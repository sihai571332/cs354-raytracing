#pragma once

// Note: you can put kd-tree here

// struct kdNode{
//  SplitNode;
//  LeafNode;
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
    virtual bool findIntersection(ray& r, isect& i, double t_min, double t_max) = 0;
    //Node();
    //virtual ~Node();
};


class SplitNode : public Node
{
public: 
    int axis;
    double position;
    BoundingBox nbox;
    Node* left;
    Node* right;

    SplitNode( int _axis,
               int _pos,
               BoundingBox _b,
               Node* _l,
               Node* _r)
             : axis(_axis), position(_pos), nbox(_b), left(_l), right(_r)  {}

    bool findIntersection(ray& r, isect& i, double t_min, double t_max){

        nbox.intersect(r, t_min, t_max);
        //Use these instead of r.getPosition()????
        glm::dvec3 pos_min = r.at(t_min);
        glm::dvec3 pos_max = r.at(t_max);

        if(r.getDirection()[axis] < 1e-6 && r.getDirection()[axis] > -1e-6 ){
            //calculate as near parallel()
            //add ray epsilon
            if(position > (pos_min[axis] + RAY_EPSILON) && position > (pos_max[axis] + RAY_EPSILON)){
                if(left->findIntersection(r, i, t_min, t_max))
                    return true;
            }
            else if(position < (pos_min[axis] + RAY_EPSILON) && position < (pos_max[axis]+ RAY_EPSILON)){
                if(right->findIntersection(r, i, t_min, t_max))
                     return true;
            }
            else{
                if (left->findIntersection(r, i, t_min, t_max)) return true;
                if (right->findIntersection(r, i, t_min, t_max)) return true;
            }
            return false;
        }
        else{
            // Not sure what ray position means here
            if(position > pos_min[axis] && position > pos_max[axis]){
                if(left->findIntersection(r, i, t_min, t_max))
                    return true;
            }
            else if(position < pos_min[axis] && position < pos_max[axis]){
                if(right->findIntersection(r, i, t_min, t_max))
                     return true;
            }
            else{
                    if (left->findIntersection(r, i, t_min, t_max)) return true;
                    if (right->findIntersection(r, i, t_min, t_max)) return true;
            }
            return false;
         }
         return false;  
    }

    ~SplitNode() {
        delete right;
        delete left;
    };

};
class LeafNode : public Node
{
public:

    std::vector<Geometry*> objList;
    LeafNode(std::vector<Geometry*> _obj) : objList(_obj) {}

    bool findIntersection(ray& r, isect& i, double t_min, double t_max){
        bool found = false;
        i.setT(1e13);

        for(std::vector<Geometry*>::iterator t = objList.begin(); 
            t != objList.end(); ++t){

            Geometry* obj = *t;
            BoundingBox obj_bbox = obj->getBoundingBox();
            obj_bbox.intersect(r, t_min, t_max);
            isect curr;

            if(obj->intersect(r, curr) && curr.t < i.t && curr.t >= t_min && curr.t <= t_max){
                i = curr;
                found = true;
            }
        }
        if (found) return true;
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
    Node* root;

    KdTree() : depth(0) {}
    KdTree(std::vector<Geometry*>& objList, 
        BoundingBox bbox, int depthLimit, int leafSize) {
        depth = 0;
        root = buildTree(objList, bbox, depthLimit, leafSize);
    }
    ~KdTree() {
        delete root;
    };

    //   use beginObjects() and scene->bounds() for initial call
    Node* buildTree(std::vector<Geometry*> objList, 
                   BoundingBox bbox, int depthLimit, int leafSize) {

        if (objList.size() <= leafSize || ++depth == depthLimit){ 
            return new LeafNode(objList);   
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
            return new LeafNode(objList);
        
        else return new SplitNode(bestPlane.axis, bestPlane.position, bbox,
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
                         * plane.rightBBoxArea)/bbox.area(); 

            
            if (SAM < minSam){
                minSam = SAM;
                bestPlane = plane;
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
