/*
  World geometry adapted from ExN02 for Stereo Impact SEPT
  $Id: world.cc,v 1.13 2008/02/07 14:58:09 bottcher Exp $
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
 
#include "world.hh"
#include "materials.hh"
#include "sensor_geometry.hh"
#include "magnetic_field.hh"

#include <G4PVPlacement.hh>
#include <G4RunManager.hh>

#include <stdio.h>


// Method for formatted output of G4MagneticField *BF into datafile *out
inline static void Bfield_dump(G4MagneticField *BF, FILE *out)
{
  double x[4];
  x[3]=0;
  double B[3];

  fprintf(out, "tesla=%G\n", tesla);

  for (x[0]=-sensor_geometry::X_width/2;
       x[0] <= 3*sensor_geometry::X_width/2;
       x[0] += 1*mm )
    for (x[1]=-sensor_geometry::Y_height/2;
	 x[1] <= sensor_geometry::Y_height/2;
	 x[1] += 1*mm )
      for (x[2]=-sensor_geometry::Z_depth/2;
	   x[2] <= sensor_geometry::Z_depth/2;
	   x[2] += 1*mm )
	{
	  BF->GetFieldValue(x, B);
	  fprintf(out,
		  "%G %G %G %G %G %G\n",
		  x[0]/mm, x[1]/mm, x[2]/mm, 
		  B[0]/tesla, B[1]/tesla, B[2]/tesla); 
	}
}

DetectorConstruction::DetectorConstruction(G4RunManager *run)
{
#ifdef KASTEN_FELD
  //Define simple magnetic field
  Bfield = new magnetic_field(sensor_geometry::det_X,  //bug move detector to origin, comment from Stephan
			      sensor_geometry::det_X + 40*mm,
			      sensor_geometry::det_Y,
			      0.5*tesla);
#else
  //Bfield from datafile
  Bfield = new magnetic_field_table("sept_bfield.dat",
				    20*mm, 0, -sensor_geometry::det_Z);
#endif 

  //Dump Bfield in datafile
  FILE *f = fopen("sept_magentic_field_dump.dat", "w");
  Bfield_dump(Bfield, f);
  fclose(f);
}

DetectorConstruction::~DetectorConstruction()
{
  delete Bfield;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4cerr << "Creating the World" << G4endl;

  make_materials();

  G4cerr << "Manufacture the Sensor" << G4endl;

  //Create sensor
  G4LogicalVolume *sensor = new sensor_geometry;
  
  //Place sensor in world and return world
  return 
    new G4PVPlacement(0,           // no rotation
		      G4ThreeVector(),     // at (0,0,0)
		      "World",             // its name
		      sensor,              // its logical volume
		      0,                   // its mother  volume
		      false,               // no boolean operations
		      0);                  // no field specific to volume  
}
