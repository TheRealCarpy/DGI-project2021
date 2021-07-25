#include "MyQuat.h"

MyQuat::MyQuat() {}

MyQuat::MyQuat(float angleDeg, MyVector& axis) {	//create from axis, angle
	this->w = cos(angleDeg / 2);
	this->v = axis;

	this->v.x *= sin(angleDeg / 2);
	this->v.y *= sin(angleDeg / 2);
	this->v.z *= sin(angleDeg / 2);
}

MyQuat::MyQuat(MyPosition& p) {	//create from position
	this->v.x = p.x;
	this->v.y = p.y;
	this->v.z = p.z;
}

MyQuat MyQuat::addTo(const MyQuat& other) const {
	MyQuat result;
	result.v.x = this->v.x + other.v.x;
	result.v.y = this->v.y + other.v.y;
	result.v.z = this->v.z + other.v.z;
	result.w = this->w + other.w;
	return result;
}

MyQuat MyQuat::multiplyBy(const MyQuat& other) const {
	MyQuat result;
	result.w = (this->w * other.w) - this->v.getDotProduct(other.v);
	MyVector dot;
	result.v.x = (this->w * other.v.x) + (other.w * this->v.x) + dot.x;
	result.v.y = (this->w * other.v.y) + (other.w * this->v.y) + dot.y;
	result.v.z = (this->w * other.v.z) + (other.w * this->v.z) + dot.z;
	return result;
}

float MyQuat::getMagnitude() const {
	return sqrt(pow(this->w, 2) + pow(this->v.x, 2) + pow(this->v.y, 2) + pow(this->v.z, 2));
}

void MyQuat::normalise() {
	float m = getMagnitude();
	this->w /= m;
	this->v.x /= m;
	this->v.y /= m;
	this->v.z /= m;
}

MyQuat MyQuat::getConjugate() const {
	MyQuat result;
	result.v.uniformScale(-1);
	return result;
}

MyQuat MyQuat::getInverse(void) const {
	MyQuat result = getConjugate();
	float m = getMagnitude();
	result.w /= m;
	result.v.x /= m;
	result.v.y /= m;
	result.v.z /= m;
	return result;
}

MyMatrix MyQuat::convertToRotationMatrix(void) const {
	MyMatrix matrix;
	matrix.set_element(0, (1 - 2 * pow(this->v.y, 2) - 2 * pow(this->v.z, 2)));
	matrix.set_element(1, (2 * this->v.x * this->v.y + 2 * this->v.z * this->w));
	matrix.set_element(2, (2 * this->v.x * this->v.z - 2 * this->v.y * this->w));
	matrix.set_element(3, 0);
	matrix.set_element(4, (2 * this->v.x * this->v.y - 2 * this->v.z * this->w));
	matrix.set_element(5, (1 - 2 * pow(this->v.x, 2) - 2 * pow(this->v.z, 2)));
	matrix.set_element(6, (2 * this->v.y * this->v.z - 2 * this->v.x * this->w));
	matrix.set_element(7, 0);
	matrix.set_element(8, (2 * this->v.x * this->v.z - 2 * this->v.y * this->w));
	matrix.set_element(9, (2 * this->v.y * this->v.z - 2 * this->v.x * this->w));
	matrix.set_element(10, (1 - 2 * pow(this->v.x, 2) - 2 * pow(this->v.y, 2)));
	matrix.set_element(11, 0);
	matrix.set_element(12, 0);
	matrix.set_element(13, 0);
	matrix.set_element(14, 0);
	matrix.set_element(15, 1);

	return matrix;
}

