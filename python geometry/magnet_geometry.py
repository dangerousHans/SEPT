from geant_classes import *
from globals import *

size_X = 16.0*mm
size_Y =  8.0*mm
size_Z = 16.0*mm
gap    = 16.0*mm


class MagnetGeometry(G4LogicalVolume):
    def __init__(self):
        magnet_solid = G4Box("magnet",
                             size_X/2,
                             size_Y/2,
                             size_Z/2)

        G4LogicalVolume.__init__(self, magnet_solid, G4Material("NdFeB"), "magnet")