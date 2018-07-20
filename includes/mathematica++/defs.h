/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef WSTP_DEFS_H
#define WSTP_DEFS_H

namespace mathematica{

enum packet_type{
    packet_ILLEGAL=0,

    packet_CALL=7,
    packet_EVALUATE=13,
    packet_RETURN=3,

    packet_INPUTNAME=8,
    packet_ENTERTEXT=14,
    packet_ENTEREXPR=15,
    packet_OUTPUTNAME=9,
    packet_RETURNTEXT=4,
    packet_RETURNEXPR=16,

    packet_DISPLAY=11,
    packet_DISPLAYEND=12,

    packet_MESSAGE=5,
    packet_TEXT=2,

    packet_INPUT=1,
    packet_INPUTSTR=21,
    packet_MENU=6,
    packet_SYNTAX=10,

    packet_SUSPEND=17,
    packet_RESUME=18,

    packet_BEGINDLG=19,
    packet_ENDDLG=20,

    packet_FIRSTUSER=128,
    packet_LASTUSER=255
};

}

#endif
