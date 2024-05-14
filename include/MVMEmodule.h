#include <cstdlib>
#include <cstdint>
#include <string>
#include <map>
#include <tuple>
#include <vector>
#include <fstream>
#include <filesystem>

#include "nlohmann/json.hpp"
#include "modules.h"
#include "dumper_constants.h"

namespace fs = std::filesystem;

using json = nlohmann::json;

#ifndef MVMEMODULE
#define MVMEMODULE

enum class VMEModuleType
{
    Invalid = 0,
    MADC32 = 1,
    MQDC32 = 2,
    MTDC32 = 3,
    MDPP16_SCP = 4,
    MDI2 = 6,
    MDPP16_RCP = 7,
    MDPP16_QDC = 8,
    VMMR = 9,
    MDPP32_SCP = 11,
    MDPP32_QDC = 13,
    MesytecCounter = 16,
    MDPP16_CSI = 18,
    MDPP16_PADC = 19,
    MDPP32_PADC = 20,
    VMMR_1NS = 21,
};

static const std::map <VMEModuleType, std::string> ModuleNames
{
    { VMEModuleType::Invalid, "Invalid" },
    { VMEModuleType::MADC32, "MADC-32" },
    { VMEModuleType::MQDC32, "MQDC-32" },
    { VMEModuleType::MTDC32, "MTDC-32" },
    { VMEModuleType::MDPP16_SCP, "MDPP-16_SCP" },
    { VMEModuleType::MDI2, "MDI-2" },
    { VMEModuleType::MDPP16_RCP, "MDPP-16_RCP" },
    { VMEModuleType::MDPP16_QDC, "MDPP-16_QDC" },
    { VMEModuleType::VMMR, "VMMR" },
    { VMEModuleType::MDPP32_SCP, "MDPP-32_SCP" },
    { VMEModuleType::MDPP32_QDC, "MDPP-32_QDC" },
    { VMEModuleType::MesytecCounter, "Mesytec_Counter" },    
    { VMEModuleType::MDPP16_CSI, "MDPP-16_CSI" },
    { VMEModuleType::MDPP16_PADC, "MDPP-16_PADC" },
    { VMEModuleType::MDPP32_PADC, "MDPP-32_PADC" },
    { VMEModuleType::VMMR_1NS, "VMMR-1ns" },
};

class MVMEmodule
{
    public:

        using enum VMEModuleType;

        MVMEmodule();
        MVMEmodule(uint32_t moduleType);

        ~MVMEmodule();

        int setModule(int moduleType); // set the ModuleType, does NOT set a template
        int setTemplate(std::string template_filename); // load a template from disk, and set the MVMEmodule up based on the template

        void setCrateIndex(uint32_t crateindex);
        void setEventIndex(uint32_t eventindex);

        void printModuleSetting(); // pretty prints all module variables including

