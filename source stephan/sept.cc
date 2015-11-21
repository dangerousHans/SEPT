
#include <globals.hh>
#include <G4ios.hh>

#include <G4ThreeVector.hh>
#include <G4String.hh>

const double infinity = 100*mm;
const double slack = 0.001*mm;

#include <G4UIcmdWithADoubleAndUnit.hh>
#include "ETRadiationSource.hh"
#include "fourpigun.hh"

class GeneratorAction : public fourpigun
{
public:
  GeneratorAction();
  virtual ~GeneratorAction();
  virtual void GeneratePrimaries(G4Event*);
  virtual void SetNewValue(G4UIcommand *cmd, G4String val);
  virtual G4String GetCurrentValue(G4UIcommand *cmd);

private:
  ETRadiationSource spectrum;
  double Emin, Emax;
  G4UIcmdWithADoubleAndUnit *Emin_cmd;
  G4UIcmdWithADoubleAndUnit *Emax_cmd;
};

#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>

#include <CLHEP/Random/Randomize.h>

GeneratorAction::GeneratorAction()
  : fourpigun(infinity), spectrum("/gun/spectrum/")
{
  Emin = 100*MeV;
  Emax = Emin;
	
  Emin_cmd = new G4UIcmdWithADoubleAndUnit("/gun/Emin", this);
  Emin_cmd->SetGuidance("Particle kinetic energy range: minimum");
  Emin_cmd->SetParameterName("Emin",false,false);
  Emin_cmd->SetDefaultUnit("keV");
  Emin_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  Emax_cmd = new G4UIcmdWithADoubleAndUnit("/gun/Emax", this);
  Emax_cmd->SetGuidance("Particle kinetic energy range: maximum");
  Emax_cmd->SetParameterName("Emax",false,false);
  Emax_cmd->SetDefaultUnit("keV");
  Emax_cmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

GeneratorAction::~GeneratorAction()
{
  delete Emin_cmd;
  delete Emax_cmd;
}

void GeneratorAction::GeneratePrimaries(G4Event *e)
{ 
  if (spectrum.size())
    {
      spectrum.shoot(Gun);
    }
  else if (Emax > Emin)
    {
      double r = log(Emax/Emin);
      Gun->SetParticleEnergy(Emin*exp(CLHEP::RandFlat::shoot(r)));
    }
  fourpigun::GeneratePrimaries(e);
}

void GeneratorAction::SetNewValue(G4UIcommand *cmd, G4String val)
{
  if (cmd==Emin_cmd)
    Emin = Emin_cmd->GetNewDoubleValue(val);
  else if (cmd==Emax_cmd)
    Emax = Emax_cmd->GetNewDoubleValue(val);
  else fourpigun::SetNewValue(cmd, val);
}

#include <sstream>

G4String GeneratorAction::GetCurrentValue(G4UIcommand *cmd)
{
  std::ostringstream os;

  if (cmd==Emin_cmd)
    os << Emin/keV << " keV" << '\0';
  else if (cmd==Emax_cmd)
    os << Emax/keV << " keV" << '\0';
  else
    return fourpigun::GetCurrentValue(cmd);

  return G4String(os.str());
}

#include "world.hh"

#include <G4RunManager.hh>
#include <G4UImanager.hh>
#include <G4UIterminal.hh>
#include <G4UItcsh.hh>

#include "ETPhysicsList.hh"
#include "VisManager.hh"
#include "draw_event.hh"

#include <unistd.h>
#include <string.h>

#include "stopit.hh"

void init_runoutput(); // What is that for?

int main(int argc, char **argv) 
{
  G4RunManager * runManager = new G4RunManager;
  DetectorConstruction* detector = new DetectorConstruction(runManager);
  runManager->SetUserInitialization(detector);

  G4VUserPhysicsList *physics = new ETPhysicsList;
  physics->SetDefaultCutValue(1*mm);
  physics->SetVerboseLevel(2);
  runManager->SetUserInitialization(physics);

  runManager->SetUserAction(new draw_event);
  runManager->SetUserAction(new GeneratorAction());

  G4VisManager* visManager = new VisManager;
  visManager->Initialize();

  runManager->Initialize();

  G4UImanager * UI = G4UImanager::GetUIpointer();  

  init_runoutput(); // Does nothing?
  
  
  // Process macro or start UI session
  if (argc>1 && !strcmp(argv[1], "-n"))
    {
      argc--;
      argv++;
    }
  else 
    {
      UI->ApplyCommand("/control/execute g4septrc"); //applied if no call options
    }

  bool interactive = argc==1;

  while (argc>1)
    {
      argc--;
      argv++;
      if (!strcmp(argv[0], "-i")) //interactive mode
	{
	  interactive = true;
	}
      else if (!strcmp(argv[0], "-c")) //command
	{
	  if (argc>1)
	    {
	      UI->ApplyCommand(argv[1]);
	      argc--;
	      argv++;
	    }
	}
      else 
	{
	  UI->ApplyCommand(G4String("/control/execute ")+argv[0]); //execute file
	}
    }

  //interatcive mode
  if (interactive)
    {
      G4UIsession *session = new G4UIterminal(new G4UItcsh);      
      stopit ctrl_c;
      session->SessionStart();
      delete session;
    }

  delete UI;
  delete visManager;
  delete runManager;
  
  return 0;
}
