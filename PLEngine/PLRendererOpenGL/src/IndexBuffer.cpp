/*********************************************************\
 *  File: IndexBuffer.cpp                                *
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
#include <PLGeneral/System/System.h>
#include "PLRendererOpenGL/Renderer.h"
#include "PLRendererOpenGL/IndexBuffer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLGeneral;
namespace PLRendererOpenGL {


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Destructor
*/
IndexBuffer::~IndexBuffer()
{
	Clear();

	// Update renderer statistics
	((PLRenderer::RendererBackend&)GetRenderer()).GetStatisticsT().nIndexBufferNum--;
}

/**
*  @brief
*    Makes this index buffer to the renderers current index buffer
*/
bool IndexBuffer::MakeCurrent()
{
	// Is there an index buffer?
	if (!m_nIndexBuffer && !m_pData) return false; // Error!

	// Bind the index buffer
	if (m_nIndexBuffer) {
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nIndexBuffer);

		// Do we need to update the VBO?
		if (m_bUpdateVBO && m_pData) {
			m_bUpdateVBO = false;

			// Upload new data
			glBufferSubDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0, m_nSize, m_pData);
		}
	} else if (m_pData) {
		// Nothing to do
	}

	// Done
	return true;
}

/**
*  @brief
*    Returns the dynamic data (none VBO)
*/
void *IndexBuffer::GetDynamicData() const
{
	return m_nIndexBuffer ? nullptr : m_pData;
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
IndexBuffer::IndexBuffer(PLRenderer::Renderer &cRenderer) : PLRenderer::IndexBuffer(cRenderer),
	m_nIndexBuffer(0),
	m_pData(nullptr),
	m_pLockedData(nullptr),
	m_bLockReadOnly(false),
	m_bUpdateVBO(false),
	m_nUsageAPI(0)
{
	// Update renderer statistics
	((PLRenderer::RendererBackend&)cRenderer).GetStatisticsT().nIndexBufferNum++;
}


//[-------------------------------------------------------]
//[ Public virtual PLRenderer::Buffer functions           ]
//[-------------------------------------------------------]
bool IndexBuffer::IsAllocated() const
{
	return (m_nIndexBuffer != 0 || m_pData != nullptr);
}

bool IndexBuffer::Allocate(uint32 nElements, PLRenderer::Usage::Enum nUsage, bool bManaged, bool bKeepData)
{
	// Get API dependent type
	uint32 nElementSizeAPI;
	if (m_nElementType == UInt)
		nElementSizeAPI = sizeof(uint32);
	else if (m_nElementType == UShort)
		nElementSizeAPI = sizeof(uint16);
	else if (m_nElementType == UByte)
		nElementSizeAPI = sizeof(uint8);
	else
		return false; // Error!

	// Check if we have to reallocate the buffer
	if (m_nSize == nElementSizeAPI*nElements && nUsage == m_nUsage && m_bManaged == bManaged)
		return true; // Done

	// Vertex buffer extension available?
	if (!((Renderer&)GetRenderer()).IsGL_ARB_vertex_buffer_object()) {
		// Fallback to legacy software implementation
		nUsage = PLRenderer::Usage::Software;
		bManaged = false;
	}

	// Get API dependent usage
		 if (nUsage == PLRenderer::Usage::Static)    m_nUsageAPI = GL_STATIC_DRAW_ARB;
	else if (nUsage == PLRenderer::Usage::Dynamic)   m_nUsageAPI = GL_DYNAMIC_DRAW_ARB;
	else if (nUsage == PLRenderer::Usage::WriteOnly) m_nUsageAPI = GL_STREAM_DRAW_ARB;
	else if (nUsage != PLRenderer::Usage::Software) {
		m_nUsageAPI = 0;

		// Error!
		return false;
	} else m_nUsageAPI = 0;

	// Update renderer statistics
	((PLRenderer::RendererBackend&)GetRenderer()).GetStatisticsT().nIndexBufferMem -= m_nSize;

	// Init data
	uint8 *pDataBackup = nullptr;
	uint32 nSizeBackup = m_nSize;
	if (m_pData && ((m_nSize != nElementSizeAPI*nElements) || (m_bManaged && !bManaged))) {
		// Backup the current data
		if (bKeepData) {
			pDataBackup = new uint8[m_nSize];
			MemoryManager::Copy(pDataBackup, m_pData, m_nSize);
		}

		// Delete data
		if (m_nElementType == UInt)
			delete [] (uint32*)m_pData;
		else if (m_nElementType == UShort)
			delete [] (uint16*)m_pData;
		else
			delete [] (uint8*)m_pData;
		m_pData = nullptr;
	}
	if (m_nIndexBuffer && nUsage == PLRenderer::Usage::Software) {
		if (bKeepData && !m_pData && !pDataBackup && Lock(PLRenderer::Lock::ReadOnly)) {
			// Backup the current data
			pDataBackup = new uint8[m_nSize];
			MemoryManager::Copy(pDataBackup, GetData(), m_nSize);
			Unlock();
		}

		// Delete index buffer object
		glDeleteBuffersARB(1, &m_nIndexBuffer);
		m_nIndexBuffer = 0;
	}
	ForceUnlock();
	m_nElements  = nElements;
	m_nSize		 = nElementSizeAPI*m_nElements;
	m_nUsage	 = nUsage;
	m_bManaged   = bManaged;

	// Create the index buffer
	bool bVBO = false;
	if (nUsage != PLRenderer::Usage::Software && ((Renderer&)GetRenderer()).IsGL_ARB_vertex_buffer_object()) {
		// Use VBO
		bVBO = true;
		if (!m_nIndexBuffer) glGenBuffersARB(1, &m_nIndexBuffer);
		GLint nElementArrayBufferBackup;
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB, &nElementArrayBufferBackup);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nIndexBuffer);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nSize, nullptr, m_nUsageAPI);

		// Error checking
		int nArrayObjectSize = 0;
		glGetBufferParameterivARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &nArrayObjectSize);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, nElementArrayBufferBackup);
		if (nArrayObjectSize <= 0) {
			// Delete index buffer object
			glDeleteBuffersARB(1, &m_nIndexBuffer);
			m_nIndexBuffer = 0;

			// Error!
			return false;
		}
	}

	// No VBO or managed?
	if (!bVBO || bManaged) {
		if (!m_pData) m_pData = new uint8[m_nSize];
		m_bUpdateVBO = true;
	} else m_bUpdateVBO = false;

	// Restore old data if required
	if (pDataBackup) {
		// We can just copy the old data in...
		if (Lock(PLRenderer::Lock::WriteOnly)) {
			uint32 nSize = nSizeBackup;
			if (nSize > m_nSize) nSize = m_nSize;
			MemoryManager::Copy(GetData(), pDataBackup, nSize);
			Unlock();
		}

		// Cleanup
		delete [] pDataBackup;
	}

	// Update renderer statistics
	((PLRenderer::RendererBackend&)GetRenderer()).GetStatisticsT().nIndexBufferMem += m_nSize;

	// Done
	return true;
}

