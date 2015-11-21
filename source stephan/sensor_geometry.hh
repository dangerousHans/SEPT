/*
  STEREO SEPT half sensor geometry
  $Id: sensor_geometry.hh,v 1.10 2006/05/16 00:08:54 bottcher Exp $
*/

#ifndef _sensor_geometry_h
#define _sensor_geometry_h 1

#include "G4LogicalVolume.hh"

class sensor_geometry : public G4LogicalVolume
{
public:
  sensor_geometry();
  virtual ~sensor_geometry();

  static const double infinity; // w/o conn., pin-puller

  static const double X_width ; // w/o conn., pin-puller
  static const double Y_height; // w/o doors 
  static const double Z_depth ; // w/o conn., doors, ...
  
  static const double det_X;
  static const double det_Y;
  static const double det_Z;

  static const double foil_sep; // from pips surface

  // FIXME: The apperture z dimensions are still for 0.6*mm pips_separation

  static const double foil_apperture_distance; // from pips surface
  static const double foil_apperture_depth;
  static const double foil_apperture_rout;
  static const double foil_apperture_ropen;
  static const int    foil_apperture_rings;

  static const double mag_apperture_distance; // from pips surface 
  static const double mag_apperture_depth;
  static const double mag_apperture_rout;
  static const double mag_apperture_ropen;
  static const int    mag_apperture_rings;

  static const double house_thickness;
  
  static const double slack;

  static const double mag_delrin_thickness ;
  static const double foil_delrin_thickness;

  static const char *space;

  static const double source_window;
  static char const * source_window_mat;

private:

  static double mag_pips_z();
  static double mag_det_z();
  static double foil_pips_z();
  static double foil_det_z();
};

#endif // _sensor_geometry_h
