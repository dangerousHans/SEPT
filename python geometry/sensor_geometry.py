__author__ = 'Stefan Wraase'
from globals import *
from geant_classes import *
import detector_geometry as dg
import magnet_geometry as mg
import foil_geometry as fg
import apperture_geometry as ag

infinity = 200.0*mm                 # w/o conn., pinn-puller

X_width = 52.0*mm                   # w/o conn., pin-puller
Y_width = 82.0*mm                   # w/o doors
Z_width = 68.0*mm                   # w/0 conn., doors, ...

det_X = X_width/2 - 20.0*mm
det_Y = 9.5*mm
det_Z = 13.05*mm

foil_sep = 4.1*mm                   # from pips surface

# FIXME: The apperture z dimensions are still for 0.6*mm pips_separation

foil_apperture_distance = 20.7*mm   # from pips surface
foil_apperture_depth = 15.4*mm
foil_apperture_rout = 18.6*mm / 2
foil_apperture_ropen = 12.4*mm / 2
foil_apperture_rings = 5

mag_apperture_distance = 29.3*mm    # from pips surface
mag_apperture_depth = 7.6*mm
mag_apperture_rout = 23.6*mm / 2
mag_apperture_ropen = 20.4*mm / 2
mag_apperture_rings = 3

house_thickness = 3.0*mm

slack = 1*nanometer

mag_delrin_thickness = 1.2*mm
foil_delrin_thickness = 0.7*mm

space = "Space"

source_window =    0                # 0.0064*mm
source_window_mat = "PET"

# ---___---___---___---___---___---___---___---___---___---___---___---___---___---___---___---___---___---___---___---
# ---___---___---___---___---___---___---___---___---___---___---___---___---___---___---___---___---___---___---___---


def mag_pips_z():
    return det_Z \
           + dg.pips_z \
           - dg.pips_separation/2 \
           - dg.pips_height


def foil_pips_z():
    return det_Z \
           + dg.pips_z \
           + dg.pips_separation/2 \
           + dg.pips_height


def mag_det_z():
    return det_Z \
           - dg.stack_height/2


def foil_det_z():
    return det_Z \
           + dg.stack_height/2


