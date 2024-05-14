#include "dumper_functions.h"
#include "dumper_constants.h"
#include <cstring>
#include <vector>
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include <zlib.h>
#include <zip.h>

namespace fs = std::filesystem;

/*
template <>
std::vector<mdpp16_qdc> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    // for(auto i = 0; i < subEventData.size(); i++)
    // {
    //     printf("subEventData.at(%i) = 0x%x\n",i,subEventData.at(i));
    // }
    // printf("subEventData.size() = %lu\n",subEventData.size());
    std::vector<mdpp16_qdc> module_to_return;
    mdpp16_qdc temporary_module;

    std::vector<uint32_t> ADC, SHORT, LONG, TOF, ROUTE_EVT;
    std::vector<bool> SHORT_OVERFLOW, LONG_OVERFLOW, PILEUP, PA_OVERFLOW, OVERFLOW, OutOfRange;
    uint32_t ID{0xffffffff}, TRIGGER{0xffffffff}, ROUTE_TRIGGER{0xffffffff}, ADC_RES{0xffffffff}, TDC_RES{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool overflowflag{false}, pileupflag{false}, lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
            // isheader = ((0xf0000000 & subEventData.at(i)) >> 28 == 4);
            // istimestamp = ((0xc0000000 & subEventData.at(i)) >> 30 == 3);
            // ishightimestamp = ((0xf0000000 & subEventData.at(i)) >> 28 == 2);
            // isdata = ((0xf0000000 & subEventData.at(i)) >> 28 == 1);
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
            ADC_RES = (module.getADC_RES_Shift() & subEventData.at(i)) >> module.getADC_RES_Shift();
            TDC_RES = (module.getTDC_RES_Mask() & subEventData.at(i)) >> module.getTDC_RES_Shift();
            // uint32_t numwordfollow = (0x000003ff & subEventData.at(i));
            // printf("ModuleID = %u, word # = %u\n",moduleID,i);
        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
            // TIMESTAMP = TIMESTAMP | (module.getLowTimestamp_Mask() & subEventData.at(i));       
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i)) << module.getHighTimestamp_Shift();
            // TIMESTAMP = TIMESTAMP | hights;
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));// > module.getData_Shift();
            overflowflag = (module.getOverflow_Mask() & subEventData.at(i)) > 0;
            switch(module.getModuleType())
            {
                case VMEModuleType::MDPP16_QDC:
                {
                    if(channel >=16 && channel <=31)
                    {
                        TOF.push_back(data);
                        
                    }
                    else if(channel >=48 && channel <=63)
                    {
                        SHORT.push_back(data);
                        SHORT_OVERFLOW.push_back(overflowflag);

                    }
                    else if(channel >= 0 && channel <= 15)
                    {
                        LONG.push_back(data);
                        LONG_OVERFLOW.push_back(overflowflag);
                        ROUTE_EVT.push_back(channel);
                    }
                    else if(channel == 32 || channel == 33)
                    {

                        TRIGGER = data;
                        ROUTE_TRIGGER = channel-32;
                    }
                    break;
                }
            }

        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }

    // if(TRIGGER > 65536)
    // {
    //     printf("\n\n\n\n\n\n\nFROM ASSIGNMENT LOOP: TRIGGER = %u\n",TRIGGER);
    //     for(size_t i = 0; i < subEventData.size(); ++i)
    //     {
    //         printf("subEventData.at(%lu) = 0x%08x\n",i,subEventData.at(i));
    //     }
    // }
    // printf("LONG.size() = %lu\n",LONG.size());
    for(auto i = 0; i < LONG.size(); ++i)
    {
            switch(module.getModuleType())
            {
                case VMEModuleType::MDPP16_QDC:
                {
                    temporary_module.ID = ID;
                    temporary_module.ADC_RES = ADC_RES;
                    temporary_module.TDC_RES = TDC_RES;
                    temporary_module.SHORT = SHORT.at(i);
                    temporary_module.SHORT_OVERFLOW = SHORT_OVERFLOW.at(i);
                    temporary_module.LONG = LONG.at(i);
                    temporary_module.LONG_OVERFLOW = LONG_OVERFLOW.at(i);
                    temporary_module.TOF = TOF.at(i);
                    temporary_module.ROUTE_EVT = ROUTE_EVT.at(i);
                    temporary_module.TRIGGER = TRIGGER;
                    temporary_module.ROUTE_TRIGGER = ROUTE_TRIGGER;
                    temporary_module.TIMESTAMP = TIMESTAMP;
                    module_to_return.push_back(temporary_module);
                    break;
                }
            }

    }

    return module_to_return;
}
*/

int check_listfile(fs::path path_to_listfile)
{

    std::ifstream file_to_check;

    // The version of the file that will be read
    uint32_t fileVersion = 0;

    // Some stock listfiles to check against
    listfile version0;
    listfile version1;
    listfile version2;

    version0.setTemplate("v0listfile.json");
    version1.setTemplate("v1listfile.json");
    version2.setTemplate("v2listfile.json");

    // Read the fourCC that's at the start of listfiles from version 1 and up.
    const size_t bytesToRead = 4;
    char fourCC[bytesToRead] = {};

    try
    {
        file_to_check.open(path_to_listfile, std::ios::binary);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error open listfile to test: " << e.what() << std::endl;
        file_to_check.close();
        return -1;
    }

    // Read the first 4 bytes
    file_to_check.read(fourCC, bytesToRead);

    // Check to see if MVLC listfile
    int MVLC_listfile = std::strncmp(fourCC, "MVLC", bytesToRead);

    // Check fourCC against v1 and v2 listfiles
    int compared_listfile = std::strncmp(fourCC, version1.getFourCC().c_str(), bytesToRead);
    compared_listfile += std::strncmp(fourCC, version2.getFourCC().c_str(), bytesToRead);

    // Check if we have one of the MVLC files (8 magic bytes, either MVLC_ETH
    // or MVLC_USB. Those are not supported by this tool. Parsing the formats
    // needs a lot more work (packet loss, continuation frames, etc).
    if (MVLC_listfile == 0)
    {
        throw std::runtime_error("Detected MVLC listfile format which is not supported by the listfile-dumper.");
    }
    else if(compared_listfile == 0)
    {
        // Read the next 4 bytes
        file_to_check.read(reinterpret_cast<char *>(&fileVersion), sizeof(fileVersion));
        file_to_check.close();
        return fileVersion;
    }

    file_to_check.close();
    return -1;


}

static std::string read_string_data(std::istream &infile, size_t bytes)
{
    std::vector<char> buffer(bytes);
    infile.read(buffer.data(), buffer.size());
    std::string result(buffer.begin(), buffer.end());
    return result;;
}

void listfile_preprocessing(std::istream &infile, fs::path TheFilename, std::map<uint32_t, listfile> listfile_version_map, std::map<uint32_t, MVMEmodule> module_map, bool useR65Hack = false)
{
    // Some stock listfiles to check against
    listfile version0;
    listfile version1;
    listfile version2;

    version0.setTemplate("v0listfile.json");
    version1.setTemplate("v1listfile.json");
    version2.setTemplate("v2listfile.json");


    // Set the file name to use
    version0.setFilename(TheFilename.c_str());
    version1.setFilename(TheFilename.c_str());
    version2.setFilename(TheFilename.c_str());


    uint32_t fileVersion = 0;

    // Read the fourCC that's at the start of listfiles from version 1 and up.
    const size_t bytesToRead = 4;
    char fourCC[bytesToRead] = {};

    infile.read(fourCC, bytesToRead);

    // Check to see if MVLC listfile
    int MVLC_listfile = std::strncmp(fourCC, "MVLC", bytesToRead);

    // Check fourCC against v1 and v2 listfiles
    int compared_listfile = std::strncmp(fourCC, version1.getFourCC().c_str(), bytesToRead);
    compared_listfile += std::strncmp(fourCC, version2.getFourCC().c_str(), bytesToRead);

    // Check if we have one of the MVLC files (8 magic bytes, either MVLC_ETH
    // or MVLC_USB. Those are not supported by this tool. Parsing the formats
    // needs a lot more work (packet loss, continuation frames, etc).
    if (MVLC_listfile == 0)
    {
        throw std::runtime_error("Detected MVLC listfile format which is not supported by the listfile-dumper.");
    }
    else if(compared_listfile == 0)
    {
        // Read the next 4 bytes
        infile.read(reinterpret_cast<char *>(&fileVersion), sizeof(fileVersion));
    }

    // Move to the start of the first section
    auto firstSectionOffset = (fileVersion == 0) 
                            ? version0.getFirstSectionOffset()
                            : (fileVersion == 1)
                            ? version1.getFirstSectionOffset()
                            : version2.getFirstSectionOffset();

    infile.seekg(firstSectionOffset, std::istream::beg);

    printf("\n%i\n",firstSectionOffset);

    if (fileVersion == 0)
    {
        printf("MVME listfile version %u.\n",fileVersion);
        build_root_file(infile,version0, useR65Hack);
        infile.seekg(firstSectionOffset, std::istream::beg);
        process_listfile(infile, version0, module_map, useR65Hack);
    }
    else if(fileVersion == 1)
    {
        printf("MVME listfile version %u.\n",fileVersion);
        build_root_file(infile,version1, useR65Hack);
        infile.seekg(firstSectionOffset, std::istream::beg);
        process_listfile(infile, version1, module_map, useR65Hack);
    }
    else if(fileVersion == 2)
    {        
        printf("MVME listfile version %u.\n",fileVersion);
        build_root_file(infile,version2, useR65Hack);
        infile.seekg(firstSectionOffset, std::istream::beg);
        process_listfile(infile, version2, module_map, useR65Hack);
    }
}

