#include "MVMEmodule.h"

MVMEmodule::MVMEmodule()
{
    ModuleType = (VMEModuleType)0;
    ModuleName = ModuleNames.at(ModuleType);
}

MVMEmodule::MVMEmodule(uint32_t moduleType)
{
    try
    {
        ModuleType = (VMEModuleType)moduleType;
        ModuleName = ModuleNames.at(ModuleType);
        setTemplate(ModuleName  + ".json");
    }
    catch(const std::exception& e)
    {
        printf("Invalid module type, setting as \"invalid\".\n");
        ModuleType = (VMEModuleType)0;
        ModuleName = ModuleNames.at(ModuleType);
    }
}

MVMEmodule::~MVMEmodule()
{

}

int MVMEmodule::setModule(int moduleType)
{
    try
    {
        ModuleType = (VMEModuleType)moduleType;
        ModuleName = ModuleNames.at(ModuleType);
        setTemplate(ModuleName + ".json");

    }
    catch(const std::exception& e)
    {
        printf("Invalid module type, setting as \"invalid\".\n");
        ModuleType = (VMEModuleType)0;
        ModuleName = ModuleNames.at(ModuleType);
        return -1;
    }
    return 0;
}

int MVMEmodule::setTemplate(std::string template_filename)
{

    fs::path basedir = fs::absolute(installpath);
    std::string templatedirname = basedir.string() + "/share/templates/modules/" + template_filename;
    fs::path templatedir = fs::absolute(templatedirname);

    std::ifstream template_file_stream;
    template_file_stream.open(templatedir);
    if(template_file_stream.is_open())
    {
        json template_file;
        template_file_stream >> template_file;

        // Check to see if it is a valid template file
        bool is_right_json_file = template_file.contains("HeaderDTypeMask") && template_file.contains("HeaderDType") && template_file.contains("HeaderDTypeShift")
                                  && template_file.contains("LowTimestampDTypeMask") && template_file.contains("LowTimestampDType") && template_file.contains("LowTimestampDTypeShift")
                                  && template_file.contains("HighTimestampDTypeMask") && template_file.contains("HighTimestampDType") && template_file.contains("HighTimestampDTypeShift")
                                  && template_file.contains("DataDTypeMask") && template_file.contains("DataDType") && template_file.contains("DataDTypeShift")
                                  && template_file.contains("ID_Mask") && template_file.contains("ID_Shift") && template_file.contains("ADC_RES_Mask")
                                  && template_file.contains("ADC_RES_Shift") && template_file.contains("TDC_RES_Mask") && template_file.contains("TDC_RES_Shift")
                                  && template_file.contains("LowTimestamp_Mask") && template_file.contains("LowTimestamp_Shift") && template_file.contains("Channel_Mask")
                                  && template_file.contains("Channel_Shift") && template_file.contains("Data_Mask") && template_file.contains("Data_Shift")
                                  && template_file.contains("Overflow_Mask") && template_file.contains("Overflow_Shift") && template_file.contains("Pileup_Mask")
                                  && template_file.contains("Pileup_Shift") && template_file.contains("OutOfRange_Mask") && template_file.contains("OutOfRange_Shift")
                                  && template_file.contains("Reset_Mask") && template_file.contains("Reset_Shift") && template_file.contains("HighTimestamp_Mask") 
                                  && template_file.contains("HighTimestamp_Shift") && template_file.contains("OptDTypeMask") && template_file.contains("OptDType")
                                  && template_file.contains("OptDTypeShift") && template_file.contains("Opt1_Mask") && template_file.contains("Opt1_Shift")
                                  && template_file.contains("Opt2_Mask") && template_file.contains("Opt2_Shift") && template_file.contains("VMEModuleType")
                                  && template_file.contains("VMEModuleName");


        if(is_right_json_file)
        {
            ModuleType = (VMEModuleType)(uint32_t)template_file["VMEModuleType"];
            ModuleName = (std::string)template_file["VMEModuleName"];
            HeaderDTypeMask = (uint32_t)template_file["HeaderDTypeMask"];
            HeaderDType = (uint32_t)template_file["HeaderDType"];
            HeaderDTypeShift = (uint32_t)template_file["HeaderDTypeShift"];
            LowTimestampDTypeMask = (uint32_t)template_file["LowTimestampDTypeMask"];
            LowTimestampDType = (uint32_t)template_file["LowTimestampDType"];
            LowTimestampDTypeShift = (uint32_t)template_file["LowTimestampDTypeShift"];
            HighTimestampDTypeMask = (uint32_t)template_file["HighTimestampDTypeMask"];
            HighTimestampDType = (uint32_t)template_file["HighTimestampDType"];
            HighTimestampDTypeShift = (uint32_t)template_file["HighTimestampDTypeShift"];
            DataDTypeMask = (uint32_t)template_file["DataDTypeMask"];
            DataDType = (uint32_t)template_file["DataDType"];
            DataDTypeShift = (uint32_t)template_file["DataDTypeShift"];
            OptDTypeMask = (uint32_t)template_file["OptDTypeMask"];
            OptDType = (uint32_t)template_file["OptDType"];
            OptDTypeShift = (uint32_t)template_file["OptDTypeShift"];
            ID_Mask = (uint32_t)template_file["ID_Mask"];
            ID_Shift = (uint32_t)template_file["ID_Shift"];
            ADC_RES_Mask = (uint32_t)template_file["ADC_RES_Mask"];
            ADC_RES_Shift = (uint32_t)template_file["ADC_RES_Shift"];
            TDC_RES_Mask = (uint32_t)template_file["TDC_RES_Mask"];
            TDC_RES_Shift = (uint32_t)template_file["TDC_RES_Shift"];
            LowTimestamp_Mask = (uint32_t)template_file["LowTimestamp_Mask"];
            LowTimestamp_Shift = (uint32_t)template_file["LowTimestamp_Shift"];
            HighTimestamp_Mask = (uint32_t)template_file["HighTimestamp_Mask"];
            HighTimestamp_Shift = (uint32_t)template_file["HighTimestamp_Shift"];
            Channel_Mask = (uint32_t)template_file["Channel_Mask"];
            Channel_Shift = (uint32_t)template_file["Channel_Shift"];
            Data_Mask = (uint32_t)template_file["Data_Mask"];
            Data_Shift = (uint32_t)template_file["Data_Shift"];
            Overflow_Mask = (uint32_t)template_file["Overflow_Mask"];
            Overflow_Shift = (uint32_t)template_file["Overflow_Shift"];
            Pileup_Mask = (uint32_t)template_file["Pileup_Mask"];
            Pileup_Shift = (uint32_t)template_file["Pileup_Shift"];
            OutOfRange_Mask = (uint32_t)template_file["OutOfRange_Mask"];
            OutOfRange_Shift = (uint32_t)template_file["OutOfRange_Shift"];
            Reset_Mask = (uint32_t)template_file["Reset_Mask"];
            Reset_Shift = (uint32_t)template_file["Reset_Shift"];
            Opt1_Mask = (uint32_t)template_file["Opt1_Mask"];
            Opt1_Shift = (uint32_t)template_file["Opt1_Shift"];
            Opt2_Mask = (uint32_t)template_file["Opt2_Mask"];
            Opt2_Shift = (uint32_t)template_file["Opt2_Shift"];
            
            return 0;
        }
        std::string badfilearg = "Module template file <" + templatedir.string() + "> is invalid. Please check template file.\n";
        throw std::runtime_error(badfilearg);
    }
    std::string filednearg = "Tried to use module template file <" + templatedir.string() + ">; filename is invalid or file does not exist.\n";
    throw std::runtime_error(filednearg);
    return -1;
} 

