#include "tracker.h"
int main()
{
    map<int, login_struct*> live_user_map;
    map<int, login_struct*> user_map;
    map<int, set<string>*> filemap;
    map<string, vector<chunk_meta*>*>file_chunkmap;
    tracker_method( live_user_map, user_map, filemap, file_chunkmap);
    return 0;
}