#ifndef _QUATERNION_H
#define _QUATERNION_H


class quaternion
{
    public:

        const static float ROUNDING_ERROR_FLOAT = 0.000001f;

        // Default constructor
        quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) { }

        quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }

        // Cnstructor which converts Euler angles (radians) to a quaternion
		quaternion(float x, float y, float z);

        // Constructor which converts Euler angles (radians) to a quaternion
		quaternion(const Vector3& vec);

		// Constructor which rotates around a given axis (from 3D Software Rendere BennyBox)
        quaternion(const Vector4 &axis, float angle);

        virtual ~quaternion() { };

        //////////////////////
        //////////////////////


        //! Equality operator
		bool operator==(const quaternion& other) const;

		//! inequality operator
		bool operator!=(const quaternion& other) const;

		//! Assignment operator
		inline quaternion& operator=(const quaternion& other);

        //! Add operator
		quaternion operator+(const quaternion& other) const;

		//! Multiplication operator
		//! Be careful, unfortunately the operator order here is opposite of that in CMatrix4::operator*
		quaternion operator*(const quaternion& other) const;

		//! Multiplication operator with scalar
		quaternion operator*(float s) const;

		//! Multiplication operator with scalar
		quaternion& operator*=(float s);

		//! Multiplication operator
		quaternion& operator*=(const quaternion& other);

		//! Multiplication operator
		Vector3 operator*(const Vector3& v) const;


		//////////////////////
        //////////////////////

		//! Sets new quaternion component values
		inline quaternion& set(float x, float y, float z, float w);

        //! Sets new quaternion based on Euler angles (radians)
		inline quaternion& set(float x, float y, float z);

		//! Sets new quaternion based on Euler angles (radians)
		inline quaternion& set(const Vector3& vec);

				//! Sets new quaternion from other quaternion
		inline quaternion& set(const quaternion& quat);

		// rotates around a given axis (from 3D Software Rendere BennyBox)
		inline quaternion& rotate(const Vector4 &axis, float angle);

				//! returns if this quaternion equals the other one, taking floating point rounding errors into account
		inline bool equals(const quaternion& other,
				const float tolerance = ROUNDING_ERROR_FLOAT ) const;

				//! Normalizes the quaternion
		inline quaternion& normalize();

		//! Creates a matrix from this quaternion
		void getMatrix( Matrix4 &dest, const Vector3 &translation=Vector3() ) const;

		void getMatrixCenter( Matrix4 &dest, const Vector3 &center, const Vector3 &translation ) const;


    //protected:

    //private:
        float x;
        float y;
        float z;
        float w;
};


// Constructor which converts Euler angles to a quaternion
inline quaternion::quaternion(float x, float y, float z)
{
	set(x,y,z);
}



// Constructor which rotates around a given axis (from 3D Software Rendere BennyBox)
inline quaternion::quaternion(const Vector4 &axis, float angle)
{
    float sinHalfAngle = (float) sin(angle) / 2.0;
    float cosHalfAngle = (float) cos(angle) / 2.0;

    x = axis.x * sinHalfAngle;
    y = axis.y * sinHalfAngle;
    z = axis.z * sinHalfAngle;
    w = cosHalfAngle;
}


/////////////////////////////
// OPERATOR OVERLOADING
/////////////////////////////


// equal operator
inline bool quaternion::operator==(const quaternion& other) const
{
	return ((x == other.x) &&
		(y == other.y) &&
		(z == other.z) &&
		(w == other.w));
}

// inequality operator
inline bool quaternion::operator!=(const quaternion& other) const
{
	return !(*this == other);
}

// assignment operator
inline quaternion& quaternion::operator=(const quaternion& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;
	return *this;
}


// add operator
inline quaternion quaternion::operator+(const quaternion& b) const
{
	return quaternion(x+b.x, y+b.y, z+b.z, w+b.w);
}

// multiplication operator
inline quaternion quaternion::operator*(const quaternion& other) const
{
	quaternion tmp;

	tmp.w = (other.w * w) - (other.x * x) - (other.y * y) - (other.z * z);
	tmp.x = (other.w * x) + (other.x * w) + (other.y * z) - (other.z * y);
	tmp.y = (other.w * y) + (other.y * w) + (other.z * x) - (other.x * z);
	tmp.z = (other.w * z) + (other.z * w) + (other.x * y) - (other.y * x);

	return tmp;
}


// multiplication operator
inline quaternion quaternion::operator*(float s) const
{
	return quaternion(s*x, s*y, s*z, s*w);
}


// multiplication operator
inline quaternion& quaternion::operator*=(float s)
{
	x*=s;
	y*=s;
	z*=s;
	w*=s;
	return *this;
}

// multiplication operator
inline quaternion& quaternion::operator*=(const quaternion& other)
{
	return (*this = other * (*this));
}


inline Vector3 quaternion::operator* (const Vector3& v) const
{
	// nVidia SDK implementation

	Vector3 uv, uuv;
	Vector3 qvec(x, y, z);
	uv = qvec.cross(v);
	uuv = qvec.cross(uv);
	uv *= (2.0f * w);
	uuv *= 2.0f;

	return v + uv + uuv;
}


////////////////////////////////
// METHODS
///////////////////////////////


// sets new quaternion
inline quaternion& quaternion::set(float x, float y, float z, float w)
{
	x = x;
	y = y;
	z = z;
	w = w;
	return *this;
}


