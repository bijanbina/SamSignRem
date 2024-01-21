#include "sr_backend.h"

FILE *img_file = NULL;
FILE *raw_file = NULL;
char *f_buffer;
long  curr_pos;

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
    vector<long> positions = sr_findPositions();
    // scan 2
    sr_replaceBytes(positions);

    fclose(raw_file);
    fclose(img_file);
}

void sr_replaceBytes(vector<long> positions)
{
    size_t read_size;
    fseek(img_file, 0, SEEK_SET);
    size_t total_read = 0;
    long position_cntr = 0;
    while( 1 )
    {
        size_t min_read = sr_getReadSize(positions, position_cntr,
                                      total_read);
        total_read += min_read;

        read_size = fread(f_buffer, 1, min_read, img_file);
        fwrite(f_buffer, read_size, 1, raw_file);
        if( min_read!=SR_BLOCK_SIZE )
        {
            for( long i=0 ; i<SR_REPLACE_SIZE ; i++ )
            {
                fwrite("\0", 1, 1, raw_file);
            }
            fseek(img_file, total_read + SR_REPLACE_SIZE, SEEK_SET);
            position_cntr++;
        }
    }

    free(f_buffer);
}

// list all .img in current dir except recovery.img
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

vector<long> sr_findPositions()
{
    long counter = 0;
    vector<long> positions;
    size_t read_size;
    f_buffer = (char *)malloc(SR_BLOCK_SIZE);
    curr_pos = 0;

    while( 1 )
    {
        read_size = fread(f_buffer, 1, SR_BLOCK_SIZE, img_file);
        string block(f_buffer, read_size);
        cout << ">> " << counter << " " << read_size << endl;

        /// FIXME: based on seek it should change
        sr_findBlockPos(&block, &positions);
        if( read_size<SR_BLOCK_SIZE )
        {
            break;
        }

        // to prevent segmentation of pattern
        counter++;
        curr_pos = counter*SR_BLOCK_SIZE-SR_BUFFER_MARGIN;
        fseek(img_file, curr_pos, SEEK_SET);
    }
    return positions;
}

long sr_getReadSize(vector<long> addresses, size_t nth,
                    long curr_addr)
{
    if( nth<0 || nth>=addresses.size() )
    {
        return SR_BLOCK_SIZE;
    }
    long rs = addresses[nth]; //read size
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

void sr_findBlockPos(string *block, vector<long> *positions)
{
    long start_pos = 0;
    string pattern = "SignerVer02";

    while( 1 )
    {
        size_t index = block->find(pattern, start_pos);
        if( index!=string::npos )
        { // found
            start_pos = index;
            index += curr_pos;

            // only add index if it doesn't already exist
            vector<long>::iterator it;
            it = find(positions->begin(), positions->end(),
                      index);
            if( it==positions->end() )
            { // new found pattern in file
                positions->push_back(index);
            }
        }
        else
        {
            break;
        }
    }
}
