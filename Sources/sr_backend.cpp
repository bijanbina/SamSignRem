#include "sr_backend.h"

FILE *img_file = NULL;
FILE *raw_file = NULL;
char *f_buffer;

void sr_processFiles(string base_name)
{
    string raw_name = base_name;
    raw_name.replace(base_name.size()-4, 4, ".raw");
    string full_path = sr_getCurrentPath() + "\\" + base_name;
    string raw_path = sr_getCurrentPath() + "\\" + raw_name;
    img_file = fopen(full_path.c_str(), "rb");
    raw_file = fopen(raw_path.c_str(), "wb");
    if( img_file==NULL )
    {
        cout << "Error: cannot open file: " << full_path << endl;
        fclose(raw_file);
        return;
    }

    // scan 1
    vector<int> positions = sr_findPositions();
    // scan 2
    sr_replaceBytes(positions);

    fclose(raw_file);
    fclose(img_file);
}

void sr_replaceBytes(vector<int> positions)
{
    size_t read_size;
    fseek(img_file, 0, SEEK_SET);
    size_t total_read = 0;
    int position_cntr = 0;
    while( 1 )
    {
        size_t min_read = sr_getReadSize(positions, position_cntr,
                                      total_read);
        total_read += min_read;

        read_size = fread(f_buffer, 1, min_read, img_file);
        fwrite(f_buffer, read_size, 1, raw_file);
        if( min_read!=SR_BLOCK_SIZE )
        {
            for( int i=0 ; i<SR_REPLACE_SIZE ; i++ )
            {
                fwrite("\0", 1, 1, raw_file);
            }
            fseek(img_file, total_read + SR_REPLACE_SIZE, SEEK_SET);
            position_cntr++;
        }
    }

    free(f_buffer);
}

// check if .exe file is in debug or release directories, if yes
// change directory to project directory
void sr_checkProjDir()
{
    char buffer[MAX_PATH];
    string exe_path = sr_getCurrentPath();
    size_t delim_ind = exe_path.find_last_of("\\");
    string exe_dir = exe_path.substr(delim_ind+1);
    if( exe_dir=="debug" || exe_dir=="release" )
    {
        string proj_dir = exe_path.substr(0, delim_ind);
        SetCurrentDirectoryA(proj_dir.c_str());
        GetCurrentDirectoryA(MAX_PATH, buffer);
        cout << "New current directory: " << buffer << endl;
    }
}

// list imgs in proj dir except recovery.img
vector<string> sr_findImgs()
{
    vector<string> ret;
    string path = sr_getCurrentPath();

    DIR *dir = opendir(path.c_str());
    struct dirent *entry;

    while( 1 )
    {
        entry = readdir(dir);
        if( entry==NULL )
        {
            break;
        }
        string name = entry->d_name;
        if( name.find(".img")!=string::npos &&
            name!="recovery.img" )
        {
            ret.push_back(name);
        }
    }
    closedir(dir);

    return ret;
}

string sr_getCurrentPath()
{
    char buffer[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, buffer);

    string path(buffer);
    return path;
}

void sr_raw2img(string base_name)
{
    string raw_name = base_name;
    raw_name.replace(base_name.size()-4, 4, ".raw");
    string full_path = sr_getCurrentPath() + "\\" + base_name;
    string raw_path = sr_getCurrentPath() + "\\" + raw_name;

    remove(full_path.c_str());
    rename(raw_path.c_str(), full_path.c_str());
}

vector<int> sr_findPositions()
{
    int counter = 0;
    vector<int> positions;
    size_t read_size;
    f_buffer = (char *)malloc(SR_BLOCK_SIZE);

    // SCAN 1
    while( 1 )
    {
        read_size = fread(f_buffer, 1, SR_BLOCK_SIZE, img_file);
        string block(f_buffer, read_size);
        cout << ">> " << counter << " " << read_size << endl;

        /// FIXME: based on seek it should change
        sr_findBlockPos(&block, &positions,
                        counter*SR_BLOCK_SIZE);
        if( read_size<SR_BLOCK_SIZE )
        {
            break;
        }

        // to prevent segmentation of pattern
        counter++;
        fseek(img_file, counter*SR_BLOCK_SIZE-SR_BUFFER_MARGIN,
              SEEK_SET);
    }
    return positions;
}

int sr_getReadSize(vector<int> addresses, size_t nth, int curr_addr)
{
    if( nth<0 || nth>=addresses.size() )
    {
        return SR_BLOCK_SIZE;
    }
    int rs = addresses[nth]; //read size
    if( rs>=curr_addr+SR_BLOCK_SIZE )
    {
        return SR_BLOCK_SIZE;
    }
    while( rs>=SR_BLOCK_SIZE )
    {
        rs -= SR_BLOCK_SIZE;
    }
    return rs;
}

void sr_findBlockPos(string *block, vector<int> *positions,
                     int file_offset)
{
    int start_pos = 0;
    string pattern = "SignerVer02";

    while( 1 )
    {
        size_t index = block->find(pattern, start_pos);
        if( index!=string::npos )
        { // found
            start_pos = index;

            // only add index if it doesn't exist already
            vector<int>::iterator it;
            it = find(positions->begin(), positions->end(),
                      file_offset + index);
            if( it==positions->end() )
            { // new found pattern in file
                positions->push_back(file_offset + index);
            }
        }
        else
        {
            break;
        }
    }
}
