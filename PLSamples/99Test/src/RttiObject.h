/*********************************************************\
 *  File: RttiObject.h                                   *
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


#ifndef __PLSAMPLE_RTTIOBJECT_H__
#define __PLSAMPLE_RTTIOBJECT_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/Base/Object.h>
#include <PLCore/Base/Class.h>
#include <PLCore/Base/Rtti.h>

// Test enumeration
enum TestEnum {
	Zero,
	One,
	Two,
	Three
};

namespace Hallo {

// Test flags
enum TestFlags {
	Big = 1,
	Fat = 2,
	Ugly = 4
};

// Define enum in RTTI
pl_enum(TestFlags)
	pl_enum_value(Big,  "Don't mind the size")
	pl_enum_value(Fat,  "Don't mind the weight")
	pl_enum_value(Ugly, "Just don't mind")
pl_enum_end

}

// Test flags #2
enum TestFlags2 {
	Red = 8,
	Green = 16,
	Blue = 32
};

// Define enum in RTTI
pl_enum(TestEnum)
	pl_enum_value(Zero,  "Number 0")
	pl_enum_value(One,   "Number 1")
	pl_enum_value(Two,   "Number 2")
	pl_enum_value(Three, "Number 3")
pl_enum_end

// Define enum in RTTI
pl_enum(TestFlags2)
	pl_enum_base(Hallo::TestFlags)
	pl_enum_value(Red,   "Have a red color")
	pl_enum_value(Green, "Have a green color")
	pl_enum_value(Blue,  "Have a blue color")
pl_enum_end

// Define float-enum (not really possible in C++, use empty helper class)
struct FamousNumbers {};

// Define enum in RTTI
pl_enum_direct(FamousNumbers, float)
	pl_enum_value_direct(Zero,			0.0f,		"Ask a mathematician whether zero is a number (if you want some fun)")
	pl_enum_value_direct(Pi,			3.1415f,	"Also known as Archimedes Constant")
	pl_enum_value_direct(e,				2.71828f,	"Euler number")
	pl_enum_value_direct(TheTruth,		42,			"Answer to the Ultimate Question of Life, the Universe, and Everything")
	pl_enum_value_direct(HalfTheTruth,	21,			"21 is just half the truth")
pl_enum_end


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Test object
*/
class RttiObject : public PLCore::Object {


	//[-------------------------------------------------------]
	//[ Public data types                                     ]
	//[-------------------------------------------------------]
	public:
		// Enumeration
		enum NestedEnum {
			Inside1 = 1,
			Inside2
		};

		// Define enum in RTTI
		pl_enum(NestedEnum)
			pl_enum_value(Inside1, "")
			pl_enum_value(Inside2, "")
		pl_enum_end


	//[-------------------------------------------------------]
	//[ RTTI interface                                        ]
	//[-------------------------------------------------------]
	pl_class(pl_rtti_export, RttiObject, "", PLCore::Object, "Samples RTTI class")
		pl_properties
			pl_property	 ("PluginType",  "Widget")
			pl_property  ("FileFormats", "avi mpg mp4")
		pl_properties_end
		pl_attribute	 (BoolValue,	bool,						 true, ReadWrite, DirectValue,	"Boolean value",			"")
//		pl_attribute	 (IntValue,		int,						  200, ReadWrite, DirectValue,	"Integer value #1",			"")
		pl_attribute	 (IntValue,		int,						  200, ReadWrite, GetSet,		"Integer value #1",			"")
		pl_attribute	 (IntValue2,	int,						   10, ReadOnly,  DirectValue,	"Integer value #2",			"")
		pl_attribute	 (Number,		pl_enum_type(RttiObject::NestedEnum),Inside1, ReadWrite, DirectValue,	"Enum number",				"")
		pl_attribute	 (State,		pl_flag_type(Hallo::TestFlags),	0, ReadWrite, DirectValue,	"Flag number",				"")
		pl_attribute	 (Float,		pl_enum_type(FamousNumbers), 3.1415f, ReadWrite, DirectValue,	"Famous number",			"")
		pl_method_2		 (Test,			pl_ret_type(void),	int, float,								"Test method",				"")
		pl_method_1		 (MyMethod,		pl_ret_type(bool),	PLGeneral::String&,						"Test",						"")
		pl_signal_0		 (Event0,																	"Test event",				"")
		pl_signal_2		 (Event1,							int,	int,							"Test event #2",			"")
		pl_slot_1		 (OnSomeEvent0,						int,									"Test event handler",		"")
		pl_constructor_0 (Create,																	"Default constructor",		"")
		pl_constructor_1 (CreateValue,						int,									"Create with given value",	"")
		pl_constructor_2 (CreateValues,						int, float,								"Create with two values",	"")
	pl_class_end


	//[-------------------------------------------------------]
	//[ Public functions                                      ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*/
		RttiObject();

		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] nValue
		*    Integer value
		*/
		RttiObject(int nValue);

		/**
		*  @brief
		*    Constructor
		*
		*  @param[in] nValue
		*    Integer value
		*  @param[in] fValue
		*    Float value
		*/
		RttiObject(int nValue, float fValue);

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~RttiObject();

		int GetIntValue();
		void SetIntValue(int nValue);

		/**
		*  @brief
		*    Test function
		*/
		void Test(int, float);

		/**
		*  @brief
		*    Test event handler
		*/
		void OnSomeEvent0(int nParam0);

		bool MyMethod(PLGeneral::String &sParam);

	private:
		int m_nIntValue;



};


#endif // __PLSAMPLE_RTTIOBJECT_H__