void process_listfile(std::istream &infile, listfile TheListfile, std::map<uint32_t, MVMEmodule> module_map, bool useR65Hack)
{
    bool continueReading = true;
    uint32_t subEventHeader,subEventSize,moduleType;
    auto roofile=TFile::Open(TheListfile.getFilename().c_str(),"update");
    std::map<uint32_t, TTree *> moduleIDtoTTreeMap;

    int iFilled = 0;


    // A pointer is necessary for TTree and TFile to properly write data
    // Creating pointers for the necessary data type saves from
    // constantly needing to call malloc() and free the memory
    // afterwards.

    mdpp16_qdc *fillmodule_mdpp16_qdc = new mdpp16_qdc;
    mdpp16_scp *fillmodule_mdpp16_scp = new mdpp16_scp;
    mdpp16_rcp *fillmodule_mdpp16_rcp = new mdpp16_rcp;
    mdpp16_padc *fillmodule_mdpp16_padc = new mdpp16_padc;
    mdpp16_csi *fillmodule_mdpp16_csi = new mdpp16_csi;
    mdpp32_qdc *fillmodule_mdpp32_qdc = new mdpp32_qdc;
    mdpp32_scp *fillmodule_mdpp32_scp = new mdpp32_scp;
    mdpp32_padc *fillmodule_mdpp32_padc = new mdpp32_padc;
    mtdc32 *fillmodule_mtdc32 = new mtdc32;
    madc32 *fillmodule_madc32 = new madc32;
    mqdc32 *fillmodule_mqdc32 = new mqdc32;
    vmmr *fillmodule_vmmr = new vmmr;
    mdi2 *fillmodule_mdi2 = new mdi2;


    // This gets passed to the various sortData() functions and contains the module type/masks/shifts the data
    // currently describes
    MVMEmodule currentmodule;

    // This map contains the unique module ID, unique module name, and the VMEModuleType
    // Used for selecting the proper TTree to write to, 
    std::map<uint32_t, std::tuple<std::string,VMEModuleType>> ModulesInFile = TheListfile.getTreename();

    // Print off the modules that occupy the VME crate(s)
    {
        uint32_t ssize = 90;
        std::string separator,mused;
        separator.resize(ssize);
        std::fill(separator.begin(), separator.end(), '-');
        mused.resize(ssize);
        std::fill(mused.begin(), mused.end(), ' ');
        std::string musedtext = "MODULES USED IN LISTFILE";
        uint32_t centerstring = ssize/2 - musedtext.size()/2;
        mused = mused.replace(centerstring,musedtext.size(),musedtext);
        printf("\n\n\n%s\n%s\n%s\n%s\n%s\n",separator.c_str(),separator.c_str(),mused.c_str(),separator.c_str(),separator.c_str());
        for (std::map<uint32_t, std::tuple<std::string,VMEModuleType>>::iterator it=ModulesInFile.begin(); it!=ModulesInFile.end(); ++it)
        {
            printf("Module ID =  <%u>, Module TTree name = <%s>\n",it->first,std::get<0>(it->second).c_str());
        }
        printf("%s\n%s\n\n\n",separator.c_str(),separator.c_str());
    }

    // The total # of individual events that have been processed.
    // The definition of event is data corresponding to a single hit on a single input
    uint32_t total_events = 0;

    // The total runtime covered in the current file
    // Corresponds to the the actual runtime of the DAQ.
    // Ex: 1000 seconds data collection, 0 <= currentSecondInRun <= 1000
    uint64_t currentSecondInRun = 0;

    // This is the VME crate for the specific event
    // Only relevant to listfiles with version >= 2
    uint32_t crateIndex = 0;

    // This is the event type from the main header
    uint32_t eventType = 0;

    // Define a data store for the section header
    uint32_t sectionHeader = 0;

    // This is the section type from the main event header
    uint32_t sectionType = 0;

    // The section size of the event
    uint32_t sectionSize = 0;

    // The total size in bytes of the section 
    uint32_t sectionBytes = 0;

    // Print the progress
    printProgress((double)currentSecondInRun/(double)TheListfile.getTimeticks());

    while (continueReading)
    {
        // Read the section header from istream. Section headers/types are defined in the MVME source code/
        infile.read((char *)&sectionHeader, sizeof(uint32_t));


        sectionType   = (sectionHeader & TheListfile.getSectionTypeMask()) >> TheListfile.getSectionTypeShift();
        sectionSize   = (sectionHeader & TheListfile.getSectionSizeMask()) >> TheListfile.getSectionSizeShift();
        sectionBytes = sectionSize * sizeof(uint32_t);

        // printf("sectionHeader = %x, SectionType = %x, SectionSize = %x\n",sectionHeader,sectionType,sectionSize);

        // progress = static_cast<double>(infile.tellg)/filesize;
        // printProgress(progress);

        switch ((SectionType)sectionType)
        {
            case listfile::SectionType_Config:
                {
                    std::string str = read_string_data(infile, sectionBytes);

                    // std::cout << "Config section of size " << sectionSize
                    //     << " words, " << sectionBytes << " bytes." << std::endl
                    //     << "First 1000 characters:" << std::endl;
                    // std::cout << str << std::endl << "---" << std::endl;
                } break;

            case listfile::SectionType_Event:
                {
                    // Read ALL of the section data into memory. 
                    uint32_t *rawEventData = (uint32_t *)malloc((sectionSize) * sizeof(uint32_t));
                    infile.read((char *)rawEventData, (sectionSize)*sizeof(uint32_t));

                    // Build a std::vector to hold the data the was read into memory
                    std::vector<uint32_t> EventData(rawEventData,rawEventData+sectionSize);

                    // Delete the rawEventData to prevent memory leaks
                    delete(rawEventData);


                    // for(auto i = 0; i < EventData.size(); ++i)
                    // {
                    //     printf("EventData.at(%i), %x\n",i,EventData.at(i));
                    // }

                    // A crateIndex of 0xffffffff indicates that this data is unusable  when processing,
                    // that is the listfile was a v.0 or v.1 listfile.
                    crateIndex = 0xffffffff;
                    // Get the eventType (really the event #) from the section header
                    eventType = (sectionHeader & TheListfile.getEventTypeMask()) >> TheListfile.getEventTypeShift();
                    // Only set a crate index if the listfile is v.2 or greater
                    if(TheListfile.getVersion() >= 2)
                    {
                        // Get the crateIndex from the section header
                        crateIndex = (sectionHeader & TheListfile.getCrateIndexMask()) >> TheListfile.getCrateIndexShift();
                    }

                    std::vector<std::vector<uint32_t>> Events;

                    for(auto i = 0; i < EventData.size(); ++i)
                    {
                        // Read the sub event header, containing the subevent section size and 
                        // the module type as a integer
                        subEventHeader = EventData.at(i);
                        // infile.read((char *)&subEventHeader, sizeof(uint32_t));

                        if(subEventHeader != 0x87654321)
                        {

                            // // This is the module type, determines how the data will be processed
                            // moduleType = (subEventHeader & TheListfile.getModuleTypeMask()) >> TheListfile.getModuleTypeShift();

                            // This is the sub event section size, how much data is in the subevent
                            subEventSize = (subEventHeader & TheListfile.getSubEventSizeMask()) >> TheListfile.getSubEventSizeShift();
                            
                            std::vector<uint32_t> event(EventData.begin()+i,EventData.begin()+i+subEventSize);
                            
                            // std::cout<<event.size() << std::endl;

                            Events.push_back(event);
                            i += subEventSize;
                        }
                    }

                    for(auto i = 0; i < Events.size(); ++i)
                    {
                        // Read the sub event header, containing the subevent section size and 
                        // the module type as a integer
                        subEventHeader = Events.at(i).at(0);
                        // std::cout << Events.at(i).size() << std::endl;
                        std::vector<uint32_t> subEventData(Events.at(i).begin()+1,Events.at(i).end()); 
                        // std::cout << subEventData.size() << std::endl;
                        // This is the module type, determines how the data will be processed
                        moduleType = (subEventHeader & TheListfile.getModuleTypeMask()) >> TheListfile.getModuleTypeShift();

                        // Set the current module that for the data type
                        // The try-catch is to check if the moduleType is available in the modulemap
                        // If it isn't, the code will need to modified to add it to the available types if it is
                        // a new module, otherwise the templates need to be checked.
                        try
                        {
                            currentmodule = module_map.at(moduleType);
                        }
                        catch(std::out_of_range &e)
                        {   
                            std::string badmoduletype = "\n\n\n\n\nModule type <" + std::to_string(moduleType) + "> mismatch, module type not found in modulemap, error " + e.what() + "\n";
                            // printf("\n\n\n\n\nModule type <%u> mismatch, module type not found in modulemap, error %s\n",moduleType,e.what());
                            for (std::map<uint32_t,MVMEmodule>::iterator it=module_map.begin(); it!=module_map.end(); ++it)
                            {
                                badmoduletype.append("Available module: name = <" + it->second.getModuleName() + ">, type = <" + std::to_string((uint32_t)it->second.getModuleType()) + ">.\n");
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            }
                            badmoduletype.append("Module type out of range. Check available module templates.\n");
                            throw std::out_of_range(badmoduletype);
                        }
                        
                        // Write the crateIndex and eventType to the module file, after the proper module has been set
                        currentmodule.setEventIndex(eventType);
                        currentmodule.setCrateIndex(crateIndex);


                        // Get the unique module ID from the module settings header in the vector
                        uint32_t moduleID= (subEventData.at(0) & currentmodule.getID_Mask()) >> currentmodule.getID_Shift();
                        if(useR65Hack)
                        {
                            moduleID += 1000*moduleType;
                        }

                        // printf("  subEventHeader=0x%08x, moduleType=%u (%s), subEventSize=%u\n",
                        //     subEventHeader, moduleType, currentmodule.getModuleName().c_str(),
                        //     subEventSize);

                        // printf("subEventData.at(0) = %X, ModuleID = %u\n", subEventData.at(0), moduleID);


                        switch(currentmodule.getModuleType())
                        {
                            case VMEModuleType::MDPP16_QDC:
                            {
                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<mdpp16_qdc>(subEventData, currentmodule);


                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                }


                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_qdc);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_mdpp16_qdc = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;

                                }
                                break;
                            }
                            case VMEModuleType::MDPP16_SCP:
                            {
                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<mdpp16_scp>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_scp);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_mdpp16_scp = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;
                            }
                            case VMEModuleType::MDPP16_RCP:
                            {
                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<mdpp16_rcp>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_rcp);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_mdpp16_rcp = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;

                            }
                            case VMEModuleType::MDPP16_PADC:
                            {
                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<mdpp16_padc>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_padc);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_mdpp16_padc = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;

                            }
                            case VMEModuleType::MDPP16_CSI:
                            {
                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<mdpp16_csi>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_csi);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_mdpp16_csi = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;

                            }
                            case VMEModuleType::MDPP32_QDC:
                            {
                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<mdpp32_qdc>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_mdpp32_qdc);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_mdpp32_qdc = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;
                            }
                            case VMEModuleType::MDPP32_SCP:
                            {
                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<mdpp32_scp>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_mdpp32_scp);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_mdpp32_scp = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;
                            }
                            case VMEModuleType::MDPP32_PADC:
                            {
                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<mdpp32_padc>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_mdpp32_padc);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_mdpp32_padc = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;

                            }
                            case VMEModuleType::MTDC32:
                            {

                                // for(auto l = 0; l < subEventData.size(); ++l)
                                // {printf("subEventData.at(%i) = %x\n",l,subEventData.at(l));}

                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<mtdc32>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_mtdc32);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_mtdc32 = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;
                            }
                            case VMEModuleType::MADC32:
                            {
                                // for(auto l = 0; l < subEventData.size(); ++l)
                                // {printf("subEventData.at(%i) = %x\n",l,subEventData.at(l));}

                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<madc32>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // printf("%s\n",TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_madc32);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_madc32 = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;
                            }
                            case VMEModuleType::MQDC32:
                            {
                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<mqdc32>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_mqdc32);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_mqdc32 = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;

                            }
                            case VMEModuleType::VMMR:
                            {
                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<vmmr>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_vmmr);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_vmmr = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;
                            }
                            case VMEModuleType::VMMR_1NS:
                            {
                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<vmmr>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_vmmr);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_vmmr = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;
                            }
                            case VMEModuleType::MDI2:
                            {
                                // Sort the subEventData into a vector
                                auto modulereturndata = sortData<mdi2>(subEventData, currentmodule);

                                // Get the tree name to store the data in
                                // If the moduleID is not found, indicating an error in build_root_file, 
                                // print the moduleID with an error message.
                                std::string TreeName;
                                try
                                {
                                    TreeName = std::get<0>(ModulesInFile.at(moduleID));

                                }
                                catch(std::out_of_range &e)
                                {
                                    std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                    badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                    badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                    throw std::out_of_range(badmoduleid);
                                    // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                                };

                                // Set the tree the data will be stored in
                                TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                                // Set the branch on the TTree the data will be stored in
                                rtree->SetBranchAddress("DATA",&fillmodule_mdi2);

                                // This loop takes the sorted data, moves it to the fillmodule,
                                // and then fills the TTree and increments the total number
                                // of events that are in the data file
                                for(auto j = 0; j < modulereturndata.size(); j++)
                                {
                                    // Copy the data from the vector to the pointer
                                    fillmodule_mdi2 = &modulereturndata.at(j);

                                    // Fill the TBranch on the TTree
                                    iFilled = rtree->Fill();

                                    // Increment the total number of events
                                    // total_events++;
                                }
                                break;

                            }
                        }
                    }
                    total_events++;

                    /*
                    // Set up a block of memory equivalent in size to the amount of 32-bit words that will be read
                    uint32_t *rawSubEventData = (uint32_t *)malloc((subEventSize+1) * sizeof(uint32_t));

                    // Read the data from the istream into rawSubEventData
                    infile.read((char *)rawSubEventData, (subEventSize+1)*sizeof(uint32_t));

                    // For convenience a vector is used to pass the data that needs to be sorted to the sorting function
                    std::vector<uint32_t> subEventData(subEventSize+1);

                    // Copy the data from memory to the vector
                    std::copy(rawSubEventData, rawSubEventData + subEventSize, subEventData.begin());

                    // Set the current module that for the data type
                    // The try-catch is to check if the moduleType is available in the modulemap
                    // If it isn't, the code will need to modified to add it to the available types if it is
                    // a new module, otherwise the templates need to be checked.
                    try
                    {
                        currentmodule = module_map.at(moduleType);
                    }
                    catch(std::out_of_range &e)
                    {   
                        std::string badmoduletype = "\n\n\n\n\nModule type <" + std::to_string(moduleType) + "> mismatch, module type not found in modulemap, error " + e.what() + "\n";
                        // printf("\n\n\n\n\nModule type <%u> mismatch, module type not found in modulemap, error %s\n",moduleType,e.what());
                        for (std::map<uint32_t,MVMEmodule>::iterator it=module_map.begin(); it!=module_map.end(); ++it)
                        {
                            badmoduletype.append("Available module: name = <" + it->second.getModuleName() + ">, type = <" + std::to_string((uint32_t)it->second.getModuleType()) + ">.\n");
                            // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                        }
                        badmoduletype.append("Module type out of range. Check available module templates.\n");
                        throw std::out_of_range(badmoduletype);
                    }
                    
                    // Write the crateIndex and eventType to the module file, after the proper module has been set
                    currentmodule.setEventIndex(eventType);
                    currentmodule.setCrateIndex(crateIndex);


                    // Get the unique module ID from the module settings header in the vector
                    uint32_t moduleID= (subEventData.at(0) & currentmodule.getID_Mask()) >> currentmodule.getID_Shift();
                    moduleID += 1000*moduleType;

                    printf("  subEventHeader=0x%08x, moduleType=%u (%s), subEventSize=%u\n",
                           subEventHeader, moduleType, currentmodule.getModuleName().c_str(),
                           subEventSize);

                    // printf("subEventData.at(0) = %X, ModuleID = %u\n", subEventData.at(0), moduleID);

                    // Now delete the memory the raw data occupied to prevent memory leaks
                    delete(rawSubEventData);

                    switch(currentmodule.getModuleType())
                    {
                        case VMEModuleType::MDPP16_QDC:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp16_qdc>(subEventData, currentmodule);


                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            }


                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_qdc);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp16_qdc = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;

                            }
                            break;
                        }
                        case VMEModuleType::MDPP16_SCP:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp16_scp>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_scp);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp16_scp = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::MDPP16_RCP:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp16_rcp>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_rcp);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp16_rcp = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;

                        }
                        case VMEModuleType::MDPP16_PADC:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp16_padc>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_padc);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp16_padc = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;

                        }
                        case VMEModuleType::MDPP16_CSI:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp16_csi>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_csi);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp16_csi = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;

                        }
                        case VMEModuleType::MDPP32_QDC:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp32_qdc>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp32_qdc);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp32_qdc = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::MDPP32_SCP:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp32_scp>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp32_scp);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp32_scp = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::MDPP32_PADC:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp32_padc>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp32_padc);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp32_padc = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;

                        }
                        case VMEModuleType::MTDC32:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mtdc32>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mtdc32);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mtdc32 = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::MADC32:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<madc32>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_madc32);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_madc32 = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::MQDC32:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mqdc32>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mqdc32);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mqdc32 = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;

                        }
                        case VMEModuleType::VMMR:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<vmmr>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_vmmr);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_vmmr = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::VMMR_1NS:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<vmmr>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_vmmr);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_vmmr = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::MDI2:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdi2>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdi2);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdi2 = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;

                        }
                    }
                
                    */
                } break;

            case listfile::SectionType_Timetick:
                {
                    ++currentSecondInRun;
                    std::string str = read_string_data(infile, sectionBytes);
                    printf("\nTimetick section: %s\nSecond %lu in the DAQ run begins.\033[F\033[F\r",str.c_str(),currentSecondInRun);
                    // Prints a status bar when loading large amounts of data
                    printProgress((double)currentSecondInRun/(double)TheListfile.getTimeticks());
                    // std::cout << "Timetick section: " << str << std::endl;
                    // std::cout << "Second " << currentSecondInRun << " in the DAQ run begins" << std::endl;
                    fflush(stdout);
                } break;

            case listfile::SectionType_End:
                {
                    std::string str = read_string_data(infile, sectionBytes);
                    printf("\n\n\n\nEnd section: %s\nTotal events processed: %u.\n",str.c_str(),total_events);
                    continueReading = false;

                    auto currentFilePos = infile.tellg();
                    // infile.seekg(0, std::ifstream::end);
                    infile.seekg(0, std::istream::end);

                    auto endFilePos = infile.tellg();

                    if (currentFilePos != endFilePos)
                    {
                        std::cout << "Warning: " << (endFilePos - currentFilePos)
                            << " bytes left after Listfile End Section" << std::endl;
                    }
                } break;

            case listfile::SectionType_Pause:
                {
                    if (sectionSize == 1)
                    {
                        uint32_t value = 0;
                        infile.read((char *)&value, sizeof(value));

                        switch (static_cast<PauseAction>(value))
                        {
                            case listfile::Pause:
                                std::cout << "Pause" << std::endl;
                                break;
                            case listfile::Resume:
                                std::cout << "Resume" << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Pause section containing " << sectionSize << " words. "
                            << "Expected sectionSize to be 1 word!" << std::endl;
                        // infile.seekg(sectionBytes, std::ifstream::cur);
                        infile.seekg(sectionBytes, std::istream::cur);

                    }
                } break;

            default:
                {
                    printf("Warning: Unknown section type %u of size %u, skipping...\n",
                        sectionType, sectionSize);
                    // infile.seekg(sectionBytes, std::ifstream::cur);
                    infile.seekg(sectionBytes, std::istream::cur);

                } break;
        }
    }
    
    roofile->Write("",TObject::kOverwrite);
    roofile->Close();
}