class SensorGeometry(G4LogicalVolume):
    def __init__(self):
        # World Volume >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        sensor_solid = G4Box("sensor",
                             2 * infinity,
                             2 * infinity,
                             2 * infinity)
        G4LogicalVolume.__init__(self, sensor_solid, G4Material(space), "sensor")

        # Halfsensor >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        halfsensor_solid = G4Box("halfsensor",
                                   infinity,
                                   infinity,
                                   infinity)
        halfsensor_lvolume = G4LogicalVolume(halfsensor_solid, G4Material(space), "halfsensor")
        G4PVPlacement(0, [-det_X, -det_Y, -det_Z],
                      halfsensor_lvolume,
                      "halfsensor",
                      self)
        # Detector >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        detector = dg.DetectorGeometry()
        G4PVPlacement(0, [det_X, det_Y, det_Z],
                      detector,
                      "detector",
                      halfsensor_lvolume)

        # Foil >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        foil = fg.FoilGeometry()
        G4PVPlacement(0, [det_X, det_Y, foil_pips_z() + foil_sep],
                      foil,
                      "foil",
                      halfsensor_lvolume)

        # Magnets >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        magnet = mg.MagnetGeometry()
        mag_Y = (mg.size_Y + mg.gap)/2

        G4PVPlacement(0, [det_X, det_Y - mag_Y, 0],
                      magnet,
                      "magnet1",
                      halfsensor_lvolume)

        G4PVPlacement(0, [det_X, det_Y + mag_Y, 0],
                      magnet,
                      "magnet2",
                      halfsensor_lvolume)

        # Magnet-Apperture >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        mag_app = ag.AppertureGeometry("magnet_apperture",
                                       mag_apperture_rout,
                                       mag_apperture_ropen,
                                       mag_apperture_rings,
                                       mag_apperture_distance,
                                       mag_apperture_depth)
        magapp_z = mag_pips_z() \
                   - mag_apperture_distance \
                   + mag_apperture_depth/2

        mirror = G4Rotation("RotateX180+", "x", "PI")

        G4PVPlacement(mirror, [det_X, det_Y, magapp_z],
                      mag_app,
                      "magnet_apperture",
                      halfsensor_lvolume)

        # Foil-Apperture >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        foil_app = ag.AppertureGeometry("foil_apperture",
                                       foil_apperture_rout,
                                       foil_apperture_ropen,
                                       foil_apperture_rings,
                                       foil_apperture_distance,
                                       foil_apperture_depth)

        foilapp_z = foil_pips_z() \
                    + foil_apperture_distance \
                    - foil_apperture_depth/2

        G4PVPlacement(0, [det_X, det_Y, foilapp_z],
                      foil_app,
                      "foil_apperture",
                      halfsensor_lvolume)

        # Source-Window >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        if source_window > 0.0:
            win_solid = G4Tubs("win",
                               0, foil_apperture_ropen,
                               source_window/2,
                               0, 2*pi)
            win_lvolume = G4LogicalVolume(win_solid, G4Material(source_window_mat), "win")
            G4PVPlacement(0, [det_X, det_Y, foilapp_z + foil_apperture_depth/2 + source_window/2 + slack],
                          win_lvolume,
                          "win",
                          halfsensor_lvolume)

        # Housing etc >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        # >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
        house_solid = G4Box("house",
                            house_thickness/2 - slack,
                            mg.gap/2 + mg.size_Y - slack,
                            mg.size_Z/2 - slack)
        house_lvolume = G4LogicalVolume(house_solid, G4Material("Al"), "house")

        G4PVPlacement(0, [det_X - mg.size_X/2 - house_thickness/2, det_Y, 0],
                      house_lvolume,
                      "house_V1",
                      halfsensor_lvolume)
        G4PVPlacement(0, [det_X + mg.size_X/2 + house_thickness/2, det_Y, 0],
                      house_lvolume,
                      "house_V2",
                      halfsensor_lvolume)

        #-------------------------------------------------------------
        # Wrap detector in Aluminum

        delrin_diameter = dg.stack_diameter
        mag_delrin_solid = G4Tubs("mag_delrin",
                                  dg.hole_diameter/2 + slack, delrin_diameter/2 - slack,
                                  mag_delrin_thickness/2 - slack,
                                  0, 2*pi)
        mag_delrin_lvolume = G4LogicalVolume(mag_delrin_solid, G4Material("Al"), "mag_delrin")

        G4PVPlacement(0, [det_X, det_Y, mag_det_z() - mag_delrin_thickness/2],
                      mag_delrin_lvolume,
                      "mag_delrin",
                      halfsensor_lvolume)

        #-------------------------------------------------------------

        foil_delrin_solid = G4Tubs("foil_delrin",
                                   dg.hole_diameter/2 + slack, delrin_diameter/2 - slack,
                                   foil_delrin_thickness/2 - slack,
                                   0, 2*pi)
        foil_delrin_lvolume = G4LogicalVolume(foil_delrin_solid, G4Material("Al"), "foil_delrin")

        G4PVPlacement(0, [det_X, det_Y, foil_det_z() + foil_delrin_thickness/2],
                      foil_delrin_lvolume,
                      "foil_delrin",
                      halfsensor_lvolume)

        #-------------------------------------------------------------
        # Close the Z-gaps with aluminum plates

        plate_diameter = dg.stack_diameter
        plate_hole = mg.size_X + 1*mm

        mag_app_plate_thickness = mag_apperture_distance \
                                  - mag_apperture_depth \
                                  - mag_pips_z() \
                                  - mg.size_Z/2

        mag_app_plate_solid = G4Tubs("mag_app_plate",
                                     plate_hole/2 + slack, plate_diameter/2 - slack,
                                     mag_app_plate_thickness/2 - slack,
                                     0, 2*pi)
        mag_app_plate_lvolume = G4LogicalVolume(mag_app_plate_solid, G4Material("Al"), "mag_app_plate")

        G4PVPlacement(0, [det_X, det_Y, - mg.size_Z/2 - mag_app_plate_thickness/2],
                      mag_app_plate_lvolume,
                      "mag_app_plate",
                      halfsensor_lvolume)

        #-------------------------------------------------------------

        mag_det_plate_thickness = mag_det_z() \
                                  - mg.size_Z/2 \
                                  - mag_delrin_thickness

        mag_det_plate_solid = G4Tubs("mag_det_plate",
                                     dg.hole_diameter/2 + slack, plate_diameter/2 - slack,
                                     mag_det_plate_thickness/2 - slack,
                                     0, 2*pi)
        mag_det_plate_lvolume = G4LogicalVolume(mag_det_plate_solid, G4Material("Al"), "mag_det_plate")

        G4PVPlacement(0, [det_X, det_Y, mg.size_Z/2 + mag_det_plate_thickness/2],
                      mag_det_plate_lvolume,
                      "mag_det_plate",
                      halfsensor_lvolume)

        #-------------------------------------------------------------

        foil_det_plate_thickness = foil_pips_z() \
                                   + foil_apperture_distance \
                                   - foil_apperture_depth \
                                   - foil_det_z() \
                                   - foil_delrin_thickness

        foil_det_plate_solid = G4Tubs("foil_det_plate",
                                      dg.hole_diameter/2 + slack, plate_diameter/2 - slack,
                                      foil_det_plate_thickness/2 - slack,
                                      0, 2*pi)
        foil_det_plate_lvolume = G4LogicalVolume(foil_det_plate_solid, G4Material("Al"), "foil_det_plate")

        G4PVPlacement(0, [det_X, det_Y, foil_det_z() + foil_delrin_thickness + foil_det_plate_thickness/2],
                      foil_det_plate_lvolume,
                      "foil_det_plate",
                      halfsensor_lvolume)

        #-------------------------------------------------------------