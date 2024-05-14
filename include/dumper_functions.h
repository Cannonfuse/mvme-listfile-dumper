#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <filesystem>
#include "listfile.h"
#include "MVMEmodule.h"

namespace fs = std::filesystem;

#ifndef DUMPERFUNCS
#define DUMPERFUNCS

static std::string read_string_data(std::istream &infile, size_t bytes);

int check_listfile(std::filesystem::path path_to_listfile);

void listfile_preprocessing(std::istream &infile, fs::path TheFilename, std::map<uint32_t, listfile> listfile_version_map, std::map<uint32_t, MVMEmodule> module_map, bool useR65Hack);

void process_listfile(std::istream &infile, listfile TheListfile, std::map<uint32_t, MVMEmodule> module_map, bool useR65Hack);

void build_root_file(std::istream &infile, listfile &TheListfile, bool useR65Hack);

// Prints a status bar when loading large amounts of data
void printProgress(double percentage);

template <class T>
std::vector<T> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template <>
std::vector<mdpp16_qdc> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template<>
std::vector<mdpp16_scp> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template<>
std::vector<mdpp16_rcp> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template<>
std::vector<mdpp16_padc> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template <>
std::vector<mdpp16_csi> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template <>
std::vector<mdpp32_qdc> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template<>
std::vector<mdpp32_scp> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template<>
std::vector<mdpp32_padc> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template<>
std::vector<madc32> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template<>
std::vector<mqdc32> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template<>
std::vector<mtdc32> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template<>
std::vector<vmmr> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);

template<>
std::vector<mdi2> sortData(std::vector<uint32_t> subEventData, MVMEmodule module);


#endif