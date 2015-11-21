__author__ = 'Stefan Wraase'
from globals import *
from geant_classes import *
import sensor_geometry as sg

slack = 10*micrometer
thickness = 1*mm
rdet = 8.214*mm/2   # detector_geometry.center_diameter/2


class AppertureGeometry(G4LogicalVolume):
    # name = Name, rout = outer radius, ropen = radius of the opening,
    # nz = number of apperture rings, z0 = distance from detector of outer AR, dz = total height
    def __init__(self, name, rout, ropen, nz, z0, dz):

        def aradius(ropen, z0, dz):
            return (ropen - rdet) * (z0 - dz) / z0 + rdet

        # The apperture is a cone in space
        app_solid = G4Cons(name,
                              aradius(ropen, z0, dz), rout,
                              ropen, rout,
                              dz/2,
                              0, 2 * pi)

        G4LogicalVolume.__init__(self, app_solid, G4Material(sg.space), name)

        aluminium = G4Material("Al")

        wall_solid = G4Tubs("wall",
                            rout - thickness, rout - slack,
                            dz/2 - slack,
                            0, 2 * pi)
        wall_lvolume = G4LogicalVolume(wall_solid, aluminium, "wall")
        G4PVPlacement(0, [0, 0, 0],
                      wall_lvolume,
                      "wall",
                      self)

        # Create and place apperture-rings
        for iz in range(0, nz):
            arname = self.name[0] + "_aring" + "0" + str(iz)

            z1 = iz * dz/nz
            z2 = z1 + thickness
            r1 = aradius(ropen, z0, z1)
            r2 = aradius(ropen, z0, z2)

            if iz > 0:
                r1i = (z0 - z1)/(z0 - z2) * (r2 + rdet) - rdet
                r2i = z2/z1 * (r1 + ropen) - ropen


                if (r2i - r1 < r1i-r2):
                    r2 = r2i
                else:
                    r1 = r1i

            aring_solid = G4Cons(arname,
                                r2 + slack, rout - thickness - slack,
                                r1 + slack, rout - thickness - slack,
                                thickness/2 - slack,
                                0, 2 * pi)
            aring_lvolume = G4LogicalVolume(aring_solid, aluminium, arname)
            G4PVPlacement(0, [0, 0, (dz - z1 - z2)/2],
                          aring_lvolume,
                          arname,
                          self)