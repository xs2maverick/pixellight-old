/*********************************************************\
 *  File: FontTexture.cpp                                *
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
#include <ft2build.h>
#include FT_FREETYPE_H
#include <PLGeneral/File/File.h>
#include <PLRenderer/Renderer/Program.h>
#include "PLRendererOpenGLES/Renderer.h"
#include "PLRendererOpenGLES/FontManager.h"
#include "PLRendererOpenGLES/FontGlyphTexture.h"
#include "PLRendererOpenGLES/ProgramUniformGLSL.h"
#include "PLRendererOpenGLES/FontTexture.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLGeneral;
using namespace PLMath;
using namespace PLGraphics;
namespace PLRendererOpenGLES {


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Destructor
*/
FontTexture::~FontTexture()
{
	// Destroy the glyph texture atlas
	DestroyGlyphTextureAtlas();

	// Destroy the FreeType library face
	if (m_pFTFace) {
		FT_Done_Face(*m_pFTFace);
		delete m_pFTFace;
	}

	// Cleanup
	if (m_pFontFileData)
		delete [] m_pFontFileData;
}

/**
*  @brief
*    Returns the FreeType library face object (aka "The Font")
*/
FT_Face *FontTexture::GetFTFace() const
{
	return m_pFTFace;
}

/**
*  @brief
*    Returns the size of the glyph texture atlas (in pixel)
*/
const Vector2i &FontTexture::GetGlyphTextureAtlasSize() const
{
	return m_vGlyphTextureAtlasSize;
}


//[-------------------------------------------------------]
//[ Public virtual Font functions                         ]
//[-------------------------------------------------------]
bool FontTexture::SetSize(uint32 nSize, uint32 nResolution)
{
	// Destroy the glyph texture atlas - it's now dirty
	DestroyGlyphTextureAtlas();

	// Destroy the currently active glyphs
	DestroyGlyphs();

	// Is there a FreeType library face?
	if (m_pFTFace) {
		// The FreeType library measures font size in terms of 1/64ths of pixels, so we have to adjust with *64
		if (!FT_Set_Char_Size(*m_pFTFace, 0L, nSize*64, nResolution, nResolution)) {
			// All went fine
			m_nSize       = nSize;
			m_nResolution = nResolution;
			return true;
		}
	}

	// Error!
	m_nSize       = 0;
	m_nResolution = 0;
	return false;
}

bool FontTexture::IsValid() const
{
	return (m_pFTFace != nullptr);
}

float FontTexture::GetAscender() const
{
	// The FreeType library measures font size in terms of 1/64ths of pixels, so we have to adjust with /64
	return m_pFTFace ? float((*m_pFTFace)->size->metrics.ascender)/64.0f : 0.0f;
}

float FontTexture::GetDescender() const
{
	// The FreeType library measures font size in terms of 1/64ths of pixels, so we have to adjust with /64
	return m_pFTFace ? float((*m_pFTFace)->size->metrics.descender)/64.0f : 0.0f;
}

float FontTexture::GetHeight() const
{
	// The FreeType library measures font size in terms of 1/64ths of pixels, so we have to adjust with /64
	return m_pFTFace ? float((*m_pFTFace)->size->metrics.height)/64.0f : 0.0f;
}

float FontTexture::GetTextWidth(const String &sText)
{
	// Create glyph texture atlas if required
	if (!m_nOpenGLESGlyphTextureAtlas)
		CreateGlyphTextureAtlas();

	// Let the base implementation to the rest of the job
	return m_nOpenGLESGlyphTextureAtlas ? PLRenderer::FontTexture::GetTextWidth(sText) : 0.0f;
}

