#include "cubeMap.h"
#include "ray.h"
#include "../ui/TraceUI.h"
extern TraceUI* traceUI;

glm::dvec3 CubeMap::getColor(ray r) const {


	// YOUR CODE HERE
	// FIXME: Implement Cube Map here
	bool faceX = false, faceY = false, faceZ = false;
	double x = r.getDirection().x;
	double y = r.getDirection().y;
	double z = r.getDirection().z;

	// Test Which face it is facing.
	if(abs(x)>=abs(y) && abs(x)>=abs(z)){
		faceX = true;
	}else if(abs(y)>=abs(x) && abs(y)>=abs(z)){
		faceY = true;
	}else{
		faceZ = true;
	}

	glm::dvec2 test (0.0, 0.0); 


	// Returns the tested value from getMappedValue

	if(faceX && x >= 0){
		test.x = -z;
		test.y = y;
		
		return tMap[0]->getMappedValue(test);
	}else if(faceX && x < 0){
		test.x = z;
		test.y = y;
		return tMap[1]->getMappedValue(test);
	}else if(faceY && y >= 0){
		test.x = x;
		test.y = -z;
		return tMap[2]->getMappedValue(test);
	}else if(faceY && y < 0){
		test.x = x;
		test.y = z;
		return tMap[3]->getMappedValue(test);
	}else if(faceZ && z >= 0){
		test.x = x;
		test.y = y;
		return tMap[4]->getMappedValue(test);
	}else if(faceZ && z < 0){
		test.x = -x;
		test.y = y;
		return tMap[5]->getMappedValue(test);
	}else{
		return glm::dvec3 (1, 1, 1);
	}


}
