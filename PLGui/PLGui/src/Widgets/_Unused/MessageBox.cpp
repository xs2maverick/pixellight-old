/*********************************************************\
 *  File: MessageBox.cpp                                 *
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
#include "PLGui/Widgets/Layout/Box.h"
#include "PLGui/Widgets/Controls/Label.h"
#include "PLGui/Widgets/Controls/Button.h"
#include "PLGui/Widgets/Windows/MessageBox.h"


//[-------------------------------------------------------]
//[ Compiler settings                                     ]
//[-------------------------------------------------------]
PL_WARNING_PUSH
PL_WARNING_DISABLE(4355) // "'this' : used in base member initializer list"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLGeneral;
using namespace PLMath;
namespace PLGui {


//[-------------------------------------------------------]
//[ Class implementation                                  ]
//[-------------------------------------------------------]
pl_implement_class(MessageBox)


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
MessageBox::MessageBox(Widget *pParent, EMessageBox nType, const String &sTitle, const String &sText) : Dialog(pParent),
	EventHandlerOk	  (&MessageBox::OnButtonOk, this),
	EventHandlerCancel(&MessageBox::OnButtonCancel, this),
	EventHandlerYes	  (&MessageBox::OnButtonYes, this),
	EventHandlerNo	  (&MessageBox::OnButtonNo, this),
	m_nType(MessageBoxOk),
	m_pLabel(nullptr),
	m_pBox(nullptr),
	m_pButtonOk(nullptr),
	m_pButtonCancel(nullptr),
	m_pButtonYes(nullptr),
	m_pButtonNo(nullptr)
{
	// Create message box
	CreateMessageBox(nType, sTitle, sText);
}

/**
*  @brief
*    Constructor
*/
MessageBox::MessageBox(EMessageBox nType, const String &sTitle, const String &sText) : Dialog(nullptr),
	EventHandlerOk	  (&MessageBox::OnButtonOk, this),
	EventHandlerCancel(&MessageBox::OnButtonCancel, this),
	EventHandlerYes	  (&MessageBox::OnButtonYes, this),
	EventHandlerNo	  (&MessageBox::OnButtonNo, this),
	m_nType(MessageBoxOk),
	m_pLabel(nullptr),
	m_pBox(nullptr),
	m_pButtonOk(nullptr),
	m_pButtonCancel(nullptr),
	m_pButtonYes(nullptr),
	m_pButtonNo(nullptr)
{
	// Create message box
	CreateMessageBox(nType, sTitle, sText);
}

/**
*  @brief
*    Destructor
*/
MessageBox::~MessageBox()
{
}

/**
*  @brief
*    Get message box type
*/
EMessageBox MessageBox::GetType() const
{
	// Return type
	return m_nType;
}

/**
*  @brief
*    Get text
*/
String MessageBox::GetText() const
{
	// Return text
	return m_sText;
}

/**
*  @brief
*    Set text
*/
void MessageBox::SetText(const String &sText)
{
	// Set text
	m_sText = sText;

	// Set label text
	if (m_pLabel) {
		m_pLabel->SetText(m_sText);
	}
}

/**
*  @brief
*    Get button text
*/
String MessageBox::GetButtonText(EMessageBoxButton nButton) const
{
	// Get text
	String sText;
	if (nButton == MessageBoxButtonOk && m_pButtonOk) {
		// Get text from button 'Ok'
		sText = m_pButtonOk->GetText();
	} else if (nButton == MessageBoxButtonCancel && m_pButtonCancel) {
		// Get text from button 'Cancel'
		sText = m_pButtonCancel->GetText();
	} else if (nButton == MessageBoxButtonYes && m_pButtonYes) {
		// Get text from button 'Yes'
		sText = m_pButtonYes->GetText();
	} else if (nButton == MessageBoxButtonNo && m_pButtonNo) {
		// Get text from button 'No'
		sText = m_pButtonNo->GetText();
	}

	// Return text
	return sText;
}

/**
*  @brief
*    Set button text
*/
void MessageBox::SetButtonText(EMessageBoxButton nButton, const String &sText)
{
	// Set text
	if (nButton == MessageBoxButtonOk && m_pButtonOk) {
		// Set text for button 'Ok'
		m_pButtonOk->SetText(sText);
	} else if (nButton == MessageBoxButtonCancel && m_pButtonCancel) {
		// Set text for button 'Cancel'
		m_pButtonCancel->SetText(sText);
	} else if (nButton == MessageBoxButtonYes && m_pButtonYes) {
		// Set text for button 'Yes'
		m_pButtonYes->SetText(sText);
	} else if (nButton == MessageBoxButtonNo && m_pButtonNo) {
		// Set text for button 'No'
		m_pButtonNo->SetText(sText);
	}
}


