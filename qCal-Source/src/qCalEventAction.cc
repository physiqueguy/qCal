#include "qCalEventAction.hh"
#include "qCalSiPMHit.hh"
#include "qCalUserEventInformation.hh"
#include "qCalTrajectory.hh"

#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "G4UImanager.hh"
#include "G4SystemOfUnits.hh"


qCalEventAction::qCalEventAction()
:fSaveThreshold(0),fSiPMCollID(-1),fVerbose(0),
fSiPMThreshold(1),fForcedrawphotons(false),fForcenophotons(false)
{
   fEventMessenger = new qCalEventMessenger(this);
}


qCalEventAction::~qCalEventAction()
{
   
}


void qCalEventAction::BeginOfEventAction(const G4Event* anEvent)
{
   
   //New event, add the user information object
   G4EventManager::GetEventManager()->SetUserInformation(new qCalUserEventInformation);
   
   G4SDManager* SDman = G4SDManager::GetSDMpointer();
   if ( fSiPMCollID < 0 )
   {
      fSiPMCollID=SDman->GetCollectionID("SiPMHitCollection");
   }
   
   if ( fRecorder )
   {
      fRecorder->RecordBeginOfEvent(anEvent);
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void qCalEventAction::EndOfEventAction(const G4Event* anEvent)
{
   qCalUserEventInformation* eventInformation =(qCalUserEventInformation*)anEvent->GetUserInformation();
   
   G4TrajectoryContainer* trajectoryContainer=anEvent->GetTrajectoryContainer();
   
   G4int n_trajectories = 0;
   if ( trajectoryContainer )
   {
      n_trajectories = trajectoryContainer->entries();
   }
   
   // extract the trajectories and draw them
   if ( G4VVisManager::GetConcreteInstance() )
   {
      for ( G4int i=0; i<n_trajectories; i++ )
      {
         qCalTrajectory* trj = (qCalTrajectory*)((*(anEvent->GetTrajectoryContainer()))[i]);
         if( trj->GetParticleName() == "opticalphoton" )
         {
            trj->SetForceDrawTrajectory(fForcedrawphotons);
            trj->SetForceNoDrawTrajectory(fForcenophotons);
         }
         trj->DrawTrajectory();
      }
   }
   
   qCalSiPMHitsCollection* SiPMHC = 0;
   G4HCofThisEvent* hitsCE = anEvent->GetHCofThisEvent();
   
   //Get the hit collections
   if( hitsCE )
   {
      if( fSiPMCollID >= 0 )
      {
         SiPMHC = (qCalSiPMHitsCollection*)(hitsCE->GetHC(fSiPMCollID));
      }
   }
   
   if(SiPMHC)
   {
      G4ThreeVector reconPos(0.,0.,0.);
      G4int SiPMs = SiPMHC->entries();
      //Gather info from all SiPMs
      for ( G4int i = 0 ; i < SiPMs; i++ )
      {
         eventInformation->IncHitCount((*SiPMHC)[i]->GetPhotonCount());
         reconPos+=(*SiPMHC)[i]->GetSiPMPos()*(*SiPMHC)[i]->GetPhotonCount();
         if ( (*SiPMHC)[i]->GetPhotonCount() >= fSiPMThreshold )
         {
            eventInformation->IncSiPMSAboveThreshold();
         }
         else{
            //wasnt above the threshold, turn it back off
            (*SiPMHC)[i]->SetDrawit(false);
         }
      }
      
      //dont bother unless there were hits
      if ( eventInformation->GetHitCount() > 0 )
      {
         reconPos/=eventInformation->GetHitCount();
         if(fVerbose>0){
            G4cout << "\tReconstructed position of hits in qCal : "
            << reconPos/mm << G4endl;
         }
         eventInformation->SetReconPos(reconPos);
      }
      SiPMHC->DrawAllHits();
   }
   
   if ( fVerbose > 0 )
   {
      //End of event output. later to be controlled by a verbose level
      G4cout << "\tNumber of photons that hit SiPMs in this event : "
      << eventInformation->GetHitCount() << G4endl;
      G4cout << "\tNumber of SiPMs above threshold("<<fSiPMThreshold<<") : "
      << eventInformation->GetSiPMSAboveThreshold() << G4endl;
      G4cout << "\tNumber of photons produced by scintillation in this event : "
      << eventInformation->GetPhotonCount_Scint() << G4endl;
      G4cout << "\tNumber of photons produced by cerenkov in this event : "
      << eventInformation->GetPhotonCount_Ceren() << G4endl;
      G4cout << "\tNumber of photons absorbed (OpAbsorption) in this event : "
      << eventInformation->GetAbsorptionCount() << G4endl;
      G4cout << "\tNumber of photons absorbed at boundaries (OpBoundary) in "
      << "this event : " << eventInformation->GetBoundaryAbsorptionCount()
      << G4endl;
      G4cout << "Unacounted for photons in this event : "
      << (eventInformation->GetPhotonCount_Scint() +
          eventInformation->GetPhotonCount_Ceren() -
          eventInformation->GetAbsorptionCount() -
          eventInformation->GetHitCount() -
          eventInformation->GetBoundaryAbsorptionCount())
      << G4endl;
   }
   //If we have set the flag to save 'special' events, save here
   if ( fSaveThreshold&&eventInformation->GetPhotonCount() <= fSaveThreshold )
   {
      G4RunManager::GetRunManager()->rndmSaveThisEvent();
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void qCalEventAction::SetSaveThreshold(G4int save){
   /*Sets the save threshold for the random number seed. If the number of photons
    generated in an event is lower than this, then save the seed for this event
    in a file called run###evt###.rndm
    */
   fSaveThreshold = save;
   G4RunManager::GetRunManager()->SetRandomNumberStore(true);
   G4RunManager::GetRunManager()->SetRandomNumberStoreDir("random/");
   //  G4UImanager::GetUIpointer()->ApplyCommand("/random/setSavingFlag 1");
}

