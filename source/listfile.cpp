/* listfile class - a class for MVME formatted listfiles
 * This class will be used in the dumper-to-root project
 *
 * 
 * Copyright 2022 Justin Warren <jw402216@ohio.edu>
 * 
 * Listfile descriptors and format Copyright (C) 2017  Florian Lüke <f.lueke@mesytec.com>
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
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include "listfile.h"


listfile::listfile() // default constructor for listfile()
{
            Version = 0xffffffff;
            FourCC = "";
            FirstSectionOffset = 0xffffffff;
            SectionMaxWords  = 0xffffffff;
            SectionMaxSize   = 0xffffffff;
            SectionTypeMask  = 0xffffffff;
            SectionTypeShift = 0xffffffff;
            SectionSizeMask  = 0xffffffff;
            SectionSizeShift = 0xffffffff;
            EventTypeMask    = 0xffffffff;
            EventTypeShift   = 0xffffffff;
            CrateIndexMask   = 0xffffffff;
            CrateIndexShift   = 0xffffffff;
            ModuleTypeMask  = 0xffffffff;
            ModuleTypeShift = 0xffffffff;
            SubEventMaxWords  = 0xffffffff;
            SubEventMaxSize   = 0xffffffff;
            SubEventSizeMask  = 0xffffffff;
            SubEventSizeShift = 0xffffffff;
}

listfile::~listfile()
{

}

int listfile::setTemplate(std::string template_filename)
{
    fs::path basedir = fs::absolute(installpath);
    std::string templatedirname = basedir.string() + "/share/templates/listfile/" + template_filename;
    fs::path templatedir = fs::absolute(templatedirname);

    std::ifstream template_file_stream;
    template_file_stream.open(templatedir);


    if(template_file_stream.is_open())
    {
        json template_file;
        template_file_stream >> template_file;

        // Check to see if it is a valid template file
        bool is_right_json_file = template_file.contains("Version") && template_file.contains("FourCC") && template_file.contains("FirstSectionOffset")
                                  && template_file.contains("SectionMaxWords") && template_file.contains("SectionMaxWords") && template_file.contains("SectionMaxSize")
                                  && template_file.contains("SectionTypeMask") && template_file.contains("SectionTypeShift") && template_file.contains("SectionSizeMask")
                                  && template_file.contains("SectionSizeShift") && template_file.contains("EventTypeMask") && template_file.contains("EventTypeShift")
                                  && template_file.contains("CrateIndexMask") && template_file.contains("CrateIndexShift") && template_file.contains("ModuleTypeMask")
                                  && template_file.contains("ModuleTypeShift") && template_file.contains("SubEventMaxWords") && template_file.contains("SubEventMaxSize")
                                  && template_file.contains("SubEventSizeMask") && template_file.contains("SubEventSizeShift");

        if(is_right_json_file)
        {
            Version = (uint32_t)template_file["Version"];
            FourCC = (std::string)template_file["FourCC"];
            FirstSectionOffset = (uint32_t)template_file["FirstSectionOffset"];
            SectionMaxWords  = (uint32_t)template_file["SectionMaxWords"];
            SectionMaxSize   = (uint32_t)template_file["SectionMaxSize"];
            SectionTypeMask  = (uint32_t)template_file["SectionTypeMask"];
            SectionTypeShift = (uint32_t)template_file["SectionTypeShift"];
            SectionSizeMask  = (uint32_t)template_file["SectionSizeMask"];
            SectionSizeShift = (uint32_t)template_file["SectionSizeShift"];
            EventTypeMask    = (uint32_t)template_file["EventTypeMask"];
            EventTypeShift   = (uint32_t)template_file["EventTypeShift"];
            CrateIndexMask   = (uint32_t)template_file["CrateIndexMask"];
            CrateIndexShift   = (uint32_t)template_file["CrateIndexShift"];
            ModuleTypeMask  = (uint32_t)template_file["ModuleTypeMask"];
            ModuleTypeShift = (uint32_t)template_file["ModuleTypeShift"];
            SubEventMaxWords  = (uint32_t)template_file["SubEventMaxWords"];
            SubEventMaxSize   = (uint32_t)template_file["SubEventMaxSize"];
            SubEventSizeMask  = (uint32_t)template_file["SubEventSizeMask"];
            SubEventSizeShift = (uint32_t)template_file["SubEventSizeShift"];

            return 0;
        }
        std::string badfilearg = "Listfile template file <" + templatedir.string() + "> is invalid. Please check template file.\n";
        throw std::runtime_error(badfilearg);
    }
    std::string filednearg = "Tried to use listfile template file <" + templatedir.string() + ">; filename is invalid or file does not exist.\n";
    throw std::runtime_error(filednearg);
    return -1;
} 

void listfile::printListfileSetting() // pretty prints all listfile variables including filename
{
    std::string separator;
    separator.resize(50);
    std::fill(separator.begin(), separator.end(), '-');
    printf("\n%s\n",separator.c_str());
    printf("Version: %u\n",getVersion());
    printf("FourCC: %s\n", getFourCC().c_str());
    printf("FirstSectionOffset: %u\n",getFirstSectionOffset());
    printf("SectionMaxWords: 0x%x\n",getSectionMaxWords());
    printf("SectionMaxSize: 0x%x\n",getSectionMaxSize());
    printf("SectionTypeMask: 0x%x\n",getSectionTypeMask());
    printf("SectionTypeShift: %u\n",getSectionTypeShift());
    printf("SectionSizeMask: 0x%x\n",getSectionSizeMask());
    printf("SectionSizeShift: %u\n",getSectionSizeShift());
    printf("EventTypeMask: 0x%x\n",getEventTypeMask());
    printf("EventTypeShift: %u\n",getEventTypeShift());
    printf("CrateIndexMask: 0x%x\n",getCrateIndexMask());
    printf("CrateIndexShift: %u\n",getCrateIndexShift());
    printf("ModuleTypeMask: 0x%x\n",getModuleTypeMask());
    printf("ModuleTypeShift: %u\n",getModuleTypeShift());
    printf("SubEventMaxWords: 0x%x\n",getSubEventMaxWords());
    printf("SubEventMaxSize: 0x%x\n",getSubEventMaxSize());
    printf("SubEventSizeMask: 0x%x\n",getSubEventSizeMask());
    printf("SubEventSizeShift: %u\n",getSubEventSizeShift());
    printf("%s\n",separator.c_str());
}

void listfile::setFilename(std::string newFilename) // set Filename - available to set output file name
{
    Filename = newFilename;
}

std::map<uint32_t, std::tuple<std::string,VMEModuleType>> listfile::getTreename()  // return ROOT Tree names
{
    return RootTreeNames;
}

void listfile::setTreenames(std::map<uint32_t, std::tuple<std::string,VMEModuleType>> treenames) // set map for ROOT Tree names
{
    RootTreeNames = treenames;
}

uint64_t listfile::getTimeticks() // Get the number of time ticks in the file
{
    return TimeTicks;
}

void listfile::setTimeticks(uint64_t numTimeticks) // Set the number of time ticks in the file
{
    TimeTicks = numTimeticks;
}