/*
void process_listfile(fs::path infile, listfile TheListfile, std::map<uint32_t, MVMEmodule> module_map)
{
    bool continueReading = true;
    uint32_t subEventHeader,subEventSize,moduleType;
    auto roofile=TFile::Open(TheListfile.getFilename().c_str(),"update");
    std::map<uint32_t, TTree *> moduleIDtoTTreeMap;


    // A pointer is necessary for TTree and TFile to properly write data
    // Creating pointers for the necessary data type saves from
    // constantly needing to call malloc() and free the memory
    // afterwards.

    mdpp16_qdc *fillmodule_mdpp16_qdc = new mdpp16_qdc;
    mdpp16_scp *fillmodule_mdpp16_scp = new mdpp16_scp;
    mdpp16_rcp *fillmodule_mdpp16_rcp = new mdpp16_rcp;
    mdpp16_padc *fillmodule_mdpp16_padc = new mdpp16_padc;
    mdpp16_csi *fillmodule_mdpp16_csi = new mdpp16_csi;
    mdpp32_qdc *fillmodule_mdpp32_qdc = new mdpp32_qdc;
    mdpp32_scp *fillmodule_mdpp32_scp = new mdpp32_scp;
    mdpp32_padc *fillmodule_mdpp32_padc = new mdpp32_padc;
    mtdc32 *fillmodule_mtdc32 = new mtdc32;
    madc32 *fillmodule_madc32 = new madc32;
    mqdc32 *fillmodule_mqdc32 = new mqdc32;
    vmmr *fillmodule_vmmr = new vmmr;
    mdi2 *fillmodule_mdi2 = new mdi2;


    // This gets passed to the various sortData() functions and contains the module type/masks/shifts the data
    // currently describes
    MVMEmodule currentmodule;

    // This map contains the unique module ID, unique module name, and the VMEModuleType
    // Used for selecting the proper TTree to write to, 
    std::map<uint32_t, std::tuple<std::string,VMEModuleType>> ModulesInFile = TheListfile.getTreename();

    // Print off the modules that occupy the VME crate(s)
    {
        uint32_t ssize = 90;
        std::string separator,mused;
        separator.resize(ssize);
        std::fill(separator.begin(), separator.end(), '-');
        mused.resize(ssize);
        std::fill(mused.begin(), mused.end(), ' ');
        std::string musedtext = "MODULES USED IN LISTFILE";
        uint32_t centerstring = ssize/2 - musedtext.size()/2;
        mused = mused.replace(centerstring,musedtext.size(),musedtext);
        printf("\n\n\n%s\n%s\n%s\n%s\n%s\n",separator.c_str(),separator.c_str(),mused.c_str(),separator.c_str(),separator.c_str());
        for (std::map<uint32_t, std::tuple<std::string,VMEModuleType>>::iterator it=ModulesInFile.begin(); it!=ModulesInFile.end(); ++it)
        {
            printf("Module ID =  <%u>, Module TTree name = <%s>\n",it->first,std::get<0>(it->second).c_str());
        }
        printf("%s\n%s\n\n\n",separator.c_str(),separator.c_str());
    }

    // The total # of individual events that have been processed.
    // The definition of event is data corresponding to a single hit on a single input
    uint32_t total_events = 0;

    // The total runtime covered in the current file
    // Corresponds to the the actual runtime of the DAQ.
    // Ex: 1000 seconds data collection, 0 <= currentSecondInRun <= 1000
    uint64_t currentSecondInRun = 0;

    // This is the VME crate for the specific event
    // Only relevant to listfiles with version >= 2
    uint32_t crateIndex = 0;

    // This is the event type from the main header
    uint32_t eventType = 0;

    // Define a data store for the section header
    uint32_t sectionHeader = 0;

    // This is the section type from the main event header
    uint32_t sectionType = 0;

    // The section size of the event
    uint32_t sectionSize = 0;

    // The total size in bytes of the section 
    uint32_t sectionBytes = 0;

    // Print the progress
    printProgress((double)currentSecondInRun/(double)TheListfile.getTimeticks());

    while (continueReading)
    {
        // Read the section header from istream. Section headers/types are defined in the MVME source code/
        infile.read((char *)&sectionHeader, sizeof(uint32_t));


        sectionType   = (sectionHeader & TheListfile.getSectionTypeMask()) >> TheListfile.getSectionTypeShift();
        sectionSize   = (sectionHeader & TheListfile.getSectionSizeMask()) >> TheListfile.getSectionSizeShift();
        sectionBytes = sectionSize * sizeof(uint32_t);

        // progress = static_cast<double>(infile.tellg)/filesize;
        // printProgress(progress);

        switch ((SectionType)sectionType)
        {
            case listfile::SectionType_Config:
                {
                    std::string str = read_string_data(infile, sectionBytes);

                    // std::cout << "Config section of size " << sectionSize
                    //     << " words, " << sectionBytes << " bytes." << std::endl
                    //     << "First 1000 characters:" << std::endl;
                    // std::cout << str << std::endl << "---" << std::endl;
                } break;

            case listfile::SectionType_Event:
                {
                    // A crateIndex of 0xffffffff indicates that this data is unusable  when processing,
                    // that is the listfile was a v.0 or v.1 listfile.
                    crateIndex = 0xffffffff;
                    // Get the eventType (really the event #) from the section header
                    eventType = (sectionHeader & TheListfile.getEventTypeMask()) >> TheListfile.getEventTypeShift();
                    // Only set a crate index if the listfile is v.2 or greater
                    if(TheListfile.getVersion() >= 2)
                    {
                        // Get the crateIndex from the section header
                        crateIndex = (sectionHeader & TheListfile.getCrateIndexMask()) >> TheListfile.getCrateIndexShift();
                    }

                    // Read the sub event header, containing the subevent section size and 
                    // the module type as a integer
                    infile.read((char *)&subEventHeader, sizeof(uint32_t));

                    // This is the module type, determines how the data will be processed
                    moduleType = (subEventHeader & TheListfile.getModuleTypeMask()) >> TheListfile.getModuleTypeShift();

                    // This is the sub event section size, how much data is in the subevent
                    subEventSize = (subEventHeader & TheListfile.getSubEventSizeMask()) >> TheListfile.getSubEventSizeShift();
                    
                    // Set up a block of memory equivalent in size to the amount of 32-bit words that will be read
                    uint32_t *rawSubEventData = (uint32_t *)malloc((subEventSize+1) * sizeof(uint32_t));

                    // Read the data from the istream into rawSubEventData
                    infile.read((char *)rawSubEventData, (subEventSize+1)*sizeof(uint32_t));

                    // For convenience a vector is used to pass the data that needs to be sorted to the sorting function
                    std::vector<uint32_t> subEventData(subEventSize+1);

                    // Copy the data from memory to the vector
                    std::copy(rawSubEventData, rawSubEventData + subEventSize, subEventData.begin());

                    // Set the current module that for the data type
                    // The try-catch is to check if the moduleType is available in the modulemap
                    // If it isn't, the code will need to modified to add it to the available types if it is
                    // a new module, otherwise the templates need to be checked.
                    try
                    {
                        currentmodule = module_map.at(moduleType);
                    }
                    catch(std::out_of_range &e)
                    {   
                        std::string badmoduletype = "\n\n\n\n\nModule type <" + std::to_string(moduleType) + "> mismatch, module type not found in modulemap, error " + e.what() + "\n";
                        // printf("\n\n\n\n\nModule type <%u> mismatch, module type not found in modulemap, error %s\n",moduleType,e.what());
                        for (std::map<uint32_t,MVMEmodule>::iterator it=module_map.begin(); it!=module_map.end(); ++it)
                        {
                            badmoduletype.append("Available module: name = <" + it->second.getModuleName() + ">, type = <" + std::to_string((uint32_t)it->second.getModuleType()) + ">.\n");
                            // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                        }
                        badmoduletype.append("Module type out of range. Check available module templates.\n");
                        throw std::out_of_range(badmoduletype);
                    }
                    
                    // Write the crateIndex and eventType to the module file, after the proper module has been set
                    currentmodule.setEventIndex(eventType);
                    currentmodule.setCrateIndex(crateIndex);


                    // Get the unique module ID from the module settings header in the vector
                    uint32_t moduleID= (subEventData.at(0) & currentmodule.getID_Mask()) >> currentmodule.getID_Shift();

                    // Now delete the memory the raw data occupied to prevent memory leaks
                    delete(rawSubEventData);

                    switch(currentmodule.getModuleType())
                    {
                        case VMEModuleType::MDPP16_QDC:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp16_qdc>(subEventData, currentmodule);


                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            }


                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_qdc);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp16_qdc = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;

                            }
                            break;
                        }
                        case VMEModuleType::MDPP16_SCP:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp16_scp>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_scp);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp16_scp = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::MDPP16_RCP:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp16_rcp>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_rcp);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp16_rcp = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;

                        }
                        case VMEModuleType::MDPP16_PADC:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp16_padc>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_padc);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp16_padc = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;

                        }
                        case VMEModuleType::MDPP16_CSI:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp16_csi>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp16_csi);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp16_csi = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;

                        }
                        case VMEModuleType::MDPP32_QDC:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp32_qdc>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp32_qdc);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp32_qdc = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::MDPP32_SCP:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp32_scp>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp32_scp);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp32_scp = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::MDPP32_PADC:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdpp32_padc>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdpp32_padc);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdpp32_padc = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;

                        }
                        case VMEModuleType::MTDC32:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mtdc32>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mtdc32);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mtdc32 = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::MADC32:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<madc32>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_madc32);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_madc32 = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::MQDC32:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mqdc32>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mqdc32);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mqdc32 = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;

                        }
                        case VMEModuleType::VMMR:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<vmmr>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_vmmr);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_vmmr = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::VMMR_1NS:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<vmmr>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_vmmr);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_vmmr = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;
                        }
                        case VMEModuleType::MDI2:
                        {
                            // Sort the subEventData into a vector
                            auto modulereturndata = sortData<mdi2>(subEventData, currentmodule);

                            // Get the tree name to store the data in
                            // If the moduleID is not found, indicating an error in build_root_file, 
                            // print the moduleID with an error message.
                            std::string TreeName;
                            try
                            {
                                TreeName = std::get<0>(ModulesInFile.at(moduleID));

                            }
                            catch(std::out_of_range &e)
                            {
                                std::string badmoduleid = "\n\n\n\n\nmoduleID = <" + std::to_string(moduleID) + "> not found in std::map ModulesInFile.\n";
                                badmoduleid.append("Error issued in process_listfile, indicating failure in build_root_file function.\n");
                                badmoduleid.append("Exiting program, error {" + (std::string)e.what() + "}. Out-Of-Range error.\n");
                                throw std::out_of_range(badmoduleid);
                                // printf("Available module: name = <%s>, type = <%u>.\n",it->second.getModuleName().c_str(),(uint32_t)it->second.getModuleType());
                            };

                            // Set the tree the data will be stored in
                            TTree *rtree = roofile->Get<TTree>(TreeName.c_str());

                            // Set the branch on the TTree the data will be stored in
                            rtree->SetBranchAddress("DATA",&fillmodule_mdi2);

                            // This loop takes the sorted data, moves it to the fillmodule,
                            // and then fills the TTree and increments the total number
                            // of events that are in the data file
                            for(auto j = 0; j < modulereturndata.size(); j++)
                            {
                                // Copy the data from the vector to the pointer
                                fillmodule_mdi2 = &modulereturndata.at(j);

                                // Fill the TBranch on the TTree
                                int iFilled = rtree->Fill();

                                // Increment the total number of events
                                total_events++;
                            }
                            break;

                        }
                    }
                } break;

            case listfile::SectionType_Timetick:
                {
                    ++currentSecondInRun;
                    std::string str = read_string_data(infile, sectionBytes);
                    printf("\nTimetick section: %s\nSecond %lu in the DAQ run begins.\033[F\033[F\r",str.c_str(),currentSecondInRun);
                    // Prints a status bar when loading large amounts of data
                    printProgress((double)currentSecondInRun/(double)TheListfile.getTimeticks());
                    // std::cout << "Timetick section: " << str << std::endl;
                    // std::cout << "Second " << currentSecondInRun << " in the DAQ run begins" << std::endl;
                    fflush(stdout);
                } break;

            case listfile::SectionType_End:
                {
                    std::string str = read_string_data(infile, sectionBytes);
                    printf("\n\n\n\nEnd section: %s\nTotal events processed: %u.\n",str.c_str(),total_events);
                    continueReading = false;

                    auto currentFilePos = infile.tellg();
                    // infile.seekg(0, std::ifstream::end);
                    infile.seekg(0, std::istream::end);

                    auto endFilePos = infile.tellg();

                    if (currentFilePos != endFilePos)
                    {
                        std::cout << "Warning: " << (endFilePos - currentFilePos)
                            << " bytes left after Listfile End Section" << std::endl;
                    }
                } break;

            case listfile::SectionType_Pause:
                {
                    if (sectionSize == 1)
                    {
                        uint32_t value = 0;
                        infile.read((char *)&value, sizeof(value));

                        switch (static_cast<PauseAction>(value))
                        {
                            case listfile::Pause:
                                std::cout << "Pause" << std::endl;
                                break;
                            case listfile::Resume:
                                std::cout << "Resume" << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Pause section containing " << sectionSize << " words. "
                            << "Expected sectionSize to be 1 word!" << std::endl;
                        // infile.seekg(sectionBytes, std::ifstream::cur);
                        infile.seekg(sectionBytes, std::istream::cur);

                    }
                } break;

            default:
                {
                    printf("Warning: Unknown section type %u of size %u, skipping...\n",
                        sectionType, sectionSize);
                    // infile.seekg(sectionBytes, std::ifstream::cur);
                    infile.seekg(sectionBytes, std::istream::cur);

                } break;
        }
    }
    
    roofile->Write("",TObject::kOverwrite);
    roofile->Close();
}
*/

