 
#include "magnet_geometry.hh"

#include <G4Material.hh>
#include <G4Box.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>

const double magnet_geometry::size_X = 16*mm;
const double magnet_geometry::size_Y =  8*mm;
const double magnet_geometry::size_Z = 16*mm;

const double magnet_geometry::gap    = 16*mm;

magnet_geometry::~magnet_geometry()
{
}

magnet_geometry::magnet_geometry()

  : G4LogicalVolume(new G4Box("magnet",
			      size_X/2, size_Y/2, size_Z/2),
		    G4Material::GetMaterial("NdFeB"),
		    "magnet")

{
  G4VisAttributes *va = new G4VisAttributes(G4Colour(1, 0, 1));
  va->SetForceSolid(true);
  SetVisAttributes(va);
}
