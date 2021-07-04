
//******************************************************************************
//! \file curConditionsView.hpp
//!
//!
//!
//!
//******************************************************************************
#ifndef curConditionsView_hpp
#define curConditionsView_hpp

#include "../weather/openWeather.hpp"
#include "forecastWidget.hpp"

#include <lvgl.h>


//******************************************************************************
//! class curConditionsViewT
//
//! View that shows
//!  - Date and Time on top
//!  - Current conditions in the middle
//!  - Forecast at the bottom
//
//******************************************************************************
class curConditionsViewT
{
        //### CONSTRUCTOR/DESTRUCTOR ###//
    public:
        curConditionsViewT();

        ~curConditionsViewT()
        { }


        //### ATTRIBUTES ###//
    public:


    protected:


        //### OPERATIONS ###//
    public:
        //! Create UI objects
        void Init(lv_obj_t * scrIP);

        //! Update display with latest date time
        void UpdateDateTime();

        void UpdateImgSrc();

        //! Update display with current weather values
        void UpdateCurrentWeather(const weatherT & weatherIRC);

        //! Update forecast displays
        void UpdateForecasts(const std::vector<dailyT> & dailyIRAC);


    protected:


        //### DATA MEMBERS ###//
    protected:
        //! The screen this view occupies
        lv_obj_t * scrM;

        //! Container to hold top, middle, and bottom in a column
        lv_obj_t * conAllM;

        //! Top container holding the date time
        lv_obj_t * conTopM;

        //! Middle container holding current conditions
        lv_obj_t * conMidM;

        //! Bottom container holding forecasts
        lv_obj_t * conBotM;

        //! The label containing the current date and time
        lv_obj_t * lblDateTimeM;

        //! Containers for 3 columns holding current conditions
        lv_obj_t * conTempM;
        lv_obj_t * conCondsM;

        //! Current weather icon
        lv_obj_t * imgCurWeatherM;

        //! Current temp label
        lv_obj_t * lblTempM;

        //! Current weather description label
        lv_obj_t * lblDescM;

        //! Labels for current weather stats
        lv_obj_t * lblPressM;
        lv_obj_t * lblHumidM;
        lv_obj_t * lblCloudsM;
        lv_obj_t * lblWindSpeedM;

        //! Forecast widgets along the bottom
        std::array<forecastWidgetT *, 4> fcastsMA;

        //! Style
        lv_style_t styleM;

        //! Date Time style
        lv_style_t styleDtM;


}; // end curConditionsViewT


#endif // curConditionsView_hpp
