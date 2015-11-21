/*
  STEREO SEPT detector geometry
*/

#ifndef _detector_geometry_h
#define _detector_geometry_h 1

#include "G4LogicalVolume.hh"
class G4VPhysicalVolume;

class detector_geometry : public G4LogicalVolume
{
public:
  detector_geometry();
  virtual ~detector_geometry();

  static const double nm;

  static const double slack;

  static const double stack_height  ;
  static const double stack_diameter;

  // A PIPS detector consists of five layers:

  //      50 nm  passive Si
  // 299.680 um  depleted Si <-- sensitive volume
  //     120 nm  passive Si
  //     150 nm  Al
  
  // Octagon, hight=13.8mm, diagonal = 15mm
  
  static const double pips_diameter  ;
  static const double center_diameter; // 53.0 mm^2
  static const double xtalk_width    ;
  static const double guard_diameter ; // 56.5 mm^2

   // junction side
  static const double chip_inner_metal ;
  static const double chip_inner_window;
  static const double pips_height      ;
  static const double chip_outer_window;
  static const double chip_outer_metal ;
  // ohmic side

  // There are two PIPS detectors, slightly off-center in z

  static const double pips_separation;
  static const double pips_z;
  
  // The detector stack housing, aluminum tubes

  static const double cover_height  ;
  static const double hole_diameter ;
  static const double bottom_height ;
  static const double wall_thickness;

};

class foil_geometry : public G4LogicalVolume
{
public:
  foil_geometry();
  virtual ~foil_geometry();

  static const double slack;
  static const double diameter;
  static const double foil_thickness;
  static const double foil_metal_thickness;
  static const char * foil_material;
};

#endif // _detector_geometry_h
