/**********************************************************************
 *      Author: tstern
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

    Misfit Tech invests time and resources providing this open source code,
    please support MisfitTech and open-source hardware by purchasing
	products from MisfitTech, www.misifittech.net!
 *********************************************************************/
#ifndef __AS5047D_H
#define __AS5047D_H

#include "mydef.h"
#define AS5047D_DEGREES_PER_BIT  (360.0/(float)(0x3FFF))    //每比特的角度值


#define AS5047D_CMD_NOP         0x0000          //空操作
#define AS5047D_CMD_ERRFL       0x0001          //错误寄存器
#define AS5047D_CMD_PROG        0x0003          //Programming register
#define AS5047D_CMD_DIAAGC      0x3FFC          //Diagnostic and AGC   磁场自动增益控制值
#define AS5047D_CMD_MAG         0x3FFD          //CORDIC magnitude     磁场强度值
#define AS5047D_CMD_ANGLEUNC    0x3FFE          //Measured angle without dynamic angle error compensation
#define AS5047D_CMD_ANGLECOM    0x3FFF          //Measured angle with dynamic angle error compensation

#define AS5047D_CMD_ZPOSM       0X0016          //Zero position MSB 0位置寄存器
#define AS5047D_CMD_ZPOSL       0X0017          //Zero position LSB /MAG diagnostic
#define AS5047D_CMD_SETTINGS1   0X0018          //Custom setting register 1
#define AS5047D_CMD_SETTINGS2   0X0019          //Custom setting register 2
#define AS5047D_CMD_RED         0X001A          //Redundancy register



#define AS5048A_CMD_NOP         0x0000
#define AS5048A_CMD_ERRFL       0x0001
#define AS5048A_CMD_PROG        0x0003
#define AS5048A_CMD_DIAAGC      0x3FFD
#define AS5048A_CMD_MAG         0x3FFE
#define AS5048A_CMD_ANGLE       0x3FFF


#define AS5047_CS       PBout(12)               //as5047d片选信号

void As5047d_SPI_Init(void);
int16_t readAddress(uint16_t addr);
int16_t ReadAngle_Init(uint16_t addr);

uint8_t As5047d_begin(void);
int16_t readEncoderAngle(void);
float ReadEncoderAngle(void);
    
void diagnostics(char *ptrStr);
int16_t readEncoderAnglePipeLineRead(void);
uint8_t getError(void);
int16_t Average_Num_Filter(void);
int16_t Move_Filter(void);

#endif //__AS5047D_H__


