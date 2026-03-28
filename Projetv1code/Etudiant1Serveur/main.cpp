#include <iostream>   // blibiothéque entrée sortie standard 
#include <iomanip>    // Pour l'affichage propre des chiffres
#include <unistd.h>   // permet d'interagir avec le système d'exploitation
#include "spac_ina226_base.h"

using namespace std;

int main() {
    //  Initialisation du capteur à l'adresse par défaut 0x40
    INA226 capteur(0x40);

    // Ouverture du bus I2C standard du Raspberry Pi
    if (!capteur.openBus("/dev/i2c-1")) {
        cout << "Erreur : Impossible d'accéder au bus I2C. Vérifie le câblage !" << endl;
        return 1;
    }

    cout << "========================================" << endl;
    cout << "   SAPC - Mesure d'Énergie  " << endl;
    cout << "========================================" << endl;
   
   // On utilise la méthode demandée dans ton document
    // shuntOhm = 0.1 
    // maxCurrent = 1.0 Ampère 
    if (capteur.calibrate(0.1, 1.0)) {
        // appelle une fonction
        // elle prend deux paramètres
        cout << "Calibration effectuée !" << endl;
    } else {
        cout << "Échec de la calibration !" << endl; 
        
    }
    cout << "Lecture en cours... "<< endl;
 
   while (true) {
        // On appelle la méthode qu'on a créée dans la classe
        capteur.puissance_affichage();

        // On attend 1 seconde
        sleep(1);
    }

    // Fermeture propre du bus 
    capteur.closeBus();
    return 0;
}
