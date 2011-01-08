/*********************************************************\
 *  File: ProgramGLSL.cpp                                *
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
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "PLRendererOpenGLES/ShaderLanguageGLSL.h"
#include "PLRendererOpenGLES/FragmentShaderGLSL.h"
#include "PLRendererOpenGLES/VertexShaderGLSL.h"
#include "PLRendererOpenGLES/Renderer.h"
#include "PLRendererOpenGLES/ShaderToolsGLSL.h"
#include "PLRendererOpenGLES/ProgramAttributeGLSL.h"
#include "PLRendererOpenGLES/ProgramUniformGLSL.h"
#include "PLRendererOpenGLES/ProgramGLSL.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLGeneral;
namespace PLRendererOpenGLES {


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Destructor
*/
ProgramGLSL::~ProgramGLSL()
{
	// Destroy the attribute information
	DestroyAttributeInformation();

	// Destroy the uniform information
	DestroyUniformInformation();

	// Destroy the OpenGL ES program
	glDeleteProgram(m_nOpenGLESProgram);
}

/**
*  @brief
*    Returns the OpenGL ES program
*/
GLuint ProgramGLSL::GetOpenGLESProgram(bool bAutomaticLink)
{
	// Is the program already linked?
	if (!m_bLinked && bAutomaticLink) {
		// Program linked already failed? (so we don't see the linker errors only once!)
		if (!m_bLinkedFailed) {
			// Link the program
			glLinkProgram(m_nOpenGLESProgram);

			// Check the link status
			GLint nLinked = GL_FALSE;
			glGetProgramiv(m_nOpenGLESProgram, GL_LINK_STATUS, &nLinked);
			if (nLinked == GL_TRUE) {
				// Congratulations, the program is now linked!
				m_bLinked = true;
			} else {
				// Error, program link failed!
				m_bLinkedFailed = true;
				ShaderToolsGLSL::WriteProgramInformationIntoLog(m_nOpenGLESProgram);
			}
		}
	}
	return m_nOpenGLESProgram;
}

