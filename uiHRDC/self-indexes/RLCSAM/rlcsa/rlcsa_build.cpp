#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "rlcsa_builder.h"
#include "misc/utils.h"


using namespace CSA;


/* Returns the lenght of file filename
 * returns -1 on error
 */
int fileLength(char const* filename){
    FILE* file = NULL;
    int len;
    file = fopen(filename,"r");
    /*Check for validity of the file.*/
    if(file == NULL){
        fprintf(stderr,"ERROR: Can't open file: %s\n",filename);
        return -1;
    }
    /*Calculate the file length in bytes. This will be the length of the source string.*/
    fseek(file, 0, SEEK_END);
    len = (unsigned int)ftell(file);
    fclose(file);
    return len;
}
/* reads the content of file filename and stores it in an arrays of size length+1 */
unsigned char* readFile(char const* filename, unsigned int* length){
    unsigned char* str = NULL;    
    FILE* file = NULL;
    int read;
    int len;
    *length = 0; 
    file = fopen(filename,"r");
    /*Check for validity of the file.*/
    if(file == NULL){
        fprintf(stderr,"ERROR: Can't open file: %s\n",filename);
        return NULL;
    }
    /*Calculate the file length in bytes. This will be the length of the source string.*/
    fseek(file, 0, SEEK_END);
    len = (unsigned int)ftell(file);
    fseek(file, 0, SEEK_SET);
    //str = (unsigned char*)malloc((len+1)*sizeof(char));
    str = new unsigned char[len+1];
    if(str == NULL){
        fprintf(stderr,"ERROR: readFile: Out of memory when trying to alloc %d bytes\n",len+1);
        return NULL;
    }
    read = fread(str, sizeof(char), len, file);
    fclose(file);
    if(read<len){
        fprintf(stderr,"ERROR: readFile: Read less chars than size of file\n");
        return NULL;
    }
    *length = len;
    return str;
}

/* reads the content of file filename and returns it as an \0-ended array
 * it also set length to the length of the array
 * when the file is \0-ended the length is the length of the file
 * when the file is not \0-ended the length is the length of the file plus one
 */
unsigned char* readText(char const* filename, unsigned int* length){
    unsigned char* str = readFile(filename, length);
    if(str != NULL){
        if(str[*length-1]!='\0' && str[*length-1]!='\xA'){/*removes trailing \n*/
            str[*length] = '\0';
            *length = *length+1;
        }else{
            str[*length-1] = '\0';
            /**length = *length;*/
        }
    }
    return str;
}


int
wholeFileRLCSA(std::string base_name, usint block_size, usint sample_rate, usint buffer_size)
{
  Parameters parameters;
  parameters.set(RLCSA_BLOCK_SIZE.first, block_size);
  parameters.set(SAMPLE_RATE.first, sample_rate);

  if(sample_rate > 0)
  {
    parameters.set(SUPPORT_LOCATE.first, 1);
    parameters.set(SUPPORT_DISPLAY.first, 1);
  }
  else
  {
    parameters.set(SUPPORT_LOCATE.first, 0);
    parameters.set(SUPPORT_DISPLAY.first, 0);
  }

  std::string parameters_name = base_name + PARAMETERS_EXTENSION;
  parameters.print();
  parameters.write(parameters_name);

  std::cout << "Input: " << base_name << std::endl;
  std::ifstream input_file(base_name.c_str(), std::ios_base::binary);
  if(!input_file)
  {
    std::cerr << "Error opening input file!" << std::endl;
    return 2;
  }
  std::cout << "Buffer size: " << buffer_size << " MB" << std::endl;
  std::cout << std::endl;

  double start = readTimer();
  RLCSABuilder builder(parameters.get(RLCSA_BLOCK_SIZE), parameters.get(SAMPLE_RATE), buffer_size * MEGABYTE);

    unsigned int len;
    unsigned char* text = readText(base_name.c_str(), &len);
    builder.insertSequence((char*)text, len, false);

  RLCSA* rlcsa = 0;
  if(builder.isOk())
  {
    rlcsa = builder.getRLCSA();
    rlcsa->writeTo(base_name);
  }
  else
  {
    std::cerr << "Error: RLCSA construction failed!" << std::endl;
    return 3;
  }

  double time = readTimer() - start;
  double build_time = builder.getBuildTime();
  double search_time = builder.getSearchTime();
  double sort_time = builder.getSortTime();
  double merge_time = builder.getMergeTime();

  double megabytes = rlcsa->getSize() / (double)MEGABYTE;
  usint sequences = rlcsa->getNumberOfSequences();
  std::cout << sequences << " sequences" << std::endl;
  std::cout << megabytes << " megabytes in " << time << " seconds (" << (megabytes / time) << " MB/s)" << std::endl;
  std::cout << "(build " << build_time << " s, search " << search_time << "s, sort " << sort_time << " s, merge " << merge_time << " s)" << std::endl;
  std::cout << "Time: "<<time<<std::endl;
  std::cout << std::endl;

  delete rlcsa;
  return 0;
}


int
main(int argc, char** argv)
{
  std::cout << "Line-by-line RLCSA builder" << std::endl;
  if(argc < 3)
  {
    std::cout << "Usage: build_rlcsa base_name buffer_size [block_size [sample_rate]]" << std::endl;
    return 1;
  }
  std::cout << std::endl;

  int name_arg = 1, buffer_arg = 2, block_arg = 3, sample_arg = 4;
  usint block_size = (argc > block_arg ? atoi(argv[block_arg]) : RLCSA_BLOCK_SIZE.second);
  usint sample_rate = (argc > sample_arg ? atoi(argv[sample_arg]) : SAMPLE_RATE.second);
  return wholeFileRLCSA(argv[name_arg], block_size, sample_rate, atoi(argv[buffer_arg]));
}
