#include <iostream> // blibiothéque entrÃ©e sortie standard
#include <iomanip> // Pour l'affichage propre des chiffres
#include <unistd.h> // permet d'interagir avec le systéme d'exploitation
#include <cmath>
#include "spac_ina226_base.h"

using namespace std;

int main() {
// Initialisation du capteur a l'adresse par d'Ã©faut 0x40
    INA226 capteur(0x40);
    // Ouverture du bus I2C standard du Raspberry Pi
    if (!capteur.openBus("/dev/i2c-1")) {
    cout << "Erreur : Impossible d'accÃ©der au bus I2C. VÃ©rifie le cÃ©blage !" << endl;
    return 1;
    }


cout << "========================================" << endl;
cout << " SAPC - Mesure d'Ã©nergie " << endl;
cout << "========================================" << endl;
// On utilise la mÃ©thode demandÃ©e dans ton document
// shuntOhm = 0.1
// maxCurrent = 1.0 AmpÃ©re
    if (capteur.calibrate(0.1, 1.0)) {
    // appelle une fonction
    // elle prend deux paramztres
        cout << "Calibration effectuÃ©e !" << endl;
        } else {
        cout << "Ã©chec de la calibration !" << endl;
        }
        cout << "Lecture en cours... "<< endl;

// boucle infinie

int i = 0 ;

    while(true) {

    cout << ++i << endl ; // le nombre il conte combien il affiche
    float voltage = capteur.getVoltage();
    float courant = capteur.getCurrent();
    float courantAbsolu = abs(courant); 
    float puissance = voltage * courant;
    float puissanceInterne = capteur.getPower(); // Lit le registre 03h
// CALCULS
    float capaciteBatterieAh = 12.0;
    float coefficientSecurite = 0.67; // Tes 67% d'efficacitÃ©
    float autonomieHeures = 0;
    float tensionRecalculee = 0;
    // U = P/I
    // recalculer la tension  partir de la puissance et du courant
    // il refuse de faire la division pour protÃ©ger le Raspberry Pi
     if (courantAbsolu > 0.01) {
            tensionRecalculee = abs(puissanceInterne) / courantAbsolu;
        }


// AFFICHAGE

    cout << "\n MESURES \n" << endl;
    cout << "Tension \n" << voltage << "V"<< endl;
    cout << "Courant consomme \n" << courant << "A"<< endl;
    cout << "Puissance absorbee \n" << puissance << "W"<< endl;
    cout << "Tension Calculee (P/I): " << tensionRecalculee << " V" << endl;


  // Calcul et affichage de l'autonomie (en tenant compte des 67%)
        if (courantAbsolu > 0.01) { 
            autonomieHeures = (capaciteBatterieAh * coefficientSecurite) / courantAbsolu;
            printf("Autonomie estimee  : %.1f heures (%.1f jours)\n", 
                    autonomieHeures, autonomieHeures / 24.0);
        } else {
            printf("Autonomie          : Maximale (Repos)\n");
        }
        
    if (voltage > 0.1 && voltage < 36.0) {
    capteur.calculer_energie();
    } else {
    cout << "[Warning] Tension hors plage ou capteur dÃ©branchÃ© !" << endl;
    }



// CALCUL ENERGIE
    capteur.calculer_energie();
    sleep(10); // On attend 1 seconde entre chaque mesure
    }

// Fermeture propre du bus
    capteur.closeBus();
    return 0; 
}

