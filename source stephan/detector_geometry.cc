 
#include "detector_geometry.hh"
#include "sensor_geometry.hh"

#include <G4Material.hh>
#include <G4Tubs.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4SDManager.hh>
#include "scintillator.hh"


const double detector_geometry::nm = 1e-9*m;

const double detector_geometry::slack = 1*nm;

const double detector_geometry::stack_height   =  5.7*mm;
const double detector_geometry::stack_diameter = 27.6*mm;

  // A PIPS detector consists of five layers:

  //      50 nm  passive Si
  // 299.680 um  depleted Si <-- sensitive volume
  //     120 nm  passive Si
  //     150 nm  Al
  
  // Octagon, hight=13.8mm, diagonal = 15mm
  
const double detector_geometry::pips_diameter   =  14.17*mm;
const double detector_geometry::center_diameter =   8.214*mm; // 53.0 mm^2
const double detector_geometry::xtalk_width     =   0.1*mm;
const double detector_geometry::guard_diameter  =  11.948*mm; // 56.5 mm^2

   // junction side
const double detector_geometry::chip_inner_metal  = 1000*nm;
const double detector_geometry::chip_inner_window =  500*nm;
const double detector_geometry::pips_height       =  0.3*mm;
const double detector_geometry::chip_outer_window =  150*nm;
const double detector_geometry::chip_outer_metal = 170*nm;

  // There are two PIPS detectors, slightly off-center in z

const double detector_geometry::pips_separation =  1.0*mm;
const double detector_geometry::pips_z          = (stack_height/2 
						   - 2.6*mm 
						   - pips_height
						   - pips_separation / 2);
  
  // The detector stack housing, aluminum tubes

const double detector_geometry::cover_height   =  0.9*mm;
const double detector_geometry::hole_diameter  = 10.6*mm;
const double detector_geometry::bottom_height  =  0.5*mm;
const double detector_geometry::wall_thickness =  (27.6-24.7)/2*mm;

detector_geometry::~detector_geometry()
{
}

detector_geometry::detector_geometry()

  // The detector stack is a cylinder in Space

  : G4LogicalVolume(new G4Tubs("detectorstack",
			       0, stack_diameter / 2, 
			       stack_height / 2,
			       0, 2*M_PI ),
		    G4Material::GetMaterial(sensor_geometry::space),
		    "detectorstack")

