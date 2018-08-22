/*
 * Copyright (c) 2018, Sunanda Bose (Neel Basu) (neel.basu.z@gmail.com) 
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met: 
 * 
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY 
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
 * DAMAGE. 
 */

#ifndef WSTP_DECLARES_H
#define WSTP_DECLARES_H

#include "m.h"

#define MATHEMATICA_DECLARE(F) namespace mathematica{ \
template <typename... T> \
mathematica::m F(T... args){return mathematica::m(#F)(args...);} \
}


// namespace mathematica{
// template <typename... T>
// mathematica::m Power(T... args){return mathematica::m("Power")(args...);}
// }

MATHEMATICA_DECLARE(Import)
MATHEMATICA_DECLARE(List)
MATHEMATICA_DECLARE(Total)
MATHEMATICA_DECLARE(Power)
MATHEMATICA_DECLARE(Surd)
MATHEMATICA_DECLARE(Sqrt)
MATHEMATICA_DECLARE(CubeRoot)
MATHEMATICA_DECLARE(Exp)
MATHEMATICA_DECLARE(Log)
MATHEMATICA_DECLARE(Log10)
MATHEMATICA_DECLARE(Log2)
MATHEMATICA_DECLARE(Factorial)
MATHEMATICA_DECLARE(FactorInteger)
MATHEMATICA_DECLARE(Sin)
MATHEMATICA_DECLARE(Cos)
MATHEMATICA_DECLARE(Tan)
MATHEMATICA_DECLARE(ArcSin)
MATHEMATICA_DECLARE(ArcCos)
MATHEMATICA_DECLARE(ArcTan)
MATHEMATICA_DECLARE(Csc)
MATHEMATICA_DECLARE(Sec)
MATHEMATICA_DECLARE(Cot)
MATHEMATICA_DECLARE(Max)
MATHEMATICA_DECLARE(Min)
MATHEMATICA_DECLARE(Solve)
MATHEMATICA_DECLARE(NSolve)
MATHEMATICA_DECLARE(Maximize)
MATHEMATICA_DECLARE(NMaximize)
MATHEMATICA_DECLARE(Minimize)
MATHEMATICA_DECLARE(NMinimize)

#endif // WSTP_DECLARES_H