bool IndexBuffer::Clear()
{
	if (IsAllocated()) {
		ForceUnlock();
		if (m_nIndexBuffer) {
			glDeleteBuffersARB(1, &m_nIndexBuffer);
			m_nIndexBuffer = 0;
		}
		if (m_pData) {
			if (m_nElementType == UInt)
				delete [] (uint32*)m_pData;
			else if (m_nElementType == UShort)
				delete [] (uint16*)m_pData;
			else
				delete [] (uint8*)m_pData;
			m_pData = nullptr;
		}

		// Update renderer statistics
		((PLRenderer::RendererBackend&)GetRenderer()).GetStatisticsT().nIndexBufferMem -= m_nSize;

		// Init
		m_nElements  = 0;
		m_nSize		 = 0;
		m_nUsage	 = PLRenderer::Usage::Unknown;
		m_nUsageAPI  = 0;
		m_bUpdateVBO = false;
	}

	// Done
	return true;
}

void *IndexBuffer::Lock(uint32 nFlag)
{
	// Check whether there's an index buffer
	if (!m_nIndexBuffer && !m_pData) return nullptr; // Error!

	// Check whether the index buffer is already locked
	m_nLockCount++;
	if (!m_pLockedData) {
		// Get API dependent flag
		uint32 nFlagAPI;
		if (nFlag == PLRenderer::Lock::ReadOnly) {
			nFlagAPI        = GL_READ_ONLY_ARB;
			m_bLockReadOnly = true;
		} else if (nFlag == PLRenderer::Lock::WriteOnly) {
			nFlagAPI        = GL_WRITE_ONLY_ARB;
			m_bLockReadOnly = false;
		} else if (nFlag == PLRenderer::Lock::ReadWrite) {
			nFlagAPI        = GL_READ_WRITE_ARB;
			m_bLockReadOnly = false;
		} else return nullptr; // Error!

		// Map the index buffer
		((PLRenderer::RendererBackend&)GetRenderer()).GetStatisticsT().nIndexBufferLocks++;
		m_nLockStartTime = System::GetInstance()->GetMicroseconds();
		if (m_pData) m_pLockedData = m_pData;
		else if (m_nIndexBuffer) {
			// Make this index buffer to the current one
			MakeCurrent();

			// Map
			m_pLockedData = glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, nFlagAPI);
		}

		// Lock valid?
		if (!m_pLockedData) m_nLockCount--;
	}

	// Return the locked data
	return m_pLockedData;
}

