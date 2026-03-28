#include "spac_ina226_base.h"
#include <linux/i2c-dev.h>  // il communique avec le bus rasberry 
#include <sys/ioctl.h>     // contrôler les périphériques
#include <fcntl.h>         // Pour définit des flags = O_RDWR
#include <unistd.h>        // Pour read(), write(), close()
#include <iostream>        // blibiothéque entrée sortie standard 

using namespace std; // ca permet de utiliser sans les std

INA226::INA226(uint8_t addr) : fd(-1), address(addr) {} 
// INA226(uint8_t addr) = C'est le nom de la fonction. Elle reçoit l'adresse I2C
// fd(-1) = om lui donne la valeur -1 pour indiquer que le bus n'est pas encore ouvert
// address(addr) : On prend l'adresse reçue (0x40) et on l'enregistre dans la mémoire

// le destructeur de la classe Ina226 : ferme le bus 
INA226::~INA226() 
{ 
    closeBus(); 
} 

// elle retourne une booléenne 0 ou 1 et il prend le paramétre de device une chaine de charactére 
bool INA226::openBus(const string& device) {
    fd = ::open(device.c_str(), O_RDWR); 
    //il permet de ouvrir un fichier/périphérique
    // O_RDWR = read/write
    //device.c_str() = il permet de convertir le string en char 
    // fd est un file descriptor (un entier)
    if (fd < 0) return false; 
    if (ioctl(fd, I2C_SLAVE, address) < 0) return false;
    // ioctl = permet d’envoyer une commande spéciale au périphérique
    // I2C_SLAVE = il permet de parler a un appareil 
    // address = c'est address I2C du capteur
    // return false = il arrete la fonction
    return true; 
}


void INA226::closeBus() {
    if (fd >= 0) { 
        // On vérifie si le bus I2C est bien ouvert
        ::close(fd); 
        // On demande au système Linux de libérer ce fichier
        fd = -1; 
        // on remet identifiant a -1
    }
}

// Fonction pour lire un registre de 16 bits sur le bus I2C
int16_t INA226::readRegister(uint8_t reg) {
    uint8_t buf[2];
    // tableau a deux cases
    uint16_t res_16 = 0;
    // le variable de 16 bits qui permet de stocker les valeurs 
    write(fd, &reg, 1);
    //  on dit au capteur quel registre on veut lire
   
    read(fd, buf, 2);
     //  On lit les 2 octets de données
    res_16 = (uint16_t)buf[0]; // On prend l'octet de poids fort
    res_16 = res_16 << 8 ; // On le décale de 8 bits vers la gauche
    res_16 = res_16 | buf[1]; // On ajoute l'octet de poids faible avec un OU logique
    return (int16_t)res_16;      // On retourne le résultat final

}

float INA226::getVoltage() {
    // Le registre 0x02 contient la tension du bus
    // L'unité est de 1.25mV par bit
    int16_t raw = readRegister(0x02);
    return raw * 0.00125; 
}

float INA226::getCurrent() {
    // On lit le registre 04h 
    int16_t raw = readRegister(0x04); 
    // Le courant réel = Valeur_Registre * Current_LSB
    // Avec maxCurrent = 1.0A, le Current_LSB est 1.0 / 32768
    return (float)raw * (1.0 / 32768.0); 
}

// Implémentation de la méthode de calibration selon la doc technique
bool INA226::calibrate(double shuntOhm, double maxCurrent) {
    // Calcul du Current_LSB : courant max divisé par 2^15
    double currentLSB = maxCurrent / 32768.0;

    // Calcul de la valeur de calibration (CAL)
    // Formule : CAL = 0.00512 / (Current_LSB * R_SHUNT)
    uint16_t calValue = (uint16_t)(0.00512 / (currentLSB * shuntOhm)); 

    // Écriture du résultat dans le registre 05h
    return writeRegister(0x05, calValue);
}

bool INA226::writeRegister(uint8_t reg, uint16_t value) {
    uint8_t buffer[3]; // on crée un tableau de 3 valeurs
    buffer[0] = reg; // L'adresse du registre où on veut écrire
    buffer[1] = (value >> 8) & 0xFF; // Octet de poids fort (MSB)
    buffer[2] = value & 0xFF;        // Octet de poids faible (LSB)

    int resultat = write(fd, buffer, 3);
    // il va ecrire les données 
    // fd =  file descriptor
    // buffer = envoie les donneés 
    // la il envoie 3 bits 

    // On verifie si le résultat est envoyé a 3 octet 
    if (resultat == 3) { // si oui 
        return true;  // bon
    } else {
        return false; // pas bon
    }
}

void INA226::puissance_affichage() {
    //  Récupération des données brutes du capteur
    float tension = getVoltage(); // Voltage
    float courant = getCurrent(); // Courrent 

    //  Calculs (Loi d'Ohm et Énergie)
    float puissance = tension * courant; // P = U * I
    float capaciteBatterieAh = 12.0; // Ta batterie de 6Ah * 2 en parallèle = 12Ah
    float  autonomieHeures = 0; 

    //  Affichage des mesures de base
    printf("Tension : %.2f V | Courant : %.3f A | Puissance : %.2f W\n", tension, courant, puissance);
    //  %.2f = Cette ligne de code imprime la valeur de la variable total formatée à 2 décimales
    // %.3f = il permet d'afficher un nombre avec exactement trois chiffres après la virgule


    //  Calcul et affichage de l'autonomie
    if (courant > 0.01) { 
        autonomieHeures = capaciteBatterieAh / courant; // t = Q / I
        printf("Autonomie estimée : %.1f heures\n", autonomieHeures , autonomieHeures / 24.0);
        // %.1f est une instruction de formatage qui affiche un nombre avec un seul chiffre après la virgule
    } else {
        printf("Autonomie : Maximale \n"); 
    }
    
}