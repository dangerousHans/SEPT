/*
  half sensor geometry adapted from ExN02 for Stereo Impact SEPT
  $Id: sensor_geometry.cc,v 1.18 2008/02/08 11:52:04 bottcher Exp $
  Copyright (c) 2002 Stephan Boettcher <boettcher@physik.uni-kiel.de>
*/
 
#include "sensor_geometry.hh"
#include "detector_geometry.hh"
#include "magnet_geometry.hh"
#include "apperture_geometry.hh"

#include <G4Region.hh>
#include <G4RegionStore.hh>
#include <G4ProductionCuts.hh>
#include <G4VisAttributes.hh>
#include <G4Material.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4PVPlacement.hh>
#include <ETUserLimits.hh>

#define dG4cerr if (0) G4cerr

const double sensor_geometry::infinity = 200.0*mm; // w/o conn., pin-puller

const double sensor_geometry::X_width  = 52.0*mm; // w/o conn., pin-puller
const double sensor_geometry::Y_height = 82.0*mm; // w/o doors 
const double sensor_geometry::Z_depth  = 68.0*mm; // w/o conn., doors, ...

const double sensor_geometry::det_X    =  X_width/2 - 20.0*mm;
const double sensor_geometry::det_Y    =  9.5*mm;
const double sensor_geometry::det_Z    = 13.05*mm;

const double sensor_geometry::foil_sep =  4.1*mm; // from pips surface

// FIXME: The apperture z dimensions are still for 0.6*mm pips_separation

const double sensor_geometry::foil_apperture_distance = 20.7*mm; // from pips surface
const double sensor_geometry::foil_apperture_depth = 15.4*mm;
const double sensor_geometry::foil_apperture_rout = 18.6*mm / 2;
const double sensor_geometry::foil_apperture_ropen = 12.4*mm / 2;
const int    sensor_geometry::foil_apperture_rings = 5;

const double sensor_geometry::mag_apperture_distance = 29.3*mm; // from pips surface 
const double sensor_geometry::mag_apperture_depth = 7.6*mm;
const double sensor_geometry::mag_apperture_rout = 23.6*mm / 2;
const double sensor_geometry::mag_apperture_ropen = 20.4*mm / 2;
const int    sensor_geometry::mag_apperture_rings = 3;

const double sensor_geometry::house_thickness = 3*mm;

const double sensor_geometry::slack = 1*nanometer;

const double sensor_geometry::mag_delrin_thickness  = 1.2*mm;
const double sensor_geometry::foil_delrin_thickness = 0.7*mm;

const char *sensor_geometry::space = "Space";

const double sensor_geometry::source_window = 0; // 0.0064*mm;
char const * sensor_geometry::source_window_mat = "PET";


sensor_geometry::~sensor_geometry()
{
}

// inline functions
inline double sensor_geometry::mag_pips_z()
{
  return det_Z
    + detector_geometry::pips_z
    - detector_geometry::pips_separation/2
    - detector_geometry::pips_height
    ;
}
inline double sensor_geometry::foil_pips_z()
{
  return det_Z
    + detector_geometry::pips_z
    + detector_geometry::pips_separation/2
    + detector_geometry::pips_height
    ;
}
inline double sensor_geometry::mag_det_z()
{
  return det_Z
    - detector_geometry::stack_height/2
    ;
}
inline double sensor_geometry::foil_det_z()
{
  return det_Z
    + detector_geometry::stack_height/2
    ;
}
 
// ------------------------------------- Constructor
// -------------------------------------------------
sensor_geometry::sensor_geometry()

  : G4LogicalVolume(new G4Box("sensor",
			      2*infinity, 2*infinity, 2*infinity),
		    G4Material::GetMaterial(sensor_geometry::space),
		    "sensor")