// sets new quaternion based on Euler angles
inline quaternion& quaternion::set(float x, float y, float z)
{
	double angle;

	angle = x * 0.5;
	const double sr = sin(angle);
	const double cr = cos(angle);

	angle = y * 0.5;
	const double sp = sin(angle);
	const double cp = cos(angle);

	angle = z * 0.5;
	const double sy = sin(angle);
	const double cy = cos(angle);

	const double cpcy = cp * cy;
	const double spcy = sp * cy;
	const double cpsy = cp * sy;
	const double spsy = sp * sy;

	x = (float)(sr * cpcy - cr * spsy);
	y = (float)(cr * spcy + sr * cpsy);
	z = (float)(cr * cpsy - sr * spcy);
	w = (float)(cr * cpcy + sr * spsy);

	return normalize();
}


// sets new quaternion based on Euler angles
inline quaternion& quaternion::set(const Vector3& vec)
{
	return set(vec.x, vec.y, vec.z);
}


// sets new quaternion based on other quaternion
inline quaternion& quaternion::set(const quaternion& quat)
{
	return (*this=quat);
}



inline quaternion& quaternion::rotate(const Vector4 &axis, float angle) {

    float sinHalfAngle = (float) sin(angle) / 2.0;
    float cosHalfAngle = (float) cos(angle) / 2.0;

    x = axis.x * sinHalfAngle;
    y = axis.y * sinHalfAngle;
    z = axis.z * sinHalfAngle;
    w = cosHalfAngle;

    return *this;
}

//! returns if this quaternion equals the other one, taking floating point rounding errors into account
inline bool quaternion::equals(const quaternion& other, const float tolerance) const
{
	return x+tolerance >= other.x &&  x-tolerance <= other.x &&
		y+tolerance >= other.y &&  y-tolerance <= other.y &&
		z+tolerance >= other.z &&  z-tolerance <= other.z &&
		w+tolerance >= other.w &&  w-tolerance <= other.w;
}


// normalizes the quaternion
inline quaternion& quaternion::normalize()
{
	const float n = x*x + y*y + z*z + w*w;

	if (n == 1)
		return *this;

    //!  TODO: Replace with fast sqrt (approximation)
	//n = 1.0f / sqrtf(n);
	return (*this *= 1.f/sqrt(n));
}


/*!
	Creates a matrix from this quaternion
*/
inline void quaternion::getMatrix(Matrix4 &dest,
		const Vector3 &center) const
{
    // TODO: Set directly?
    float m[16];

	m[0] = 1.0f - 2.0f*y*y - 2.0f*z*z;
	m[1] = 2.0f*x*y + 2.0f*z*w;
	m[2] = 2.0f*x*z - 2.0f*y*w;
	m[3] = 0.0f;

	m[4] = 2.0f*x*y - 2.0f*z*w;
	m[5] = 1.0f - 2.0f*x*x - 2.0f*z*z;
	m[6] = 2.0f*z*y + 2.0f*x*w;
	m[7] = 0.0f;

	m[8] = 2.0f*x*z + 2.0f*y*w;
	m[9] = 2.0f*z*y - 2.0f*x*w;
	m[10] = 1.0f - 2.0f*x*x - 2.0f*y*y;
	m[11] = 0.0f;

	m[12] = center.x;
	m[13] = center.y;
	m[14] = center.z;
	m[15] = 1.f;

	std::cout << "x " << x << " y " << y << std::endl;

	dest.set(m);

	// TODO: is set in Irrlicht
	//dest.setDefinitelyIdentityMatrix ( false );
}



/*!
	Creates a matrix from this quaternion
	Rotate about a center point
	shortcut for
	core::quaternion q;
	q.rotationFromTo(vin[i].Normal, forward);
	q.getMatrix(lookat, center);

	core::matrix4 m2;
	m2.setInverseTranslation(center);
	lookat *= m2;
*/
inline void quaternion::getMatrixCenter(Matrix4 &dest,
					const Vector3 &center,
					const Vector3 &translation) const
{
    // TODO: Set directly?
    float m[16];

	m[0] = 1.0f - 2.0f*y*y - 2.0f*z*z;
	m[1] = 2.0f*x*y + 2.0f*z*w;
	m[2] = 2.0f*x*z - 2.0f*y*w;
	m[3] = 0.0f;

	m[4] = 2.0f*x*y - 2.0f*z*w;
	m[5] = 1.0f - 2.0f*x*x - 2.0f*z*z;
	m[6] = 2.0f*z*y + 2.0f*x*w;
	m[7] = 0.0f;

	m[8] = 2.0f*x*z + 2.0f*y*w;
	m[9] = 2.0f*z*y - 2.0f*x*w;
	m[10] = 1.0f - 2.0f*x*x - 2.0f*y*y;
	m[11] = 0.0f;

	m[12] = -m[0]*center.x - m[4]*center.y - m[8]*center.z + (center.x - translation.x );
    m[13] = -m[1]*center.x - m[5]*center.y - m[9]*center.z + (center.y - translation.y );
    m[14] = -m[2]*center.x - m[6]*center.y - m[10]*center.z + (center.z - translation.z );
    m[15] =  1.0f;

    dest.set(m);
	//dest.setRotationCenter ( center, translation );
}




#endif // _QUATERNION_H
