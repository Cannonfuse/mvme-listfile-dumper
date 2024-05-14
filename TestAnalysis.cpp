#include "TFile.h"
#include "TTree.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TCanvas.h"
#include <vector>
#include "/home/justinwarren/Software/Installs/mvme-listfile-dumper/include/modules.h"

// R__LOAD_LIBRARY(lib/libmodules.so)

void TestAnalysis()
{
    auto processeddata = TFile::Open("C7LYC_Test_035.root","read");
    auto dataread = processeddata->Get<TTree>("MDPP-16_QDC_1");
    // mdpp16_qdc testmodule;
    // mdpp16_qdc *testmodule = (mdpp16_qdc *)malloc(sizeof(mdpp16_qdc));
    std::unique_ptr<mdpp16_qdc> testmodule;
    // mdpp16_qdc *testmodule = &testmodule_start;

    // dataread->SetBranchAddress("DATA",&testmodule);
    // dataread->Print();

    TBranch *testbranch = dataread->GetBranch("DATA");
    testbranch->SetAddress(&testmodule);
    testbranch->Print();

    uint32_t ID,SHORT,LONG,TOF,ROUTE_EVT,TRIGGER,ROUTE_TRIGGER;
    uint64_t TIMESTAMP;



    TH1I *TRIGGERHIST = new TH1I("LONG","LONG",65535,0,4096);
    TH2I *TIMEPHHIST = new TH2I("TIMEPH","TRTIMEPHIGGER",16384,0,16384,1024,0,4096);


    std::vector<mdpp16_qdc> tofill;

    for(auto i = 0; i < testbranch->GetEntries(); ++i)
    {
        testbranch->GetEntry(i);
        // printf("MADE IT HERE\n");

        tofill.push_back(*testmodule);
        // if(testmodule->TRIGGER > 65535)
        // {
        //     counter++;
        //     // ID = testmodule->ID;
        //     // SHORT = testmodule->SHORT;
        //     // LONG = testmodule->LONG;
        //     // ROUTE_EVT = testmodule->ROUTE_EVT;

        //     // ROUTE_TRIGGER = testmodule->ROUTE_TRIGGER;
        //     // TIMESTAMP = testmodule->TIMESTAMP;
            // printf("ID = %u\n", testmodule->ID);
            // printf("SHORT = %u\n",testmodule->SHORT);
            // printf("LONG = %u\n",testmodule->LONG);
            // printf("TOF = %u\n",testmodule->TOF);
            // printf("ROUTE_EVT = %u\n",testmodule->ROUTE_EVT);
            // printf("TRIGGER = %u\n",testmodule->TRIGGER);
            // printf("ROUTE_TRIGGER = %u\n",testmodule->ROUTE_TRIGGER);
            // printf("TIMESTAMP = %lu\n",testmodule->TIMESTAMP);
        // }
    }

    for(auto i = 0; i < tofill.size(); ++i)
    {
        if(tofill.at(i).ROUTE_EVT == 0)
        {
            TRIGGER = tofill.at(i).TRIGGER + tofill.at(i).TOF;
            LONG = tofill.at(i).LONG;
            TIMESTAMP = tofill.at(i).TIMESTAMP;

            TRIGGERHIST->Fill(LONG);
            TIMEPHHIST->Fill(TRIGGER,LONG);
        }
    }

    // printf("Total bad events = %u\n",counter);

    auto c1 = new TCanvas();
    TRIGGERHIST->Draw();
    c1->Modified();
    c1->Update();

    auto c2 = new TCanvas();
    TIMEPHHIST->Draw("COLZ");
    c2->Modified();
    c2->Update();

}