{
  SetVisAttributes(G4VisAttributes::Invisible);

  //--------------------------------------------------------------------
  // New logical volume "halfsensor" with region "SensorRegion"
  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  G4LogicalVolume *halfsensor 
    = new G4LogicalVolume(new G4Box("halfsensor",
				    infinity, infinity, infinity),
			  G4Material::GetMaterial(sensor_geometry::space),
			  "halfsensor");
  halfsensor -> SetVisAttributes (G4VisAttributes::Invisible);
  G4Region *Sensor_Region = new G4Region("The Sensor Region");
  Sensor_Region->AddRootLogicalVolume(halfsensor);

  // Limits "sensorcuts" for SensorRegion of halfsensor
  G4UserLimits *sensorcuts =
    new ETUserLimits("/geometry/sept/limits/sensor",
		     1*mm,      // step max
		     DBL_MAX,   // track max
		     DBL_MAX,   // Time max
		     0*keV,     // Ekin min
		     0*mm );    // Range min
  Sensor_Region->SetUserLimits(sensorcuts);

  // Place halfsensor in this (world) volume
  new G4PVPlacement(0,               // no rotation
		    G4ThreeVector(-det_X, -det_Y, -det_Z),
		    halfsensor,      // its logical volume
		    "halfsensor",    // its name
		    this,            // its mother  volume
		    false,           // no boolean operations
		    -1);              // no particular field
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

  // Limits for world ?
  G4UserLimits *spacecuts = 
    new ETUserLimits("/geometry/sept/limits/space",
		     1*mm,      // step max
		     DBL_MAX,   // track max
		     DBL_MAX,   // Time max
		     0*keV,     // Ekin min
		     0*mm );    // Range min
  SetUserLimits(spacecuts);

  //--------------------------------------------------------------------
  // New logical volume "detector" with region "Det-Region"
  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  G4UserLimits *siliconcuts = 
    new ETUserLimits("/geometry/sept/limits/detector",
		     0.2*mm,    // step max
		     DBL_MAX,   // track max
		     DBL_MAX,   // Time max
		     0*keV,     // Ekin min
		     0*mm );    // Range min

  G4Region *Det_Region = new G4Region("Solid State Detector Region");
  Det_Region->SetProductionCuts(new G4ProductionCuts);
  Det_Region->GetProductionCuts()->SetProductionCut(0.01*mm);
  Det_Region->GetProductionCuts()->SetProductionCut(0.3*mm, "gamma");
  Det_Region->SetUserLimits(siliconcuts);
  
  // Create detector "volume" with region "Det_Region" and "siliconcuts"
  G4LogicalVolume* detector = new detector_geometry;
  Det_Region->AddRootLogicalVolume(detector);
  
  //Place detector in halfsensor
  new G4PVPlacement(0,               // no rotation
		    G4ThreeVector(det_X, det_Y, det_Z),
		    detector,        // its logical volume
		    "detector",      // its name
		    halfsensor,      // its mother  volume
		    false,           // no boolean operations
		    -1);              // no particular field
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  
  //--------------------------------------------------------------------
  // New logical volume "foil"
  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  G4LogicalVolume* foil = new foil_geometry;
  new G4PVPlacement(0,               // no rotation
		    G4ThreeVector(det_X, det_Y, foil_pips_z() + foil_sep),
		    foil,            // its logical volume
		    "foil",          // its name
		    halfsensor,      // its mother  volume
		    false,           // no boolean operations
		    0);              // no particular field
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>		    

  //--------------------------------------------------------------------
  // New magnet_geometry "magnet"
  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  magnet_geometry* magnet = new magnet_geometry;
  static const double mag_Y = (magnet->size_Y + magnet->gap)/2;
 
  //Place first magnet
  new G4PVPlacement(0,               // no rotation
		    G4ThreeVector(det_X, det_Y-mag_Y, 0),
		    magnet,          // its logical volume
		    "magnet1",       // its name
		    halfsensor,      // its mother  volume
		    false,           // no boolean operations
		    0);              // no particular field
		    
  //Place second magnet
  new G4PVPlacement(0,               // no rotation
		    G4ThreeVector(det_X, det_Y+mag_Y, 0),
		    magnet,          // its logical volume
		    "magnet2",       // its name
		    halfsensor,      // its mother  volume
		    false,           // no boolean operations
		    1);              // no particular field
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  
  //--------------------------------------------------------------------
  // New apperture_geometry "mag_app"
  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  
  apperture_geometry *mag_app 
    = new apperture_geometry("magnet_apperture",
			     mag_apperture_rout,
			     mag_apperture_ropen,
			     mag_apperture_rings,
			     mag_apperture_distance,
			     mag_apperture_depth
			     );
  double magapp_z = mag_pips_z()
    - mag_apperture_distance
    + mag_apperture_depth/2;
  
  // Rotation matrix for placement
  G4RotationMatrix *mirror = new G4RotationMatrix;
  mirror->rotateX(M_PI);

  new G4PVPlacement(mirror,
		    G4ThreeVector(det_X, det_Y, magapp_z),
		    mag_app,
		    "magnet_apperture",
		    halfsensor,
		    false,
		    0);
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>		    
  
  //--------------------------------------------------------------------
  // New apperture_geometry "foil_app"
  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<   
    apperture_geometry *foil_app 
    = new apperture_geometry("foil_apperture",
			     foil_apperture_rout,
			     foil_apperture_ropen,
			     foil_apperture_rings,
			     foil_apperture_distance,
			     foil_apperture_depth
			     );
  double foilapp_z = foil_pips_z()
    + foil_apperture_distance
    - foil_apperture_depth/2
    ;

  new G4PVPlacement(0,
		    G4ThreeVector(det_X, det_Y, foilapp_z),
		    foil_app,
		    "foil_apperture",
		    halfsensor,
		    false,
		    0);
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>			    
  
  if (source_window > 0)
    {
      G4LogicalVolume *win = 
	new G4LogicalVolume (new G4Tubs("win",
					0, foil_apperture_ropen,
					source_window/2,
					0, 2*M_PI ),
			     G4Material::GetMaterial(source_window_mat),
			     "win" );
      
      new G4PVPlacement(0,
			G4ThreeVector(det_X, det_Y, 
				      foilapp_z
				      + foil_apperture_depth/2
				      + source_window/2+slack), 
			win,
			"win",
			halfsensor,
			false,
			0);  

      win->SetVisAttributes(new G4VisAttributes(G4Colour(1, 1, 0)));
    }

  //--------------------------------------------------------------------
  // here goes all the sensor housings, magnet yoke, ...

  // Those 3mm vertical bars at each side of the magnets, the outer
  // one being part of the housing frame, and the inner one being part
  // of the center/magnet separator.
  //--------------------------------------------------------------------

  G4LogicalVolume *house =
    new G4LogicalVolume(new G4Box("house",
				  house_thickness/2 - slack, 
				  magnet->gap/2 + magnet->size_Y - slack, 
				  magnet->size_Z/2 - slack),
			G4Material::GetMaterial("Al"),
			"house");
  
  G4VisAttributes *va = new G4VisAttributes(G4Colour(0.2, 0.2, 0.2));
  va->SetVisibility(true);
  house->SetVisAttributes(va);
  new G4PVPlacement(0,
		    G4ThreeVector(det_X
				  - magnet->size_X/2
				  - house_thickness/2, 
				  det_Y, 
				  0),
		    house,
		    "house_V1",
		    halfsensor,
		    false,
		    1 );

  new G4PVPlacement(0,
		    G4ThreeVector(det_X
				  + magnet->size_X/2
				  + house_thickness/2, 
				  det_Y, 
				  0),
		    house,
		    "house_V2",
		    halfsensor,
		    false,
		    2 );
		    
  //--------------------------------------------------------------------
  
  // Wrap the detector in Aluminum
  const double delrin_diameter = detector_geometry::stack_diameter;
  
  G4LogicalVolume *mag_delrin =
    new G4LogicalVolume(new G4Tubs("mag_delrin",
				   detector_geometry::hole_diameter/2 + slack, 
				   delrin_diameter/2 - slack,
				   mag_delrin_thickness/2 - slack,
				   0, 2*M_PI ),
			G4Material::GetMaterial("Al"),
			"mag_delrin" );
  
  mag_delrin->SetVisAttributes(va);
  new G4PVPlacement(0,
		    G4ThreeVector(det_X,
				  det_Y,
				  mag_det_z()-mag_delrin_thickness/2),
		    mag_delrin,
		    "mag_delrin",
		    halfsensor,
		    false,
		    0 );
  
  dG4cerr << "mag Delrin " 
	  << mag_delrin_thickness/mm 
	  << "mm at z="
	  << (mag_det_z()-mag_delrin_thickness/2)/mm
	  << "mm"
	  << G4endl
    ;
    
  //--------------------------------------------------------------------
  
  G4LogicalVolume *foil_delrin = 
    new G4LogicalVolume(new G4Tubs("foil_delrin",
				   detector_geometry::hole_diameter/2 + slack, 
				   delrin_diameter/2 - slack,
				   foil_delrin_thickness/2 - slack,
				   0, 2*M_PI ),
			G4Material::GetMaterial("Al"),
			"foil_delrin" );
  
  foil_delrin->SetVisAttributes(va);
  new G4PVPlacement(0,
		    G4ThreeVector(det_X,
				  det_Y,
				  foil_det_z()+foil_delrin_thickness/2),
		    foil_delrin,
		    "foil_delrin",
		    halfsensor,
		    false,
		    0 );

  dG4cerr << "foil Delrin " 
	  << foil_delrin_thickness/mm 
	  << "mm at z="
	  << (foil_det_z()+foil_delrin_thickness/2)/mm
	  << "mm"
	  << G4endl
    ;
    
  //--------------------------------------------------------------------
 
  // CLose the Z-gaps with aluminum plates
  const double plate_diameter = detector_geometry::stack_diameter;
  const double plate_hole = magnet_geometry::size_X + 1*mm;

  const double mag_app_plate_thickness = 
    sensor_geometry::mag_apperture_distance
    -  sensor_geometry::mag_apperture_depth
    -  mag_pips_z()
    -  magnet_geometry::size_Z/2
    ;
  
  G4LogicalVolume *mag_app_plate =
    new G4LogicalVolume(new G4Tubs("mag_app_plate",
				   plate_hole/2 + slack, 
				   plate_diameter/2 - slack,
				   mag_app_plate_thickness/2 - slack,
				   0, 2*M_PI ),
			G4Material::GetMaterial("Al"),
			"mag_app_plate" );
  
  mag_app_plate->SetVisAttributes(va);
  new G4PVPlacement(0,
		    G4ThreeVector(det_X,
				  det_Y,
				  - magnet_geometry::size_Z/2 
				  - mag_app_plate_thickness/2),
		    mag_app_plate,
		    "mag_app_plate",
		    halfsensor,
		    false,
		    0 );
  
  dG4cerr << "Magnet App plate " 
	  << mag_app_plate_thickness/mm 
	  << "mm at z="
	  << (- magnet_geometry::size_Z/2 - mag_app_plate_thickness/2)/mm
	  << "mm"
	  << G4endl
    ;
    
  //--------------------------------------------------------------------
  
  const double mag_det_plate_thickness = 
    mag_det_z() 
    - magnet_geometry::size_Z/2 
    - mag_delrin_thickness
    ;
  
  G4LogicalVolume *mag_det_plate =
    new G4LogicalVolume(new G4Tubs("mag_det_plate",
				   detector_geometry::hole_diameter/2 + slack, 
				   plate_diameter/2 - slack,
				   mag_det_plate_thickness/2 - slack,
				   0, 2*M_PI ),
			G4Material::GetMaterial("Al"),
			"mag_det_plate" );
  
  mag_det_plate->SetVisAttributes(va);
  new G4PVPlacement(0,
		    G4ThreeVector(det_X,
				  det_Y,
				  magnet_geometry::size_Z/2 
				  + mag_det_plate_thickness/2),
		    mag_det_plate,
		    "mag_det_plate",
		    halfsensor,
		    false,
		    0 );
  
  dG4cerr << "Magnet Det plate " 
	  << mag_det_plate_thickness/mm 
	  << "mm at z="
	  << (magnet_geometry::size_Z/2 + mag_det_plate_thickness/2)/mm
	  << "mm"
	  << G4endl
    ;
    
  //--------------------------------------------------------------------
  
  const double foil_det_plate_thickness = 
    foil_pips_z()
    + sensor_geometry::foil_apperture_distance
    - sensor_geometry::foil_apperture_depth
    - foil_det_z()
    - foil_delrin_thickness
    ;
  
  G4LogicalVolume *foil_det_plate =
    new G4LogicalVolume(new G4Tubs("foil_det_plate",
				   detector_geometry::hole_diameter/2 + slack, 
				   plate_diameter/2 - slack,
				   foil_det_plate_thickness/2 - slack,
				   0, 2*M_PI ),
			G4Material::GetMaterial("Al"),
			"foil_det_plate" );
  
  foil_det_plate->SetVisAttributes(va);
  new G4PVPlacement(0,
		    G4ThreeVector(det_X,
				  det_Y,
				  foil_det_z()
				  + foil_delrin_thickness
				  + foil_det_plate_thickness/2),
		    foil_det_plate,
		    "foil_det_plate",
		    halfsensor,
		    false,
		    0 );
  
  dG4cerr << "Foil Det plate " 
	  << foil_det_plate_thickness/mm 
	  << "mm at z="
	  << (foil_det_z()
	      + foil_delrin_thickness
	      + foil_det_plate_thickness/2)/mm
	  << "mm"
	  << G4endl
    ;
  //--------------------------------------------------------------------
}
