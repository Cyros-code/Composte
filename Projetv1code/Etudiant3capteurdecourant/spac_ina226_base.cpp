#include "spac_ina226_base.h"
#include <linux/i2c-dev.h>  // il communique avec le bus rasberry 
#include <sys/ioctl.h>     // controler les périphériques
#include <fcntl.h>         // Pour dÃ©finit des flags = O_RDWR
#include <unistd.h>        // Pour read(), write(), close()
#include <iostream>        // blibiothéque entrée sortie standard 
#include <cstdio>

using namespace std; // ca permet de utiliser sans les std

INA226::INA226(uint8_t addr) : fd(-1), address(addr) , energieCumuleeWh(0.0){} 
// INA226(uint8_t addr) = C'est le nom de la fonction. Elle reÃ§oit l'adresse I2C
// fd(-1) = om lui donne la valeur -1 pour indiquer que le bus n'est pas encore ouvert
// address(addr) : On prend l'adresse reÃ§ue (0x40) et on l'enregistre dans la mÃ©moire
//energieCumuleeWh(0.0) = Cette variable va augmenter petit a petit a chaque seconde

// le destructeur de la classe Ina226 : ferme le bus 
INA226::~INA226() 
{ 
    closeBus(); 
} 

// La fonction openBus sert a établir la communication
// physique entre le Raspberry Pi et le bus I2C
bool INA226::openBus(const string& device) {
    fd = ::open(device.c_str(), O_RDWR); 
    //il permet de ouvrir un fichier/périphérique
    // O_RDWR = read/write float INA226::getVoltage() {
    //device.c_str() = il permet de convertir le string en char 
    // fd est un file descriptor (un entier)
    if (fd < 0) return false; 
    if (ioctl(fd, I2C_SLAVE, address) < 0) return false;
    // ioctl = permet d'envoyer une commande spéciale au périphérique
    // I2C_SLAVE = il permet de parler a un appareil 
    // address = c'est address I2C du capteur
    // return false = il arrete la fonction
    return true; 
}

//La fonction closeBus sert a fermer proprement la communication 
//avec le bus I2C et a libérer le fichier de périphérique
void INA226::closeBus() {
    if (fd >= 0) { 
        // On vérifie si le bus I2C est bien ouvert
        ::close(fd); 
        // On demande au systéme Linux de libérer ce fichier
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
    // le write il permet de envoyer 1 octet au capteur 
    // ex : "Attention , je veux lire cette registre 
    write(fd, &reg, 1);
    //  on dit au capteur quel registre on veut lire
    read(fd, buf, 2);
    //  On lit les 2 octets de données
    res_16 = (uint16_t)buf[0]; // On prend l'octet de poids fort
    res_16 = res_16 << 8 ; // On le décale de 8 bits vers la gauche
    res_16 = res_16 | buf[1]; // On ajoute l'octet de poids faible avec un OU logique
    return (int16_t)res_16;// On retourne le résultat final

}

// ici c'est le tension 
float INA226::getVoltage() {
    // Le registre 0x02 contient la tension du bus
    // L'unite est de 1.25mV par bit
    uint16_t raw = (uint16_t)readRegister(0x02); 
    return (float)raw * 0.00125;
}

float INA226::getCurrent() {
    // On lit le registre 04h 
    int16_t raw = readRegister(0x04); 
    // Le courant reel = Valeur_Registre * Current_LSB
    // Avec maxCurrent = 1.0A, le Current_LSB est 1.0 / 32768
    // convertir la valeur numerique brute (les bits) 
    //-> en une valeur physique relle exprimÃ©e en Amperes 
    return (float)raw * (1.0 / 32768.0); 
}

// configurer le capteur INA226 au démarrage pour qu'il 
//soit capable de calculer lui-méme, de maniére autonome 
bool INA226::calibrate(double shuntOhm, double maxCurrent) {
    // Calcul du Current_LSB : courant max divisé par 2^15
    double currentLSB = maxCurrent / 32768.0;
    // Calcul de la valeur de calibration (CAL)
    // Formule : CAL = 0.00512 <-(constant fixe) / (Current_LSB * R_SHUNT)
    uint16_t calValue = (uint16_t)(0.00512 / (currentLSB * shuntOhm)); 
    // écriture du résultat dans le registre 05h
    return writeRegister(0x05, calValue);
}

bool INA226::writeRegister(uint8_t reg, uint16_t value) {
    uint8_t buffer[3]; // on crée un tableau de 3 valeurs
    buffer[0] = reg; // L'adresse du registre ou on veut écrire
    buffer[1] = (value >> 8) & 0xFF; // Octet de poids fort (MSB)
    buffer[2] = value & 0xFF;        // Octet de poids faible (LSB)

    int resultat = write(fd, buffer, 3);
    // il va ecrire les données 
    // fd =  file descriptor
    // buffer = envoie les donneÃ©s 
    // la il envoie 3 bits 

    // On verifie si le résultat est envoye a 3 octet 
    if (resultat == 3) { // si oui 
        return true;  // bon
    } else {
        return false; // pas bon
    }
}


float INA226::getPower() {
    // Le registre 0x03 contient la puissance calculee par l'INA226
    int16_t raw = readRegister(0x03);
    // La puissance reelle = Valeur_Registre * Power_LSB
    // Power_LSB est fixe a 25 * Current_LSB (selon la datasheet)
    // Avec ta calibration a 1.0A max, le Power_LSB est de 0.0007629
    return (float)raw * (1.0 / 32768) * 25; 
    // 32768 = 2^15
}


void INA226::calculer_energie() {
    // On recupere la puissance actuelle (P)
    // Note : getPower() utilise le registre 0x03 de l'INA226
    float puissance = getPower(); 
    // Calcul de l'energie pour 1 seconde :
    // Formule : E(Wh) = P(W) * t(h)
    // Comme t = 10 seconde, on convertit en heures : 10s = 1/360 h 
    // += qui va additionnner les deux valeur et qui stocke le résultat 
    energieCumuleeWh += puissance * (1 / 360.0);

    // Affichage avec 4 decimales car Wh est une petite unité
    printf("Energie totale consommee : %.4f Wh\n", energieCumuleeWh);
    printf("---------------------------------\n");
}