void build_root_file(std::istream &infile, listfile &TheListfile, bool useR65Hack)
{
    bool dumpData = true;
    bool continueReading = true;
    auto roofile=TFile::Open(TheListfile.getFilename().c_str(),"recreate");
    uint64_t currentSecondInRun = 0;
    uint32_t ADC,LONG,SHORT,TOF,ROUTE_EVT,TRIGGER,ROUTE_TRIGGER;
    bool SHORT_OVERFLOW,LONG_OVERFLOW,OVERFLOW,PILEUP,IS_RESET;
    uint64_t TIMESTAMP;

    MVMEmodule currentmodule;

    uint32_t isRoute0 = 0;

    uint32_t total_events = 0;

    // double progress{0};
    // double filesize = infile.tellg();

    // int loopvar = 0;

    std::map<uint32_t,std::tuple<std::string, VMEModuleType>> moduleNameTypeMap;
    std::map<uint32_t, MVMEmodule> moduleTypetoTemplateMap;
    uint32_t OldModuleType;

    // std::map<uint32_t, VMEModuleType> moduleTypeMap;

    printf("%lu\n",moduleNameTypeMap.size());

    uint32_t sectionHeader;
    while (continueReading)
    {
        infile.read((char *)&sectionHeader, sizeof(uint32_t));


        uint32_t sectionType   = (sectionHeader & TheListfile.getSectionTypeMask()) >> TheListfile.getSectionTypeShift();
        uint32_t sectionSize   = (sectionHeader & TheListfile.getSectionSizeMask()) >> TheListfile.getSectionSizeShift();
        uint32_t sectionBytes  = sectionSize * sizeof(uint32_t);

        // progress = static_cast<double>(infile.tellg)/filesize;
        // printProgress(progress);

        switch ((SectionType)sectionType)
        {
            case listfile::SectionType_Config:
                {
                    std::string str = read_string_data(infile, sectionBytes);

                    std::cout << "Config section of size " << sectionSize
                        << " words, " << sectionBytes << " bytes." << std::endl
                        << "First 1000 characters:" << std::endl;
                    std::cout << str << std::endl << "---" << std::endl;
                } break;

            case listfile::SectionType_Event:
                {
                    uint32_t eventType = (sectionHeader & TheListfile.getEventTypeMask()) >> TheListfile.getEventTypeShift();
                    // printf("Event section: eventHeader=0x%08x, eventType=%u, eventSize=%u\n",
                    //        sectionHeader, eventType, sectionSize);

                    uint32_t wordsLeft = sectionSize;
                    // printf("Num Words = %u\n",sectionSize);
                    uint32_t channel{0}, data{0}, dattype{0};
                    // TNtuple test;
                    while (wordsLeft > 1)
                    {
                    
                        uint32_t subEventHeader;
                        infile.read((char *)&subEventHeader, sizeof(uint32_t));
                        --wordsLeft;

                        uint32_t moduleType = (subEventHeader & TheListfile.getModuleTypeMask()) >> TheListfile.getModuleTypeShift();
                        uint32_t subEventSize = (subEventHeader & TheListfile.getSubEventSizeMask()) >> TheListfile.getSubEventSizeShift();

                        // MVMEmodule TheModule(moduleType);
                        // currentmodule.set

                        if(moduleTypetoTemplateMap.count(moduleType) == 0)
                        {   
                            MVMEmodule TestTheModuleType;
                            try
                            {
                                TestTheModuleType.setModule(moduleType);
                            }
                            catch(const std::invalid_argument& e)
                            {
                                std::cerr << e.what() << '\n';
                            }
                            
                            moduleTypetoTemplateMap.insert(std::pair<uint32_t,MVMEmodule>(moduleType, TestTheModuleType));
                            currentmodule = moduleTypetoTemplateMap.at(moduleType);
                            // currentmodule.setModule(moduleType);
                            // OldModuleType = moduleType;
                        }
                        else
                        {
                            currentmodule = moduleTypetoTemplateMap.at(moduleType);
                        }

                        // printf("  subEventHeader=0x%08x, moduleType=%u (%s), subEventSize=%u\n",
                        //        subEventHeader, moduleType, currentmodule.getModuleName().c_str(),
                        //        subEventSize);

                        uint32_t *rawSubEventData = (uint32_t *)malloc(subEventSize * sizeof(uint32_t));
                        infile.read((char *)rawSubEventData, subEventSize*sizeof(uint32_t));
                        std::vector<uint32_t> subEventData(subEventSize);
                        // printf("%lu\n",subEventData.size());
                        std::copy(rawSubEventData, rawSubEventData + subEventSize, subEventData.begin());

                        std::string testModuleName = currentmodule.getModuleName();

                        uint32_t moduleID= (subEventData.at(0) & 0x00ff0000) >> 16;
                        if(useR65Hack)
                        {
                            moduleID += 1000*moduleType;
                        }
                        // printf("subEventData.at(0) = %X, ModuleID = %u\n", subEventData.at(0), moduleID);


                        free (rawSubEventData);

                        testModuleName += "_" + std::to_string(moduleID);

                        if(moduleNameTypeMap.empty())
                        {
                            moduleNameTypeMap.insert(std::pair<uint32_t,std::tuple<std::string,VMEModuleType>>(moduleID,std::make_tuple(testModuleName,currentmodule.getModuleType())));
                        }
                        else
                        {
                            if(moduleNameTypeMap.count(moduleID) == 0)
                            {
                                moduleNameTypeMap.insert(std::pair<uint32_t,std::tuple<std::string,VMEModuleType>>(moduleID,std::make_tuple(testModuleName,currentmodule.getModuleType())));
                            }
                        }

                        wordsLeft -= subEventSize;
                    }

                    uint32_t eventEndMarker;
                    infile.read((char *)&eventEndMarker, sizeof(uint32_t));
                    // printf("   eventEndMarker=0x%08x\n", eventEndMarker);
                } break;

            case listfile::SectionType_Timetick:
                {
                    ++currentSecondInRun;
                    std::string str = read_string_data(infile, sectionBytes);
                    printf("\nTimetick section: %s\nSecond %lu in the DAQ run begins.\033[F\033[F\r",str.c_str(),currentSecondInRun);

                    // std::cout << "Timetick section: " << str << std::endl;
                    // std::cout << "Second " << currentSecondInRun << " in the DAQ run begins" << std::endl;
                    fflush(stdout);
                } break;

            case listfile::SectionType_End:
                {
                    std::string str = read_string_data(infile, sectionBytes);
                    std::cout << "End section: " << str << std::endl;
                    printf("Total events processed: %u.\n\n\n",total_events);
                    continueReading = false;

                    auto currentFilePos = infile.tellg();
                    // infile.seekg(0, std::ifstream::end);
                    infile.seekg(0, std::istream::end);

                    auto endFilePos = infile.tellg();

                    if (currentFilePos != endFilePos)
                    {
                        std::cout << "Warning: " << (endFilePos - currentFilePos)
                            << " bytes left after Listfile End Section" << std::endl;
                    }
                    if(moduleNameTypeMap.size() > 0)
                    {
                        printf("%lu\n",moduleNameTypeMap.size());
                        for (std::map<uint32_t,std::tuple<std::string,VMEModuleType>>::iterator it=moduleNameTypeMap.begin(); it!=moduleNameTypeMap.end(); ++it)
                        {
                            auto *addtree = new TTree(std::get<0>(it->second).c_str(),std::get<0>(it->second).c_str());
                            switch (std::get<1>(it->second))
                            {
                                case VMEModuleType::MDPP16_QDC:
                                {
                                    mdpp16_qdc addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;
                                }
                                case VMEModuleType::MDPP16_SCP:
                                {
                                    mdpp16_scp addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;
                                }
                                case VMEModuleType::MDPP16_RCP:
                                {
                                    mdpp16_rcp addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;

                                }
                                case VMEModuleType::MDPP16_PADC:
                                {
                                    mdpp16_padc addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;
                                }
                                case VMEModuleType::MDPP16_CSI:
                                {
                                    mdpp16_csi addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;

                                }
                                case VMEModuleType::MDPP32_QDC:
                                {
                                    mdpp32_qdc addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;
                                }
                                case VMEModuleType::MDPP32_SCP:
                                {
                                    mdpp32_scp addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;
                                }
                                case VMEModuleType::MDPP32_PADC:
                                {
                                    mdpp32_padc addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;
                                }
                                case VMEModuleType::MTDC32:
                                {
                                    mtdc32 addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;
                                }
                                case VMEModuleType::MADC32:
                                {
                                    madc32 addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;
                                }
                                case VMEModuleType::MQDC32:
                                {
                                    mqdc32 addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;
                                }
                                case VMEModuleType::VMMR:
                                {
                                    vmmr addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;
                                }
                                case VMEModuleType::VMMR_1NS:
                                {
                                    vmmr addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;
                                }
                                case VMEModuleType::MDI2:
                                {
                                    mdi2 addbranch;
                                    addtree->Branch("DATA", &addbranch,10000000);
                                    break;
                                }

                            }
                            roofile->Write("",TObject::kOverwrite);
                        }
                    }
                    TheListfile.setTimeticks(currentSecondInRun);

                } break;

            case listfile::SectionType_Pause:
                {
                    if (sectionSize == 1)
                    {
                        uint32_t value = 0;
                        infile.read((char *)&value, sizeof(value));

                        switch (static_cast<PauseAction>(value))
                        {
                            case listfile::Pause:
                                std::cout << "Pause" << std::endl;
                                break;
                            case listfile::Resume:
                                std::cout << "Resume" << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Pause section containing " << sectionSize << " words. "
                            << "Expected sectionSize to be 1 word!" << std::endl;
                        // infile.seekg(sectionBytes, std::ifstream::cur);
                        infile.seekg(sectionBytes, std::istream::cur);

                    }
                } break;

            default:
                {
                    printf("Warning: Unknown section type %u of size %u, skipping...\n",
                        sectionType, sectionSize);
                    // infile.seekg(sectionBytes, std::ifstream::cur);
                    infile.seekg(sectionBytes, std::istream::cur);

                } break;
        }
    }
    // roofile->Write();
    roofile->Close();
    TheListfile.setTreenames(moduleNameTypeMap);
    // for (auto& x: TheListfile.getTreename()) 
    // {
    //     printf("%u: %s\n",x.first,std::get<0>(x.second).c_str());
    // }
}

void printProgress(double ratio) {
    int total_bar = 50;
    std::string bar;
    bar.resize(total_bar,' ');
    size_t percents = ratio * total_bar;
    std::fill(bar.begin(),bar.begin()+percents,'#');
    printf("\r[%s] %3.1f%% complete\r", bar.c_str(),(ratio*100.));
    fflush(stdout);
}

/* 
 * 
 * These are the function definitions for the various module-specific sortData() functions
 * Each function corresponds to a data type listed in modules.h
 *  
 */

template <>
std::vector<mdpp16_qdc> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<mdpp16_qdc> module_to_return;
    mdpp16_qdc temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();

    std::vector<uint32_t> SHORT, LONG, TOF, ROUTE_EVT;
    std::vector<bool> SHORT_OVERFLOW, LONG_OVERFLOW;
    uint32_t ID{0xffffffff}, TRIGGER{0xffffffff}, ROUTE_TRIGGER{0xffffffff}, ADC_RES{0xffffffff}, TDC_RES{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {    
        bool overflowflag{false};
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
            ADC_RES = (module.getADC_RES_Shift() & subEventData.at(i)) >> module.getADC_RES_Shift();
            TDC_RES = (module.getTDC_RES_Mask() & subEventData.at(i)) >> module.getTDC_RES_Shift();
            // printf("ADC_RES = %u\n",ADC_RES);
        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));// > module.getData_Shift();
            overflowflag = (module.getOverflow_Mask() & subEventData.at(i)) > 0;
            if(channel >=16 && channel <=31)
            {
                TOF.push_back(data);
                
            }
            else if(channel >=48 && channel <=63)
            {
                SHORT.push_back(data);
                SHORT_OVERFLOW.push_back(overflowflag);

            }
            else if(channel >= 0 && channel <= 15)
            {
                // if(subEventData.at(i) & 0x0000F000 > 0){ printf("LONG = %u, subEventData = %X\n",data,subEventData.at(i)); }
                LONG.push_back(data);
                LONG_OVERFLOW.push_back(overflowflag);
                ROUTE_EVT.push_back(channel);
            }
            else if(channel == 32 || channel == 33)
            {

                TRIGGER = data;
                ROUTE_TRIGGER = channel-32;
            }

        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
        // printf("%llu\n",TIMESTAMP);
    }

    for(auto i = 0; i < LONG.size(); ++i)
    {
        temporary_module.ID = ID;
        temporary_module.ADC_RES = ADC_RES;
        temporary_module.TDC_RES = TDC_RES;
        temporary_module.SHORT = SHORT.at(i);
        temporary_module.SHORT_OVERFLOW = SHORT_OVERFLOW.at(i);
        temporary_module.LONG = LONG.at(i);
        temporary_module.LONG_OVERFLOW = LONG_OVERFLOW.at(i);
        temporary_module.TOF = TOF.at(i);
        temporary_module.ROUTE_EVT = ROUTE_EVT.at(i);
        temporary_module.TRIGGER = TRIGGER;
        temporary_module.ROUTE_TRIGGER = ROUTE_TRIGGER;
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);
    }

    return module_to_return;
}

