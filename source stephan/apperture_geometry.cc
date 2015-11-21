 
#include "apperture_geometry.hh"
#include "detector_geometry.hh"
#include "sensor_geometry.hh"

#include <G4Material.hh>
#include <G4Tubs.hh>
#include <G4Cons.hh>
#include <G4PVPlacement.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>

const double apperture_geometry::slack = 10*micrometer;

const double apperture_geometry::thickness = 1*mm;
const double apperture_geometry::rdet = 8.214*mm/2; //detector_geometry::center_diameter/2;

apperture_geometry::~apperture_geometry()
{
}

inline double apperture_geometry::aradius(double ropen, double z0, double dz)
{
  return (ropen-rdet) * (z0-dz) / z0 + rdet;
}

apperture_geometry::apperture_geometry(G4String name,
				       double rout,
				       double ropen,
				       int nz,
				       double z0,
				       double dz)

  // The apperture is a cone in space

  : G4LogicalVolume(new G4Cons(name,
			       aradius(ropen,z0,dz), rout,
			       ropen, rout,
			       dz/2,
			       0, 2*M_PI),
		    G4Material::GetMaterial(sensor_geometry::space),
		    name)
{
  G4VisAttributes *va = new G4VisAttributes(G4Colour(0.2, 0.3, 0.4));
  // va->SetForceSolid(true);

  SetVisAttributes(G4VisAttributes::Invisible);

  G4Material *Aluminum = G4Material::GetMaterial("Al");
  
  G4LogicalVolume *wall = 
    new G4LogicalVolume (new G4Tubs("wall",
				    rout-thickness, rout-slack,
				    dz/2 - slack,
				    0, 2*M_PI ),
			 Aluminum,
			 "wall" );
  
  new G4PVPlacement(0,
		    G4ThreeVector(0,0,0), 
		    wall,
		    "wall",
		    this,
		    false,
		    0);
  
  wall->SetVisAttributes (va);

  for (int iz=0; iz<nz; iz++)
    {
      G4String arname = G4String("aring") + G4String('0'+iz);
      
      double z1 = iz*dz/nz;
      double z2 = z1+thickness;
      double r1 = aradius(ropen,z0,z1);
      double r2 = aradius(ropen,z0,z2);

      if (iz)
	{
	  double r1i = (z0-z1)/(z0-z2)*(r2+rdet) - rdet;
	  double r2i = z2/z1 * (r1+ropen) - ropen;
	  
	  if (0)
	  G4cerr << "Apperture " << name << "/" << arname 
		 << ", out: " << r1i << "-" << r2 <<  "=" << r1i-r2
		 << ", in: " << r1 << "-" << r2i << "=" << r1-r2i
		 << G4endl
	    ;
	  
	  if (r2i-r1 < r1i-r2)
	    r2 = r2i;
	  else
	    r1 = r1i;
	}
      
      G4LogicalVolume *aring = 
	new G4LogicalVolume (new G4Cons(arname,
					r2+slack, rout-thickness-slack,
					r1+slack, rout-thickness-slack,
					thickness/2-slack,
					0, 2*M_PI ),
			     Aluminum,
			     "arname" );
  
      new G4PVPlacement(0,
			G4ThreeVector(0,0, (dz-z1-z2)/2), 
			aring,
			arname,
			this,
			false,
			0);
      
      aring->SetVisAttributes (va);
    }

}