void *IndexBuffer::GetData()
{
	return m_pLockedData;
}

bool IndexBuffer::Unlock()
{
	// Check whether data is locked
	if (!m_pLockedData) return false; // Error!

	// Do we have to unlock the buffer now?
	m_nLockCount--;
	if (m_nLockCount) return true; // Nope, it's still used somewhere else...

	// Unlock the index buffer
	if (m_pData) {
		if (m_nIndexBuffer && !m_bLockReadOnly)
			m_bUpdateVBO = true;
	} else {
		if (m_nIndexBuffer) {
			// Make this index buffer to the current one
			MakeCurrent();

			// Unmap
			glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
		}
	}
	((PLRenderer::RendererBackend&)GetRenderer()).GetStatisticsT().nIndexBuffersSetupTime += System::GetInstance()->GetMicroseconds()-m_nLockStartTime;
	m_pLockedData   = nullptr;
	m_bLockReadOnly = false;

	// Done
	return true;
}


//[-------------------------------------------------------]
//[ Private virtual PLRenderer::Resource functions        ]
//[-------------------------------------------------------]
void IndexBuffer::BackupDeviceData(uint8 **ppBackup)
{
	// Backup data
	if (!m_pData && Lock(PLRenderer::Lock::ReadOnly)) {
		*ppBackup = new uint8[m_nSize];
		MemoryManager::Copy(*ppBackup, GetData(), m_nSize);
		ForceUnlock();
	} else *ppBackup = nullptr;

	// Destroy the index buffer
	if (m_nIndexBuffer) {
		glDeleteBuffersARB(1, &m_nIndexBuffer);
		m_nIndexBuffer = 0;
	}
}

void IndexBuffer::RestoreDeviceData(uint8 **ppBackup)
{
	if (*ppBackup) {
		GLint nElementArrayBufferBackup;
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB, &nElementArrayBufferBackup);
		glGenBuffersARB(1, &m_nIndexBuffer);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nIndexBuffer);
		glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nSize, nullptr, m_nUsageAPI);
		if (Lock(PLRenderer::Lock::WriteOnly)) {
			MemoryManager::Copy(GetData(), *ppBackup, m_nSize);
			Unlock();
		}
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, nElementArrayBufferBackup);

		// Delete memory
		delete [] *ppBackup;
	} else {
		if (m_bManaged) {
			GLint nElementArrayBufferBackup;
			glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING_ARB, &nElementArrayBufferBackup);
			glGenBuffersARB(1, &m_nIndexBuffer);
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nIndexBuffer);
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_nSize, nullptr, m_nUsageAPI);
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, nElementArrayBufferBackup);
			m_bUpdateVBO = true;
		}
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLRendererOpenGL
