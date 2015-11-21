// $Id: magnetic_field.hh,v 1.5 2003/02/20 09:47:26 bottcher Exp $

#ifndef magnetic_field_h
#define magnetic_field_h

#include "sensor_geometry.hh"

#include "globals.hh"
#include "G4MagneticField.hh"

class magnetic_field : public G4MagneticField
{
public:
  magnetic_field(G4double up, G4double down, G4double det_y, G4double B);
  virtual ~magnetic_field();
  virtual void GetFieldValue(const G4double where[], G4double B[]) const;
  
  void SetFieldValue(G4double B);

  G4double X_up, X_down;
  G4double Y;
  G4double By;
};

class magnetic_field_table : public G4MagneticField
{
public:
  magnetic_field_table(const char *filename,
		       double x=0, 
		       double y=sensor_geometry::det_Y, 
		       double z=0);
  virtual ~magnetic_field_table();
  virtual void GetFieldValue(const G4double where[], G4double B[]) const;

  static const int cmap[3];

protected:

  inline int idx(int x[]) const
  {
    return idx(x[0], x[1], x[2]);
  }

  inline int idx(int ix, int iy, int iz) const
  {
    return (iz*steps[1]+iy)*steps[0] + ix;
  }

  inline double get(int ix, int iy, int iz, int i) const
  {
    return table[idx(ix,iy,iz)][i];
  }

  const double (*table)[3];
  double range[3];
  double origin[3];
  double step[3];
  unsigned steps[3];
};

#endif // magnetic_field_h
