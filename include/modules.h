#include <cstdint>
#include "TROOT.h"

#ifndef MVMEMODULES
#define MVMEMODULES

class mdpp16_qdc
{
    public:

        mdpp16_qdc();
        ~mdpp16_qdc();

        uint32_t SHORT;
        bool SHORT_OVERFLOW;
        uint32_t LONG;
        bool LONG_OVERFLOW;
        uint32_t TOF;
        uint32_t ROUTE_EVT;
        uint32_t TRIGGER;
        uint32_t ROUTE_TRIGGER;
        uint64_t TIMESTAMP;
        uint32_t ID;
        uint32_t ADC_RES;
        uint32_t TDC_RES;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;

    ClassDef(mdpp16_qdc, 1)
};

class mdpp16_scp
{
    public:

        mdpp16_scp();
        ~mdpp16_scp();

        uint32_t ADC;
        bool PILEUP;
        bool OVERFLOW;
        uint32_t TOF;
        uint32_t ROUTE_EVT;
        uint32_t TRIGGER;
        uint32_t ROUTE_TRIGGER;
        uint64_t TIMESTAMP;
        uint32_t ID;
        uint32_t ADC_RES;
        uint32_t TDC_RES;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;
        
    ClassDef(mdpp16_scp, 1)

};

class mdpp16_rcp
{
    public:
        mdpp16_rcp();
        ~mdpp16_rcp();

        uint32_t ADC;
        bool PILEUP;
        bool OVERFLOW;
        bool IS_RESET;
        uint32_t TOF;
        uint32_t ROUTE_EVT;
        uint32_t TRIGGER;
        uint32_t ROUTE_TRIGGER;
        uint64_t TIMESTAMP;
        uint32_t ID;
        uint32_t ADC_RES;
        uint32_t TDC_RES;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;

    ClassDef(mdpp16_rcp, 1)

};

class mdpp16_padc
{
    public:

        mdpp16_padc();
        ~mdpp16_padc();

        uint32_t PA;
        bool OVERFLOW;
        uint32_t TOF;
        uint32_t ROUTE_EVT;
        uint32_t TRIGGER;
        uint32_t ROUTE_TRIGGER;
        uint64_t TIMESTAMP;
        uint32_t ID;
        uint32_t ADC_RES;
        uint32_t TDC_RES;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;
             
    ClassDef(mdpp16_padc, 1)

};

class mdpp16_csi
{
    public:

        mdpp16_csi();
        ~mdpp16_csi();

        uint32_t SHORT;
        bool SHORT_OVERFLOW;
        uint32_t LONG;
        bool LONG_OVERFLOW;
        uint32_t TOF;
        uint32_t ROUTE_EVT;
        uint32_t TRIGGER;
        uint32_t ROUTE_TRIGGER;
        uint64_t TIMESTAMP;
        uint32_t ID;
        uint32_t TDC_RES;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;
             
    ClassDef(mdpp16_csi, 1)

};

class mdpp32_qdc
{
    public:

        mdpp32_qdc();
        ~mdpp32_qdc();

        uint32_t SHORT;
        bool SHORT_OVERFLOW;
        uint32_t LONG;
        bool LONG_OVERFLOW;
        uint32_t TOF;
        uint32_t ROUTE_EVT;
        uint32_t TRIGGER;
        uint32_t ROUTE_TRIGGER;
        uint64_t TIMESTAMP;
        uint32_t ID;
        uint32_t ADC_RES;
        uint32_t TDC_RES;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;
             
    ClassDef(mdpp32_qdc, 1)

};

class mdpp32_scp
{
    public:

        mdpp32_scp();
        ~mdpp32_scp();

        uint32_t ADC;
        bool PILEUP;
        bool OVERFLOW;
        uint32_t TOF;
        uint32_t ROUTE_EVT;
        uint32_t TRIGGER;
        uint32_t ROUTE_TRIGGER;
        uint64_t TIMESTAMP;
        uint32_t ID;
        uint32_t ADC_RES;
        uint32_t TDC_RES;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;
             
    ClassDef(mdpp32_scp, 1)

};

class mdpp32_padc
{
    public:

        mdpp32_padc();
        ~mdpp32_padc();

        uint32_t PA;
        bool OVERFLOW;
        uint32_t TOF;
        uint32_t ROUTE_EVT;
        uint32_t TRIGGER;
        uint32_t ROUTE_TRIGGER;
        uint64_t TIMESTAMP;
        uint32_t ID;
        uint32_t ADC_RES;
        uint32_t TDC_RES;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;
             
    ClassDef(mdpp32_padc, 1)

};

class mtdc32
{
    public:

        mtdc32();
        ~mtdc32();

        uint32_t TOF;
        uint32_t ROUTE_TOF;
        uint32_t TDC_RES;
        uint32_t TRIGGER;
        uint32_t ROUTE_TRIGGER;
        uint64_t TIMESTAMP;
        uint32_t ID;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;
             
    ClassDef(mtdc32, 1)

};

class madc32
{
    public:

        madc32();
        ~madc32();

        uint32_t ADC;
        uint32_t ROUTE_ADC;
        bool OutOfRange;
        uint64_t TIMESTAMP;
        uint32_t ID;
        uint32_t ADC_RES;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;
             
    ClassDef(madc32, 1)

};

class mqdc32
{
    public:

        mqdc32();
        ~mqdc32();

        uint32_t ADC;
        uint32_t ROUTE_ADC;
        bool OutOfRange;
        uint64_t TIMESTAMP;
        uint32_t ID;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;
             
    ClassDef(mqdc32, 1)

};

class vmmr
{
    public:

        vmmr();
        ~vmmr();

        uint32_t ADC;
        uint32_t ADC_BUS;
        uint32_t ROUTE_EVT;
        uint32_t TOF;
        uint32_t TOF_BUS;
        bool TRIGGER;
        uint32_t ID;
        uint64_t TIMESTAMP;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;
             
    ClassDef(vmmr, 1)

};

class mdi2
{
    public:

        mdi2();
        ~mdi2();

        uint32_t ADC;
        uint32_t ADC_BUS;
        bool OutOfRange;
        uint32_t SAMPLENUMBER;
        uint32_t ID;
        uint64_t TIMESTAMP;
        uint32_t CRATE_INDEX;
        uint32_t EVENT_INDEX;
             
    ClassDef(mdi2, 1)

};

#endif