/*********************************************************\
 *  File: Vector2.h                                      *
 *
 *  Copyright (C) 2002-2011 The PixelLight Team (http://www.pixellight.org/)
 *
 *  This file is part of PixelLight.
 *
 *  PixelLight is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PixelLight is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with PixelLight. If not, see <http://www.gnu.org/licenses/>.
\*********************************************************/


#ifndef __PLMATH_VECTOR2_H__
#define __PLMATH_VECTOR2_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLGeneral/String/String.h>
#include "PLMath/PLMath.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLMath {


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
class Vector3;
class Vector4;
class Matrix3x3;
class Matrix3x4;
class Matrix4x4;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    2D vector
*/
class Vector2 {


	//[-------------------------------------------------------]
	//[ Public definitions                                    ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Vector component
		*/
		enum Component {
			X = 0,	/**< X component */
			Y = 1	/**< Y component */
		};


	//[-------------------------------------------------------]
	//[ Public static data                                    ]
	//[-------------------------------------------------------]
	public:
		PLMATH_API static const Vector2 Zero;			/**<  0.0,  0.0 */
		PLMATH_API static const Vector2 One;			/**<  1.0,  1.0 */
		PLMATH_API static const Vector2 NegativeOne;	/**< -1.0, -1.0 */
		PLMATH_API static const Vector2 UnitX;			/**<  1.0,  0.0 */
		PLMATH_API static const Vector2 UnitY;			/**<  0.0,  1.0 */
		PLMATH_API static const Vector2 NegativeUnitX;	/**< -1.0,  0.0 */
		PLMATH_API static const Vector2 NegativeUnitY;	/**<  0.0, -1.0 */


	//[-------------------------------------------------------]
	//[ Public data                                           ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Some direct vertex element accesses
		*/
		union {
			/**
			*  @brief
			*    Vertex element array access
			*/
			float fV[2];

			/**
			*  @brief
			*    Known vertex element names when dealing with positions or directions
			*/
			struct {
				float x, y;
			};

			/**
			*  @brief
			*    Known vertex element names when dealing with texture coordinates
			*/
			struct {
				float u, v;
			};
		};


	//[-------------------------------------------------------]
	//[ Public functions                                      ]
	//[-------------------------------------------------------]
	public:
		//[-------------------------------------------------------]
		//[ Constructors                                          ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Default constructor setting all components to 0
		*/
		inline Vector2();

		inline Vector2(float fX, float fY);
		inline Vector2(const float fV[]);
		inline Vector2(const Vector2 &vV);
		PLMATH_API Vector2(const Vector3 &vV);
		PLMATH_API Vector2(const Vector4 &vV);
		inline Vector2(const PLGeneral::String &sString);

		//[-------------------------------------------------------]
		//[ Destructor                                            ]
		//[-------------------------------------------------------]
		inline ~Vector2();

		//[-------------------------------------------------------]
		//[ Assignment operators                                  ]
		//[-------------------------------------------------------]
		inline     Vector2 &operator =(const float fV[]);
		inline     Vector2 &operator =(const Vector2 &vV);
		PLMATH_API Vector2 &operator =(const Vector3 &vV);
		PLMATH_API Vector2 &operator =(const Vector4 &vV);

		//[-------------------------------------------------------]
		//[ Comparison                                            ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Compares two vectors
		*
		*  @param[in] vV
		*    Vector to compare with
		*
		*  @return
		*    'true' if all components are equal, else 'false'
		*/
		inline bool operator ==(const Vector2 &vV) const;

		/**
		*  @brief
		*    Compares two vectors
		*
		*  @param[in] vV
		*    Vector to compare with
		*
		*  @return
		*    'false' if all components are equal, else 'true'
		*/
		inline bool operator !=(const Vector2 &vV) const;

		/**
		*  @brief
		*    Compares two vectors
		*
		*  @param[in] vV
		*    Vector to compare with
		*
		*  @return
		*    'true' if ALL components of this vector are greater, else 'false'
		*/
		inline bool operator >(const Vector2 &vV) const;

		/**
		*  @brief
		*    Compares two vectors
		*
		*  @param[in] vV
		*    Vector to compare with
		*
		*  @return
		*    'true' if ALL components of this vector are less, else 'false'
		*/
		inline bool operator <(const Vector2 &vV) const;

