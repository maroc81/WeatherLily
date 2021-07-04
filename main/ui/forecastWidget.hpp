//******************************************************************************
//! \file forecastWidget.hpp
//!
//!
//!
//!
//******************************************************************************
#ifndef forecastWidget_hpp
#define forecastWidget_hpp

#include "../weather/openWeather.hpp"

#include <lvgl.h>

#include <time.h>

//******************************************************************************
//! class forecastWidgetT
//
//! Widget that holds a forecast consisting of (from top to bottom)
//! - Day of week (e.g. Thursday)
//! - Forecast icon
//! - Low/high temps
//! - Precipitation chance
//!
//******************************************************************************
class forecastWidgetT
{
        //### CONSTRUCTOR/DESTRUCTOR ###//
    public:
        forecastWidgetT(lv_obj_t * parentIP);

        ~forecastWidgetT()
        { }


        //### ATTRIBUTES ###//
    public:


    protected:


        //### OPERATIONS ###//
    public:

        //! Set the forecast date time label
        void SetDate(const time_t & dtIRC);

        //! Update labels, icons, etc with values from daily forecast
        void Update(const dailyT & dailyIRC);

        //! Set icon source
        void SetImgSrc(const lv_img_dsc_t * imgSrcIP);

    protected:


        //### DATA MEMBERS ###//
    protected:
        //! The parent this forecast is attached to
        lv_obj_t * parentMP;

        lv_style_t styleM;

        //! Container that holds the forecast
        lv_obj_t * conM;

        //! The forecast image/icon
        lv_obj_t * imgM;

        //! The label holding the forecast day
        lv_obj_t * lblDayM;

        //! Label holding the low / high temp
        lv_obj_t * lblTempM;

        //! Label holding the precipitation chance
        lv_obj_t * lblPercipM;


}; // end forecastWidgetT


#endif // forecastWidget_hpp