template<>
std::vector<mdpp16_scp> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<mdpp16_scp> module_to_return;
    mdpp16_scp temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();

    std::vector<uint32_t> ADC, TOF, ROUTE_EVT;
    std::vector<bool> PILEUP, OVERFLOW;
    uint32_t ID{0xffffffff}, TRIGGER{0xffffffff}, ROUTE_TRIGGER{0xffffffff}, ADC_RES{0xffffffff}, TDC_RES{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool overflowflag{false}, pileupflag{false};
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
            ADC_RES = (module.getADC_RES_Shift() & subEventData.at(i)) >> module.getADC_RES_Shift();
            TDC_RES = (module.getTDC_RES_Mask() & subEventData.at(i)) >> module.getTDC_RES_Shift();
        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));
            overflowflag = (module.getOverflow_Mask() & subEventData.at(i)) > 0;
            pileupflag = (module.getPileup_Mask() & subEventData.at(i)) > 0;

            if(channel >= 0 && channel <= 15)
            {
                ADC.push_back(data);
                OVERFLOW.push_back(overflowflag);
                PILEUP.push_back(pileupflag);
                ROUTE_EVT.push_back(channel);
            }
            else if(channel >=16 && channel <=31)
            {
                TOF.push_back(data);
                
            }
            else if(channel == 32 || channel == 33)
            {
                TRIGGER = data;
                ROUTE_TRIGGER = channel-32;
            }

        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }


    for(auto i = 0; i < ADC.size(); ++i)
    {

        temporary_module.ID = ID;
        temporary_module.ADC_RES = ADC_RES;
        temporary_module.TDC_RES = TDC_RES;
        temporary_module.ADC = ADC.at(i);
        temporary_module.OVERFLOW = OVERFLOW.at(i);
        temporary_module.PILEUP = PILEUP.at(i);
        temporary_module.TOF = TOF.at(i);
        temporary_module.ROUTE_EVT = ROUTE_EVT.at(i);
        temporary_module.TRIGGER = TRIGGER;
        temporary_module.ROUTE_TRIGGER = ROUTE_TRIGGER;
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);
    }

    return module_to_return;
}