		/**
		*  @brief
		*    Compares two vectors
		*
		*  @param[in] vV
		*    Vector to compare with
		*
		*  @return
		*    'true' if ALL components of this vector are greater or equal, else 'false'
		*/
		inline bool operator >=(const Vector2 &vV) const;

		/**
		*  @brief
		*    Compares two vectors
		*
		*  @param[in] vV
		*    Vector to compare with
		*
		*  @return
		*    'true' if ALL components of this vector are less or equal, else 'false'
		*/
		inline bool operator <=(const Vector2 &vV) const;

		//[-------------------------------------------------------]
		//[ Vector                                                ]
		//[-------------------------------------------------------]
		inline Vector2  operator +(const Vector2 &vV) const;
		inline Vector2  operator +(float fN) const;
		inline Vector2 &operator +=(const Vector2 &vV);
		inline Vector2 &operator +=(float fN);
		inline Vector2  operator -() const;
		inline Vector2  operator -(float fN) const;
		inline Vector2  operator -(const Vector2 &vV) const;
		inline Vector2 &operator -=(const Vector2 &vV);
		inline Vector2 &operator -=(float fN);

		/**
		*  @brief
		*    Per component multiplication
		*
		*  @param[in] vV
		*    Vector to multiplicate with
		*
		*  @return
		*    The resulting vector
		*/
		inline Vector2 operator *(const Vector2 &vV) const;
		inline Vector2 operator *(float fS) const;

		/**
		*  @brief
		*    Per component multiplication
		*
		*  @param[in] vV
		*    Vector to multiplicate with
		*
		*  @return
		*    This vector which is now the resulting vector
		*/
		inline Vector2 &operator *=(const Vector2 &vV);
		inline Vector2 &operator *=(float fS);

		/**
		*  @brief
		*    Rotates the vector
		*
		*  @param[in] mRot
		*    Matrix which rotates the vector
		*
		*  @return
		*    This vector which is now the resulting vector
		*/
		PLMATH_API Vector2 &operator *=(const Matrix3x3 &mRot);

		/**
		*  @brief
		*    Rotates the vector
		*
		*  @param[in] mRot
		*    Matrix which rotates the vector
		*
		*  @return
		*    This vector which is now the resulting vector
		*/
		PLMATH_API Vector2 &operator *=(const Matrix3x4 &mRot);

		/**
		*  @brief
		*    Transforms the vector
		*
		*  @param[in] mTrans
		*    Matrix which transforms the vector
		*
		*  @return
		*    This vector which is now the resulting vector
		*/
		PLMATH_API Vector2 &operator *=(const Matrix4x4 &mTrans);

		/**
		*  @brief
		*    Per component division
		*
		*  @param[in] vV
		*    Vector to divide through
		*
		*  @return
		*    The resulting vector
		*/
		inline Vector2 operator /(const Vector2 &vV) const;
		inline Vector2 operator /(float fS) const;

		/**
		*  @brief
		*    Per component division
		*
		*  @param[in] vV
		*    Vector to divide through
		*
		*  @return
		*    This vector which is now the resulting vector
		*/
		inline Vector2 &operator /=(const Vector2 &vV);
		inline Vector2 &operator /=(float fS);

		//[-------------------------------------------------------]
		//[ Get and set                                           ]
		//[-------------------------------------------------------]
		inline operator float *() const;
		inline operator const float *() const;
		inline float &operator [](int nIndex);
		inline const float &operator [](int nIndex) const;
		inline void  GetXY(float &fX, float &fY) const;
		inline float GetX() const;
		inline float GetY() const;
		inline void  SetXY(float fX = 0.0f, float fY = 0.0f);
		inline void  SetXY(const float fV[]);
		inline void  SetX(float fX = 0.0f);
		inline void  SetY(float fY = 0.0f);
		inline void  IncXY(float fX = 0.0f, float fY = 0.0f);
		inline void  IncXY(const float fV[]);
		inline void  IncX(float fX);
		inline void  IncY(float fY);