        // These functions return the data the class holds. As
        // they are simple one line functions, they have been declared here.
        uint32_t getCrateIndex() { return CrateIndex; } // return CrateIndex
        uint32_t getEventIndex() { return EventIndex; } // return EventIndex
        std::string getModuleName() { return ModuleName; } // return ModuleName
        VMEModuleType getModuleType() { return ModuleType; } // return ModuleType
        uint32_t getHeaderDTypeMask() { return HeaderDTypeMask; } // return HeaderDTypeMask
        uint32_t getHeaderDType() { return HeaderDType; } // return HeaderDType
        uint32_t getHeaderDTypeShift() { return HeaderDTypeShift; } // return HeaderDTypeShift
        uint32_t getLowTimestampDTypeMask() { return LowTimestampDTypeMask; } // return LowTimestampDTypeMask
        uint32_t getLowTimestampDType() { return LowTimestampDType; } // return LowTimestampDType
        uint32_t getLowTimestampDTypeShift() { return LowTimestampDTypeShift; } // return LowTimestampDTypeShift
        uint32_t getHighTimestampDTypeMask() { return HighTimestampDTypeMask; } // return HighTimestampDTypeMask
        uint32_t getHighTimestampDType() { return HighTimestampDType; } // return HighTimestampDType
        uint32_t getHighTimestampDTypeShift() { return HighTimestampDTypeShift; } // return HighTimestampDTypeShift
        uint32_t getDataDTypeMask() { return DataDTypeMask; } // return DataDTypeMask
        uint32_t getDataDType() { return DataDType; } // return DataDType
        uint32_t getDataDTypeShift() { return DataDTypeShift; } // return DataDTypeShift
        uint32_t getOptDTypeMask() { return OptDTypeMask; } // return OptDTypeMask
        uint32_t getOptDType() { return OptDType; } // return OptDType
        uint32_t getOptDTypeShift() { return OptDTypeShift; } // return OptDTypeShift
        uint32_t getID_Mask() { return ID_Mask; } // return ID_Mask
        uint32_t getID_Shift() { return ID_Shift; } // return ID_Shift
        uint32_t getADC_RES_Mask() { return ADC_RES_Mask; } // return ADC_RES_Mask
        uint32_t getADC_RES_Shift() { return ADC_RES_Shift; } // return ADC_RES_Shift
        uint32_t getTDC_RES_Mask() { return TDC_RES_Mask; } // return TDC_RES_Mask
        uint32_t getTDC_RES_Shift() { return TDC_RES_Shift; } // return TDC_RES_Shift
        uint32_t getLowTimestamp_Mask() { return LowTimestamp_Mask; } // return LowTimestamp_Mask
        uint32_t getLowTimestamp_Shift() { return LowTimestamp_Shift; } // return LowTimestamp_Shift
        uint32_t getHighTimestamp_Mask() { return HighTimestamp_Mask; } // return HighTimestamp_Mask
        uint32_t getHighTimestamp_Shift() { return HighTimestamp_Shift; } // return HighTimestamp_Shift
        uint32_t getChannel_Mask() { return Channel_Mask; } // return Channel_Mask
        uint32_t getChannel_Shift() { return Channel_Shift; } // return Channel_Shift
        uint32_t getData_Mask() { return Data_Mask; } // return Data_Mask
        uint32_t getData_Shift() { return Data_Shift; } // return Data_Shift
        uint32_t getOverflow_Mask() { return Overflow_Mask; } // return Overflow_Mask
        uint32_t getOverflow_Shift() { return Overflow_Shift; } // return Overflow_Shift
        uint32_t getPileup_Mask() { return Pileup_Mask; } // return Pileup_Mask
        uint32_t getPileup_Shift() { return Pileup_Shift; } // return Pileup_Shift
        uint32_t getOutOfRange_Mask() { return OutOfRange_Mask; } // return OutOfRange_Mask
        uint32_t getOutOfRange_Shift() { return OutOfRange_Shift; } // return OutOfRange_Shift
        uint32_t getReset_Mask() { return Reset_Mask; } // return Reset_Mask
        uint32_t getReset_Shift() { return Reset_Shift; } // return Reset_Shift
        uint32_t getOpt1_Mask() { return Opt1_Mask; } // return Opt1_Mask
        uint32_t getOpt1_Shift() { return Opt1_Shift; } // return Opt1_Shift
        uint32_t getOpt2_Mask() { return Opt2_Mask; } // return Opt2_Mask
        uint32_t getOpt2_Shift() { return Opt2_Shift; } // return Opt2_Shift
       
    private:

        uint32_t CrateIndex{0};
        uint32_t EventIndex{0};

        VMEModuleType ModuleType;
        std::string ModuleName;
        uint32_t HeaderDTypeMask{0};
        uint32_t HeaderDType{0};
        uint32_t HeaderDTypeShift{0};
        uint32_t LowTimestampDTypeMask{0};
        uint32_t LowTimestampDType{0};
        uint32_t LowTimestampDTypeShift{0};
        uint32_t HighTimestampDTypeMask{0};
        uint32_t HighTimestampDType{0};
        uint32_t HighTimestampDTypeShift{0};
        uint32_t DataDTypeMask{0};
        uint32_t DataDType{0};
        uint32_t DataDTypeShift{0};
        uint32_t OptDTypeMask{0};
        uint32_t OptDType{0};
        uint32_t OptDTypeShift{0};
        uint32_t ID_Mask{0};
        uint32_t ID_Shift{0};
        uint32_t ADC_RES_Mask{0};
        uint32_t ADC_RES_Shift{0};
        uint32_t TDC_RES_Mask{0};
        uint32_t TDC_RES_Shift{0};
        uint32_t LowTimestamp_Mask{0};
        uint32_t LowTimestamp_Shift{0};
        uint32_t HighTimestamp_Mask{0};
        uint32_t HighTimestamp_Shift{0};
        uint32_t Channel_Mask{0};
        uint32_t Channel_Shift{0};
        uint32_t Data_Mask{0};
        uint32_t Data_Shift{0};
        uint32_t Overflow_Mask{0};
        uint32_t Overflow_Shift{0};
        uint32_t Pileup_Mask{0};
        uint32_t Pileup_Shift{0};
        uint32_t OutOfRange_Mask{0};
        uint32_t OutOfRange_Shift{0};
        uint32_t Reset_Mask{0};
        uint32_t Reset_Shift{0};
        uint32_t Opt1_Mask{0};
        uint32_t Opt1_Shift{0};
        uint32_t Opt2_Mask{0};
        uint32_t Opt2_Shift{0};


        
};



#endif