#include "sapc_bdd.h"

int main(int argc, char **argv) {
    

   sapc_bdd BDD(argv[1]) ;

   //création des différent User 
   if(!BDD.createUser("PROF","admin","1234","test@outlook.fr", "ADMIN"))
   {
    std::cerr<<"Erreur creation user "<< std::endl;
    BDD.fin();
   }
   else
   {
   std::cout<<"creation user admin --> ok" <<std::endl;
   }


   if(!BDD.login("PROF","admin", "1234"))
   {
    std::cerr<<"Mauvais login ou mot de passe"<< std::endl;
    
   }
   else{
    std::cout<<"Authentification PROF admin --> ok" <<std::endl;
   }

   if(!BDD.createUser("Alice","User_1","5678","Alice@outlook.fr", "USER"))
   {
    std::cerr<<"Erreur creation user "<< std::endl;
    BDD.fin();
   }
   else
   {
   std::cout<<"creation user User_1--> ok" <<std::endl;
   }


   if(!BDD.login("Alice","User_1", "5678"))
   {
    std::cerr<<"Mauvais login ou mot de passe"<< std::endl;
    
   }
   else{
    std::cout<<"Authentification Alice User_1 --> ok" <<std::endl;
   }

   //création des entrer des capteur 
   if(!BDD.createCapteur("ina226","1","Temprature, Humidité")){
      std::cerr << "Erreur creation capteur" << std::endl;
      BDD.fin();
   }else {
      std::cout << "Creatiion Capteur --> OK" << std::endl;
   }


   if(!BDD.createCapteur("MC3008","1","Energie")){
      std::cerr << "Erreur creation capteur" << std::endl;
      BDD.fin();
   }else {
      std::cout << "Creatiion Capteur --> OK" << std::endl;
   }
   
   //Création des entrer des Mesure 
   if (!BDD.createMesure("24/04/2026 15:30", "30%")){
      std::cerr << "Erreur Création Mesure" << std::endl;
      BDD.fin();
   }else {
      std::cout << "Création --> OK" << std::endl;
   }
   
   if (!BDD.createMesure("25/04/2026 16:30", "20%"))
   {
      std::cerr << "Erreur Création Mesure " << std::endl;
      BDD.fin();
   }else{
      std::cout << "Création Mesure --> OK" << std::endl;
   }

   


   return 0;
}