		//[-------------------------------------------------------]
		//[ Misc                                                  ]
		//[-------------------------------------------------------]
		/**
		*  @brief
		*    Returns whether the vector is null or not
		*
		*  @return
		*    'true' if the vector is null, else 'false'
		*/
		inline bool IsNull() const;

		/**
		*  @brief
		*    Returns whether the vector is packed (within range of 0-1) or not
		*
		*  @return
		*    'true' if the vector is packed, else 'false'
		*/
		inline bool IsPacked() const;

		/**
		*  @brief
		*    Packs/clamps the vector into a range of 0-1
		*
		*  @remarks
		*    First, the vector is normalized - now each component is between -1 and 1.
		*    This normalized vector is scaled by 0.5 and 0.5 is added.
		*/
		inline void PackTo01();

		/**
		*  @brief
		*    Returns a vector which is packed/clamped into the range of 0-1
		*
		*  @return
		*    The packed vector
		*
		*  @see
		*    - PackTo01()
		*/
		inline Vector2 GetPackedTo01() const;

		/**
		*  @brief
		*    Unpacks the packed vector into a range of -1 to 1
		*
		*  @remarks
		*    The vector is scaled by 2 and 1 is subtracted.
		*
		*  @note
		*    - There's no internal check whether the vector is packed or not, you can do this
		*      by yourself using the IsPacked() function
		*/
		inline void UnpackFrom01();

		/**
		*  @brief
		*    Returns a unpacked vector of the range of -1 to 1
		*
		*  @return
		*    The unpacked vector
		*
		*  @see
		*    - UnpackFrom01()
		*/
		inline Vector2 GetUnpackedFrom01() const;

		/**
		*  @brief
		*    Returns the smallest component
		*
		*  @return
		*    The smallest component
		*
		*  @remarks
		*    If x is 1 and y is 2, this function will return the x component.
		*/
		inline Component GetSmallestComponent() const;

		/**
		*  @brief
		*    Returns the value of the smallest component
		*
		*  @return
		*    The value of the smallest component
		*
		*  @see
		*    - GetSmallestComponent() above
		*/
		inline float GetSmallestValue() const;

		/**
		*  @brief
		*    Returns the greatest component
		*
		*  @return
		*    The greatest component
		*
		*  @remarks
		*    If x is 1 and y is 2, this function will return the y component.
		*/
		inline Component GetGreatestComponent() const;

		/**
		*  @brief
		*    Returns the value of the greatest component
		*
		*  @return
		*    The value of the greatest component
		*
		*  @see
		*    - GetGreatestValue() above
		*/
		inline float GetGreatestValue() const;

		/**
		*  @brief
		*    Inverts the vector
		*
		*  @remarks
		*    v'.x = -v.x\n
		*    v'.y = -v.y
		*/
		inline void Invert();

		/**
		*  @brief
		*    Returns the inverted vector
		*
		*  @return
		*    Inverted vector
		*
		*  @see
		*    - Invert()
		*/
		inline Vector2 GetInverted() const;

		/**
		*  @brief
		*    Returns the length of the vector (also called magnitude)
		*
		*  @return
		*    Vector length
		*
		*  @remarks
		*    l = sqrt(x*x + y*y)
		*/
		inline float GetLength() const;

		/**
		*  @brief
		*    Returns the squared length of the vector (also called norm)
		*
		*  @return
		*    Squared vector length
		*
		*  @remarks
		*    l = x*x + y*y
		*
		*  @note
		*    - For better performance, use this function instead of GetLength() whenever
		*      possible. You can often work with squared lengths instead of the 'real' ones.
		*/
		inline float GetSquaredLength() const;

		/**
		*  @brief
		*    Sets the vector to the given length
		*
		*  @param[in] fLength
		*    Length to set
		*
		*  @remarks
		*    v' = v*l/sqrt(x*x + y*y)
		*/
		inline void SetLength(float fLength = 1.0f);

		/**
		*  @brief
		*    Normalizes the vector
		*
		*  @return
		*    This instance
		*
		*  @remarks
		*    v' = v*1/sqrt(x*x + y*y)
		*
		*  @note
		*    - This will set the vector to a length of 1 (same as SetLength(1.0f) :)
		*    - A normalized vector is called 'unit vector'
		*/
		inline Vector2 &Normalize();

