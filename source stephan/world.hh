/*
  World geometry adapted from ExN02 for Stereo Impact SEPT
  $Id: world.hh,v 1.3 2002/05/13 15:30:38 bottcher Exp $
  Copyright (c) 2002 Stephan Boettcher <boettcher@physik.uni-kiel.de>
*/

// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************

#ifndef _world_h
#define _world_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4MagneticField;
class G4RunManager;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction(G4RunManager *run);
  ~DetectorConstruction();
  
  G4VPhysicalVolume *Construct();

private:
  G4MagneticField *Bfield;
};

#endif // _world_h
