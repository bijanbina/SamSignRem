#include "sr_backend.h"

FILE *img_file = NULL;
FILE *raw_file = NULL;
char *f_buffer;
long  curr_pos;
vector<long> positions;

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

    cout << "Processing " << base_name << endl;

    sr_fillPositions();
    sr_replaceBytes();

    fclose(raw_file);
    fclose(img_file);
}

void sr_replaceBytes()
{
    fseek(img_file, 0, SEEK_SET);
    curr_pos = 0;
    int pos_len = positions.size();
    for( int i=0 ; i<pos_len ; i++ )
    {
        sr_rwUntilPosition(positions[i]);
        sr_printAscii(i);
        sr_printHex(i);

        for( long j=0 ; j<SR_REPLACE_SIZE ; j++ )
        {
            fwrite("\0", 1, 1, raw_file);
        }
        curr_pos += SR_REPLACE_SIZE;
        fseek(img_file, curr_pos, SEEK_SET);
    }

    while( 1 )
    {
        int read_size = fread(f_buffer, 1, SR_BLOCK_SIZE, img_file);
        fwrite(f_buffer, read_size, 1, raw_file);

        if( read_size<SR_BLOCK_SIZE )
        {
            break;
        }
    }

    free(f_buffer);
}

void sr_fillPositions()
{
    positions.clear();
    long counter = 0;
    size_t read_size;
    f_buffer = (char *)malloc(SR_BLOCK_SIZE);
    curr_pos = 0;

    while( 1 )
    {
        read_size = fread(f_buffer, 1, SR_BLOCK_SIZE, img_file);
        string block(f_buffer, read_size);

        /// FIXME: based on seek it should change
        sr_findBlockPos(&block);
        if( read_size<SR_BLOCK_SIZE )
        {
            break;
        }

        // to prevent segmentation of pattern
        counter++;
        curr_pos = counter*SR_BLOCK_SIZE-SR_BUFFER_MARGIN;
        fseek(img_file, curr_pos, SEEK_SET);
    }
}

void sr_findBlockPos(string *block)
{
    long start_pos = 0;
    string pattern = "SignerVer02";

    while( 1 )
    {
        size_t index = block->find(pattern, start_pos);
        if( index!=string::npos )
        { // found
            start_pos = index + 1;
            index += curr_pos;

            // only add index if it doesn't already exist
            vector<long>::iterator it;
            it = find(positions.begin(), positions.end(),
                      index);
            if( it==positions.end() )
            { // new found pattern in file
                positions.push_back(index);
            }
        }
        else
        {
            break;
        }
    }
}

void sr_printAscii(int index)
{
    char buffer[46];
    long cur_pos = ftell(img_file);
    fseek(img_file, positions[index], SEEK_SET);
    int read_size = fread(buffer, 1, SR_REPLACE_SIZE, img_file);

    cout << "[" << index << "] Str: ";
    for( int i=0 ; i<read_size ; i++ )
    {
        if( buffer[i]>31 && buffer[i]<127 )
        {
            printf("%c", buffer[i]);
        }
        else
        {
            printf(".");
        }
    }
    cout << endl;

    fseek(img_file, cur_pos, SEEK_SET);
}

void sr_printHex(int index)
{
    char buffer[46];
    long cur_pos = ftell(img_file);
    fseek(img_file, positions[index], SEEK_SET);
    int read_size = fread(buffer, 1, SR_REPLACE_SIZE, img_file);

    cout << "[" << index << "] Hex: ";
    if( read_size>20 )
    {
        read_size = 20;
    }
    for( int i=0 ; i<read_size ; i++ )
    {
        printf("0x%x ", buffer[i]);
    }
    cout << endl;

    fseek(img_file, cur_pos, SEEK_SET);
}

// read from file and write to the output till the input position
void sr_rwUntilPosition(long position)
{
    while( curr_pos<position )
    {
        int read_size = SR_BLOCK_SIZE;
        if( curr_pos+SR_BLOCK_SIZE>position )
        {
            read_size = position - curr_pos;
        }

        fread(f_buffer, 1, read_size, img_file);
        fwrite(f_buffer, read_size, 1, raw_file);
        curr_pos += read_size;
    }
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