void FontTexture::Draw(const String &sText, const Color4 &cColor, const Matrix4x4 &mObjectSpaceToClipSpace, const Vector2 &vScale, const Vector2 &vBias, uint32 nFlags)
{
	// [TODO] Do no longer set this inside the font method, should be set from outside!
	GetRenderer().SetRenderState(PLRenderer::RenderState::BlendEnable, true);

	// Create glyph texture atlas if required
	if (!m_nOpenGLESGlyphTextureAtlas)
		CreateGlyphTextureAtlas();
	if (m_nOpenGLESGlyphTextureAtlas) {
		// Get and set the program for font rendering
		FontManager::GeneratedProgramUserData *pGeneratedProgramUserData = nullptr;
		PLRenderer::Program *pProgram = ((FontManager*)m_pFontManager)->GetProgram(&pGeneratedProgramUserData);
		if (pProgram && pGeneratedProgramUserData && pGeneratedProgramUserData->pObjectSpaceToClipSpaceMatrix &&
			pGeneratedProgramUserData->pGlyphMap && pGeneratedProgramUserData->pColor && pGeneratedProgramUserData->pGlyphSizePenPosition &&
			pGeneratedProgramUserData->pTextureCoordinateMinMax && m_pFontManager->GetRenderer().SetProgram(pProgram)) {
			// Set object space to clip space matrix
			pGeneratedProgramUserData->pObjectSpaceToClipSpaceMatrix->Set(mObjectSpaceToClipSpace);

			// Set glyph texture atlas - must be GLSL because that's the only supported shader language in here :D
			((ProgramUniformGLSL*)pGeneratedProgramUserData->pGlyphMap)->Set((GLenum)GL_TEXTURE_2D, m_nOpenGLESGlyphTextureAtlas);

			// Enable/disable mipmapping
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (nFlags & Mipmapping) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);

			// Set color
			pGeneratedProgramUserData->pColor->Set(cColor);

			// Get the text to draw - we only support ASCII
			const char *pszText = sText.GetASCII();

			// The current object space pen position
			Vector2 vPenPosition = vBias;

			// Center the text?
			if (nFlags & CenterText)
				vPenPosition.x -= GetTextWidth(sText)/2;

			// Get the renderer instance
			Renderer &cRenderer = (Renderer&)GetFontManager().GetRenderer();

			// Iterate through all characters of the text to draw
			for (uint32 i=0; i<sText.GetLength(); i++, pszText++) {
				// Get the character code
				const unsigned char nCharacterCode = (unsigned char)*pszText;

				// Get the glyph instance of the character to draw
				FontGlyphTexture *pFontGlyphTexture = (FontGlyphTexture*)m_lstGlyphs[nCharacterCode];
				if (pFontGlyphTexture) {
					// Set glyph size and pen position
					pGeneratedProgramUserData->pGlyphSizePenPosition->Set(float(pFontGlyphTexture->GetSize().x)*vScale.x,					// Object space glyph x size in points   (x) => x scale
																		  float(pFontGlyphTexture->GetSize().y)*vScale.y,					// Object space glyph y size in points   (y) => y scale
																		  (vPenPosition.x + pFontGlyphTexture->GetCorner().x)*vScale.x,		// Object space pen x position in points (z) => x bias
																		  (vPenPosition.y + pFontGlyphTexture->GetCorner().y)*vScale.y);	// Object space pen y position in points (w) => y bias

					// Set the normalized minimum and maximum glyph texture coordinate inside the glyph texture atlas
					pGeneratedProgramUserData->pTextureCoordinateMinMax->Set(pFontGlyphTexture->GetTexCoordMin().x, pFontGlyphTexture->GetTexCoordMin().y,
																			 pFontGlyphTexture->GetTexCoordMax().x, pFontGlyphTexture->GetTexCoordMax().y);

					// Draw the glyph quad
					cRenderer.DrawPrimitives(PLRenderer::Primitive::TriangleStrip, 0, 4);

					// Draw the glyph and let the pen advance to the object space position of the next character
					vPenPosition.x += pFontGlyphTexture->GetPenAdvance().x;
				}
			}

			// [TODO] UnderlineText & CrossoutText => Draw lines
		}
	}
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
FontTexture::FontTexture(FontManager &cFontManager, File &cFile) : PLRenderer::FontTexture(cFontManager, cFile.GetUrl().GetUrl()),
	m_nFontFileSize(cFile.GetSize()),
	m_pFontFileData(new uint8[m_nFontFileSize]),
	m_pFTFace(nullptr),
	m_nGlyphTextureAtlasPadding(3),
	m_nOpenGLESGlyphTextureAtlas(0)
{
	// Read in the whole file
	cFile.Read(m_pFontFileData, 1, m_nFontFileSize);

	// Create the FreeType library face
	m_pFTFace = new FT_Face;
	if (FT_New_Memory_Face(*cFontManager.GetFTLibrary(), (FT_Byte const*)m_pFontFileData, (FT_Long)m_nFontFileSize, 0, m_pFTFace)) {
		// Error!
		delete m_pFTFace;
		m_pFTFace = nullptr;
	}
}