template<>
std::vector<mdpp16_rcp> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<mdpp16_rcp> module_to_return;
    mdpp16_rcp temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();

    std::vector<uint32_t> ADC, TOF, ROUTE_EVT;
    std::vector<bool> PILEUP, OVERFLOW, IS_RESET;
    uint32_t ID{0xffffffff}, TRIGGER{0xffffffff}, ROUTE_TRIGGER{0xffffffff}, ADC_RES{0xffffffff}, TDC_RES{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool overflowflag{false}, pileupflag{false};
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
            ADC_RES = (module.getADC_RES_Shift() & subEventData.at(i)) >> module.getADC_RES_Shift();
            TDC_RES = (module.getTDC_RES_Mask() & subEventData.at(i)) >> module.getTDC_RES_Shift();
        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {

            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));
            overflowflag = (module.getOverflow_Mask() & subEventData.at(i)) > 0;
            pileupflag = (module.getPileup_Mask() & subEventData.at(i)) > 0;
            bool isreset = (data < 16) && (channel == 33);

            if(!isreset)
            {
                if(channel >= 0 && channel <= 15)
                {
                    ADC.push_back(data);
                    OVERFLOW.push_back(overflowflag);
                    PILEUP.push_back(pileupflag);
                    ROUTE_EVT.push_back(channel);
                    IS_RESET.push_back(isreset);
                }
                else if(channel >=16 && channel <=31)
                {
                    TOF.push_back(data);
                    
                }
                else if(channel == 32 || channel == 33)
                {
                    TRIGGER = data;
                    ROUTE_TRIGGER = channel-32;
                }
            }
            else
            {
                ADC.push_back(0xffffffff);
                OVERFLOW.push_back(false);
                PILEUP.push_back(false);
                ROUTE_EVT.push_back(channel);
                IS_RESET.push_back(isreset);
                TOF.push_back(0xffffffff);
                TRIGGER = 0xffffffff;
                ROUTE_TRIGGER = 0xffffffff;
            }
        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }


    for(auto i = 0; i < ADC.size(); ++i)
    {

        temporary_module.ID = ID;
        temporary_module.ADC_RES = ADC_RES;
        temporary_module.TDC_RES = TDC_RES;
        temporary_module.ADC = ADC.at(i);
        temporary_module.OVERFLOW = OVERFLOW.at(i);
        temporary_module.PILEUP = PILEUP.at(i);
        temporary_module.TOF = TOF.at(i);
        temporary_module.ROUTE_EVT = ROUTE_EVT.at(i);
        temporary_module.TRIGGER = TRIGGER;
        temporary_module.ROUTE_TRIGGER = ROUTE_TRIGGER;
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);


    }

    return module_to_return;
}

