#include "modules.h"

ClassImp(mdpp16_qdc)

mdpp16_qdc::mdpp16_qdc()
{
    SHORT = 0;
    SHORT_OVERFLOW = false;
    LONG = 0;
    LONG_OVERFLOW = false;
    TOF = 0;
    ROUTE_EVT = 0;
    TRIGGER = 0;
    ROUTE_TRIGGER = 0;
    TIMESTAMP = 0;
    ID = 0;
    ADC_RES = 0;
    TDC_RES = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

mdpp16_qdc::~mdpp16_qdc()
{
    
}

ClassImp(mdpp16_scp)


mdpp16_scp::mdpp16_scp()
{
    ADC = 0;
    PILEUP = false;
    OVERFLOW = false;
    TOF = 0;
    ROUTE_EVT = 0;
    TRIGGER = 0;
    ROUTE_TRIGGER = 0;
    TIMESTAMP = 0;
    ID = 0;
    ADC_RES = 0;
    TDC_RES = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

mdpp16_scp::~mdpp16_scp()
{

}

ClassImp(mdpp16_rcp)

mdpp16_rcp::mdpp16_rcp()
{
    ADC = 0;
    PILEUP = false;
    OVERFLOW = false;
    IS_RESET = false;
    TOF = 0;
    ROUTE_EVT = 0;
    TRIGGER = 0;
    ROUTE_TRIGGER = 0;
    TIMESTAMP = 0;
    ID = 0;
    ADC_RES = 0;
    TDC_RES = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

mdpp16_rcp::~mdpp16_rcp()
{

}

ClassImp(mdpp16_padc)

mdpp16_padc::mdpp16_padc()
{
    PA = 0;
    OVERFLOW = false;
    TOF = 0;
    ROUTE_EVT = 0;
    TRIGGER = 0;
    ROUTE_TRIGGER = 0;
    TIMESTAMP = 0;
    ID = 0;
    ADC_RES = 0;
    TDC_RES = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

mdpp16_padc::~mdpp16_padc()
{

}

ClassImp(mdpp16_csi)

mdpp16_csi::mdpp16_csi()
{
    SHORT = 0;
    SHORT_OVERFLOW = false;
    LONG = 0;
    LONG_OVERFLOW = false;
    TOF = 0;
    ROUTE_EVT = 0;
    TRIGGER = 0;
    ROUTE_TRIGGER = 0;
    TIMESTAMP = 0;
    ID = 0;
    TDC_RES = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

mdpp16_csi::~mdpp16_csi()
{

}

ClassImp(mdpp32_qdc)

mdpp32_qdc::mdpp32_qdc()
{
    SHORT = 0;
    SHORT_OVERFLOW = false;
    LONG = 0;
    LONG_OVERFLOW = false;
    TOF = 0;
    ROUTE_EVT = 0;
    TRIGGER = 0;
    ROUTE_TRIGGER = 0;
    TIMESTAMP = 0;
    ID = 0;
    ADC_RES = 0;
    TDC_RES = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

mdpp32_qdc::~mdpp32_qdc()
{

}

ClassImp(mdpp32_scp)

mdpp32_scp::mdpp32_scp()
{
    ADC = 0;
    PILEUP = false;
    OVERFLOW = false;
    TOF = 0;
    ROUTE_EVT = 0;
    TRIGGER = 0;
    ROUTE_TRIGGER = 0;
    TIMESTAMP = 0;
    ID = 0;
    ADC_RES = 0;
    TDC_RES = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

mdpp32_scp::~mdpp32_scp()
{

}

ClassImp(mdpp32_padc)

mdpp32_padc::mdpp32_padc()
{
    PA = 0;
    OVERFLOW = false;
    TOF = 0;
    ROUTE_EVT = 0;
    TRIGGER = 0;
    ROUTE_TRIGGER = 0;
    TIMESTAMP = 0;
    ID = 0;
    ADC_RES = 0;
    TDC_RES = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

mdpp32_padc::~mdpp32_padc()
{

}

ClassImp(mtdc32)

mtdc32::mtdc32()
{
    TOF = 0;
    ROUTE_TOF = 0;
    TDC_RES = 0;
    TRIGGER = 0;
    ROUTE_TRIGGER = 0;
    TIMESTAMP = 0;
    ID = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

mtdc32::~mtdc32()
{

}

ClassImp(madc32)

madc32::madc32()
{
    ADC = 0;
    ROUTE_ADC = 0;
    ADC_RES = 0;
    OutOfRange = false;
    TIMESTAMP = 0;
    ID = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

madc32::~madc32()
{

}

ClassImp(mqdc32)

mqdc32::mqdc32()
{
    ADC = 0;
    ROUTE_ADC = 0;
    OutOfRange = false;
    TIMESTAMP = 0;
    ID = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

mqdc32::~mqdc32()
{

}

ClassImp(vmmr)

vmmr::vmmr()
{
    ADC = 0;
    ADC_BUS = 0;
    ROUTE_EVT = 0;
    TOF = 0;
    TOF_BUS = 0;
    ID = 0;
    TIMESTAMP = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

vmmr::~vmmr()
{

}

ClassImp(mdi2)

mdi2::mdi2()
{
    ADC = 0;
    ADC_BUS = 0;
    OutOfRange = false;
    SAMPLENUMBER = 0;
    ID = 0;
    TIMESTAMP = 0;
    CRATE_INDEX = 0;
    EVENT_INDEX = 0;
}

mdi2::~mdi2()
{
    
}


// ClassImp(mdi_2)

// mdi_2::mdi_2()
// {
//     ADC = 0;
//     ADC_BUS = 0;
//     OutOfRange = false;
//     SAMPLENUMBER = 0;
//     ID = 0;
//     TIMESTAMP = 0;
// }

// mdi_2::~mdi_2()
// {

// }
