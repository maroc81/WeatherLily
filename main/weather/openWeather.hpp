//******************************************************************************
//! \file openWeather.hpp
//!
//!
//!
//!
//******************************************************************************
#ifndef openWeather_hpp
#define openWeather_hpp

#include <array>
#include <vector>

#include <time.h>


//******************************************************************************
//! struct weatherT
//
//! Holds the current weather conditions.
//
//******************************************************************************
struct weatherT
{
    //! City ID
    int id;

    //! City Name
    char name[64];

    //! City latitude
    float lat;

    //! City longitude
    float lon;

    //! Offset in seconds from UTC
    int tzOffset;

    //! The date time the conditions apply to
    time_t dt;

    //! The single word description of current conditions
    char main[64];

    //! Simple description of current conditions
    char desc[128];

    //! Icon to use for conditions
    char icon[4];

    //! Temperature
    float temp;

    //! Feels like temp
    float feelsLike;

    //! Min temp measured within large cities
    float tempMin;

    //! Max temp measured within large cities
    float tempMax;

    //! Current pressure in hPa
    int16_t pressure;

    //! Humidity in percent 0-100
    int8_t humidity;

    //! Percent cloud coverage in percent 0 - 100
    int8_t clouds;

    //! Wind speed in miles per hour for imperial, meters per sec for all others
    float windSpeed;
};

struct tempT {
    float min;
    float max;
    float morn;
    float day;
    float eve;
    float night;
};

struct dailyT {
    time_t dt;
    char main[64];
    char desc[128];
    char icon[4];
    time_t sunrise;
    time_t sunset;
    time_t moonrise;
    time_t moonset;
    float moonphase;
    float tempMin;
    float tempMax;
    int16_t pressure;
    int humidity;
    int8_t clouds;

    // Rain in mm
    float rain;

    // Probability of precipitation
    float pop;

    float windSpeed;
};


#include "net/httpClient.hpp"

//******************************************************************************
//! class openWeatherT
//
//!
//******************************************************************************
class openWeatherT
{
        //### CONSTRUCTOR/DESTRUCTOR ###//
    public:
        openWeatherT();

        ~openWeatherT()
        { }


        //### ATTRIBUTES ###//
    public:


    protected:


        //### OPERATIONS ###//
    public:
        //! Fetches current weather and stores it in the given weatherT
        void FetchWeather(weatherT & weatherIR);

        //! Fetches daily forecasts and stores in the given dailyT
        void FetchDailyForecasts(std::vector<dailyT> & dailyIRA);

    protected:
        //! Capitalize first character and each character after a space
        void CapitalizeWords(char * wordsIP, const size_t lenIC);


        //### DATA MEMBERS ###//
    protected:

        //! HTTP client for connecting to open weather map API
        httpClientT clientM;

        //! API key needed to access API
        const char * keyM;

        //! Units
        const char * unitsM;

        //! Zip code used for current weather and to get lat/lon for forecasts
        const int zipM;

        //! Latitude for forecast
        float latM;

        //! Longitude for forecast
        float lonM;

}; // end openWeatherT


#endif // openWeather_hpp
