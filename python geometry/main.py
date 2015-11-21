__author__ = 'Stefan Wraase'
from geant_classes import *
import sensor_geometry as sg
import xml_out as xml
import sys

# Check for XML-Output data path
XML_out_path = ""
if len(sys.argv) > 1:
    XML_out_path = sys.argv[1]
    xml_out = open(XML_out_path, "w")

# Build sensor
sensor_lvolume = sg.SensorGeometry()

# Create world
world = G4PVPlacement(0, [0, 0, 0],
                      sensor_lvolume,
                      "World",
                      0)

if XML_out_path != "":
    xml_out.write("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n\n")

    # Write PI constants
    xml_out.write("Constants:\n" + "-"*75 + "\n")
    xml_out.write("<constant name=\"PI\" value=\"1.*pi\"/>" + "\n")

    # Write positions
    xml_out.write("\n\nPositions and Rotations\n" + "-"*75 + "\n")
    xml_out.write(xml.get_positions_xml())

    # Write solids
    xml_out.write("\n\nSolids\n" + "-"*75 + "\n")
    xml_out.write(xml.get_solids_xml())

    # Write structure
    xml_out.write("\n\nStructure\n" + "-"*75 + "\n")
    xml_out.write(xml.get_structure_xml())

    xml_out.close()

    print "\tWATCH OUT: f_aring_04 rmins need to be exchanged !!!"