{
  SetVisAttributes (G4VisAttributes::Invisible);

  G4Material *Si = G4Material::GetMaterial("detSi"); 
  G4Material *Al = G4Material::GetMaterial("Al");
  
  G4LogicalVolume *pips =
    new G4LogicalVolume(new G4Tubs("pips",
				   0, pips_diameter / 2,
				   pips_height / 2,
				   0, 2*M_PI ),
			Al,
			"pips" );

  G4VisAttributes *pips_attr = new G4VisAttributes(G4Colour(0, 1, 1));
  pips_attr->SetForceSolid(true);

  pips->SetVisAttributes(pips_attr);
  
  G4LogicalVolume *silicon = 
    new G4LogicalVolume (new G4Tubs("silicon",
				    0, pips_diameter / 2 - slack,
				    (pips_height 
				     - chip_inner_metal 
				     - chip_outer_metal) / 2 - slack,
				    0, 2*M_PI ),
			 Si,
			 "silicon" );
  
  silicon->SetVisAttributes (G4VisAttributes::Invisible);
  
  new G4PVPlacement(0,
		    G4ThreeVector(0,
				  0,
				  (chip_outer_metal - chip_inner_metal)/2), 
		    silicon,
		    "silicon",
		    pips,      
		    false,
		    -1);

  G4LogicalVolume *guard_ring = 
    new G4LogicalVolume (new G4Tubs("guard_ring",
				    center_diameter/2 + xtalk_width,
				    guard_diameter / 2,
				    (pips_height
				     -chip_inner_window
				     -chip_inner_metal
				     -chip_outer_window
				     -chip_outer_metal) / 2,
				    0, 2*M_PI ),
			 Si,
			 "guard_ring" );

  G4LogicalVolume *center = 
    new G4LogicalVolume (new G4Tubs("center",
				    0, center_diameter/2,
				    (pips_height
				     -chip_inner_window
				     -chip_inner_metal
				     -chip_outer_window
				     -chip_outer_metal) / 2,
				    0, 2*M_PI ),
			 Si,
			 "center" );
  
  guard_ring->SetVisAttributes (G4VisAttributes::Invisible);
  center->SetVisAttributes (pips_attr);
  
  new G4PVPlacement(0,
		    G4ThreeVector(0,
				  0,
				  (chip_outer_window-chip_inner_window) / 2), 
		    guard_ring,   // its logical volume
		    "guard_ring", // its name
		    silicon,      // its mother  volume
		    false,        // no boolean operations
		    0);

  new G4PVPlacement(0,
		    G4ThreeVector(0,
				  0,
				  (chip_outer_window-chip_inner_window) / 2), 
		    center,       // its logical volume
		    "center",     // its name
		    silicon,      // its mother  volume
		    false,        // no boolean operations
		    1);

  G4SDManager* sd_man = G4SDManager::GetSDMpointer();

  scint_detector *ssd = new scint_detector("ssd");
  sd_man->AddNewDetector(ssd);
  center->SetSensitiveDetector(ssd);
  guard_ring->SetSensitiveDetector(ssd);

  // The bottom detector goes in unrotated

  new G4PVPlacement(0,
		    G4ThreeVector(0,0, 
				  -(pips_separation+pips_height)/2 + pips_z), 
		    pips,   // its logical volume
		    "pips1", // its name
		    this,           // its mother  volume
		    false,          // no boolean operations
		    1);

  G4RotationMatrix *mirror = new G4RotationMatrix;
  mirror->rotateX(M_PI);

  new G4PVPlacement(mirror,
		    G4ThreeVector(0,0, 
				  (pips_separation+pips_height)/2 + pips_z), 
		    pips,   // its logical volume
		    "pips2", // its name
		    this,           // its mother  volume
		    false,          // no boolean operations
		    3);

  G4Material *alu = Al;

  G4LogicalVolume *cover = 
    new G4LogicalVolume (new G4Tubs("detectorcover",
				    hole_diameter / 2, 
				    stack_diameter / 2 - slack,
				    cover_height / 2 - slack,
				    0, 2*M_PI ),
			 alu,
			 "detectorcover" );

  new G4PVPlacement(0,
		    G4ThreeVector(0,0, (stack_height-cover_height)/2), 
		    cover,           // its logical volume
		    "detectorcover", // its name
		    this,            // its mother  volume
		    false,           // no boolean operations
		    -1);              // no field specific to volume

  G4LogicalVolume *bottom = 
    new G4LogicalVolume (new G4Tubs("detectorbottom",
				    hole_diameter / 2, 
				    stack_diameter / 2 - slack,
				    bottom_height / 2 - slack,
				    0, 2*M_PI ),
			 alu,
			 "detectorbottom" );

  new G4PVPlacement(0,
		    G4ThreeVector(0,0, -(stack_height-bottom_height)/2), 
		    bottom,           // its logical volume
		    "detectorbottom", // its name
		    this,            // its mother  volume
		    false,           // no boolean operations
		    -1);              // no field specific to volume

  G4LogicalVolume *wall = 
    new G4LogicalVolume (new G4Tubs("detectorwall",
				    stack_diameter/2 - wall_thickness,
				    stack_diameter / 2 - slack,
				    (stack_height
				     - cover_height
				     - bottom_height)/2 - slack,
				    0, 2*M_PI ),
			 alu,
			 "detectorwall" );

  new G4PVPlacement(0,
		    G4ThreeVector(0,0, (bottom_height-cover_height)/2), 
		    wall,           // its logical volume
		    "detectorwall", // its name
		    this,            // its mother  volume
		    false,           // no boolean operations
		    -1);              // no field specific to volume
  
  G4VisAttributes *va = new G4VisAttributes(G4Colour(0.2, 0.2, 0.2));

  cover->SetVisAttributes (va);
  bottom->SetVisAttributes (va);
  wall->SetVisAttributes (va);

  // The G10 PCB
  // ...
}
