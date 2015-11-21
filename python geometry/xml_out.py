from geant_classes import *
import detector_geometry as dg
import globals
import collections
import sys

if globals.mm == 1.0 and globals.rad == 1.0 :
    lunit = "mm"
    aunit = "rad"
else:
    print "\tABORT: Millimeter and Radian are not of value 1.0 !!!"
    sys.exit()

types = {"G4Box"  : "box",
         "G4Tubs" : "tube",
         "G4Cons" : "cone"}
sep = " "

def get_solids_xml():
    ret_string = ""
    for solid in solids:
        # Collect Solid Geometry in an Ordered Dict
        args = collections.OrderedDict()
        args["type"] = solid.type
        args["name"] = solid.pName

        if solid.type   == "G4Box"  :       # Solid is G4Box
            args["x"] = str(solid.pX)
            args["y"] = str(solid.pY)
            args["z"] = str(solid.pZ)

        elif solid.type == "G4Tubs" :       # Solid is G4Tubs
            args["rmin"]     = str(solid.pRMin)
            args["rmax"]     = str(solid.pRMax)
            args["z"]        = str(solid.pDz)

            if solid.pDPhi == globals.pi*2:
                args["deltaphi"] = "2*PI"
            elif solid.pDPhi == globals.pi:
                args["deltaphi"] = "PI"
            else:
                args["deltaphi"] = str(solid.pDPhi)

            if solid.pSPhi != 0:
                args["startphi"] = str(solid.pSPhi)

            args["aunit"]    = aunit

        elif solid.type == "G4Cons" :       # Solid is G4Cons
            args["rmin1"]    = str(solid.pRmin1)
            args["rmax1"]    = str(solid.pRmax1)
            args["rmin2"]    = str(solid.pRmin2)
            args["rmax2"]    = str(solid.pRmax2)
            args["z"]        = str(solid.pDz)

            if solid.pDPhi == globals.pi*2:
                args["deltaphi"] = "2*PI"
            elif solid.pDPhi == globals.pi:
                args["deltaphi"] = "PI"
            else:
                args["deltaphi"] = str(solid.pDPhi)

            if solid.pSPhi != 0:
                args["startphi"] = str(solid.pSPhi)

            args["aunit"]    = aunit

        args["lunit"] = lunit

        # Concatenate solid information in formatted xml-style in solid_cml
        solid_xml = "<"
        for arg in args:
            if arg == "type":
                solid_xml += types[args[arg]]
            else:
                solid_xml += sep + arg + "=" + "\"" + args[arg] + "\""
        solid_xml  +=" />\n"
        ret_string += solid_xml
    return ret_string


def get_positions_xml():
    ret_string = ""
    for pos in positions:
        ret_string += "<position"\
                      + sep + "name=" + "\"" + pos[0]         + "\"" \
                      + sep + "unit=" + "\"" + lunit          + "\"" \
                      + sep + "x="    + "\"" + str(pos[1][0]) + "\"" \
                      + sep + "y="    + "\"" + str(pos[1][1]) + "\"" \
                      + sep + "z="    + "\"" + str(pos[1][2]) + "\"" \
                      + " />\n"
    ret_string += "\n"
    for rot in rotations:
        ret_string += "<rotation"\
                      + sep + "name=\"" + rot.name + "\""         \
                      + sep + rot.xyz + "=\"" + str(rot.amount) + "\"" \
                      + sep + "unit=\"rad\""                      \
                      + " />\n"
    return ret_string


def get_structure_xml():
    ret_string = "<structure>\n"

    def vol_string(v, c):
        ret_string = "\t"*(c)   + "<volume" + sep + "name=" + "\"" + v.name + "\" >\n"          \
                   + "\t"*(c+1) + "<materialref  ref=" + "\"" + v.pMaterial.name + "\" />\n" \
                   + "\t"*(c+1) + "<solidref     ref=" + "\"" + v.pSolid.pName      + "\" />\n" \
                   + "\t"*(c+1) + "<colorref     ref=" + "\"" + "clWhite"           + "\" />\n"

        if v.name in dg.sensitive_volumes:
            ret_string += "\t"*(c+1) + dg.sensitive_xml_string + "\n"
        return ret_string


    # Print non-daughter volumes first
    volumes_deep = []
    for vol in volumes:
        volumes_deep.append(vol)
        if vol.fDaughters == []:
            ret_string += vol_string(vol, 1)
            ret_string += "\t"*1 + "</volume>\n\n"
            volumes_deep.remove(vol)

    # Print volumes with daughters
    for vol in reversed(volumes_deep):
        if vol.fDaughters != []:
            ret_string += "\n" + vol_string(vol, 1)
            for daughter in vol.fDaughters:
                ret_string += "\n" + "\t"*2 + "<physvol>\n"                                                          \
                              + "\t"*3 + "<volumeref   ref=" + "\"" + daughter.pCurrentLogical.name + "\"" + " />\n" \
                              + "\t"*3 + "<positionref ref=" + "\"" + daughter.pName + "_pos\"" + " />\n"

                # Add Rotation Ref and sdid ref if necessary
                if daughter.pRot != 0:
                    ret_string += "\t"*3 + "<rotationref ref=" + "\"" + daughter.pRot.name + "\"" + " />\n"
                if daughter.sdID != None:
                    ret_string += "\t"*3 + "<sdID id=\"" + str(daughter.sdID) + "\" />\n"

                ret_string += "\t"*2 + "</physvol>\n"
            ret_string += "\t"*1 + "</volume>\n"
    return ret_string + "</structure>"