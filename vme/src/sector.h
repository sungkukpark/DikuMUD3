#ifndef SECTOR_HPP
#define SECTOR_HPP
#include <vector>
#include <string>
#include "vme.h"
using namespace std;
class cSector
{
private:
    std::vector< string > names;
    std::vector< std::vector < int > > endurance;
    std::vector< std::vector < int > > path;

public:

    string get_name(int sector);
    inline int get_num_of_sectors() {
        return ((int)names.size());
    };
    int get_path_cost(int from, int to);
    int get_enduance_cost (int from, int to);
    void add_sector( int sector, string sector_name );
    void add_sector( int sector, char *sector_name );
    void set_path_cost (int from, int to, int cost);
    void set_endurance_cost (int from, int to, int cost);
    void set_path_endurance_cost (int from, int to, int pcost, int ecost);
};
#endif
