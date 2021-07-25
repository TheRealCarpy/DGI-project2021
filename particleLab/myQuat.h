//Introductory quaternion class

#include <math.h>
#include "myvector.h"
#include "mymatrix.h"

class MyQuat
{
	public:
		MyQuat(void);
		MyQuat(float angleDeg, MyVector &axis);	//create from axis, angle
		MyQuat(MyPosition &p);	//create from position
		
		~MyQuat(void){;}

		MyQuat addTo(const MyQuat &other) const;
		MyQuat multiplyBy(const MyQuat &other) const;

		float getMagnitude(void) const;	
		void normalise(void);
		MyQuat getConjugate(void) const;	
		MyQuat getInverse(void) const;

		MyMatrix convertToRotationMatrix(void) const; 

		float w;
		MyVector v;
};
