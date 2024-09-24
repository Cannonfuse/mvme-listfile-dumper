/* mvme-listfile-dumper - format mvme listfile content and print it to stdout
 *
 * Copyright (C) 2017  Florian Lüke <f.lueke@mesytec.com>
 * Copyright (C) 2023-2024 Justin Warren <jw402216@ohio.edu>
 * 
 *
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <tuple>
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include <zlib.h>
#include <zip.h>
#include <filesystem>
#include "dumper_functions.h"
#include "dumper_constants.h"
#include "listfile.h"
#include "MVMEmodule.h"


namespace fs = std::filesystem;

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char *argv[])
{

    // Enable ROOT implicit multithreading to speed up certain functions. Uses up to half of system cores
    // On second thought dont do this. Any speedups are inconsequential and often it slows everything down.
    // ROOT::EnableImplicitMT((uint32_t)(sysconf(_SC_NPROCESSORS_CONF)/2));

    bool useHack{false}, useSeparateDirectory{false}, has_h_option{false}, has_w_option{false};
    fs::path writeFileDirectory;


    for(auto i = 0; i < argc; ++i)
    {
        if(argv[i][0] == '-' and not(useHack) and not(useSeparateDirectory))
        {
            std::string options = argv[i];
            std::transform(options.begin(),options.end(),options.begin(),[](unsigned char c){return std::tolower(c);});
            // printf("%s, %u, %u\n", options.c_str(), (int)options.find_first_of("h"), (int)options.find_first_of("w"));
            has_h_option = ((int)options.find_first_of("h") > 0);
            has_w_option = ((int)options.find_first_of("w") > 0);

        }

        {
            if(has_h_option and not(useHack))
            {
                useHack = true;
                printf("Using R65 hack.\n");

            }
        }

        if(has_w_option and not(useSeparateDirectory))
        {
            if(i+1 != argc)
            {
                useSeparateDirectory = true;
                writeFileDirectory = fs::absolute(argv[i+1]);
                printf("Output files will be written to %s.\n",writeFileDirectory.c_str());
            }
            else
            {
                cerr << "If using the -w option you must provide a directory to write the result to." << endl;
                return 1;
            }
        }
        // printf("%i = %s\n",i,argv[i]);
    }


    fs::path listfile_templates_dir = (std::string)installpath + "/share/templates/listfile";
    fs::path modules_templates_dir = (std::string)installpath + "/share/templates/modules";

    std::map<uint32_t, listfile> listfile_version_map;
    // Fill the listfile map
    for (const auto & entry : fs::directory_iterator(listfile_templates_dir))
    {
        listfile temporary_listfile;
        // printf("%s\n",entry.path().c_str());
        temporary_listfile.setTemplate(entry.path().filename());
        listfile_version_map.insert(std::pair<uint32_t, listfile>(temporary_listfile.getVersion(),temporary_listfile));
    }

    std::map<uint32_t, MVMEmodule> module_map;
    // Fill the module map
    for (const auto & entry : fs::directory_iterator(modules_templates_dir))
    {
        MVMEmodule temporary_module;
        // printf("%s\n",entry.path().c_str());
        temporary_module.setTemplate(entry.path().filename());
        module_map.insert(std::pair<uint32_t, MVMEmodule>((uint32_t)temporary_module.getModuleType(),temporary_module));
        // temporary_module.printModuleSetting();

    }

    // MVMEmodule testModule(8);

    // testModule.printModuleSetting();

    // This program is modified
    
    // Throw error if no input file to open was provided
    if (not(useHack) and not(useSeparateDirectory) and argc != 2)
    {
        cerr << "Invalid number of arguments" << endl;
        cerr << "Usage: " << argv[0] << " <listfile>" << endl;
        return 1;
    }

    // Get the absolute current path with the filename and without the filename
    fs::path the_current_directory = argv[argc-1];
    fs::path the_absolute_path_with_file = fs::absolute(the_current_directory);
    fs::path the_absolute_path = the_absolute_path_with_file.parent_path();
    fs::path the_filename = the_current_directory.filename();


    // Declare some useful variables
    std::string file_in_zip_archive_name; // the name of the current file
    int position_of_file_type{0}; // if we have a MVME listfile, store the position
    auto the_file_to_read_position{-1}; // the index of the file I want to read
    size_t file_size_in_bytes{0}; // the size of the file in bytes we are processing
    size_t file_index{0}; // the index (location) of the file in the ZIP archive

    // Open the zip archive, if it fails, then return
    int zip_file_open_err_code = 0; // stores error code from opening zip file if any
    zip *mvme_archive_to_open = zip_open(the_absolute_path_with_file.c_str(), ZIP_RDONLY, &zip_file_open_err_code); // the zip file we want to open
    int numfiles = zip_get_num_entries(mvme_archive_to_open,0); // the number of files in the archive

    // This code checks to see if the file is a uncompressed listfile or a compressed MVME archive
    bool is_list_file = false;
    if(zip_file_open_err_code == ZIP_ER_NOZIP)
    {
        printf("Checking to see if file is a .mvmelst file.\n");
        int check_file = check_listfile(the_absolute_path_with_file);
        if(check_file >= 0 && check_file <= 2)
        {
            is_list_file = true;
            printf("Processing MVME listfile.\n");
        }
        // If the file ISNT an MVME listfile, the return
        else
        {
            cerr << "File is invalid. Please check file." << endl;
            return 1;
        }
    }
    // Throw error if the file is an invalid archive
    else if(zip_file_open_err_code != 0)
    {
        cerr << "Error opening archive." << endl;
        return 1;
    }

    if(!is_list_file)
    {
        for(auto i = 0; i < numfiles; i++)
        {
            file_in_zip_archive_name = zip_get_name(mvme_archive_to_open,i,0); // get the name of the file in the archive
            position_of_file_type = file_in_zip_archive_name.find("mvmelst",0); // check to see if it is a MVME listfile
            if(position_of_file_type >= 0) 
            {
                the_file_to_read_position = i; // Store the index of the MVME listfile in the zip file
                file_index = i;
                struct zip_stat st;
                zip_stat_init(&st);
                zip_stat(mvme_archive_to_open, file_in_zip_archive_name.c_str(), 0, &st);
                file_size_in_bytes = st.size;
                if(useSeparateDirectory)
                {

                    std::string printWriteDirectory = writeFileDirectory;
                    if(printWriteDirectory.at(printWriteDirectory.size()-1) != '/')
                    {
                        printWriteDirectory += "/";
                    }
                    printWriteDirectory += (std::string)the_filename.replace_extension(".root");
                    printf("Success, MVME listfile found in archive: %s\nROOT file name will be: %s\nFile size is %lu bytes\n",
                    file_in_zip_archive_name.c_str(),printWriteDirectory.c_str(), file_size_in_bytes);
                }
                else
                {
                    printf("Success, MVME listfile found in archive: %s\nROOT file name will be: %s\nFile size is %lu bytes\n",
                    file_in_zip_archive_name.c_str(),the_absolute_path_with_file.replace_extension(".root").c_str(), file_size_in_bytes);
                }
            }
        }

        // Throw error if we didn't get the right kind of zip archive
        if(the_file_to_read_position < 0)
        {
            cerr << "ZIP archive does not contain an MVME listfile" << endl;
            cerr << "Please use a proper MVME listfile archive." << endl;
            return 1;
        }

        // If we have a file but it has no size, exit the program
        if(file_size_in_bytes == 0)
        {
            cerr << "Listfile in MVME listfile archive has size 0 bytes" << endl;
            cerr << "Filename: " << the_absolute_path_with_file.c_str() << endl;
            return 1;
        }

        // If the decompressed file will be larger than 1 GB, 
        // we actually want to decompress it to the disk
        // in a temporary folder and then perform the operation.
        bool is2BIG = (file_size_in_bytes > maxfilesize);

        if(is2BIG)
        {
            // Get a path to the OS temporary directory
            fs::path tempDir = fs::temp_directory_path();
            // Set the filename we want to use
            tempDir += "/temp.mvmelst";
            // Create a ofstream for writing the file and
            // open it in binary mode
            std::ofstream tempFile;
            tempFile.open(tempDir,std::ios::binary);


            printf("Uncompressed file is larger than %u GB. Processing data from disk.\n",maxfilesize/1000000000);

            // Check to see if we actually opened the file. 
            // If not, exit program
            if(tempFile.is_open())
            {
                printf("Temporary file opened. File is stored at %s.\n",fs::absolute(tempDir).c_str());
            }
            else
            {
                printf("Temporary file has not been opened. Bye\n");
                return 1;

            }

            // This will store the data in memory as it is
            // loaded from the zip archive
            char *listfilecontents = new char[maxread];
            // Open the file of interest in the zip archive
            zip_file *file_in_archive = zip_fopen_index(mvme_archive_to_open, file_index, 0);

            // The total bytes of the decompressed file written to disk
            size_t totalbyteswritten{0};
            // The bytes that we read from the compressed file
            zip_int64_t readbytes{0};
            // Decompress the file, and write the bytes to the disk
            while(totalbyteswritten < file_size_in_bytes)
            {
                // printf("Total bytes written: %lu\n",totalbyteswritten);
                if(totalbyteswritten == 0) 
                {
                    printProgress((double)totalbyteswritten/(double)file_size_in_bytes);
                }

                // Get the file data from the archive
                readbytes = zip_fread(file_in_archive, listfilecontents, maxread);
                // Check to see if we had a valid read of the file in the 
                // zip archive and if we didnt exit the program
                if(readbytes < 0)
                {
                    printf("Invalid read of file in zip archive. Bye\n");
                    return 1;
                }
                // Write the data to the disk
                tempFile.write(listfilecontents,readbytes);
                // Count the number of bytes
                totalbyteswritten += readbytes;
                // Display the progress concerning the
                // making of the temporary file
                printProgress((double)totalbyteswritten/(double)file_size_in_bytes);
                readbytes = 0;
            }



            // Close the file in the archive
            zip_fclose(file_in_archive);

            // And close the zip archive
            zip_close(mvme_archive_to_open);

            // Close the temporary file
            tempFile.close();

            // Delete array from memory
            delete[] listfilecontents;
            
            // The ifstream that will read from the temporary file
            std::ifstream listfile_input_to_buffer;
            // Open the temporary file
            listfile_input_to_buffer.open(fs::absolute(tempDir));
            // Make a read buffer. This buffer allows the use of istream methods,
            // which are generalized stream inputs. Allows the use of only one
            // function type with no overloads to do this conversion
            std::streambuf *listfile_buffer = listfile_input_to_buffer.rdbuf();
            // Make the istream
            std::istream input_to_preprocessor(listfile_buffer);

            // Send the listfile istream to the preprocessor. Also
            // set the file name that the .root file will use.
            if(useSeparateDirectory)
            {
                writeFileDirectory += the_filename;
                writeFileDirectory.replace_extension(".root");
                listfile_preprocessing(input_to_preprocessor, writeFileDirectory, listfile_version_map, module_map, useHack);
            }
            else
            {
                listfile_preprocessing(input_to_preprocessor, the_absolute_path_with_file.replace_extension(".root"), listfile_version_map, module_map, useHack);
            }
        }
        else
        {
            printf("Uncompressed file is smaller than 4 GB. Processing data from memory.\n");
            // This will store the data in memory as it is
            // loaded from the zip archive
            char *listfilecontents = new char[file_size_in_bytes];
            // Open the file of interest in the zip archive
            zip_file *file_in_archive = zip_fopen_index(mvme_archive_to_open, file_index, 0);
            // Get the file data from the archive
            zip_int64_t readbytes = zip_fread(file_in_archive, listfilecontents, file_size_in_bytes);
            // Check to see if we had a valid read of the file in the 
            // zip archive and if we didnt exit the program
            if(readbytes != file_size_in_bytes)
            {
                printf("File was not read completely. \n");
                return 1;
            }




            zip_fclose(file_in_archive);



            // And close the archive
            zip_close(mvme_archive_to_open);

            // Convert the data to a string. This ensures that
            // the istringstream to follows will read the correct
            // size for the data.
            std::string listfiledata(listfilecontents,file_size_in_bytes);
            delete[] listfilecontents;
            // The istringstream that will read from memory
            std::istringstream listfile_input_to_buffer(listfiledata);
            // Make a read buffer. This buffer allows the use of istream methods,
            // which are generalized stream inputs. Allows the use of only one
            // function type with no overloads to do this conversion
            std::streambuf *listfile_buffer = listfile_input_to_buffer.rdbuf();
            // Make the istream
            std::istream input_to_preprocessor(listfile_buffer);
            // Send the listfile istream to the preprocessor. Also
            // set the file name that the .root file will use.
            if(useSeparateDirectory)
            {
                writeFileDirectory += the_filename;
                writeFileDirectory.replace_extension(".root");
                listfile_preprocessing(input_to_preprocessor, writeFileDirectory, listfile_version_map, module_map, useHack);
            }
            else
            {
                listfile_preprocessing(input_to_preprocessor, the_absolute_path_with_file.replace_extension(".root"), listfile_version_map, module_map, useHack);
            }
            // printf("Made it here\n");
            // listfile_preprocessing(input_to_preprocessor, the_absolute_path_with_file.replace_extension(".root"), listfile_version_map, module_map);
        }
    }

    else if(is_list_file)
    {
        // The ifstream that will read from disk
        // In this case, the file is a .mvmelst already
        // written to disk
        std::ifstream listfile_input_to_buffer;
        // Open the .mvmelist file
        listfile_input_to_buffer.open(the_absolute_path_with_file);
        // Make a read buffer. This buffer allows the use of istream methods,
        // which are generalized stream inputs. Allows the use of only one
        // function type with no overloads to do this conversion
        std::streambuf *listfile_buffer = listfile_input_to_buffer.rdbuf();
        // Make the istream
        std::istream input_to_preprocessor(listfile_buffer);
        // Send the listfile istream to the preprocessor. Also
        // set the file name that the .root file will use.
        if(useSeparateDirectory)
        {
            writeFileDirectory += the_filename;
            writeFileDirectory.replace_extension(".root");
            listfile_preprocessing(input_to_preprocessor, writeFileDirectory, listfile_version_map, module_map, useHack);
        }
        else
        {
            listfile_preprocessing(input_to_preprocessor, the_absolute_path_with_file.replace_extension(".root"), listfile_version_map, module_map, useHack);
        }
    }

    return 0;
}

