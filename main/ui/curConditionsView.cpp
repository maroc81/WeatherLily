//******************************************************************************
//! \file curConditionsView.cpp
//!
//!
//!
//******************************************************************************

#include <ctime>
#include <map>

#include "esp_log.h"

#include "icons/icons100.h"
#include "icons/icons128.h"
#include "curConditionsView.hpp"

// Map with the icon id as the key and image pointer as the value
std::array<std::pair<const char*, const lv_img_dsc_t *>, 18> iconMap128LC = {{
    {"01d", &clear128},
    {"01n", &nt_clear128},
    {"02d", &partlycloudy128},
    {"02n", &nt_partlycloudy128},
    {"03d", &mostlycloudy128},
    {"03n", &nt_mostlycloudy128},
    {"04d", &cloudy128},
    {"04n", &nt_cloudy128},
    {"09d", &chancerain128},
    {"09n", &nt_chancerain128},
    {"10d", &rain128},
    {"10n", &nt_rain128},
    {"11d", &tstorms128},
    {"11n", &nt_tstorms128},
    {"13d", &snow128},
    {"13n", &nt_snow128},
    {"50d", &fog128},
    {"50n", &nt_fog128}
}};

// Map with the icon id as the key and image pointer as the value
std::array<std::pair<const char*, const lv_img_dsc_t *>, 18> iconMap100LC = {{
    {"01d", &clear100},
    {"01n", &nt_clear100},
    {"02d", &partlycloudy100},
    {"02n", &nt_partlycloudy100},
    {"03d", &mostlycloudy100},
    {"03n", &nt_mostlycloudy100},
    {"04d", &cloudy100},
    {"04n", &nt_cloudy100},
    {"09d", &chancerain100},
    {"09n", &nt_chancerain100},
    {"10d", &rain100},
    {"10n", &nt_rain100},
    {"11d", &tstorms100},
    {"11n", &nt_tstorms100},
    {"13d", &snow100},
    {"13n", &nt_snow100},
    {"50d", &fog100},
    {"50n", &nt_fog100}
}};


//******************************************************************************
//! curConditionsViewT() -- Constructor
//
//!
//!
//******************************************************************************
curConditionsViewT::curConditionsViewT() :
    scrM(nullptr)
{

} // end curConditionsViewT() - Constructor


