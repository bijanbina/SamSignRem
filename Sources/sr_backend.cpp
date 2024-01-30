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
    DIR    *dir = opendir(path.c_str());

    dirent *file = readdir(dir);
    while( file )
    {
        string filename = file->d_name;
        if( isValidImage(filename) )
        {
            ret.push_back(filename);
        }
        file = readdir(dir);
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

    string pattern = "SignerVer02";
    // add null bytes
    pattern.resize(pattern.length()+3, '\0');

    string rep;
    rep.resize(SR_REPLACE_SIZE, '\0');

    sr_fillPositions(pattern);
    sr_replaceBytes(rep);

    fclose(raw_file);
    fclose(img_file);
}

void sr_replaceBytes(string replacement)
{
    fseek(img_file, 0, SEEK_SET);
    curr_pos = 0;
    int pos_len = positions.size();
    for( int i=0 ; i<pos_len ; i++ )
    {
        sr_rwUntilPosition(positions[i]);
        sr_printAscii(i);
        sr_printHex(i);

        fwrite(replacement.c_str(), 1,
               replacement.length(), raw_file);
        curr_pos += SR_REPLACE_SIZE;
        fseek(img_file, curr_pos, SEEK_SET);
    }

    while( 1 )
    {
        int read_size = fread(f_buffer, 1, SR_BLOCK_SIZE, img_file);
        fwrite(f_buffer, 1, read_size, raw_file);

        if( read_size<SR_BLOCK_SIZE )
        {
            break;
        }
    }

    free(f_buffer);
}

void sr_fillPositions(string pattern)
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
        sr_findPattInBlock(&block, pattern);
        if( read_size<SR_BLOCK_SIZE )
        {
            break;
        }

        counter++;
        curr_pos = counter*SR_BLOCK_SIZE-SR_BUFFER_MARGIN;
        fseek(img_file, curr_pos, SEEK_SET);
    }
}

void sr_findPattInBlock(string *block, string pattern)
{
    long start_pos = 0;
    while( 1 )
    {
        int index = block->find(pattern, start_pos);
        if( index==-1 ) // not found
        {
            break;
        }

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
        fwrite(f_buffer, 1, read_size, raw_file);
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

void sr_rmRaw(string base_name)
{
    string raw_name = base_name;
    raw_name.replace(base_name.size()-4, 4, ".raw");
    string raw_path = sr_getCurrentPath() + "\\" + raw_name;
    remove(raw_path.c_str());
}

// return true if its a valid image format
int isValidImage(string name)
{
    if( name=="recovery.img" ||
        name=="recovery.bin" )
    {
        return 0;
    }

    int have_img = name.find(".img");
    int have_bin = name.find(".bin");
    if( have_img==-1 && have_bin==-1 )
    {
        return 0;
    }

    return 1;
}
