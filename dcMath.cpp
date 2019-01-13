#include "dcMath.h"

float dcMath::Magnitude(const glm::vec2& vector) {
	return sqrtf((vector.x * vector.x) + (vector.y * vector.y));
}

glm::vec2 dcMath::Normalize(glm::vec2 vector) {
	float length = Magnitude(vector);

	if (length == 0) return glm::vec2(0, 0);

	vector.x = vector.x / length;
	vector.y = vector.y / length;

	return vector;
}

void dcMath::Limit(glm::vec2& vector, float limit) {
	if (Magnitude(vector) > limit) {
		vector = Normalize(vector);
		vector.x *= limit;
		vector.y *= limit;
	}
}

float dcMath::Dot(glm::vec2 v1, glm::vec2 v2) {
	return v1.x*v2.x + v1.y*v2.y;
}

float dcMath::Dot(sf::Vector3f v1, sf::Vector3f v2) {
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

float dcMath::AngleBetween(glm::vec2 v1, glm::vec2 v2) {
	return acosf(Dot(v1, v2) / (Magnitude(v1) * Magnitude(v2)));
}

//Returns angle in radians
float dcMath::VectorToAngle(const glm::vec2& vector) {
	glm::vec2 v = Normalize(vector);
	float result = atan2f(v.y, v.x);
	//result = result * (float)(180 / M_PI);
	return result;
}

glm::vec2 dcMath::AngleToVector(float angle) {
	return glm::vec2(cosf(angle), sinf(angle));
}

float dcMath::Map(float value, float start1, float  stop1, float  start2, float stop2) {
	float slope = (stop2 - start2) / (stop1 - start1);
	return start2 + slope * (value - start1);
}

glm::vec3 dcMath::ForwardVector(glm::quat rotation) {
	glm::vec3 forward;
	forward.x = 2 * (rotation.x*rotation.y + rotation.w*rotation.y);
	forward.y = 2 * (rotation.y*rotation.z + rotation.w*rotation.x);
	forward.z = 1 - 2 * (rotation.x*rotation.x - rotation.y*rotation.y);
	return forward;
}

glm::vec3 dcMath::LeftVector(glm::quat rotation) {
	glm::vec3 left;
	left.x = 1 - 2 * (rotation.y*rotation.y - rotation.z*rotation.z);
	left.y = 2 * (rotation.x*rotation.y + rotation.w*rotation.z);
	left.z = 2 * (rotation.x*rotation.z + rotation.w*rotation.y);
	return left;
}

glm::vec3 dcMath::UpVector(glm::quat rotation) {
	glm::vec3 up;
	up.x = 2 * (rotation.x*rotation.y + rotation.w*rotation.z);
	up.y = 1 - 2 * (rotation.x*rotation.x - rotation.z*rotation.z);
	up.z = 2 * (rotation.y*rotation.z + rotation.w*rotation.x);
	return up;
}