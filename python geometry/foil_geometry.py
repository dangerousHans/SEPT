from globals import *
from geant_classes import *

slack                =    1.0*nm
diameter             =   10.6*mm        # detector_geometry.hole_diameter
foil_thickness       = 5000.0*nm
foil_metal_thickness =  200.0*nm
foil_material        = "ParyleneN"

class FoilGeometry(G4LogicalVolume):
    def __init__(self):
        # A round disk of very thin parylene foil, coated with Aluminum
        foil_solid = G4Tubs("foil",
                            0, diameter/2,
                            foil_thickness/2,
                            0, 2*pi)
        G4LogicalVolume.__init__(self, foil_solid, G4Material("Al"), "foil")

        # Parylene Core
        core_solid = G4Tubs("foil_core",
                            0, diameter/2 - slack,
                            (foil_thickness - foil_metal_thickness)/2,
                            0, 2*pi)
        core_lvolume = G4LogicalVolume(core_solid, G4Material(foil_material), "foil_core")
        G4PVPlacement(0, [0, 0, 0],
                      core_lvolume,
                      "foil_core",
                      self)