//******************************************************************************
//! Init()
//
//!
//******************************************************************************
void curConditionsViewT::Init(lv_obj_t * scrIP)
{
    scrM = scrIP;

    // Container for this view
    conAllM = lv_cont_create(scrM, NULL);
    lv_cont_set_layout(conAllM, LV_LAYOUT_COLUMN_MID);
    lv_cont_set_fit(conAllM, LV_FIT_MAX);

    // Container for date time at top
    conTopM = lv_cont_create(conAllM, NULL);
    lv_cont_set_layout(conTopM, LV_LAYOUT_COLUMN_MID);
    lv_cont_set_fit2(conTopM, LV_FIT_MAX, LV_FIT_TIGHT);

    // Label to hold date and time
    lblDateTimeM = lv_label_create(conTopM, NULL);
    lv_label_set_text(lblDateTimeM, "");


    // Container in the middle to hold the current conditions
    conMidM = lv_cont_create(conAllM, NULL);
    lv_cont_set_layout(conMidM, LV_LAYOUT_ROW_MID);
    lv_cont_set_fit2(conMidM, LV_FIT_TIGHT, LV_FIT_TIGHT);

    // Container holding the current temperature and weather description
    // Layout the children in this container in a centered column
    // Don't change width automatically but adjust height to minimum
    // Set width to 176 which is (480 - 128) / 2
    conTempM = lv_cont_create(conMidM, NULL);
    lv_cont_set_layout(conTempM, LV_LAYOUT_COLUMN_MID);
    lv_cont_set_fit2(conTempM, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(conTempM, 176);


    lblTempM = lv_label_create(conTempM, NULL);
    lv_label_set_text(lblTempM, "");
    lblDescM = lv_label_create(conTempM, NULL);
    lv_label_set_text(lblDescM, "");
    // Wrap label to next line if too long
    lv_label_set_long_mode(lblDescM, LV_LABEL_LONG_BREAK);
    // Must set width after setting the long mode
    // otherwise, long mode overrides the size
    lv_obj_set_width(lblDescM, 176);
    lv_label_set_align(lblDescM, LV_LABEL_ALIGN_CENTER);

    imgCurWeatherM = lv_img_create(conMidM, NULL);
    lv_img_set_src(imgCurWeatherM, &unknown128);

    conCondsM = lv_cont_create(conMidM, NULL);
    lv_cont_set_layout(conCondsM, LV_LAYOUT_COLUMN_RIGHT);
    lv_cont_set_fit2(conCondsM, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(conCondsM, 176);

    lblPressM = lv_label_create(conCondsM, NULL);
    lv_label_set_text(lblPressM, "Pressure: ");
    lblHumidM = lv_label_create(conCondsM, NULL);
    lv_label_set_text(lblHumidM, "Humidity: ");
    lblCloudsM = lv_label_create(conCondsM, NULL);
    lv_label_set_text(lblCloudsM, "Clouds: ");
    lblWindSpeedM = lv_label_create(conCondsM, NULL);
    lv_label_set_text(lblWindSpeedM, "Wind: ");


    // Bottom container to hold forecast widgets
    conBotM = lv_cont_create(conAllM, NULL);
    lv_cont_set_layout(conBotM, LV_LAYOUT_ROW_MID);
    lv_cont_set_fit2(conBotM, LV_FIT_TIGHT, LV_FIT_TIGHT);

    // Create forecast widgets
    fcastsMA[0] = new forecastWidgetT(conBotM);
    fcastsMA[1] = new forecastWidgetT(conBotM);
    fcastsMA[2] = new forecastWidgetT(conBotM);
    fcastsMA[3] = new forecastWidgetT(conBotM);


    // Create screen style
    lv_style_init(&styleM);
    lv_style_set_bg_color(&styleM, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_color(&styleM, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&styleM, LV_STATE_DEFAULT, &lv_font_montserrat_20);
    lv_style_set_pad_inner(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_left(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_margin_top(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_margin_bottom(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_margin_left(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_margin_right(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_border_width(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_border_color(&styleM, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_style_set_outline_width(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_radius(&styleM, LV_STATE_DEFAULT, 0);

    LV_FONT_DECLARE(droid_mono_28);

    lv_style_init(&styleDtM);
    lv_style_copy(&styleDtM, &styleM);
    //lv_style_set_text_font(&styleDtM, LV_STATE_DEFAULT, &lv_font_montserrat_28);
    lv_style_set_text_font(&styleDtM, LV_STATE_DEFAULT, &droid_mono_28);
    lv_obj_add_style(lblDateTimeM, LV_STATE_DEFAULT, &styleDtM);
    lv_obj_add_style(lblTempM, LV_STATE_DEFAULT, &styleDtM);


    // Replace existing style and refresh it
    lv_obj_add_style(scrM, LV_OBJ_PART_MAIN, &styleM);
    lv_obj_refresh_style(scrM, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);

    lv_obj_add_style(conAllM, LV_CONT_PART_MAIN, &styleM);
    lv_obj_refresh_style(conAllM, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);

    lv_obj_add_style(conTopM, LV_CONT_PART_MAIN, &styleM);
    lv_obj_refresh_style(conTopM, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);

    lv_obj_add_style(conMidM, LV_CONT_PART_MAIN, &styleM);
    lv_obj_refresh_style(conMidM, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);

    lv_obj_add_style(conTempM, LV_CONT_PART_MAIN, &styleM);
    lv_obj_refresh_style(conTempM, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);

    lv_obj_add_style(conCondsM, LV_CONT_PART_MAIN, &styleM);
    lv_obj_refresh_style(conCondsM, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);

    lv_obj_add_style(conBotM, LV_CONT_PART_MAIN, &styleM);
    lv_obj_refresh_style(conBotM, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);

    UpdateDateTime();

} // end Init()


//******************************************************************************
//! UpdateDateTime()
//
//!
//******************************************************************************
void curConditionsViewT::UpdateDateTime()
{
    time_t rawtime;
    time(&rawtime);
    //lv_coord_t h = lv_obj_get_height(lblDateTimeM);
    //lv_label_set_text_fmt(lblDateTimeM, "%d", h);
    //lv_label_set_text_fmt(lblDateTimeM, "%s %d", ctime(&rawtime), h);

    char buffer [80] = {'\0'};
    struct tm * timeInfo = localtime(&rawtime);
    strftime(&buffer[0], 80, "%a %b %d %I:%M:%S %p", timeInfo);
    lv_label_set_text_fmt(lblDateTimeM, "%s", buffer);

} // end UpdateDateTime()


//******************************************************************************
//! UpdateImgSrc()
//
//!
//******************************************************************************
void curConditionsViewT::UpdateImgSrc()
{
    lv_img_set_src(imgCurWeatherM, &unknown128);

} // end UpdateImgSrc()


//******************************************************************************
//! UpdateCurrentWeather()
//
//!
//******************************************************************************
void curConditionsViewT::UpdateCurrentWeather(const weatherT & weatherIRC)
{

    lv_label_set_text_fmt(lblTempM, "%.1f°", weatherIRC.temp);
    lv_label_set_text_fmt(lblDescM, "%s", weatherIRC.desc);
    lv_label_set_text_fmt(lblPressM,       "Pressure: %6d ", weatherIRC.pressure);
    lv_label_set_text_fmt(lblHumidM,       "Humidity: %6d%%", weatherIRC.humidity);
    lv_label_set_text_fmt(lblCloudsM,        "Clouds: %6d%%", weatherIRC.clouds);
    lv_label_set_text_fmt(lblWindSpeedM,       "Wind: %6.1f", weatherIRC.windSpeed);

    for( const auto & pair : iconMap128LC )
    {
        if( strncmp(pair.first, weatherIRC.icon, 3) == 0 )
        {
            lv_img_set_src(imgCurWeatherM, pair.second);
            break;
        }
    }

/*     printf("Scr width %d\n", lv_obj_get_width_margin(scrM));
    printf("Mid width %d\n", lv_obj_get_width_margin(conMidM));
    printf("Temp width %d\n", lv_obj_get_width_margin(conTempM));
    printf("Img width %d\n", lv_obj_get_width_margin(imgCurWeatherM));
    printf("Cond width %d\n", lv_obj_get_width_margin(conCondsM));
    printf("Bottom width %d\n", lv_obj_get_width_margin(conBotM)); */

} // end UpdateCurrentWeather()


//******************************************************************************
//! UpdateForecasts()
//
//!
//******************************************************************************
void curConditionsViewT::UpdateForecasts(const std::vector<dailyT> & dailyIRAC)
{
    for( int idX = 0; idX < fcastsMA.size() && idX < dailyIRAC.size(); idX++ )
    {
        forecastWidgetT * fcastP = fcastsMA.at(idX);
        fcastP->Update(dailyIRAC.at(idX));
        for( const auto & pair : iconMap100LC )
        {
            if( strncmp(pair.first, dailyIRAC.at(idX).icon, 3) == 0 )
            {
                fcastP->SetImgSrc(pair.second);
                break;
            }
        }
    }

} // end UpdateForecasts()