		/**
		*  @brief
		*    Returns the normalized vector
		*
		*  @return
		*    Normalized vector
		*
		*  @see
		*    - Normalize()
		*/
		inline Vector2 GetNormalized() const;

		/**
		*  @brief
		*    Returns the distance to another vector
		*
		*  @param[in] vV
		*    The other vector
		*
		*  @return
		*    Distance to the other vector
		*
		*  @remarks
		*    dx = v2.x-v1.x\n
		*    dy = v2.y-v1.y\n
		*    d  = sqrt(dx*dx + dy*dy)
		*/
		inline float GetDistance(const Vector2 &vV) const;

		/**
		*  @brief
		*    Returns the squared distance to another vector
		*
		*  @param[in] vV
		*    The other vector
		*
		*  @return
		*    Squared distance to the other vector
		*
		*  @remarks
		*    dx = v2.x-v1.x\n
		*    dy = v2.y-v1.y\n
		*    d  = dx*dx + dy*dy
		*
		*  @note
		*    - For better performance, use this function instead of GetDistance() whenever
		*      possible. You can often work with squared distances instead of the 'real' ones.
		*/
		inline float GetSquaredDistance(const Vector2 &vV) const;

		/**
		*  @brief
		*    Returns the dot product of two vectors
		*
		*  @param[in] vV
		*    Second vector
		*
		*  @return
		*    Dot product of the vectors
		*
		*  @remarks
		*    d = this->x*vV.x + this->y*vV.y
		*
		*  @note
		*    - The dot product is also known as 'scalar product' or 'inner product'
		*    - The dot product of a vector with itself is equal to it's squared length, so it's possible to
		*      use GetSquaredLength() instead of v.DotProduct(v)
		*    - The dot product is commutative
		*    - If the two vectors are perpendicular, their dot product equals zero
		*    - If the angle between the two vectors is acute (< 90 degrees) the dot product will be positive,
		*      if the angle is obtuse (> 90 degrees) the dot product will be negative
		*    - Geometric definition: a.DotProduct(b) = a.GetLength() * b.GetLength() * cos(r)
		*
		*  @see
		*    - GetAngle()
		*/
		inline float DotProduct(const Vector2 &vV) const;

		/**
		*  @brief
		*    Project vector a onto another vector b
		*
		*  @param[in] vA
		*    Vector to project
		*  @param[in] vB
		*    Vector to project onto
		*
		*  @remarks
		*    @code
		*            ^.
		*           / .
		*          /  .
		*       a /   .
		*        /    .                                  a.DotProduct(b)
		*       /     .                  projb(a) = b * -----------------
		*      /      .                                  b.DotProduct(b)
		*     /       .
		*     --->____.
		*      b      ^= projb(a)
		*    @endcode
		*
		*  @return
		*    The resulting projection vector
		*/
		inline Vector2 ProjectVector(const Vector2 &vA, const Vector2 &vB) const;

		/**
		*  @brief
		*    Calculates the angle between two vectos
		*
		*  @param[in] vV
		*    Second vector
		*
		*  @return
		*    The angle between the two vectors (in radians)
		*
		*  @remarks
		*    @code
		*                    v1.DotProduct(v2)
		*     cos A = --------------------------------
		*              v1.GetLength() * v2.GetLength()
		*    @endcode
		*
		*  @note
		*    - If the two vectors are normalized, you can also use acos(DotProduct()) which is
		*      in this case more performant
		*/
		inline float GetAngle(const Vector2 &vV) const;

		/**
		*  @brief
		*    To string
		*
		*  @return
		*    String with the data
		*/
		PLMATH_API PLGeneral::String ToString() const;

		/**
		*  @brief
		*    From string
		*
		*  @param[in] sString
		*    String with the data
		*/
		PLMATH_API bool FromString(const PLGeneral::String &sString);


};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLMath


//[-------------------------------------------------------]
//[ Implementation                                        ]
//[-------------------------------------------------------]
#include "PLMath/Vector2.inl"
#include "PLMath/TypeVector2.inl"


#endif // __PLMATH_VECTOR2_H__
