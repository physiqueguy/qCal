#ifndef qCalActionInitialization_h
#define qCalActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class qCalDetectorConstruction;
//class qCalRecorderBase;

class qCalActionInitialization : public G4VUserActionInitialization
{
public:
    qCalActionInitialization();
    virtual ~qCalActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;

private:
   
};
#endif
