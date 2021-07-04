//******************************************************************************
//! \file weatherApp.hpp
//!
//******************************************************************************
#ifndef weatherApp_hpp
#define weatherApp_hpp

#include <vector>

#include "weather/openWeather.hpp"
#include "ui/curConditionsView.hpp"


//******************************************************************************
//! class weatherAppT
//
//!
//******************************************************************************
class weatherAppT
{
        //### CONSTRUCTOR/DESTRUCTOR ###//
    public:
        weatherAppT();

        virtual ~weatherAppT()
        { }


        //### ATTRIBUTES ###//
    public:
        //! Reference to last fetched current weather conditions
        weatherT & CurrentWeather()
        { return( curWeatherM ); }


    protected:


        //### OPERATIONS ###//
    public:
        //! Initialize UI components
        void InitUi(lv_obj_t * scrIP);

        //! Updates UI
        void UpdateUI();

        //! Updates UI with latest weather
        void UpdateWeatherUI();

        //! Fetches current weather and forecasts and saves them
        void UpdateWeather();


    protected:


        //### DATA MEMBERS ###//
    protected:
        //! Client for fetching weather from the open weather map service
        openWeatherT openWeatherM;

        //! UI for current conditions and forecast
        curConditionsViewT curCondViewM;

        //! Struct holding current weather
        weatherT curWeatherM;

        //! Vector of daily forecasts
        std::vector<dailyT> dailyMA;

}; // end weatherAppT


#endif // weatherApp_hpp
