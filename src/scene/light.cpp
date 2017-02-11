#include <cmath>

#include "light.h"
#include <glm/glm.hpp>


using namespace std;

double DirectionalLight::distanceAttenuation(const glm::dvec3& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


glm::dvec3 DirectionalLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	//return glm::dvec3(1,1,1);
	isect i;
	ray shadowRay (p, -orientation, r.pixel, r.ctr, r.atten, ray::SHADOW);

	if(scene->intersect(shadowRay, i)){
		const Material& m = i.getMaterial();
		glm::dvec3 kt = m.kt(i);
		if(length(kt)!=0.0) return glm::dvec3(1,1,1);

		return glm::dvec3(0,0,0);
	}else return glm::dvec3(1,1,1);
}

glm::dvec3 DirectionalLight::getColor() const
{
	return color;
}

glm::dvec3 DirectionalLight::getDirection(const glm::dvec3& P) const
{
	return -orientation;
}

double PointLight::distanceAttenuation(const glm::dvec3& P) const
{

	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, we assume no attenuation and just return 1.0

	double l = normalize(position - P).size();

	double att = min(1.0, 1 / (constantTerm + linearTerm*l + quadraticTerm*(pow(l, 2))));

	return att;
}

glm::dvec3 PointLight::getColor() const
{
	return color;
}

glm::dvec3 PointLight::getDirection(const glm::dvec3& P) const
{
	return glm::normalize(position - P);
}


glm::dvec3 PointLight::shadowAttenuation(const ray& r, const glm::dvec3& p) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	isect i;

	//get position of light source.
	glm::dvec3 lightD = normalize(position - p);

	//Make new ray with position, direction and type.
	ray shadowRay (p, lightD, r.pixel, r.ctr, r.atten, ray::SHADOW);

	//Check to see if this ray intersects with light source.
	if(scene->intersect(shadowRay, i)){
		if(length(shadowRay.at(i.t) - p) > length(position - p))
            return glm::dvec3(1,1,1);
	    const Material& m = i.getMaterial();
		glm::dvec3 kt = m.kt(i);
		if(length(kt)!= 0.0) return glm::dvec3(1,1,1);
		//return atten = 0

		return glm::dvec3(0,0,0);
	}else return glm::dvec3(1,1,1);
	
}