/**
*  @brief
*    Returns whether or not the program is currently linked
*/
bool ProgramGLSL::IsLinked() const
{
	return m_bLinked;
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
ProgramGLSL::ProgramGLSL(PLRenderer::Renderer &cRenderer) : PLRenderer::Program(cRenderer),
	m_nOpenGLESProgram(glCreateProgram()),
	m_bLinked(false),
	m_bLinkedFailed(false),
	m_bAttributeInformationBuild(false),
	m_bUniformInformationBuild(false)
{
}

/**
*  @brief
*    Denotes that a program relink is required
*/
void ProgramGLSL::RelinkRequired()
{
	// Is the program currently linked?
	if (m_bLinked) {
		// The linked state is now dirty
		m_bLinked = m_bLinkedFailed = false;

		// Destroy the attribute information
		DestroyAttributeInformation();

		// Destroy the uniform information
		DestroyUniformInformation();

		// The program is now dirty
		EventDirty.Emit(this);
	}
}

/**
*  @brief
*    Builds the attribute information
*/
void ProgramGLSL::BuildAttributeInformation()
{
	// Attribute information already build?
	if (!m_bAttributeInformationBuild) {
		// Get the OpenGL ES program - this also ensures that the program is linked
		const GLuint nOpenGLESProgram = GetOpenGLESProgram();

		// Is the program linked?
		if (m_bLinked) {
			// Get the number of active attributes
			GLint nNumOfActiveAttributes = 0;
			glGetProgramiv(nOpenGLESProgram, GL_ACTIVE_ATTRIBUTES, &nNumOfActiveAttributes);
			if (nNumOfActiveAttributes) {
				// Get the length of the longest active attribute name, including a null terminator
				GLint nMaxNameLength = 0;
				glGetProgramiv(nOpenGLESProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &nMaxNameLength);
				if (nMaxNameLength) {
					// Allocate the name buffer
					char *pszName = new char[nMaxNameLength];

					// Iterate over all attributes
					for (int i=0; i<nNumOfActiveAttributes; i++) {
						// Get the current attribute
						GLsizei nCurrentNameLength = 0;	// The actual number of characters written into pszName, excluding the null terminator
						int		nTypeSize		   = 0;	// The size of the attribute, the value is in units of the type, for arrays, normally 1
						GLenum  nType			   = 0;	// Attribute type, e.g. GL_FLOAT_VEC3
						glGetActiveAttrib(nOpenGLESProgram, i, nMaxNameLength, &nCurrentNameLength, &nTypeSize, &nType, pszName);

						// Get the attribute location
						const int nOpenGLESAttributeLocation = glGetAttribLocation(nOpenGLESProgram, pszName);
						if (nOpenGLESAttributeLocation != -1) {
							// Create a new program attribute instance
							ProgramAttributeGLSL *pProgramAttribute = new ProgramAttributeGLSL(nOpenGLESAttributeLocation);

							// Register the new program attribute
							m_lstAttributes.Add(pProgramAttribute);
							m_mapAttributes.Add(String(pszName, true, nCurrentNameLength), pProgramAttribute);
						}
					}

					// Destroy the name buffer
					delete [] pszName;
				}
			}

			// The attribute information build is now build
			m_bAttributeInformationBuild = true;
		}
	}
}

/**
*  @brief
*    Destroys the attribute information
*/
void ProgramGLSL::DestroyAttributeInformation()
{
	// Is there anything to destroy?
	const uint32 nNumOfAttributes = m_lstAttributes.GetNumOfElements();
	if (nNumOfAttributes) {
		// Destroy the attribute instances
		for (uint32 i=0; i<nNumOfAttributes; i++)
			delete (ProgramAttributeGLSL*)m_lstAttributes[i];

		// Clear the attribute list and hash map
		m_lstAttributes.Clear();
		m_mapAttributes.Clear();
	}

	// Currently, there's no attribute information build
	m_bAttributeInformationBuild = false;
}

/**
*  @brief
*    Builds the uniform information
*/
void ProgramGLSL::BuildUniformInformation()
{
	// Uniform information already build?
	if (!m_bUniformInformationBuild) {
		// Get the OpenGL ES program - this also ensures that the program is linked
		const GLuint nOpenGLESProgram = GetOpenGLESProgram();

		// Is the program linked?
		if (m_bLinked) {
			// Get the number of active uniforms
			GLint nNumOfActiveUniforms = 0;
			glGetProgramiv(nOpenGLESProgram, GL_ACTIVE_UNIFORMS, &nNumOfActiveUniforms);
			if (nNumOfActiveUniforms) {
				// Get the length of the longest active uniform name, including a null terminator
				GLint nMaxNameLength = 0;
				glGetProgramiv(nOpenGLESProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &nMaxNameLength);
				if (nMaxNameLength) {
					// Allocate the name buffer
					char *pszName = new char[nMaxNameLength];

					// Next free texture unit which can be assigned to a sampler uniform
					int nNextFreeTextureUnit = 0;

					// Iterate over all uniforms
					for (int i=0; i<nNumOfActiveUniforms; i++) {
						// Get the current uniform
						GLsizei nCurrentNameLength = 0;	// The actual number of characters written into pszName, excluding the null terminator
						int		nTypeSize		   = 0;	// The size of the uniform, the value is in units of the type, for arrays, normally 1
						GLenum  nType			   = 0;	// Uniform type, e.g. GL_FLOAT_VEC3
						glGetActiveUniform(nOpenGLESProgram, i, nMaxNameLength, &nCurrentNameLength, &nTypeSize, &nType, pszName);

						// Get the uniform location
						const int nOpenGLESUniformLocation = glGetUniformLocation(nOpenGLESProgram, pszName);
						if (nOpenGLESUniformLocation != -1) {
							// Is this a sampler uniform?
							int nTextureUnit = -1;
							if (nType == GL_SAMPLER_2D || nType == GL_SAMPLER_CUBE || nType == GL_SAMPLER_3D_OES) {
								// Assign the next free texture unit
								nTextureUnit = nNextFreeTextureUnit;

								// Update the next free texture unit
								nNextFreeTextureUnit++;
							}

							// Create a new program uniform instance
							ProgramUniformGLSL *pProgramUniform = new ProgramUniformGLSL(nOpenGLESProgram, nOpenGLESUniformLocation, nTextureUnit);

							// Register the new program uniform
							m_lstUniforms.Add(pProgramUniform);
							m_mapUniforms.Add(String(pszName, true, nCurrentNameLength), pProgramUniform);
						}
					}

					// Destroy the name buffer
					delete [] pszName;
				}
			}

			// The uniform information build is now build
			m_bUniformInformationBuild = true;
		}
	}
}

/**
*  @brief
*    Destroys the uniform information
*/
void ProgramGLSL::DestroyUniformInformation()
{
	// Is there anything to destroy?
	const uint32 nNumOfUniforms = m_lstUniforms.GetNumOfElements();
	if (nNumOfUniforms) {
		// Destroy the uniform instances
		for (uint32 i=0; i<nNumOfUniforms; i++)
			delete (ProgramUniformGLSL*)m_lstUniforms[i];

		// Clear the uniform list and hash map
		m_lstUniforms.Clear();
		m_mapUniforms.Clear();
	}

	// Currently, there's no uniform information build
	m_bUniformInformationBuild = false;
}


//[-------------------------------------------------------]
//[ Public virtual PLRenderer::Program functions          ]
//[-------------------------------------------------------]
String ProgramGLSL::GetShaderLanguage() const
{
	return ShaderLanguageGLSL::GLSL;
}

PLRenderer::VertexShader *ProgramGLSL::GetVertexShader() const
{
	return (PLRenderer::VertexShader*)m_cVertexShaderHandler.GetResource();
}

bool ProgramGLSL::SetVertexShader(PLRenderer::VertexShader *pVertexShader)
{
	// Is the new vertex shader the same one as the current one?
	PLRenderer::VertexShader *pCurrentVertexShader = (PLRenderer::VertexShader*)m_cVertexShaderHandler.GetResource();
	if (pCurrentVertexShader != pVertexShader) {
		// The shader language of the program and the vertex shader must match
		if (pVertexShader && pVertexShader->GetShaderLanguage() != ShaderLanguageGLSL::GLSL)
			return false; // Error, shader language mismatch!

		// Detach the current vertex shader from the program
		if (pCurrentVertexShader)
			glDetachShader(m_nOpenGLESProgram, ((VertexShaderGLSL*)pCurrentVertexShader)->GetOpenGLESVertexShader());

		// Update the vertex shader resource handler
		m_cVertexShaderHandler.SetResource(pVertexShader);

		// Attach the new vertex shader to the program
		if (pVertexShader)
			glAttachShader(m_nOpenGLESProgram, ((VertexShaderGLSL*)pVertexShader)->GetOpenGLESVertexShader());

		// Denote that a program relink is required
		RelinkRequired();
	}

	// Done
	return true;
}

PLRenderer::GeometryShader *ProgramGLSL::GetGeometryShader() const
{
	// OpenGL ES 2.0 has no support for geometry shaders
	return nullptr;
}

bool ProgramGLSL::SetGeometryShader(PLRenderer::GeometryShader *pGeometryShader)
{
	// OpenGL ES 2.0 has no support for geometry shaders
	return false;
}

PLRenderer::FragmentShader *ProgramGLSL::GetFragmentShader() const
{
	return (PLRenderer::FragmentShader*)m_cFragmentShaderHandler.GetResource();
}

bool ProgramGLSL::SetFragmentShader(PLRenderer::FragmentShader *pFragmentShader)
{
	// Is the new fragment shader the same one as the current one?
	PLRenderer::FragmentShader *pCurrentFragmentShader = (PLRenderer::FragmentShader*)m_cFragmentShaderHandler.GetResource();
	if (pCurrentFragmentShader != pFragmentShader) {
		// The shader language of the program and the fragment shader must match
		if (pFragmentShader && pFragmentShader->GetShaderLanguage() != ShaderLanguageGLSL::GLSL)
			return false; // Error, shader language mismatch!

		// Detach the current fragment shader from the program
		if (pCurrentFragmentShader)
			glDetachShader(m_nOpenGLESProgram, ((FragmentShaderGLSL*)pCurrentFragmentShader)->GetOpenGLESFragmentShader());

		// Update the fragment shader resource handler
		m_cFragmentShaderHandler.SetResource(pFragmentShader);

		// Attach the new fragment shader to the program
		if (pFragmentShader)
			glAttachShader(m_nOpenGLESProgram, ((FragmentShaderGLSL*)pFragmentShader)->GetOpenGLESFragmentShader());

		// Denote that a program relink is required
		RelinkRequired();
	}

	// Done
	return true;
}

const Array<PLRenderer::ProgramAttribute*> &ProgramGLSL::GetAttributes()
{
	// Build the attribute information, if required
	if (!m_bAttributeInformationBuild)
		BuildAttributeInformation();

	// Return the list of all attributes
	return m_lstAttributes;
}

PLRenderer::ProgramAttribute *ProgramGLSL::GetAttribute(const String &sName)
{
	// Build the attribute information, if required
	if (!m_bAttributeInformationBuild)
		BuildAttributeInformation();

	// Return the requested attribute
	return m_mapAttributes.Get(sName);
}

const Array<PLRenderer::ProgramUniform*> &ProgramGLSL::GetUniforms()
{
	// Build the uniform information, if required
	if (!m_bUniformInformationBuild)
		BuildUniformInformation();

	// Return the list of all uniforms
	return m_lstUniforms;
}

PLRenderer::ProgramUniform *ProgramGLSL::GetUniform(const String &sName)
{
	// Build the uniform information, if required
	if (!m_bUniformInformationBuild)
		BuildUniformInformation();

	// Return the requested uniform
	return m_mapUniforms.Get(sName);
}


//[-------------------------------------------------------]
//[ Protected virtual PLRenderer::Program functions       ]
//[-------------------------------------------------------]
bool ProgramGLSL::MakeCurrent()
{
	// Get the OpenGL ES program - this also ensures that the program is linked
	const GLuint nOpenGLESProgram = GetOpenGLESProgram();

	// Is the OpenGL program linked?
	if (IsLinked()) {
		// Make this to the currently used program
		glUseProgram(nOpenGLESProgram);

		// Enable all vertex attribute arrays
		for (uint32 i=0; i<m_lstAttributes.GetNumOfElements(); i++)
			glEnableVertexAttribArray(((ProgramAttributeGLSL*)m_lstAttributes[i])->m_nOpenGLESAttributeLocation);

		// Done
		return true;
	} else {
		// Error!
		return false;
	}
}

bool ProgramGLSL::UnmakeCurrent()
{
	// Disable all vertex attribute arrays
	for (uint32 i=0; i<m_lstAttributes.GetNumOfElements(); i++)
		glDisableVertexAttribArray(((ProgramAttributeGLSL*)m_lstAttributes[i])->m_nOpenGLESAttributeLocation);

	// Currently, no program is set
	glUseProgram(0);

	// Done
	return true;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLRendererOpenGLES
