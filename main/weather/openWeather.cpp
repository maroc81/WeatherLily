//******************************************************************************
//! \file openWeather.cpp
//!
//!
//******************************************************************************

#include <array>
#include <algorithm>
#include <cstring>
#include <memory>
#include <vector>

#include "esp_log.h"

#include "appConfig.h"
#include "jsonParser.hpp"
#include "openWeather.hpp"


// Map for processing JSON elements into weatherT
static const std::array<jsonElemT, 17> weatherMapLC = {{
    { "id",          offsetof(weatherT, id),          JT_int,                 4 },
    { "name",        offsetof(weatherT, name),        JT_string,             64 },
    { "lat",         offsetof(weatherT, lat),         JT_float,               4 },
    { "lon",         offsetof(weatherT, lon),         JT_float,               4 },
    { "timezone",    offsetof(weatherT, tzOffset),    JT_int,                 4 },
    { "dt",          offsetof(weatherT, dt),          JT_long,   sizeof(time_t) },
    { "main",        offsetof(weatherT, main),        JT_string,             64 },
    { "description", offsetof(weatherT, desc),        JT_string,            128 },
    { "icon",        offsetof(weatherT, icon),        JT_string,              4 },
    { "temp",        offsetof(weatherT, temp),        JT_float,               4 },
    { "feels_like",  offsetof(weatherT, feelsLike),   JT_float,               4 },
    { "temp_min",    offsetof(weatherT, tempMin),     JT_float,               4 },
    { "temp_max",    offsetof(weatherT, tempMax),     JT_float,               4 },
    { "pressure",    offsetof(weatherT, pressure),    JT_short,               2 },
    { "speed",       offsetof(weatherT, windSpeed),   JT_float,               4 },
    { "humidity",    offsetof(weatherT, humidity),    JT_byte,                1 },
    { "all",         offsetof(weatherT, clouds),      JT_byte,                1 }
    //{ "visibility", offsetof(weatherT, ) },
}};


static const std::array<jsonElemT, 17> dailyMapLC = {{
    { "dt",          offsetof(dailyT, dt),            JT_long,   sizeof(time_t) },
    { "moon_phase",  offsetof(dailyT, moonphase),     JT_float,               4 },
    { "main",        offsetof(dailyT, main),          JT_string,             64 },
    { "description", offsetof(dailyT, desc),          JT_string,            128 },
    { "icon",        offsetof(dailyT, icon),          JT_string,              4 },
    { "sunrise",     offsetof(dailyT, sunrise),       JT_long,   sizeof(time_t) },
    { "sunset",      offsetof(dailyT, sunset),        JT_long,   sizeof(time_t) },
    { "moonrise",    offsetof(dailyT, moonrise),      JT_long,   sizeof(time_t) },
    { "moonset",     offsetof(dailyT, moonset),       JT_long,   sizeof(time_t) },
    { "min",         offsetof(dailyT, tempMin),       JT_float,               4 },
    { "max",         offsetof(dailyT, tempMax),       JT_float,               4 },
    { "pressure",    offsetof(dailyT, pressure),      JT_short,               2 },
    { "humidity",    offsetof(dailyT, humidity),      JT_byte,                1 },
    { "wind_speed",  offsetof(dailyT, windSpeed),     JT_float,               4 },
    { "all",         offsetof(dailyT, clouds),        JT_byte,                1 },
    { "rain",        offsetof(dailyT, rain),          JT_float,               4 },
    { "pop",         offsetof(dailyT, pop),           JT_float,               4 },
}};



// URLs for open weather map apis
static const char * weatherPathLC = "/data/2.5/weather";
static const char * oneCallPathLC = "/data/2.5/onecall";
static const char * forecastPathLC = "/data/2.5/forecast";

//******************************************************************************
//! Constructor()
//
//!
//******************************************************************************
openWeatherT::openWeatherT() :
    clientM("api.openWeathermap.org", weatherPathLC),
    keyM(APP_OPEN_WEATHER_API_KEY),
    unitsM(APP_UNITS),
    zipM(APP_WEATHER_ZIP)
{

} // end openWeatherT


