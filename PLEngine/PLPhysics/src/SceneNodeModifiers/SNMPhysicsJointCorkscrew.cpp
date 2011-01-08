/*********************************************************\
 *  File: SNMPhysicsJointCorkscrew.cpp                   *
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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include "PLPhysics/Joint.h"
#include "PLPhysics/World.h"
#include "PLPhysics/SceneNodes/SCPhysicsWorld.h"
#include "PLPhysics/SceneNodeModifiers/SNMPhysicsBody.h"
#include "PLPhysics/SceneNodeModifiers/SNMPhysicsJointCorkscrew.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLGeneral;
using namespace PLMath;
using namespace PLScene;
namespace PLPhysics {


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(SNMPhysicsJointCorkscrew)


//[-------------------------------------------------------]
//[ Public RTTI get/set functions                         ]
//[-------------------------------------------------------]
const Vector3 &SNMPhysicsJointCorkscrew::GetPivotPoint() const
{
	return m_vPivotPoint;
}

void SNMPhysicsJointCorkscrew::SetPivotPoint(const Vector3 &vValue)
{
	if (m_vPivotPoint != vValue) {
		m_vPivotPoint = vValue;
		RecreatePhysicsJoint();
	}
}

const Vector3 &SNMPhysicsJointCorkscrew::GetPinDir() const
{
	return m_vPinDir;
}

void SNMPhysicsJointCorkscrew::SetPinDir(const Vector3 &vValue)
{
	if (m_vPinDir != vValue) {
		m_vPinDir = vValue;
		RecreatePhysicsJoint();
	}
}


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
SNMPhysicsJointCorkscrew::SNMPhysicsJointCorkscrew(SceneNode &cSceneNode) : SNMPhysicsJoint(cSceneNode),
	PivotPoint(this),
	PinDir(this),
	m_vPinDir(0.0f, 1.0f, 0.0f)
{
}

/**
*  @brief
*    Destructor
*/
SNMPhysicsJointCorkscrew::~SNMPhysicsJointCorkscrew()
{
}


//[-------------------------------------------------------]
//[ Private virtual SNMPhysicsJoint functions             ]
//[-------------------------------------------------------]
void SNMPhysicsJointCorkscrew::CreatePhysicsJoint()
{
	if (m_pWorldContainer && m_pWorldContainer->GetWorld()) {
		// Get the parent and child physics bodies the joint is attached to
		const SNMPhysicsBody *pParentBody = GetParentBodyModifier();
		const SNMPhysicsBody *pChildBody  = GetChildBodyModifier();

		// Create the joint
		const Matrix3x4 &mTrans = GetSceneNode().GetTransform().GetMatrix();
		m_pJointHandler->SetElement(m_pWorldContainer->GetWorld()->CreateJointCorkscrew(pParentBody ? pParentBody->GetBody() : nullptr,
																						pChildBody  ? pChildBody->GetBody()  : nullptr,
																						mTrans*m_vPivotPoint,
																						mTrans.RotateVector(m_vPinDir).Normalize()));
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLPhysics
