#include "sapc_bdd.h"

sapc_bdd::sapc_bdd(std::string file)
{
    char* errMsg = 0;
    int res = 0;
    std::string sql ;
    ///
    /// Connexion à la base de donnée
    ///
   
    if (sqlite3_open(file.c_str(), &db)) {
        std::cout << "Erreur ouverture DB\n";
 
    }
    // Activer les foreign keys
        res = sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
        if(res != SQLITE_OK) {
            std::cerr << "Erreur lors de l'activation des foreign keys : " << sqlite3_errmsg(db) << std::endl;
        } else {
            std::cout << "Base ouverte et foreign_keys activées." << std::endl;
        }
    //ameliore vitesse criture sur SD
    sqlite3_exec(db, "PRAGMA synchronous = NORMAL;", nullptr, nullptr, nullptr);
    
}

///methode de hachage pour mot de passe car non gere par sqlite directement
std::string sapc_bdd::sha256(const std::string& str)
{
        unsigned char hash[SHA256_DIGEST_LENGTH];

        SHA256((unsigned char*)str.c_str(), str.size(), hash);

        std::stringstream ss;

        for(int i=0;i<SHA256_DIGEST_LENGTH;i++)
        {
            ss << std::hex << std::setw(2)
               << std::setfill('0') << (int)hash[i];
        }

        return ss.str();
}
///creation user
bool sapc_bdd::createUser(std::string nom, std::string login, std::string password, std::string email, std::string  role)
    {
        sqlite3_stmt* stmt;

        std::string sql =
        "INSERT INTO utilisateur(nom, login,mdp_hash, email, role) VALUES(?,?,?,?,?)";

        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

        //hache le password avant comparaison !
        std::string hash = sha256(password);

        sqlite3_bind_text(stmt,1,nom.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt,2,login.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt,3,hash.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt,4,email.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt,5,role.c_str(),-1,SQLITE_TRANSIENT);
        
        int rc = sqlite3_step(stmt);

        sqlite3_finalize(stmt);

        return rc == SQLITE_DONE;
    }

bool sapc_bdd::createCapteur(std::string type_capteur, std::string unite, std::string description)
{

    sqlite3_stmt* stmt;

    std::string sql =
    "INSERT INTO capteur(type_capteur, unite, description) VALUES(?,?,?)";

    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    
    sqlite3_bind_text(stmt,1,type_capteur.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,unite.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,3,description.c_str(),-1,SQLITE_TRANSIENT);
        
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool sapc_bdd::createMesure(std::string date_heure_mesure, std::string valeur)
{
    sqlite3_stmt* stmt;

    std::string sql = "INSERT INTO mesure(date_heure, valeur) VALUE(?,?)";

    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    sqlite3_bind_text(stmt,1,date_heure_mesure.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,valeur.c_str(),-1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool sapc_bdd::createSeuil_Alerte(std::string valeur_min, std::string valeur_max, std::string actif)
{
    sqlite3_stmt* stmt;

    std::string sql = "INSERT INTO seuil_alerte(valeur_min, valeur_max, actif) VALUES(?,?,?)";

    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    sqlite3_bind_text(stmt,1,valeur_min.c_str(),-1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,valeur_max.c_str(),-1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,3,actif.c_str(),-1, SQLITE_TRANSIENT);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool sapc_bdd::createAlerte(std::string date_heure_alerte, std::string valeur_mesuree, std::string type_depassement)
{
    sqlite3_stmt* stmt;

    std::string sql = "INSERT INTO alerte (date_heure, valeur_mesuree, type_depassement) VALUES(?,?,?)";

    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    sqlite3_bind_text(stmt,1,date_heure_alerte.c_str(),-1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,valeur_mesuree.c_str(),-1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,3,type_depassement.c_str(),-1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool sapc_bdd::createNotif(std::string date_envoi, std::string statut)
{
    sqlite3_stmt* stmt;

    std::string sql = "INSERT INTO notification(date_envoi, statut) VALUES(?,?)";

    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    sqlite3_bind_text(stmt,1,date_envoi.c_str(),-1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt,2,statut.c_str(),-1,SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

//PM 15/3/26
//test login du user uniquement sur nom, login et mdp haché ; voir si email et role a ajouter plus tard
bool sapc_bdd::login(std::string nom, std::string login, std::string password)
    {
        sqlite3_stmt* stmt;

        std::string sql =
        "SELECT id_utilisateur FROM utilisateur WHERE nom=? AND login=? AND mdp_hash=?";

        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

        std::string hash = sha256(password);

        sqlite3_bind_text(stmt,1,nom.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt,2,login.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt,3,hash.c_str(),-1,SQLITE_TRANSIENT);

        int rc = sqlite3_step(stmt);

        sqlite3_finalize(stmt);

        return rc == SQLITE_ROW;
    }

    
void sapc_bdd::fin()
{
    //insere temps de fin si arret prog ou scenario
    sqlite3_close(db);
    exit(0);
}


