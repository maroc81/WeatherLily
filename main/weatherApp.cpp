//******************************************************************************
//! \file weatherApp.cpp
//!
//******************************************************************************



#include "weatherApp.hpp"


//******************************************************************************
//! weatherAppT() -- Constructor
//
//!
//!
//******************************************************************************
weatherAppT::weatherAppT() :
    curWeatherM(),
    dailyMA(4)
{

} // end weatherAppT() - Constructor


//******************************************************************************
//! InitUi()
//
//!
//******************************************************************************
void weatherAppT::InitUi(lv_obj_t * scrIP)
{
    curCondViewM.Init(scrIP);

} // end InitUi()


//******************************************************************************
//! UpdateWeather()
//
//!
//******************************************************************************
void weatherAppT::UpdateWeather()
{
    openWeatherM.FetchWeather(curWeatherM);
    openWeatherM.FetchDailyForecasts(dailyMA);

} // end UpdateWeather()


//******************************************************************************
//! UpdateWeatherUI()
//
//!
//******************************************************************************
void weatherAppT::UpdateWeatherUI()
{
    curCondViewM.UpdateCurrentWeather(curWeatherM);
    curCondViewM.UpdateForecasts(dailyMA);

} // end UpdateWeatherUI()


//******************************************************************************
//! UpdateUI()
//
//!
//******************************************************************************
void weatherAppT::UpdateUI()
{
    curCondViewM.UpdateDateTime();

} // end UpdateUI()