template<>
std::vector<mdpp16_padc> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<mdpp16_padc> module_to_return;
    mdpp16_padc temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();

    std::vector<uint32_t> PA, TOF, ROUTE_EVT;
    std::vector<bool> OVERFLOW;
    uint32_t ID{0xffffffff}, TRIGGER{0xffffffff}, ROUTE_TRIGGER{0xffffffff}, ADC_RES{0xffffffff}, TDC_RES{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool overflowflag{false}, pileupflag{false};
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
            ADC_RES = (module.getADC_RES_Shift() & subEventData.at(i)) >> module.getADC_RES_Shift();
            TDC_RES = (module.getTDC_RES_Mask() & subEventData.at(i)) >> module.getTDC_RES_Shift();
        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));
            overflowflag = (module.getOverflow_Mask() & subEventData.at(i)) > 0;
            pileupflag = (module.getPileup_Mask() & subEventData.at(i)) > 0;

            if(channel >= 0 && channel <= 15)
            {
                PA.push_back(data);
                OVERFLOW.push_back(overflowflag);
                ROUTE_EVT.push_back(channel);
            }
            else if(channel >=16 && channel <=31)
            {
                TOF.push_back(data);
                
            }
            else if(channel == 32 || channel == 33)
            {
                TRIGGER = data;
                ROUTE_TRIGGER = channel-32;
            }
        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }


    for(auto i = 0; i < PA.size(); ++i)
    {

        temporary_module.ID = ID;
        temporary_module.ADC_RES = ADC_RES;
        temporary_module.TDC_RES = TDC_RES;
        temporary_module.PA = PA.at(i);
        temporary_module.OVERFLOW = OVERFLOW.at(i);
        temporary_module.TOF = TOF.at(i);
        temporary_module.ROUTE_EVT = ROUTE_EVT.at(i);
        temporary_module.TRIGGER = TRIGGER;
        temporary_module.ROUTE_TRIGGER = ROUTE_TRIGGER;
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);


    }

    return module_to_return;
}

template <>
std::vector<mdpp16_csi> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<mdpp16_csi> module_to_return;
    mdpp16_csi temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();

    std::vector<uint32_t> SHORT, LONG, TOF, ROUTE_EVT;
    std::vector<bool> SHORT_OVERFLOW, LONG_OVERFLOW, PILEUP, PA_OVERFLOW, OVERFLOW, OutOfRange;
    uint32_t ID{0xffffffff}, TRIGGER{0xffffffff}, ROUTE_TRIGGER{0xffffffff}, TDC_RES{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool overflowflag{false}, pileupflag{false}, lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
            TDC_RES = (module.getTDC_RES_Mask() & subEventData.at(i)) >> module.getTDC_RES_Shift();

        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));// > module.getData_Shift();
            overflowflag = (module.getOverflow_Mask() & subEventData.at(i)) > 0;

            if(channel >=16 && channel <=31)
            {
                TOF.push_back(data);
                
            }
            else if(channel >=48 && channel <=63)
            {
                SHORT.push_back(data);
                SHORT_OVERFLOW.push_back(overflowflag);

            }
            else if(channel >= 0 && channel <= 15)
            {
                LONG.push_back(data);
                LONG_OVERFLOW.push_back(overflowflag);
                ROUTE_EVT.push_back(channel);
            }
            else if(channel == 32 || channel == 33)
            {

                TRIGGER = data;
                ROUTE_TRIGGER = channel-32;
            }


        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }

    for(auto i = 0; i < LONG.size(); ++i)
    {
        temporary_module.ID = ID;
        temporary_module.TDC_RES = TDC_RES;
        temporary_module.SHORT = SHORT.at(i);
        temporary_module.SHORT_OVERFLOW = SHORT_OVERFLOW.at(i);
        temporary_module.LONG = LONG.at(i);
        temporary_module.LONG_OVERFLOW = LONG_OVERFLOW.at(i);
        temporary_module.TOF = TOF.at(i);
        temporary_module.ROUTE_EVT = ROUTE_EVT.at(i);
        temporary_module.TRIGGER = TRIGGER;
        temporary_module.ROUTE_TRIGGER = ROUTE_TRIGGER;
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);

    }

    return module_to_return;
}

template <>
std::vector<mdpp32_qdc> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<mdpp32_qdc> module_to_return;
    mdpp32_qdc temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();

    std::vector<uint32_t> SHORT, LONG, TOF, ROUTE_EVT;
    std::vector<bool> SHORT_OVERFLOW, LONG_OVERFLOW;
    uint32_t ID{0xffffffff}, TRIGGER{0xffffffff}, ROUTE_TRIGGER{0xffffffff}, ADC_RES{0xffffffff}, TDC_RES{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool overflowflag{false}, pileupflag{false}, lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
            ADC_RES = (module.getADC_RES_Shift() & subEventData.at(i)) >> module.getADC_RES_Shift();
            TDC_RES = (module.getTDC_RES_Mask() & subEventData.at(i)) >> module.getTDC_RES_Shift();
            // uint32_t numwordfollow = (0x000003ff & subEventData.at(i));
            // printf("ModuleID = %u, word # = %u\n",moduleID,i);
        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
            // TIMESTAMP = TIMESTAMP | (module.getLowTimestamp_Mask() & subEventData.at(i));       
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));// > module.getData_Shift();
            overflowflag = (module.getOverflow_Mask() & subEventData.at(i)) > 0;

            if(channel >=32 && channel <=63)
            {
                TOF.push_back(data);
                
            }
            else if(channel >=94 && channel <=125)
            {
                SHORT.push_back(data);
                SHORT_OVERFLOW.push_back(overflowflag);

            }
            else if(channel >= 0 && channel <= 31)
            {
                LONG.push_back(data);
                LONG_OVERFLOW.push_back(overflowflag);
                ROUTE_EVT.push_back(channel);
            }
            else if(channel == 64 || channel == 65)
            {

                TRIGGER = data;
                ROUTE_TRIGGER = channel-64;
            }


        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }

    for(auto i = 0; i < LONG.size(); ++i)
    {
        temporary_module.ID = ID;
        temporary_module.ADC_RES = ADC_RES;
        temporary_module.TDC_RES = TDC_RES;
        temporary_module.SHORT = SHORT.at(i);
        temporary_module.SHORT_OVERFLOW = SHORT_OVERFLOW.at(i);
        temporary_module.LONG = LONG.at(i);
        temporary_module.LONG_OVERFLOW = LONG_OVERFLOW.at(i);
        temporary_module.TOF = TOF.at(i);
        temporary_module.ROUTE_EVT = ROUTE_EVT.at(i);
        temporary_module.TRIGGER = TRIGGER;
        temporary_module.ROUTE_TRIGGER = ROUTE_TRIGGER;
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);
    }

    return module_to_return;
}

template<>
std::vector<mdpp32_scp> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<mdpp32_scp> module_to_return;
    mdpp32_scp temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();    

    std::vector<uint32_t> ADC, TOF, ROUTE_EVT;
    std::vector<bool> PILEUP, OVERFLOW;
    uint32_t ID{0xffffffff}, TRIGGER{0xffffffff}, ROUTE_TRIGGER{0xffffffff}, ADC_RES{0xffffffff}, TDC_RES{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool overflowflag{false}, pileupflag{false};
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
            ADC_RES = (module.getADC_RES_Shift() & subEventData.at(i)) >> module.getADC_RES_Shift();
            TDC_RES = (module.getTDC_RES_Mask() & subEventData.at(i)) >> module.getTDC_RES_Shift();
        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));
            overflowflag = (module.getOverflow_Mask() & subEventData.at(i)) > 0;
            pileupflag = (module.getPileup_Mask() & subEventData.at(i)) > 0;

            if(channel >= 0 && channel <= 31)
            {
                ADC.push_back(data);
                OVERFLOW.push_back(overflowflag);
                PILEUP.push_back(pileupflag);
                ROUTE_EVT.push_back(channel);
            }
            else if(channel >=32 && channel <=63)
            {
                TOF.push_back(data);
                
            }
            else if(channel == 64 || channel == 65)
            {
                TRIGGER = data;
                ROUTE_TRIGGER = channel-32;
            }
        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }


    for(auto i = 0; i < ADC.size(); ++i)
    {

        temporary_module.ID = ID;
        temporary_module.ADC_RES = ADC_RES;
        temporary_module.TDC_RES = TDC_RES;
        temporary_module.ADC = ADC.at(i);
        temporary_module.OVERFLOW = OVERFLOW.at(i);
        temporary_module.PILEUP = PILEUP.at(i);
        temporary_module.TOF = TOF.at(i);
        temporary_module.ROUTE_EVT = ROUTE_EVT.at(i);
        temporary_module.TRIGGER = TRIGGER;
        temporary_module.ROUTE_TRIGGER = ROUTE_TRIGGER;
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);


    }

    return module_to_return;
}