/**
*  @brief
*    Calculates the glyph texture atlas size using the current glyph settings and hardware capabilities
*/
void FontTexture::CalculateGlyphTextureAtlasSize(Vector2i &vSize)
{
	// Get the maximum supported OpenGL ES texture size
	GLint nOpenGLESMaxTextureSize = 0;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &nOpenGLESMaxTextureSize);
	if (nOpenGLESMaxTextureSize) {
		// Get the font height in pixels
		const uint32 nFontHeight = GetHeightInPixels();
		if (nFontHeight) {
			// We've got 256 glyphs, this means there are 16 glyphs per row within the glphy texture atlas
			vSize.x = Math::GetNearestPowerOfTwo(m_nGlyphTextureAtlasPadding + 16*(nFontHeight + m_nGlyphTextureAtlasPadding), false);
			vSize.y = Math::GetNearestPowerOfTwo(m_nGlyphTextureAtlasPadding + 16*(nFontHeight + m_nGlyphTextureAtlasPadding), false);

			// Check texture size - we just "cut off" everything that doesn't fitt into the texture (... normally, this is not happening... so, keep it simple!)
			vSize.x = vSize.x > nOpenGLESMaxTextureSize ? nOpenGLESMaxTextureSize : vSize.x;
			vSize.y = vSize.y > nOpenGLESMaxTextureSize ? nOpenGLESMaxTextureSize : vSize.y;
		} else {
			// Error!
			vSize.Set(0, 0);
		}
	} else {
		// Error!
		vSize.Set(0, 0);
	}
}

