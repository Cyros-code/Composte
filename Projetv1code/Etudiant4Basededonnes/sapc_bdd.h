#ifndef SAPC_BDD_H
#define SAPC_BDD_H


#include <iostream>
#include <ctime>
#include <sqlite3.h>
#include <string.h>

#include <openssl/sha.h>
#include <iostream>
#include <sstream>
#include <iomanip>

//! *********************************************************************************************
/// CONSOLE est une constante de compilation conditionnelle permettant ou NON d'avoir une IHM
/// texte d'interaction
/// avec la base de donnée sur le raspberry
/// Si commentee  => pas de console
/// Si non commentee => console
/// *********************************************************************************************

//#define CONSOLE 11111

//! *********************************************************************************************
/// SQLITE est une constante de compilation conditionnelle permettant de travailler avec SQLITE
/// avec la base de donnée sur le raspberry

/// *********************************************************************************************


#define SQLITE  22222

/** \class sapc_bdd sapc_bdd.h
    \brief : classe d'application de gestion de mesures de temperatures et humidite du compost
    (campagne de mesure) */
/**  <a href="../../">HowTocompile  </a>
*/

class sapc_bdd
{
    
public :
        ///init classe metier Base de données

    sapc_bdd(std::string file);
    //creation (ajout) d'un utilisateur dans la bdd avec son nom, login, mot de passe crypté, mail et son role
    bool createUser(std::string nom, std::string login, std::string password, std::string email, std::string role);
    //verif si utilisateur connu dans la base
    bool login(std::string nom,std::string login, std::string password);
    //Création d'un capteur dans la bdd avec type_capteur, unite et description
    bool createCapteur(std::string type_capteur, std::string unite, std::string description);
    //Création d'une mesure dans la bdd avec date_heure et valeur 
    bool createMesure(std::string date_heure_mesure, std::string valeur);
    //Création d'un seuil d'alarme dans la bdd avec valeur_min, valeur_max)
    bool createSeuil_Alerte(std::string valeur_min, std::string valeur_max, std::string actif);
    //création d'une alerte dans la bdd avec date_heure, valeur_mesure et type_depassement
    bool createAlerte(std::string date_heure_alerte, std::string valeur_mesuree, std::string type_depassement);
    //création d'une notif dans la bdd avec date_envoi et statue
    bool createNotif(std::string date_envoi, std::string statue);



  
    void fin();
private:

    sqlite3 *db; //pointeur instance bdd
    std::string sha256(const std::string& str); //methode interne a la librairie ssl permettant de crypter le mot de passe
 
};   

#endif // sapc_bdd_H
