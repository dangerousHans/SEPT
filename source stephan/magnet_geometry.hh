/*
  STEREO SEPT magnet geometry
  $Id: magnet_geometry.hh,v 1.3 2006/05/16 00:08:54 bottcher Exp $
*/

#ifndef _magnet_geometry_h
#define _magnet_geometry_h 1

#include "G4LogicalVolume.hh"

class magnet_geometry : public G4LogicalVolume
{
public:
  magnet_geometry();
  virtual ~magnet_geometry();

  static const double size_X;
  static const double size_Y;
  static const double size_Z;

  static const double gap;
};

#endif // _magnet_geometry_h
