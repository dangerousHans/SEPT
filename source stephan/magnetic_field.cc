
#include "magnetic_field.hh"
#include "magnet_geometry.hh"
#include <G4FieldManager.hh>
#include <G4TransportationManager.hh>
#include <globals.hh>
#include <math.h>

magnetic_field::magnetic_field(G4double up, G4double down, 
			       G4double det_y, G4double B) 
{
  X_up   = up;
  X_down = down;
  Y      = det_y;
  By     = B;

  G4FieldManager* fieldMgr =
    G4TransportationManager::GetTransportationManager()->GetFieldManager();

  fieldMgr->SetDetectorField(this);
  fieldMgr->CreateChordFinder(this); // What's this call for?
}

magnetic_field::~magnetic_field() {}

const int magnetic_field_table::cmap[3] = { 2,0,1 };

void magnetic_field::GetFieldValue(const G4double yTrack[],
				   G4double B[3]) const 
{
  G4double 
    x = yTrack[0], 
    y = yTrack[1], 
    z = yTrack[2];

  B[0] = B[1] = B[2] = 0;

  if (fabs(y-Y) > magnet_geometry::gap/2)
    return;

  if (fabs(z) > magnet_geometry::size_Z/2)
    return;

  if (fabs(x-X_up) < magnet_geometry::size_X/2)
    {
      B[1] = By;
    }
  else if (fabs(x-X_down) < magnet_geometry::size_X/2)
    {
      B[1] = -By;
    }

  //  G4cerr << "B(" << x/mm << "," << y/mm << "," << z/mm << ")="
  //	 << B[1]/tesla << G4endl;

  return;
}

void magnetic_field::SetFieldValue(G4double B)
{
  By = B;
  G4FieldManager* fieldMgr =
    G4TransportationManager::GetTransportationManager()->GetFieldManager();
  fieldMgr->SetDetectorField(this);
}
