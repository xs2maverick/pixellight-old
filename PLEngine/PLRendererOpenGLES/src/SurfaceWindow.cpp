/*********************************************************\
 *  File: SurfaceWindow.cpp                              *
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
#ifdef WIN32
	#include <PLGeneral/PLGeneralWindowsIncludes.h>
#endif
#include <PLGeneral/Log/Log.h>
#include "PLRendererOpenGLES/Renderer.h"
#include "PLRendererOpenGLES/SurfaceWindow.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLGeneral;
using namespace PLMath;
namespace PLRendererOpenGLES {


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Destructor
*/
SurfaceWindow::~SurfaceWindow()
{
	DeInit();
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
SurfaceWindow::SurfaceWindow(PLRenderer::SurfaceWindowHandler &cHandler, handle nWindow, bool bFullscreen) :
	PLRenderer::SurfaceWindow(cHandler, nWindow, bFullscreen),
	m_hSurface(nullptr)
{
	// Init
	Init();
}


//[-------------------------------------------------------]
//[ Public virtual PLRenderer::SurfaceWindow functions    ]
//[-------------------------------------------------------]
bool SurfaceWindow::GetGamma(float &fRed, float &fGreen, float &fBlue) const
{
	// Error!
	return false;
}

bool SurfaceWindow::SetGamma(float fRed, float fGreen, float fBlue)
{
	// Error!
	return false;
}


//[-------------------------------------------------------]
//[ Public virtual PLRenderer::Surface functions          ]
//[-------------------------------------------------------]
Vector2i SurfaceWindow::GetSize() const
{
	if (GetWindow()) {
		#ifdef WIN32
			RECT sRect;
			GetClientRect((HWND)GetWindow(), &sRect);
			return Vector2i(sRect.right, sRect.bottom);
		#endif
		#ifdef LINUX
			// Get the X server display connection
			Display *pDisplay = ((Renderer&)GetRenderer()).m_pDisplay;
			if (pDisplay) {
				// Get X window geometry information
				::Window nRootWindow = 0;
				int	nPositionX = 0, nPositionY = 0;
				unsigned int nWidth = 0, nHeight = 0, nBorder = 0, nDepth = 0;
				XGetGeometry(pDisplay, GetWindow(), &nRootWindow, &nPositionX, &nPositionY, &nWidth, &nHeight, &nBorder, &nDepth);
				return Vector2i(nWidth, nHeight);
			}
		#endif
	}

	// Error
	return Vector2i::Zero;
}


//[-------------------------------------------------------]
//[ Private virtual PLRenderer::Surface functions         ]
//[-------------------------------------------------------]
bool SurfaceWindow::Init()
{
	// First check if theres a window
	const handle nWindow = GetWindow();
	if (nWindow) {
		// Get the OpenGL ES renderer
		Renderer &cRendererOpenGLES = (Renderer&)GetRenderer();

		// Create window surface
		m_hSurface = eglCreateWindowSurface(cRendererOpenGLES.GetEGLDisplay(), cRendererOpenGLES.GetEGLConfig(), (EGLNativeWindowType)nWindow, nullptr);
		if (m_hSurface == EGL_NO_SURFACE) {
			PL_LOG(Warning, "Could not create OpenGL ES surface");

			// Error!
			return false;
		}
	}

	// Done
	return true;
}

void SurfaceWindow::DeInit()
{
	// Initialized?
	if (m_hSurface) {
		// Get the OpenGL ES renderer
		Renderer &cRendererOpenGLES = (Renderer&)GetRenderer();

		// If this is the current render target, make the main window to the new current one
		if (cRendererOpenGLES.GetRenderTarget() == this) {
			// Make the internal dummy surface to the currently used one
			cRendererOpenGLES.MakeCurrent(nullptr);
		}

		// Destroy the EGL surface
		eglDestroySurface(cRendererOpenGLES.GetEGLDisplay(), m_hSurface);
	}
}

bool SurfaceWindow::MakeCurrent(uint8 nFace)
{
	// Make this surface current
	return (((Renderer&)GetRenderer()).MakeCurrent(m_hSurface) == EGL_TRUE);
}

bool SurfaceWindow::UnmakeCurrent()
{
	// Make the internal dummy surface to the currently used one
	return (((Renderer&)GetRenderer()).MakeCurrent(nullptr) == EGL_TRUE);
}

bool SurfaceWindow::Present()
{
	// Get the OpenGL ES renderer
	Renderer &cRendererOpenGLES = (Renderer&)GetRenderer();

	// Swap buffers
	return (cRendererOpenGLES.GetEGLDisplay() && m_hSurface && eglSwapBuffers(cRendererOpenGLES.GetEGLDisplay(), m_hSurface) == EGL_TRUE);
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLRendererOpenGLES
