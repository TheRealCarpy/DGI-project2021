#include "MyVector.h"

MyVector::MyVector(void)
{
	this->x = this->y = this->z = 0.0;
}

MyVector::MyVector(float x, float y, float z)
{
	this->x = x; this->y = y; this->z = z;
}

MyVector::MyVector(MyPosition& start, MyPosition& end)
{
	this->x = end.x - start.x;
	this->y = end.y - start.y;
	this->z = end.z - start.z;
}

MyVector MyVector::addTo(const MyVector &other) const
{
	MyVector result;
	//your code here
	return result;
}

MyVector MyVector::subtractFrom(const MyVector &other) const
{
	MyVector result;
	//your code here
	return result;
}
		
float MyVector::getMagnitude(void) const
{
	float result;
	//your code here
	return result;
}

void MyVector::uniformScale(const float m)
{
	//your code here
}

void MyVector::normalise(void)
{
	//your code here
}

void MyVector::setMagnitude(const float m)
{
	if (this->getMagnitude() > 0.0)
	{
		this->normalise();
		this->uniformScale(m);
	}
}
	
float MyVector::getDotProduct(const MyVector &other) const
{
	//return the dot product between this vector and other
	float result;
	//your code here
	result = (this->x * other.x) + (this->y * other.y) + (this->z * other.z);
	return result;
}

MyVector MyVector::getCrossProduct(const MyVector &other) const
{
	MyVector result;
	//your code here
	return result;
}

float MyVector::getDistance(const MyVector& other) const
{
    float result;
    result = sqrt(pow((this->x - other.x), 2) + pow((this->y - other.y), 2) + pow((this->z - other.z), 2));
    return result;
}

float MyVector::getDistanceXZ(const MyVector& other) const
{
    float result;
    result = sqrt(pow((this->x - other.x), 2) + pow((this->z - other.z), 2));
    return result;
}

MyVector MyVector::operator + ( const MyVector &other )
{
    MyVector vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x + other.x;
    vResult.y = y + other.y;
    vResult.z = z + other.z;

    return vResult;
}

MyVector MyVector::operator + ( void ) const
{
    return *this;
}

MyVector MyVector::operator - ( const MyVector &other )
{
    MyVector vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x - other.x;
    vResult.y = y - other.y;
    vResult.z = z - other.z;

    return vResult;
}

MyVector MyVector::operator - ( void ) const
{
    MyVector vResult(-x, -y, -z);

    return vResult;
}

MyVector MyVector::operator * ( const MyVector &other )
{
    MyVector vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x * other.x;
    vResult.y = y * other.y;
    vResult.z = z * other.z;

    return vResult;
}

MyVector MyVector::operator * ( const float scalar )
{
    MyVector vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x * scalar;
    vResult.y = y * scalar;
    vResult.z = z * scalar;

    return vResult;
}

MyVector operator * ( const float scalar, const MyVector &other )
{
    MyVector vResult(0.0f, 0.0f, 0.0f);

    vResult.x = other.x * scalar;
    vResult.y = other.y * scalar;
    vResult.z = other.z * scalar;

    return vResult;
}

MyVector MyVector::operator / ( const MyVector &other )
{
    MyVector vResult(0.0f, 0.0f, 0.0f);

    vResult.x = x / other.x;
    vResult.y = y / other.y;
    vResult.z = z / other.z;

    return vResult;
}

MyVector& MyVector::operator = ( const MyVector &other )
{
	x = other.x;
	y = other.y;
	z = other.z;

    return *this;
}

MyVector& MyVector::operator += ( const MyVector &other )
{
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

MyVector& MyVector::operator -= ( const MyVector &other )
{
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}