template<>
std::vector<mdpp32_padc> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<mdpp32_padc> module_to_return;
    mdpp32_padc temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();

    std::vector<uint32_t> PA, TOF, ROUTE_EVT;
    std::vector<bool> OVERFLOW;
    uint32_t ID{0xffffffff}, TRIGGER{0xffffffff}, ROUTE_TRIGGER{0xffffffff}, ADC_RES{0xffffffff}, TDC_RES{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool overflowflag{false}, pileupflag{false};
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
            ADC_RES = (module.getADC_RES_Shift() & subEventData.at(i)) >> module.getADC_RES_Shift();
            TDC_RES = (module.getTDC_RES_Mask() & subEventData.at(i)) >> module.getTDC_RES_Shift();
        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));
            overflowflag = (module.getOverflow_Mask() & subEventData.at(i)) > 0;
            pileupflag = (module.getPileup_Mask() & subEventData.at(i)) > 0;

            if(channel >= 0 && channel <= 31)
            {
                PA.push_back(data);
                OVERFLOW.push_back(overflowflag);
                ROUTE_EVT.push_back(channel);
            }
            else if(channel >=32 && channel <=63)
            {
                TOF.push_back(data);
                
            }
            else if(channel == 64 || channel == 65)
            {
                TRIGGER = data;
                ROUTE_TRIGGER = channel-32;
            }
        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }


    for(auto i = 0; i < PA.size(); ++i)
    {

        temporary_module.ID = ID;
        temporary_module.ADC_RES = ADC_RES;
        temporary_module.TDC_RES = TDC_RES;
        temporary_module.PA = PA.at(i);
        temporary_module.OVERFLOW = OVERFLOW.at(i);
        temporary_module.TOF = TOF.at(i);
        temporary_module.ROUTE_EVT = ROUTE_EVT.at(i);
        temporary_module.TRIGGER = TRIGGER;
        temporary_module.ROUTE_TRIGGER = ROUTE_TRIGGER;
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);


    }

    return module_to_return;
}

template<>
std::vector<madc32> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<madc32> module_to_return;
    madc32 temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();

    std::vector<uint32_t> ADC, ROUTE_ADC;
    std::vector<bool> OUTOFRANGE;
    uint32_t ID{0xffffffff}, ADC_RES{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool overflowflag{false};
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
        }
        // auto f = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift();
        // std::cout << f << ", " << isdata << std::endl;
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
            ADC_RES = (module.getADC_RES_Mask() & subEventData.at(i)) >> module.getADC_RES_Shift();
        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i)) >> module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));
            overflowflag = (module.getOutOfRange_Mask() & subEventData.at(i)) >> module.getOutOfRange_Shift();
            
            ADC.push_back(data);
            OUTOFRANGE.push_back(overflowflag);
            ROUTE_ADC.push_back(channel);

            // printf("ADC = %u, OUTOFRANGE = %u, ROUTE_ADC = %u\n",data,overflowflag,channel);


        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }


    for(auto i = 0; i < ADC.size(); ++i)
    {

        temporary_module.ID = ID;
        temporary_module.ADC_RES = ADC_RES;
        temporary_module.ADC = ADC.at(i);
        temporary_module.ROUTE_ADC = ROUTE_ADC.at(i);
        temporary_module.OutOfRange = OUTOFRANGE.at(i);
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);
        // printf("ID = %u, ADC = %u, TIMESTAMP = %u\n",ID,ADC,TIMESTAMP);

    }


    return module_to_return;
}

template<>
std::vector<mqdc32> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<mqdc32> module_to_return;
    mqdc32 temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();

    std::vector<uint32_t> ADC, ROUTE_ADC;
    std::vector<bool> OUTOFRANGE;
    uint32_t ID{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool overflowflag{false};
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));
            overflowflag = (module.getOverflow_Mask() & subEventData.at(i)) > 0;
            
            ADC.push_back(data);
            OUTOFRANGE.push_back(overflowflag);
            ROUTE_ADC.push_back(channel);


        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }


    for(auto i = 0; i < ADC.size(); ++i)
    {

        temporary_module.ID = ID;
        temporary_module.ADC = ADC.at(i);
        temporary_module.OutOfRange = OUTOFRANGE.at(i);
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);
    }

    return module_to_return;
}

template<>
std::vector<mtdc32> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<mtdc32> module_to_return;
    mtdc32 temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();

    std::vector<uint32_t> TOF, ROUTE_TOF;
    uint32_t ID{0xffffffff}, TRIGGER{0xffffffff}, ROUTE_TRIGGER{0xffffffff}, TDC_RES{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
            TDC_RES = (module.getTDC_RES_Mask() & subEventData.at(i)) >> module.getTDC_RES_Shift();
        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));

            if(channel >= 0 && channel <= 31)
            {
                TOF.push_back(data);
                ROUTE_TOF.push_back(channel);
            }

            else if(channel == 32 || channel == 33)
            {
                TRIGGER = data;
                ROUTE_TRIGGER = channel-32;
            }

        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }


    for(auto i = 0; i < TOF.size(); ++i)
    {

        temporary_module.ID = ID;
        temporary_module.TDC_RES = TDC_RES;
        temporary_module.TOF = TOF.at(i);
        temporary_module.ROUTE_TOF = ROUTE_TOF.at(i);
        temporary_module.TRIGGER = TRIGGER;
        temporary_module.ROUTE_TRIGGER = ROUTE_TRIGGER;
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);
    }

    return module_to_return;
}

template<>
std::vector<vmmr> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<vmmr> module_to_return;
    vmmr temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();

    std::vector<uint32_t> ADC, ADC_BUS, ROUTE_EVT, TOF, TOF_BUS;
    bool TRIGGER;
    uint32_t ID{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data;
    bool lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false}, istof{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
            istof = (module.getOptDTypeMask() & subEventData.at(i)) >> module.getOptDTypeShift() == module.getOptDType();
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
            TRIGGER = (module.getTDC_RES_Mask() & subEventData.at(i)) > 0; // Use TDC_RES_Mask for TRIGGER in VMMR

        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));


            ADC.push_back(data);
            ADC_BUS.push_back(channel);
            ROUTE_EVT.push_back(channel);


        }
        else if(istof)
        {            
            channel = (module.getOpt1_Mask() & subEventData.at(i))>>module.getOpt1_Shift();
            data = (module.getOpt2_Mask() & subEventData.at(i));

            TOF.push_back(data);
            TOF_BUS.push_back(channel);
        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }


    for(auto i = 0; i < ADC.size(); ++i)
    {

        temporary_module.ID = ID;

        temporary_module.ADC = ADC.at(i);
        temporary_module.ADC_BUS = ADC_BUS.at(i);
        temporary_module.ROUTE_EVT = ROUTE_EVT.at(i);
        temporary_module.TOF = TOF.at(i);
        temporary_module.TOF_BUS = TOF_BUS.at(i);
        temporary_module.TRIGGER = TRIGGER;
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);
    }

    return module_to_return;
}

template<>
std::vector<mdi2> sortData(std::vector<uint32_t> subEventData, MVMEmodule module)
{
    std::vector<mdi2> module_to_return;
    mdi2 temporary_module;

    // Set the crate index and event index for the module data type
    temporary_module.CRATE_INDEX = module.getCrateIndex();
    temporary_module.EVENT_INDEX = module.getEventIndex();

    std::vector<uint32_t> ADC, ADC_BUS, SAMPLENUMBER;
    std::vector<bool> OVERFLOW;
    uint32_t ID{0xffffffff};
    uint64_t TIMESTAMP{0}, hights{0}, lowts{0};
    uint32_t channel, data, bus;
    bool lowtsset{false};
    for(auto i = 0; i < subEventData.size(); ++i)
    {
        bool overflowflag{false};
        bool isnotallones{subEventData.at(i) != 0xffffffff}, isheader{false},istimestamp{false}, ishightimestamp{false}, isdata{false};
        if(isnotallones)
        {
            isheader = (module.getHeaderDTypeMask() & subEventData.at(i)) >> module.getHeaderDTypeShift() == module.getHeaderDType();
            ishightimestamp = (module.getHighTimestampDTypeMask() & subEventData.at(i)) >> module.getHighTimestampDTypeShift() == module.getHighTimestampDType();
            isdata = (module.getDataDTypeMask() & subEventData.at(i)) >> module.getDataDTypeShift() == module.getDataDType();
        }
        // The low 30 bits of the timestamp can be 0x3fffffff which means the timestamp word would be 0xffffffff
        // meaning the timestamp may not get set appropriately if it was in the above if statement. 
        // When the low timestamp is set once, it flips the lowtsset bit to true, which means we don't want to set the low timestamp again.
        istimestamp = (module.getLowTimestampDTypeMask() & subEventData.at(i)) >> module.getLowTimestampDTypeShift() == module.getLowTimestampDType();

        if(isheader) // if the word indicates it is a header
        {
            ID = (module.getID_Mask() & subEventData.at(i)) >> module.getID_Shift();
        }
        else if(istimestamp && !lowtsset) // if the word indicates it is the low 30 bits of the timestamp and the low timestamp isn't set
        {
            lowts = (module.getLowTimestamp_Mask() & subEventData.at(i));
            lowtsset=true;
        }
        else if(ishightimestamp) // if the word indicates it is the high 16 bits of an extended timestamp
        {
            hights = (module.getHighTimestamp_Mask() & subEventData.at(i));
            hights = hights << module.getHighTimestamp_Shift();
        }
        else if(isdata) // if the word indicates it is data 
        {
            channel = (module.getChannel_Mask() & subEventData.at(i))>>module.getChannel_Shift();
            data = (module.getData_Mask() & subEventData.at(i));
            bus = (module.getOpt1_Mask() & subEventData.at(i)) >> module.getOpt1_Shift();
            overflowflag = (module.getOverflow_Mask() & subEventData.at(i)) > 0;


            ADC.push_back(data);
            ADC_BUS.push_back(bus);
            SAMPLENUMBER.push_back(channel);
            OVERFLOW.push_back(overflowflag);



        }
    }
    if(!lowtsset)
    {
        TIMESTAMP = 0xffffffffffffffff;
    }
    else
    {
        TIMESTAMP = lowts | hights;
    }


    for(auto i = 0; i < ADC.size(); ++i)
    {

        temporary_module.ID = ID;
        temporary_module.ADC = ADC.at(i);
        temporary_module.ADC_BUS = ADC_BUS.at(i);
        temporary_module.SAMPLENUMBER = SAMPLENUMBER.at(i);
        temporary_module.OutOfRange = OVERFLOW.at(i);
        temporary_module.TIMESTAMP = TIMESTAMP;
        module_to_return.push_back(temporary_module);
    }

    return module_to_return;
}



