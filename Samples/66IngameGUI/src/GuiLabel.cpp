/*********************************************************\
 *  File: GuiLabel.cpp                                   *
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
#include <PLCore/String/RegEx.h>
#include <PLMath/Vector2i.h>
#include <PLGui/Gui/Gui.h>
#include <PLGui/Gui/Resources/Graphics.h>
#include <PLGui/Gui/Resources/Font.h>
#include "Application.h"
#include "GuiLabel.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLMath;
using namespace PLGraphics;
using namespace PLGui;


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(GuiLabel)


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
GuiLabel::GuiLabel(Widget *pParent) : GuiBase(pParent),
	m_pFont(Application::GuiFont),
	m_cColor(Color4::White),
	m_nAlign(AlignCenter)
{
	// Set transparent background
	SetBackgroundColor(Color4::Transparent);
}

/**
*  @brief
*    Get text
*/
String GuiLabel::GetText() const
{
	// Return text
	return m_sText;
}

/**
*  @brief
*    Set text
*/
void GuiLabel::SetText(const String &sText)
{
	// Set text
	m_sText = sText;
}

/**
*  @brief
*    Get font
*/
PLGui::Font *GuiLabel::GetFont() const
{
	// Return font
	return m_pFont;
}

/**
*  @brief
*    Set font
*/
void GuiLabel::SetFont(PLGui::Font *pFont)
{
	// Set font
	m_pFont = pFont;;
}

/**
*  @brief
*    Get text color
*/
Color4 GuiLabel::GetColor() const
{
	// Return color
	return m_cColor;
}

/**
*  @brief
*    Set text color
*/
void GuiLabel::SetColor(const Color4 &cColor)
{
	// Set color
	m_cColor = cColor;
}

/**
*  @brief
*    Get text alignment
*/
GuiLabel::ETextAlign GuiLabel::GetAlign() const
{
	// Return alignment
	return m_nAlign;
}

/**
*  @brief
*    Set text alignment
*/
void GuiLabel::SetAlign(ETextAlign nAlign)
{
	// Set alignment
	m_nAlign = nAlign;
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Destructor
*/
GuiLabel::~GuiLabel()
{
}


//[-------------------------------------------------------]
//[ Public virtual PLGui::Widget functions                ]
//[-------------------------------------------------------]
void GuiLabel::OnDraw(Graphics &cGraphics)
{
	// Is the font valid?
	if (m_pFont) {
		// Get text alignment
		if (m_nAlign == AlignLeft) {
			// Left aligned
			cGraphics.DrawText(*m_pFont, m_cColor, Color4::Transparent, Vector2i::Zero, m_sText);
		} else if (m_nAlign == AlignRight) {
			// Right aligned
			uint32 nWidth = cGraphics.GetTextWidth(*m_pFont, m_sText);
			cGraphics.DrawText(*m_pFont, m_cColor, Color4::Transparent, Vector2i(GetSize().x - nWidth, 0), m_sText);
		} else if (m_nAlign == AlignCenter) {
			// Centered
			uint32 nWidth = cGraphics.GetTextWidth(*m_pFont, m_sText);
			cGraphics.DrawText(*m_pFont, m_cColor, Color4::Transparent, Vector2i((GetSize().x - nWidth) / 2, 0), m_sText);
		} else if (m_nAlign == AlignBlock) {
			// Draw block text
			Vector2i vPos (0, 0);
			Vector2i vSize(GetSize());

			// Draw text word for word
			RegEx cRegEx("\\s*([^\\s]+)");
			uint32 nParsePos = 0;
			while (cRegEx.Match(m_sText, nParsePos)) {
				// Get word
				nParsePos = cRegEx.GetPosition();
				String sWord = cRegEx.GetResult(0);

				// Get text width
				int nWidth = cGraphics.GetTextWidth(*m_pFont, sWord);
				if (vPos.x + nWidth >= vSize.x) {
					// Line break
					vPos.x = 0;
					vPos.y = vPos.y + cGraphics.GetTextHeight(*m_pFont, sWord);
				}

				// Draw word
				cGraphics.DrawText(*m_pFont, m_cColor, Color4::Transparent, vPos, sWord);

				// Next word
				vPos.x += nWidth + 8;
			}
		}
	}
}