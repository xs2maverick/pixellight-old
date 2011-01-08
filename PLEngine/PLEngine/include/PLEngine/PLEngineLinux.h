/*********************************************************\
 *  File: PLEngineLinux.h                                *
 *      Linux definitions for PLEngine
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


#ifndef __PLENGINE_LINUX_H__
#define __PLENGINE_LINUX_H__
#pragma once


//[-------------------------------------------------------]
//[ Import/Export                                         ]
//[-------------------------------------------------------]
#ifdef PLENGINE_EXPORTS
	#ifdef HAVE_VISIBILITY_ATTR
		// To import classes, methods and variables
		#define PL_API __attribute__ ((visibility("default")))
	#else
		// To import classes, methods and variables
		#define PL_API
	#endif

	// To export explicit template instanciations
	#define PL_TEMPLATE
#else
	// To import classes, methods and variables
	#define PL_API

	// To import explicit template instanciations
	#define PL_TEMPLATE extern
#endif

// To export RTTI elements - unlike in MSVC for Microsoft Windows, this in here must always be 1 or the RTTI may not work correctly
#define PL_RTTI_EXPORT 1


#endif // __PLENGINE_LINUX_H__
