//===========================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2003-2016, CHAI3D
    (www.chai3d.org)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    * Neither the name of CHAI3D nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE. 

    \author    <http://www.chai3d.org>
    \author    Francois Conti
    \version   3.2.0 $Rev: 1869 $
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CDemo1H
#define CDemo1H
//---------------------------------------------------------------------------
#include "chai3d.h"
#include "CGenericDemo.h"
//---------------------------------------------------------------------------

class cDemo1 : public cGenericDemo
{
    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

public:

    //! Constructor of cDemo1.
    cDemo1(const int a_numDevices,
           std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
           std::shared_ptr<cGenericHapticDevice> a_hapticDevice);
           //bool a_visualization);

    //! Destructor of cDemo1.
    virtual ~cDemo1() {};


    //-----------------------------------------------------------------------
    // PUBLIC METHODS:
    //-----------------------------------------------------------------------

public:

    //! Initialize demo
    virtual void init();

    //! Set stiffness
    virtual void setStiffness(double a_stiffness);

    //! Reset position of cubes
    virtual void resetObjectsPositions();

    //! Reset position of cube c
    virtual void resetObjectPosition(int c);

    virtual int updateDynamicsCube(int a_cube, bool a_state);

    virtual bool isObjectAvailableForMovement(cODEGenericBody* body);

    //! Set mass
    virtual void setMass(double a_smass);

    virtual void debugTemp();

    virtual int GetRigidObjects(double outInformation[]);

    virtual int GetInteractObjects(double outInformation[]);

    virtual int GetHapticInformation(double outInformation[]);

    double* GetRotationQuaternion(cMatrix3d mat);

    //-----------------------------------------------------------------------
    // PUBLIC MEMBERS:
    //-----------------------------------------------------------------------

public:

    //! ODE objects
    cODEGenericBody* m_ODEBody0;
    cODEGenericBody* m_ODEBody1;
    cODEGenericBody* m_ODEBody2;
    cODEGenericBody* m_ODEBody3;
    cODEGenericBody* m_ODEBody4;
    cODEGenericBody* m_ODEBody5;

    //! ODE obstacles
    cODEGenericBody* m_ODEWall;

    cODEGenericBody* cubeBeingUsed;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
