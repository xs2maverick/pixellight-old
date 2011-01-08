/*********************************************************\
 *  File: MeshCreatorSuperEllipse.cpp                    *
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
#include <PLMath/Vector2.h>
#include <PLRenderer/Renderer/IndexBuffer.h>
#include <PLRenderer/Renderer/VertexBuffer.h>
#include "PLMesh/Geometry.h"
#include "PLMesh/MeshManager.h"
#include "PLMesh/MeshLODLevel.h"
#include "PLMesh/MeshMorphTarget.h"
#include "PLMesh/Creator/MeshCreatorSuperEllipse.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLGeneral;
using namespace PLMath;
using namespace PLRenderer;
namespace PLMesh {


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(MeshCreatorSuperEllipse)


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Default constructor
*/
MeshCreatorSuperEllipse::MeshCreatorSuperEllipse() :
	Power1(this),
	Power2(this),
	Detail(this),
	Offset(this)
{
}

/**
*  @brief
*    Destructor
*/
MeshCreatorSuperEllipse::~MeshCreatorSuperEllipse()
{
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Internal helper function
*/
void MeshCreatorSuperEllipse::EvalSuperEllipse(float fT1, float fT2, float fP1, float fP2, Vector3 *pV) const
{
	float ct1 = Math::Cos(fT1);
	float ct2 = Math::Cos(fT2);
	float st1 = Math::Sin(fT1);
	float st2 = Math::Sin(fT2);

	float tmp = float(Math::Sign(ct1)*Math::Pow(Math::Abs(ct1), fP1));
	pV->x     = float(tmp*Math::Sign(ct2)*Math::Pow(Math::Abs(ct2), fP2));
	pV->y     = float(Math::Sign(st1)*Math::Pow(Math::Abs(st1), fP1));
	pV->z     = float(tmp*Math::Sign(st2)*Math::Pow(Math::Abs(st2), fP2));
}


//[-------------------------------------------------------]
//[ Private virtual MeshCreator functions                 ]
//[-------------------------------------------------------]
Mesh *MeshCreatorSuperEllipse::Create(Mesh &cMesh, uint32 nLODLevel, bool bStatic) const
{
	// Call base implementation
	MeshCreator::Create(cMesh, nLODLevel, bStatic);

	// Check if theres a renderer and check the parameter
	if (Detail < 4)
		return &cMesh; // Error!

	// Get morph target
	MeshMorphTarget *pMorphTarget = cMesh.GetMorphTarget(0);
	if (pMorphTarget && pMorphTarget->GetVertexBuffer()) {
		// Allocate vertex buffer
		VertexBuffer *pVertexBuffer = pMorphTarget->GetVertexBuffer();
		pVertexBuffer->AddVertexAttribute(VertexBuffer::Position, 0, VertexBuffer::Float3);
		if (TexCoords)
			pVertexBuffer->AddVertexAttribute(VertexBuffer::TexCoord, 0, VertexBuffer::Float2);
		if (Normals)
			pVertexBuffer->AddVertexAttribute(VertexBuffer::Normal,   0, VertexBuffer::Float3);
		pVertexBuffer->Allocate(Detail*(Detail+1), bStatic ? Usage::Static : Usage::Dynamic);

		// Get LOD level
		MeshLODLevel *pLODLevel = cMesh.GetLODLevel(nLODLevel);
		if (pLODLevel && pLODLevel->GetIndexBuffer()) {
			// Allocate index buffer
			IndexBuffer *pIndexBuffer = pLODLevel->GetIndexBuffer();
			pIndexBuffer->SetElementTypeByMaximumIndex(pVertexBuffer->GetNumOfElements()-1);
			pIndexBuffer->Allocate(pVertexBuffer->GetNumOfElements(), bStatic ? Usage::Static : Usage::Dynamic);

			// Setup vertices, indices & geometry
			Array<Geometry> &lstGeometries = *pLODLevel->GetGeometries();
			if (pVertexBuffer->Lock(Lock::WriteOnly)) {
				if (pIndexBuffer->Lock(Lock::WriteOnly)) {
					float fDelta = (float) (0.01*Math::Pi2/Detail);
					Vector3 p, p1, p2, en;
					uint32 nVertex = 0;
					uint32 nIndex = 0;

					// Get offset
					const Vector3 &vOffset = Offset.Get();

					for (uint32 j=0; j<Detail/2; j++) {
						// Create and setup the geometry
						Geometry &cGeometry = lstGeometries.Add();
						cGeometry.SetPrimitiveType(Primitive::TriangleStrip);
						cGeometry.SetStartIndex(nVertex);
						cGeometry.SetIndexSize((Detail+1)*2);

						float fTheta1 = (float)(j*Math::Pi2/(double)Detail - Math::PiHalf);
						float fTheta2 = (float)((j+1)*Math::Pi2/(double)Detail - Math::PiHalf);
						for (uint32 i=0; i<=Detail; i++) {
							float fTheta3 = (i && i < Detail) ? (float)(i*Math::Pi2/Detail) : 0;
							float *pfVertex;
							if (Order) {
								// V0
								EvalSuperEllipse(fTheta2,		 fTheta3,		 Power1, Power2, &p);
								EvalSuperEllipse(fTheta2+fDelta, fTheta3,		 Power1, Power2, &p1);
								EvalSuperEllipse(fTheta2,		 fTheta3+fDelta, Power1, Power2, &p2);
								if (TexCoords) {
									pfVertex = (float*)pVertexBuffer->GetData(nVertex, VertexBuffer::TexCoord);
									pfVertex[Vector2::X] = (float)i/Detail;
									pfVertex[Vector2::Y] = (float)2*(j+1)/Detail;
								}
								if (Normals) {
									en.GetFaceNormal(p1,p,p2);
									pfVertex = (float*)pVertexBuffer->GetData(nVertex, VertexBuffer::Normal);
									pfVertex[Vector3::X] = en.x;
									pfVertex[Vector3::Y] = en.y;
									pfVertex[Vector3::Z] = en.z;
								}
								pfVertex = (float*)pVertexBuffer->GetData(nVertex, VertexBuffer::Position);
								pfVertex[Vector3::X] = vOffset.x + p.x;
								pfVertex[Vector3::Y] = vOffset.y + p.y;
								pfVertex[Vector3::Z] = vOffset.z + p.z;
								pIndexBuffer->SetData(nIndex++, nVertex);
								nVertex++;

								// V1
								EvalSuperEllipse(fTheta1,		 fTheta3,		 Power1, Power2, &p);
								EvalSuperEllipse(fTheta1+fDelta, fTheta3,		 Power1, Power2, &p1);
								EvalSuperEllipse(fTheta1,		 fTheta3+fDelta, Power1, Power2, &p2);
								if (TexCoords) {
									pfVertex = (float*)pVertexBuffer->GetData(nVertex, VertexBuffer::TexCoord);
									pfVertex[Vector2::X] = (float)i/Detail;
									pfVertex[Vector2::Y] = (float)2*j/Detail;
								}
								if (Normals) {
									en.GetFaceNormal(p1, p, p2);
									pfVertex = (float*)pVertexBuffer->GetData(nVertex, VertexBuffer::Normal);
									pfVertex[Vector3::X] = en.x;
									pfVertex[Vector3::Y] = en.y;
									pfVertex[Vector3::Z] = en.z;
								}
								pfVertex = (float*)pVertexBuffer->GetData(nVertex, VertexBuffer::Position);
								pfVertex[Vector3::X] = vOffset.x + p.x;
								pfVertex[Vector3::Y] = vOffset.y + p.y;
								pfVertex[Vector3::Z] = vOffset.z + p.z;
								pIndexBuffer->SetData(nIndex++, nVertex);
								nVertex++;
							} else {
								// V0
								EvalSuperEllipse(fTheta1,		 fTheta3,		 Power1, Power2, &p);
								EvalSuperEllipse(fTheta1+fDelta, fTheta3,		 Power1, Power2, &p1);
								EvalSuperEllipse(fTheta1,		 fTheta3+fDelta, Power1, Power2, &p2);
								if (TexCoords) {
									pfVertex = (float*)pVertexBuffer->GetData(nVertex, VertexBuffer::TexCoord);
									pfVertex[Vector2::X] = (float)i/Detail;
									pfVertex[Vector2::Y] = (float)2*j/Detail;
								}
								if (Normals) {
									en.GetFaceNormal(p1, p, p2);
									pfVertex = (float*)pVertexBuffer->GetData(nVertex, VertexBuffer::Normal);
									pfVertex[Vector3::X] = en.x;
									pfVertex[Vector3::Y] = en.y;
									pfVertex[Vector3::Z] = en.z;
								}
								pfVertex = (float*)pVertexBuffer->GetData(nVertex, VertexBuffer::Position);
								pfVertex[Vector3::X] = vOffset.x + p.x;
								pfVertex[Vector3::Y] = vOffset.y + p.y;
								pfVertex[Vector3::Z] = vOffset.z + p.z;
								pIndexBuffer->SetData(nIndex++, nVertex);
								nVertex++;

								// V1
								EvalSuperEllipse(fTheta2,		 fTheta3,		 Power1, Power2, &p);
								EvalSuperEllipse(fTheta2+fDelta, fTheta3,		 Power1, Power2, &p1);
								EvalSuperEllipse(fTheta2,		 fTheta3+fDelta, Power1, Power2, &p2);
								if (TexCoords) {
									pfVertex = (float*)pVertexBuffer->GetData(nVertex, VertexBuffer::TexCoord);
									pfVertex[Vector2::X] = (float)i/Detail;
									pfVertex[Vector2::Y] = (float)2*(j+1)/Detail;
								}
								if (Normals) {
									en.GetFaceNormal(p1,p,p2);
									pfVertex = (float*)pVertexBuffer->GetData(nVertex, VertexBuffer::Normal);
									pfVertex[Vector3::X] = en.x;
									pfVertex[Vector3::Y] = en.y;
									pfVertex[Vector3::Z] = en.z;
								}
								pfVertex = (float*)pVertexBuffer->GetData(nVertex, VertexBuffer::Position);
								pfVertex[Vector3::X] = vOffset.x + p.x;
								pfVertex[Vector3::Y] = vOffset.y + p.y;
								pfVertex[Vector3::Z] = vOffset.z + p.z;
								pIndexBuffer->SetData(nIndex++, nVertex);
								nVertex++;
							}
						}
					}

					// Unlock the index buffer
					pIndexBuffer->Unlock();
				}

				// Unlock the vertex buffer
				pVertexBuffer->Unlock();
			}
		}
	}

	// Return the created mesh
	return &cMesh;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLMesh
