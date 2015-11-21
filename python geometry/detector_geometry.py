__author__ = 'Stefan Wraase'
from globals import *
from geant_classes import *
import sensor_geometry as sg

slack = 1.0*nm
stack_height = 5.7*mm
stack_diameter = 27.6*mm

# A PIPS detector consists of five layers:
#
#      50 nm  passive Si
# 299.680 um  depleted Si <-- sensitive volume
#     120 nm  passive Si
#     150 nm  Al
# Octagon, height=13.8mm, diagonal = 15mm
pips_diameter = 14.17*mm
center_diameter = 8.214*mm
xtalk_width = 0.1*mm
guard_diameter = 11.948*mm

# junction side
chip_inner_metal = 1000.0*nm
chip_inner_window = 500.0*nm
pips_height = 0.3*mm
chip_outer_window = 150.0*nm
chip_outer_metal = 170.0*nm

# There are two PIPS detectors, slightly off-center in z
pips_separation = 1.0*mm
pips_z = stack_height/2 - 2.6*mm - pips_height - pips_separation/2

# The detector stack housing, aluminum tubes
cover_height = 0.9*mm
hole_diameter = 10.6*mm
bottom_height = 0.5*mm
wall_thickness = (27.6 - 24.7)/2*mm

sensitive_volumes = ["center", "guard_ring"]
sensitive_xml_string = "<sd detail=\"true\" zeroE=\"true\" />"


class DetectorGeometry(G4LogicalVolume):
    def __init__(self):
        detector_solid = G4Tubs("detectorstack",
                                0, stack_diameter/2,
                                stack_height/2,
                                0, 2*pi)
        G4LogicalVolume.__init__(self, detector_solid, G4Material(sg.space), "detectorstack")

        Si = G4Material("detSi")
        Al = G4Material("Al")

        #-------------------------------------------------------------
        pips_solid = G4Tubs("pips",
                            0, pips_diameter/2,
                            pips_height/2,
                            0, 2*pi)

        #Here second pips volume added by Stefan
        pips_lvolume1 = G4LogicalVolume(pips_solid, Al, "pips1")
        pips_lvolume2 = G4LogicalVolume(pips_solid, Al, "pips2")

        silicon_solid = G4Tubs("silicon",
                               0, pips_diameter/2 - slack,
                               (pips_height - chip_inner_metal - chip_outer_metal)/2 - slack,
                               0, 2*pi)
        # Here second silicon volume and placement added by Stefan Wraase
        silicon_lvolume1  = G4LogicalVolume(silicon_solid, Si, "silicon1")
        silicon_lvolume2 = G4LogicalVolume(silicon_solid, Si, "silicon2")

        G4PVPlacement(0, [0, 0, (chip_outer_metal - chip_inner_metal)/2],
                      silicon_lvolume1,
                      "silicon1",
                      pips_lvolume1)
        G4PVPlacement(0, [0, 0, (chip_outer_metal - chip_inner_metal)/2],
                      silicon_lvolume2,
                      "silicon2",
                      pips_lvolume2)

        #-------------------------------------------------------------

        guard_ring_solid = G4Tubs("guard_ring",
                                  center_diameter/2 + xtalk_width, guard_diameter/2,
                                  (pips_height
                                   - chip_inner_window
                                   - chip_inner_metal
                                   - chip_outer_window
                                   - chip_outer_metal)/2,
                                  0, 2*pi)
        guard_ring_lvolume = G4LogicalVolume(guard_ring_solid, Si, "guard_ring")

        center_solid = G4Tubs("center",
                              0, center_diameter/2,
                              (pips_height
                               - chip_inner_window
                               - chip_inner_metal
                               - chip_outer_window
                               - chip_outer_metal)/2,
                              0, 2*pi)
        center_lvolume = G4LogicalVolume(center_solid, Si, "center")

        G4PVPlacement(0, [0, 0, (chip_outer_window - chip_inner_window)/2],
                      guard_ring_lvolume,
                      "guard_ring",
                      silicon_lvolume1,
                      "00")
        G4PVPlacement(0, [0, 0, (chip_outer_window - chip_inner_window)/2],
                      center_lvolume,
                      "center",
                      silicon_lvolume1,
                      "01")

        # These two placements added by Stefan Wraase:
        G4PVPlacement(0, [0, 0, (chip_outer_window - chip_inner_window)/2],
                      guard_ring_lvolume,
                      "guard_ring",
                      silicon_lvolume2,
                      "10")
        G4PVPlacement(0, [0, 0, (chip_outer_window - chip_inner_window)/2],
                      center_lvolume,
                      "center",
                      silicon_lvolume2,
                      "11")

        # The bottom detector goes in unrotated ----------------------

        G4PVPlacement(0, [0, 0, -(pips_separation + pips_height)/2 + pips_z],
                      pips_lvolume1,
                      "pips1",
                      self)

        mirror = G4Rotation("RotateX180+", "x", "PI")
        G4PVPlacement(mirror, [0, 0, (pips_separation + pips_height)/2 + pips_z],
                      pips_lvolume2,
                      "pips2",
                      self)

        # Cover and Bottom -------------------------------------------

        cover_solid = G4Tubs("detectorcover",
                             hole_diameter/2, stack_diameter/2 - slack,
                             cover_height/2 - slack,
                             0, 2*pi)
        cover_lvolume = G4LogicalVolume(cover_solid, Al, "detectorcover")
        G4PVPlacement(0, [0, 0, (stack_height - cover_height)/2],
                      cover_lvolume,
                      "detectorcover",
                      self)

        bottom_solid = G4Tubs("detectorbottom",
                              hole_diameter/2, stack_diameter/2 - slack,
                              bottom_height/2 - slack,
                              0, 2*pi)
        bottom_lvolume= G4LogicalVolume(bottom_solid, Al, "detectorbottom")
        G4PVPlacement(0, [0, 0, -(stack_height - bottom_height)/2],
                      bottom_lvolume,
                      "detectorbottom",
                      self)

        # Wall -------------------------------------------------------

        wall_solid = G4Tubs("detectorwall",
                            stack_diameter/2 - wall_thickness, stack_diameter/2 - slack,
                            (stack_height - cover_height - bottom_height)/2 - slack,
                            0, 2*pi)
        wall_lvolume = G4LogicalVolume(wall_solid, Al, "detectorwall")
        G4PVPlacement(0, [0, 0, (bottom_height - cover_height)/2],
                      wall_lvolume,
                      "detectorwall",
                      self)