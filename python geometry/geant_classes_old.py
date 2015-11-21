__author__ = 'Stefan Wraase'

solids = []
volumes = []
pvolumes = []
positions = []
materials = []

class G4LogicalVolume:
    def __init__(self, pSolid, pMaterial, name):
        self.pSolid     = pSolid
        self.pMaterial  = pMaterial
        self.name       = name
        self.fDaughters = []

        volumes.append(self)

    def show(self, c):
        if self.fDaughters != []:
            for daughter in self.fDaughters:
                daughter.show(c + 1)
        print "\/\t"*c + "\n"\
              +  self.pSolid.show()

    def show2(self, c):
        print "Material    = " + str(self.pMaterial.show2())
        print "\n" + "/\ \t" * c + "\n"
        print self.pSolid.show2()

        if self.fDaughters != []:
            for daughter in self.fDaughters:
                daughter.show2(c + 1)

    def show_xml(self, c):
        pass


class G4PVPlacement:
    def __init__(self, pRot, tlate, pCurrentLogical, pName, pMotherLogical):
        self.pRot = pRot
        self.tlate = tlate
        self.pName = pName
        self.pCurrentLogical = pCurrentLogical

        if pMotherLogical == 0:
            pass
        else:
            pMotherLogical.fDaughters.append(self)

        pvolumes.append(self)
        positions.append([self.pName + "_pos", self.tlate])

    def show(self, c):
        self.pCurrentLogical.show(c)

    def show2(self, c):
        print "\n" + ">>>>>>>>>>>>>>>>>" * (7 - c) + "|| " + str(self.pName) + " ||"

        print     "PVPl. Name  = " + str(self.pName)    + "\n" \
                + "Rotation    = " + str(self.pRot)     + "\n" \
                + "Koordinaten = " + str(self.tlate)

        self.pCurrentLogical.show2(c)

        print        "<<<<<<<<<<<<<<<<<" * (7 - c) + "|| " + str(self.pName) + " ||"

    def show_xml(self, c):
        self.pCurrentLogical.show_xml(c)


class G4Material:
    def __init__(self, name):
        self.name = name

        materials.append(self)

    def show2(self):
        return self.name

# -----------------------------------------------------||  Solids  ||
# -----------------------------------------------------------------||


class G4Box:
    def __init__(self, pName, pX, pY, pZ):
        self.pName = pName
        self.pX    = pX
        self.pY    = pY
        self.pZ    = pZ
        self.type  = "G4Box"

        solids.append(self)

    def show(self):
        return     "============================"   + "\n" \
                 + "Name   = " + str(self.pName)    + "\n" \
                 + "Solid  = " + str(self.type)     + "\n" \
                 + "----------------------------"   + "\n" \
                 + "pX     = " + str(self.pX)       + "\n" \
                 + "pY     = " + str(self.pY)       + "\n" \
                 + "pZ     = " + str(self.pZ)       + "\n" \
                 + "============================"   + "\n"

    def show2(self):
        return     "Solid  = " + str(self.type)     + "\n"   \
                 + "Name   = " + str(self.pName)    + "\n\n" \
                 + "pX     = " + str(self.pX)       + "\n"   \
                 + "pY     = " + str(self.pY)       + "\n"   \
                 + "pZ     = " + str(self.pZ)

# ---------------------------------------------------------


class G4Tubs:
    def __init__(self, pName, pRMin, pRMax, pDz, pSPhi, pDPhi):
        self.pName = pName
        self.pRMin = pRMin
        self.pRMax = pRMax
        self.pDz   = pDz
        self.pSPhi = pSPhi
        self.pDPhi = pDPhi
        self.type  = "G4Tubs"

        solids.append(self)

    def show(self):
        return     "============================"   + "\n" \
                 + "Name   = " + str(self.pName)    + "\n" \
                 + "Solid  = " + str(self.type)     + "\n" \
                 + "----------------------------"   + "\n" \
                 + "pRMin  = " + str(self.pRMin)    + "\n" \
                 + "pRMax  = " + str(self.pRMax)    + "\n" \
                 + "pDz    = " + str(self.pDz)      + "\n" \
                 + "pSPhi  = " + str(self.pSPhi)    + "\n" \
                 + "pDPhi  = " + str(self.pDPhi)    + "\n" \
                 + "============================"   + "\n"

    def show2(self):
        return     "Solid  = " + str(self.type)     + "\n"   \
                 + "Name   = " + str(self.pName)    + "\n\n" \
                 + "pRMin  = " + str(self.pRMin)    + "\n"   \
                 + "pRMax  = " + str(self.pRMax)    + "\n"   \
                 + "pDz    = " + str(self.pDz)      + "\n"   \
                 + "pSPhi  = " + str(self.pSPhi)    + "\n"   \
                 + "pDPhi  = " + str(self.pDPhi)

# ---------------------------------------------------------


class G4Cons:
    def __init__(self, pName, pRmin1, pRmax1, pRmin2, pRmax2, pDz, pSPhi, pDPhi):
        self.pName = pName
        self.pRmin1 = pRmin1
        self.pRmax1 = pRmax1
        self.pRmin2 = pRmin2
        self.pRmax2 = pRmax2
        self.pDz    = pDz
        self.pSPhi  = pSPhi
        self.pDPhi  = pDPhi
        self.type  = "G4Cons"

        solids.append(self)

    def show(self):
        return     "============================"   + "\n" \
                 + "Name   = " + str(self.pName)    + "\n" \
                 + "Solid  = " + str(self.type)     + "\n" \
                 + "----------------------------"   + "\n" \
                 + "pRmin1 = " + str(self.pRmin1)   + "\n" \
                 + "pRmax1 = " + str(self.pRmax1)   + "\n" \
                 + "pRmin2 = " + str(self.pRmin2)   + "\n" \
                 + "pRmax2 = " + str(self.pRmax2)   + "\n" \
                 + "pDz    = " + str(self.pDz)      + "\n" \
                 + "pSPhi  = " + str(self.pSPhi)    + "\n" \
                 + "pDPhi  = " + str(self.pDPhi)    + "\n" \
                 + "============================"   + "\n"

    def show2(self):
        return     "Solid  = " + str(self.type)     + "\n"   \
                 + "Name   = " + str(self.pName)    + "\n\n" \
                 + "pRmin1 = " + str(self.pRmin1)   + "\n"   \
                 + "pRmax1 = " + str(self.pRmax1)   + "\n"   \
                 + "pRmin2 = " + str(self.pRmin2)   + "\n"   \
                 + "pRmax2 = " + str(self.pRmax2)   + "\n"   \
                 + "pDz    = " + str(self.pDz)      + "\n"   \
                 + "pSPhi  = " + str(self.pSPhi)    + "\n"   \
                 + "pDPhi  = " + str(self.pDPhi)