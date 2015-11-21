 
#include "detector_geometry.hh"

#include <G4Material.hh>
#include <G4Tubs.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>

const double foil_geometry::slack = 1*nanometer;
const double foil_geometry::diameter = 10.6*mm; // detector_geometry::hole_diameter;
const double foil_geometry::foil_thickness = 5000*nm;
const double foil_geometry::foil_metal_thickness = 200*nm;
const char * foil_geometry::foil_material = "ParyleneN";

foil_geometry::~foil_geometry()
{
}

foil_geometry::foil_geometry()

  // a round disk of very thin parylene foil, coated with Aluminum

  : G4LogicalVolume(new G4Tubs("foil",
			       0, diameter/2, 
			       foil_thickness/2,
			       0, 2*M_PI ),
		    G4Material::GetMaterial("Al"),
		    "foil")

{
  SetVisAttributes(new G4VisAttributes(G4Colour(1, 1, 0)));

  G4LogicalVolume *core
    = new G4LogicalVolume(new G4Tubs("foil_core",
				     0, diameter/2-slack, 
				     (foil_thickness-foil_metal_thickness)/2,
				     0, 2*M_PI ),
			  G4Material::GetMaterial(foil_material),
			  "foil_core");
  
  core->SetVisAttributes(G4VisAttributes::Invisible);

  new G4PVPlacement(0,
		    G4ThreeVector(0,0,0),
		    core,
		    "foil_core",
		    this,
		    false,
		    0);     
}