//[-------------------------------------------------------]
//[ Private virtual Dialog functions                      ]
//[-------------------------------------------------------]
void MessageBox::OnCloseDialog(int nResult)
{
	// Call base implementation
	Dialog::OnCloseDialog(nResult);
}

/**
*  @brief
*    Button 'Ok' has been clicked
*/
void MessageBox::OnButtonOk()
{
	// Close dialog and give back MessageBoxButtonOk as the result value
	CloseDialog(MessageBoxButtonOk);
}

/**
*  @brief
*    Button 'Cancel' has been clicked
*/
void MessageBox::OnButtonCancel()
{
	// Close dialog and give back MessageBoxButtonCancel as the result value
	CloseDialog(MessageBoxButtonCancel);
}

/**
*  @brief
*    Button 'Yes' has been clicked
*/
void MessageBox::OnButtonYes()
{
	// Close dialog and give back MessageBoxButtonYes as the result value
	CloseDialog(MessageBoxButtonYes);
}

/**
*  @brief
*    Button 'No' has been clicked
*/
void MessageBox::OnButtonNo()
{
	// Close dialog and give back MessageBoxButtonNo as the result value
	CloseDialog(MessageBoxButtonNo);
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Create message box
*/
void MessageBox::CreateMessageBox(EMessageBox nType, const String &sTitle, const String &sText)
{
	// Set type
	m_nType = nType;

	// Set title
	SetTitle(sTitle);

	// Set layout
	GetContentWidget()->SetLayout("LayoutBox", "Orientation=Vertical");
	GetContentWidget()->GetLayoutHints().SetPadding(SizeHint(SizeHint::Pixel, 12));
	GetContentWidget()->GetLayoutHints().SetSpacing(20);
	GetContentWidget()->GetLayoutHints().SetVAlign(AlignMiddle);

	// Add label
	m_pLabel = new Label(GetContentWidget());
	m_pLabel->SetAlign(AlignCenter);
	m_pLabel->SetVisible(true);

	// Add button box
	m_pBox = new Box(GetContentWidget(), Horizontal);
	m_pBox->SetVisible(true);

	// Add buttons
	if (m_nType == MessageBoxOk) {
		// Add button 'Ok'
		m_pButtonOk = new Button(m_pBox->GetContentWidget());
		m_pButtonOk->EventClicked.Connect(&EventHandlerOk);
		m_pButtonOk->SetText("Ok");
		m_pButtonOk->SetVisible(true);
	} else if (m_nType == MessageBoxOkCancel) {
		// Add button 'Ok'
		m_pButtonOk = new Button(m_pBox->GetContentWidget());
		m_pButtonOk->EventClicked.Connect(&EventHandlerOk);
		m_pButtonOk->SetText("Ok");
		m_pButtonOk->SetVisible(true);

		// Add button 'Cancel'
		m_pButtonCancel = new Button(m_pBox->GetContentWidget());
		m_pButtonCancel->EventClicked.Connect(&EventHandlerCancel);
		m_pButtonCancel->SetText("Cancel");
		m_pButtonCancel->SetVisible(true);
	} else if (m_nType == MessageBoxYesNo) {
		// Add button 'Yes'
		m_pButtonYes = new Button(m_pBox->GetContentWidget());
		m_pButtonYes->EventClicked.Connect(&EventHandlerYes);
		m_pButtonYes->SetText("Yes");
		m_pButtonYes->SetVisible(true);

		// Add button 'No'
		m_pButtonNo = new Button(m_pBox->GetContentWidget());
		m_pButtonNo->EventClicked.Connect(&EventHandlerNo);
		m_pButtonNo->SetText("No");
		m_pButtonNo->SetVisible(true);
	} else if (m_nType == MessageBoxYesNoCancel) {
		// Add button 'Yes'
		m_pButtonYes = new Button(m_pBox->GetContentWidget());
		m_pButtonYes->EventClicked.Connect(&EventHandlerYes);
		m_pButtonYes->SetText("Yes");
		m_pButtonYes->SetVisible(true);

		// Add button 'No'
		m_pButtonNo = new Button(m_pBox->GetContentWidget());
		m_pButtonNo->EventClicked.Connect(&EventHandlerNo);
		m_pButtonNo->SetText("No");
		m_pButtonNo->SetVisible(true);
		m_pButtonNo->GetLayoutHints().SetMarginRight(SizeHint(SizeHint::Pixel, 25));

		// Add button 'Cancel'
		m_pButtonCancel = new Button(m_pBox->GetContentWidget());
		m_pButtonCancel->EventClicked.Connect(&EventHandlerCancel);
		m_pButtonCancel->SetText("Cancel");
		m_pButtonCancel->SetVisible(true);
	}

	// Set text
	SetText(sText);

	// Set size and center on screen
	SetSize(Vector2i(300, 120));
	Center(CenterScreen);
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLGui


//[-------------------------------------------------------]
//[ Compiler settings                                     ]
//[-------------------------------------------------------]
PL_WARNING_POP