/**
*  @brief
*    Creates the glyph texture atlas
*/
void FontTexture::CreateGlyphTextureAtlas()
{
	// Destroy the previous glyph texture atlas
	DestroyGlyphTextureAtlas();

	// Check the FreeType library face object (aka "The Font")
	if (m_pFTFace) {
		// Calculate the glyph texture atlas size using the current glyph settings and hardware capabilities
		CalculateGlyphTextureAtlasSize(m_vGlyphTextureAtlasSize);

		// Valid glyph texture atlas size?
		if (m_vGlyphTextureAtlasSize.x && m_vGlyphTextureAtlasSize.y) {
			{ // Create the OpenGL ES glyph texture atlas - for now the texture is just empty
				const uint32 nTotalNumOfBytes = GetGlyphTextureAtlasNumOfBytes(false);
				GLubyte *pData = new GLubyte[nTotalNumOfBytes];
				MemoryManager::Set(pData, 0, nTotalNumOfBytes);
				glGenTextures(1, &m_nOpenGLESGlyphTextureAtlas);
				glBindTexture(GL_TEXTURE_2D, m_nOpenGLESGlyphTextureAtlas);

				// Let OpenGL ES create the mipmap chain for us
				glGenerateMipmap(GL_TEXTURE_2D);

				// Enable mipmapping
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				// Setup wrap mode
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				// Upload the texture data
				glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, m_vGlyphTextureAtlasSize.x, m_vGlyphTextureAtlasSize.y, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pData);

				// Cleanup
				delete [] pData;
			}

			// Get the font height in pixels (if we're here, we already now that it's valid!)
			const uint32 nFontHeight = GetHeightInPixels();

			// Fill the glyph texture atlas - We've got 256 glyphs, this means there are 16 glyphs per row within the glphy texture atlas
			const uint32 nNumOfGlyphs  = 256;
			const uint32 nGlyphsPerRow = 16;
			const uint32 nXDistanceToNextGlyph = nFontHeight + m_nGlyphTextureAtlasPadding;
			const uint32 nYDistanceToNextGlyph = nFontHeight + m_nGlyphTextureAtlasPadding;
			m_lstGlyphs.Resize(nNumOfGlyphs, true, true);
			for (uint32 y=0, i=0; y<nGlyphsPerRow; y++) {
				for (uint32 x=0; x<nGlyphsPerRow; x++, i++) {
					// Add a new texture glyph
					m_lstGlyphs[i] = new FontGlyphTexture(*this, i, Vector2i(m_nGlyphTextureAtlasPadding + x*nXDistanceToNextGlyph, m_nGlyphTextureAtlasPadding + y*nYDistanceToNextGlyph));
				}
			}

			// Update renderer statistics
			((Renderer&)GetFontManager().GetRenderer()).GetStatisticsT().nTextureBuffersNum++;
			((Renderer&)GetFontManager().GetRenderer()).GetStatisticsT().nTextureBuffersMem += GetGlyphTextureAtlasNumOfBytes(true);
		}
	}
}

/**
*  @brief
*    Returns the number of bytes the glyph texture atlas allocates
*/
uint32 FontTexture::GetGlyphTextureAtlasNumOfBytes(bool bIncludeMipmaps) const
{
	// Include mipmaps?
	if (bIncludeMipmaps) {
		Vector2i vSize(m_vGlyphTextureAtlasSize.x, m_vGlyphTextureAtlasSize.y);
		uint32 nTotalNumOfBytes = 0;

		// Go down the mipmap chain
		while (vSize.x > 1 || vSize.y > 1) {
			// Add memory from this mipmap level
			nTotalNumOfBytes += vSize.x*vSize.y; // Alpha, one byte

			// Next mipmap level, please
			vSize.x >>= 1;	// /=2 using faster bitshifts
			vSize.y >>= 1;	// /=2 using faster bitshifts
			if (vSize.x < 1)
				vSize.x = 1;
			if (vSize.y < 1)
				vSize.y = 1;
		}

		// Don't forget the last tiny one :D
		nTotalNumOfBytes += vSize.x*vSize.y; // Alpha, one byte

		// Done
		return nTotalNumOfBytes;
	} else {
		// Just base map
		return m_vGlyphTextureAtlasSize.x*m_vGlyphTextureAtlasSize.y; // Alpha, one byte
	}
}

/**
*  @brief
*    Destroys the glyph texture atlas
*/
void FontTexture::DestroyGlyphTextureAtlas()
{
	// Glyph texture atlas there?
	if (m_nOpenGLESGlyphTextureAtlas) {
		// Delete the OpenGL ES glyph texture atlas
		glDeleteTextures(1, &m_nOpenGLESGlyphTextureAtlas);

		// Update renderer statistics
		((Renderer&)GetFontManager().GetRenderer()).GetStatisticsT().nTextureBuffersNum--;
		((Renderer&)GetFontManager().GetRenderer()).GetStatisticsT().nTextureBuffersMem -= GetGlyphTextureAtlasNumOfBytes(true);

		// Reset glyph texture atlas information
		m_nOpenGLESGlyphTextureAtlas = 0;
		m_vGlyphTextureAtlasSize.Set(0, 0);
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLRendererOpenGLES
