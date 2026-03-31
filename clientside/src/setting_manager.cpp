#include "setting_manager.h"
#include "DEFAULT_SETTINGS.h"

SettingManager :: SettingManager(void) :
    cs_tuss4470(        CS_TUSS4470                                     ),
    cs_ad9833(          CS_AD9833                                       ),
    cs_burst_control(   CS_BURST_CONTROL                                ),
    regulated_mode(     REGULATED_MODE                                  ), 
    io_mode(            static_cast<IOMode>         (IO_MODE)           ),
    current_mode(       static_cast<CurrentType>    (CURRENT_MODE)      ),
    pre_driver_mode(    static_cast<PreDriverMode>  (PRE_DRIVER_MODE)   ),
    voltage(            VOLTAGE                                         ), 
    pulse_count(        PULSE_COUNT                                     ), 
    frequency(          FREQUENCY                                       ), 
    timeout(            TIMEOUT                                         ), 
    burst_pd(           BURST_PD                                        ),
    stim_pd(            STIM_PD                                         ),
    burst_dc(           BURST_DC                                        ),
    stim_dc(            STIM_DC                                         ) {}
