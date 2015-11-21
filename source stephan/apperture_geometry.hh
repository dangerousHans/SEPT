/*
  STEREO SEPT apperture geometry
  $Id: apperture_geometry.hh,v 1.3 2006/05/16 00:08:54 bottcher Exp $
*/

#ifndef _apperture_geometry_h
#define _apperture_geometry_h 1

#include <G4LogicalVolume.hh>

class apperture_geometry : public G4LogicalVolume
{
public:
  apperture_geometry(G4String name,// name
		     double rout,  // outer radius
		     double ropen, // radius of the opening
		     int nz,       // number of apperture rings
		     double z0,    // distance from detector of outer AR
		     double dz);   // total height


  virtual ~apperture_geometry();

  static const double slack;

  static const double thickness;
  static const double rdet;

private:
  static double aradius(double ropen, double z0, double dz);
};

#endif // _apperture_geometry_h