//******************************************************************************
//! FetchWeather()
//
//!
//******************************************************************************
void openWeatherT::FetchWeather(weatherT & weatherIR)
{
    // Buffer to hold the query, allocated on heap
    auto queryP = std::unique_ptr<std::array<char,128>>(new std::array<char,128>());

    // Build the current weather query
    snprintf(queryP->data(), queryP->size(), "zip=%d&units=%s&appid=%s", zipM, unitsM, keyM);

    // Use vector for response buffer and reserve 1KB
    std::vector<char> respBufVec;
    respBufVec.reserve(1024);

    // Set http client to query current weather
    clientM.SetPath(weatherPathLC);
    clientM.SetQuery(queryP->data());

    // Perform HTTP query and block until query completes
    clientM.Get(respBufVec);

    printf("buff size %d\n", respBufVec.size());
    if( respBufVec.size() > 0 )
    {
        // Create parser to parse the current weather response buffer
        // and then parse buffer into JSMN tokens
        jsonParserT parser(respBufVec);
        parser.ParseIntoTokens();

        // Parse the JSON fields using the current weather map
        // and store the results in the current weather struct
        parser.Parse(weatherMapLC.data(),
                     weatherMapLC.size(),
                     &weatherIR
                    );

        // Capitalize words in description
        CapitalizeWords(&weatherIR.desc[0], sizeof(weatherIR.desc));

        // Save lat and lon for forecast call
        latM = weatherIR.lat;
        lonM = weatherIR.lon;

        printf("id %d\n", weatherIR.id);
        printf("name '%s'\n", weatherIR.name);
        printf("lat %f\n", weatherIR.lat);
        printf("lon %f\n", weatherIR.lon);
        printf("tzOffset %d\n", weatherIR.tzOffset);
        printf("dt %ld\n", weatherIR.dt);
        printf("main '%s'\n", weatherIR.main);
        printf("desc '%s'\n", weatherIR.desc);
        printf("icon '%s'\n", weatherIR.icon);
        printf("temp %f\n", weatherIR.temp);
        printf("feels like %f\n", weatherIR.feelsLike);
        printf("tempMin %f\n", weatherIR.tempMin);
        printf("tempMax %f\n", weatherIR.tempMax);
        printf("pressure %hd\n", weatherIR.pressure);
        printf("humidity %d\n", weatherIR.humidity);
        printf("clouds %d\n", weatherIR.clouds);
        printf("wind speed %f\n", weatherIR.windSpeed);
    }

} // end FetchWeather()


//******************************************************************************
//! FetchDailyForecasts()
//
//!
//******************************************************************************
void openWeatherT::FetchDailyForecasts(std::vector<dailyT> & dailyIRA)
{
    // Buffer to hold the query
    std::array<char, 192> query;

    // Use the "One Call" API in order to get daily forecasts, but exclude current weather, minutely, and hourly forecasts
    snprintf(&query[0], query.size(), "lat=%f&lon=%f&exclude=current,minutely,hourly&units=%s&appid=%s", latM, lonM, unitsM, keyM);

    // Use vector for response buffer and reserve 4KB
    std::vector<char> respBufVec;
    respBufVec.reserve(4096);

    // Set http client for one call
    clientM.SetPath(oneCallPathLC);
    clientM.SetQuery(&query[0]);
    clientM.Get(respBufVec);

    if( respBufVec.size() > 0 )
    {
        // Create vector to hold pointers to each element in the daily forecast
        std::vector<void *> dailyPointersA(dailyIRA.size());

        // Assign the pointers to the vector
        for(int idX = 0; idX < dailyIRA.size(); idX++)
        {
            dailyPointersA.at(idX) = &dailyIRA.at(idX);
        }

        // Create the parser to parse the weather response
        jsonParserT parser(respBufVec);

        // Parse the buffer into JSMN tokens
        parser.ParseIntoTokens();

        // Parse elements in the reponse under the "daily" field
        // using the given daily map and save them into the forecasts
        parser.ParseArray("daily",
                          dailyMapLC.data(),
                          dailyMapLC.size(),
                          dailyPointersA
                         );

        // Print forecast results
        for(auto & daily : dailyIRA)
        {
            CapitalizeWords(&daily.desc[0], sizeof(daily.desc));
            printf("main '%s'\n", daily.main);
            printf("desc '%s'\n", daily.desc);
            printf("icon '%s'\n", daily.icon);
            printf("tempMin %f\n", daily.tempMin);
            printf("tempMax %f\n", daily.tempMax);
            printf("pressure %hd\n", daily.pressure);
            printf("humidity %d\n", daily.humidity);
            printf("clouds %d\n", daily.clouds);
            printf("wind speed %f\n", daily.windSpeed);
            printf("rain %f\n", daily.rain);
            printf("pop %f\n", daily.pop);
        }

        //parser.PrintTokens();
    }

} // end FetchDailyForecasts()


//******************************************************************************
//! CapitalizeWords()
//
//!
//******************************************************************************
void openWeatherT::CapitalizeWords(char * wordsIP, const size_t lenIC)
{
    // Capitalize words in description
    for( int idX = 0; wordsIP[idX] != '\0' && idX < lenIC; idX++)
    {
        if( idX == 0 )
        {
            wordsIP[idX] = toupper(wordsIP[idX]);
        }
        else if( wordsIP[idX-1] == ' ')
        {
            wordsIP[idX] = toupper(wordsIP[idX]);
        }
    }

} // end CapitalizeWords()

