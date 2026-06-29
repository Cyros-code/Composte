#ifndef SPAC_INA226_BASE_H
#define SPAC_INA226_BASE_H

#include <cstdint> // types entiers de taille fixe
#include <string> // chaine de caractére 


// void = elle retourne aucune valeurs

class INA226 {
public:
    INA226(uint8_t i2cAddress = 0x40); // address de base de bus 
    ~INA226(); // destructeur 

    bool openBus(const std::string& device = "/dev/i2c-1"); 
    // il sert a ouvrir un bus i2c pour communiquer avec le périphérique
    void closeBus();
    // il ferme le bus , le bus c'est une chemin de communication 

    bool calibrate(double shuntOhm, double maxCurrent); 
    
    // calibration du resistance et du courrant max 
    bool writeRegister(uint8_t reg, uint16_t value); 
    // ecrire une valeur dans un registre d'un composant  
    // reg = c'est le registre du periphérique qu'on veut ecrire 
    
    //Méthode des classes dans le diagramme de classes 
    float getVoltage();
    float getCurrent(); // le courant 
    float getPower();
    
    void calculer_energie();
    void puissance_affichage(); // la puissance_affichage 

private:
    int fd; // une variable entiere , descripteur du bus i2c
    uint8_t address; // address de registre 
    float energieCumuleeWh;
    int16_t readRegister(uint8_t reg); 
    // lire la valeur du registre 
    // uint8_t = entier non signé sur 8 bits
    // reg = numero de registre
    
};

#endif
