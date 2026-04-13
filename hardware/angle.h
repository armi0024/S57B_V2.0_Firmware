/*
 * angle.h
 *
 *  Created on: Sep 1, 2016
 *      Author: TStern
 *
	Copyright (C) 2018  MisfitTech,  All rights reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Written by Trampas Stern for MisfitTech.
 *********************************************************************/

#ifndef __ANGLE_H
#define __ANGLE_H
#include <stdlib.h>
#include <stdint.h>

#include "mydef.h"

//#define ANGLE_STEPS 0x10000
//#define ANGLE_MAX ((uint16_t)0x0FFFF)

//#define ANGLE_FROM_DEGREES(x) ((int32_t) ( ((float)ANGLE_STEPS*(float)(x)+180.0)/360.0 ) )
//#define ANGLE_T0_DEGREES(x) ( (float) ((float(x)*360.0)/((float)ANGLE_STEPS) ))
    


    int16_t operator1( const uint16_t a2);
    uint16_t operator2(const uint16_t y);
	uint16_t operator3(const long int x);
	uint16_t operator4(const unsigned long int x);

#endif /* ANGLE_H_ */