void MVMEmodule::setCrateIndex(uint32_t crateindex) // Set the CrateIndex. Here in case extra logic is necessary for future listfile versions
{
    CrateIndex = crateindex;
}

void MVMEmodule::setEventIndex(uint32_t eventindex) // Set the EventIndex. Here in case extra logic is necessary for future listfile versions
{
    EventIndex = eventindex;
}

void MVMEmodule::printModuleSetting() // pretty prints all listfile variables including filename
{
    std::string separator;
    separator.resize(50);
    std::fill(separator.begin(), separator.end(), '-');
    printf("\n%s\n",separator.c_str());
    printf("ModuleName : %s\n", getModuleName().c_str());
    printf("ModuleType : %u\n", (uint32_t)getModuleType());
    printf("HeaderDTypeMask: 0x%08x\n", getHeaderDTypeMask());
    printf("HeaderDType: %u\n", getHeaderDType());
    printf("HeaderDTypeShift: %u\n", getHeaderDTypeShift());
    printf("LowTimestampDTypeMask: 0x%08x\n", getLowTimestampDTypeMask());
    printf("LowTimestampDType: %u\n", getLowTimestampDType());
    printf("LowTimestampDTypeShift: %u\n", getLowTimestampDTypeShift());
    printf("HighTimestampDTypeMask: 0x%08x\n", getHighTimestampDTypeMask());
    printf("HighTimestampDType: %u\n", getHighTimestampDType());
    printf("HighTimestampDTypeShift: %u\n", getHighTimestampDTypeShift());
    printf("DataDTypeMask: 0x%08x\n", getDataDTypeMask());
    printf("DataDType: %u\n", getDataDType());
    printf("DataDTypeShift: %u\n", getDataDTypeShift());
    printf("OptDTypeMask: 0x%08x\n", getOptDTypeMask());
    printf("OptDType: %u\n", getOptDType());
    printf("OptDTypeShift: %u\n", getOptDTypeShift());
    printf("ID_Mask: 0x%08x\n", getID_Mask());
    printf("ID_Shift: %u\n", getID_Shift());
    printf("ADC_RES_Mask: 0x%08x\n", getADC_RES_Mask());
    printf("ADC_RES_Shift: %u\n", getADC_RES_Shift());
    printf("TDC_RES_Mask: 0x%08x\n", getTDC_RES_Mask());
    printf("TDC_RES_Shift: %u\n", getTDC_RES_Shift());
    printf("LowTimestamp_Mask: 0x%08x\n", getLowTimestamp_Mask());
    printf("LowTimestamp_Shift: %u\n", getLowTimestamp_Shift());
    printf("HighTimestamp_Mask: 0x%08x\n", getHighTimestamp_Mask());
    printf("HighTimestamp_Shift: %u\n", getHighTimestamp_Shift());
    printf("Channel_Mask: 0x%08x\n", getChannel_Mask());
    printf("Channel_Shift: %u\n", getChannel_Shift());
    printf("Data_Mask: 0x%08x\n", getData_Mask());
    printf("Data_Shift: %u\n", getData_Shift());
    printf("Overflow_Mask: 0x%08x\n", getOverflow_Mask());
    printf("Overflow_Shift: %u\n", getOverflow_Shift());
    printf("Pileup_Mask: 0x%08x\n", getPileup_Mask());
    printf("Pileup_Shift: %u\n", getPileup_Shift());
    printf("OutOfRange_Mask: 0x%08x\n", getOutOfRange_Mask());
    printf("OutOfRange_Shift: %u\n", getOutOfRange_Shift());
    printf("Reset_Mask: 0x%08x\n", getReset_Mask());
    printf("Reset_Shift: %u\n", getReset_Shift());
    printf("Opt1_Mask: 0x%08x\n", getOpt1_Mask());
    printf("Opt1_Shift: %u\n", getOpt1_Shift());
    printf("Opt2_Mask: 0x%08x\n", getOpt2_Mask());
    printf("Opt2_Shift: %u\n", getOpt2_Shift());
    printf("%s\n\n",separator.c_str());
}