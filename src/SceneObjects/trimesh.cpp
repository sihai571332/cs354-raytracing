#include <cmath>
#include <float.h>
#include <algorithm>
#include <assert.h>
#include <string.h>
#include "trimesh.h"
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

using namespace std;

Trimesh::~Trimesh()
{
	for( Materials::iterator i = materials.begin(); i != materials.end(); ++i )
		delete *i;
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex( const glm::dvec3 &v )
{
    vertices.push_back( v );
}

void Trimesh::addMaterial( Material *m )
{
    materials.push_back( m );
}

void Trimesh::addNormal( const glm::dvec3 &n )
{
    normals.push_back( n );
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace( int a, int b, int c )
{
    int vcnt = vertices.size();

    if( a >= vcnt || b >= vcnt || c >= vcnt ) return false;

    TrimeshFace *newFace = new TrimeshFace( scene, new Material(*this->material), this, a, b, c );
    newFace->setTransform(this->transform);
    if (!newFace->degen) faces.push_back( newFace );


    // Don't add faces to the scene's object list so we can cull by bounding box
    // scene->add(newFace);
    return true;
}

const char* Trimesh::doubleCheck()
// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
{
    if( !materials.empty() && materials.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of materials.";
    if( !normals.empty() && normals.size() != vertices.size() )
        return "Bad Trimesh: Wrong number of normals.";

    return 0;
}

bool Trimesh::intersectLocal(ray& r, isect& i) const
{
	typedef Faces::const_iterator iter;
	bool have_one = false;
	for( iter j = faces.begin(); j != faces.end(); ++j )
	{
		isect cur;
		if( (*j)->intersectLocal( r, cur ) )
		{
			if( !have_one || (cur.t < i.t) )
			{
				i = cur;
				have_one = true;
			}
		}
	}
	if( !have_one ) i.setT(1000.0);
	return have_one;
} 

bool TrimeshFace::intersect(ray& r, isect& i) const {
  return intersectLocal(r, i);
}

// Intersect ray r with the triangle abc.  If it hits returns true,
// and put the parameter in t and the barycentric coordinates of the
// intersection in u (alpha) and v (beta).
bool TrimeshFace::intersectLocal(ray& r, isect& i) const
{
    // YOUR CODE HERE
    // 
    // FIXME: Add ray-trimesh intersection
    ///////////////////

     //Check if ray r intersects the plane
    glm::dvec3 N;
    glm::dvec3 P = r.getPosition();
    glm::dvec3 d = r.getDirection();
    glm::dvec3 v0 = parent->vertices[ids[0]]; //a
    glm::dvec3 v1 = parent->vertices[ids[1]]; //b
    glm::dvec3 v2 = parent->vertices[ids[2]]; //c

    glm::dvec3 p0 = v1;

    N = cross((v1 - v0),(v2 - v0)); 

    glm::dvec3 origin1(0.0, 0.0, 0.0);
    float denom = dot(d, N);
    double t = 0.0;

    if (denom < 1e-6){
        glm::dvec3 p0l0 = p0 - P;
        t = dot(p0l0, N)/ denom;
        if(t<0) return false;
        //printf("detected intersection\n");
    }
    else return false;

    glm::dvec3 ii = r.at(t); // r.getPosition() + r.getDirection() * t ;
    
    //Check if it intersects the triangle
    //Test each edge

    glm::dvec3 Bary;
    // --- Edge AB ---
    glm::dvec3 AB = v1 - v0; 
    glm::dvec3 AP = ii - v0;
    glm::dvec3 C = cross(AB, AP);
    if (dot(N, C) < 0) return false;
    //printf("detected intersection 1\n");

    //--- BC ---
    glm::dvec3 BC = v2 - v1; 
    glm::dvec3 BP = ii - v1;
    C = cross(BC, BP);
    double u = length(C) / length(N);
    if (dot(N, C) < 0) return false;
    //printf("detected intersection 2\n");

    //--- CA --- .
    glm::dvec3 CA = v0 - v2; 
    glm::dvec3 CP = ii - v2;
    C = cross(CA, CP);
    double v = length(C) / length(N);
    if (dot(N, C) < 0) return false;
    //printf("detected intersection 3\n");

    i.obj = this;
    
    i.t = t;
    i.N = N; //true normal
    
    //Set material correctly
    if (!parent->materials.empty()){
        Material m0 = *(parent->materials[ids[0]]);
        Material m1 = *(parent->materials[ids[1]]);
        Material m2 = *(parent->materials[ids[2]]);
        i.setMaterial(m0);
        //Create a new material out of 
        //the vertex materials here?
        
    }
    else i.setMaterial(this->getMaterial());

    //Interpolate normal 

    if (traceUI->smShadSw() && !parent->normals.empty()){
        glm::dvec3 phong_n(0.0, 0.0, 0.0);
        glm::dvec3 n0 = parent->normals[ids[0]]; //a
        glm::dvec3 n1 = parent->normals[ids[1]]; //b
        glm::dvec3 n2 = parent->normals[ids[2]]; //c

        Bary.x = u;
        Bary.y = v; 
        Bary.z = 1.0 - u - v;
        i.setBary(Bary);

        phong_n = (Bary.x * n0) + (Bary.y * n1) + (Bary.z * n2);
        i.N = normalize(phong_n);

    }

    return true;
}

void Trimesh::generateNormals()
// Once you've loaded all the verts and faces, we can generate per
// vertex normals by averaging the normals of the neighboring faces.
{
    int cnt = vertices.size();
    normals.resize( cnt );
    int *numFaces = new int[ cnt ]; // the number of faces assoc. with each vertex
    memset( numFaces, 0, sizeof(int)*cnt );
    
    for( Faces::iterator fi = faces.begin(); fi != faces.end(); ++fi )
    {
		glm::dvec3 faceNormal = (**fi).getNormal();
        
        for( int i = 0; i < 3; ++i )
        {
            normals[(**fi)[i]] += faceNormal;
            ++numFaces[(**fi)[i]];
        }
    }

    for( int i = 0; i < cnt; ++i )
    {
        if( numFaces[i] )
            normals[i]  /= numFaces[i];
    }

    delete [] numFaces;
    vertNorms = true;
}

