//******************************************************************************
//! \file forecastWidget.cpp
//!
//!
//!
//!
//******************************************************************************

#include "icons/icons100.h"

#include "forecastWidget.hpp"


//******************************************************************************
//! forecastWidgetT() -- Constructor
//
//!
//!
//******************************************************************************
forecastWidgetT::forecastWidgetT(lv_obj_t * parentIP) :
    parentMP(parentIP)
{

    // Create the container to hold the forecast
    conM = lv_cont_create(parentMP, NULL);
    lv_cont_set_layout(conM, LV_LAYOUT_COLUMN_MID);
    lv_cont_set_fit2(conM, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(conM, 120);

    // Label at the top
    lblDayM = lv_label_create(conM, NULL);
    lv_label_set_text(lblDayM, "");

    // Image in the middle
    imgM = lv_img_create(conM, NULL);
    lv_img_set_src(imgM, &unknown100);

    // Low/high temp at bottom with precip chance
    lblTempM = lv_label_create(conM, NULL);
    lv_label_set_text(lblTempM, "? / ?");

    lblPercipM = lv_label_create(conM, NULL);
    lv_label_set_text(lblPercipM, "?%");


    // Create style for container
    lv_style_init(&styleM);
    lv_style_set_bg_color(&styleM, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_color(&styleM, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_text_font(&styleM, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    lv_style_set_pad_inner(&styleM, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_top(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_left(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_border_width(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_outline_width(&styleM, LV_STATE_DEFAULT, 0);
    lv_style_set_outline_color(&styleM, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_style_set_radius(&styleM, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(conM, LV_CONT_PART_MAIN, &styleM);
    lv_obj_refresh_style(conM, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);


    time_t now;
    time(&now);
    SetDate(now);

} // end forecastWidgetT() - Constructor


//******************************************************************************
//! SetDate()
//
//!
//
//!
//******************************************************************************
void forecastWidgetT::SetDate(const time_t & dtIRC)
{
    char buffer [80] = {'\0'};
    struct tm * timeInfo = localtime(&dtIRC);
    strftime(&buffer[0], 80, "%A", timeInfo);
    lv_label_set_text_fmt(lblDayM, "%s", buffer);

} // end SetDate()


//******************************************************************************
//! Update()
//
//!
//******************************************************************************
void forecastWidgetT::Update(const dailyT & dailyIRC)
{
    SetDate(dailyIRC.dt);
    lv_label_set_text_fmt(lblTempM, "%.0f / %.0f", dailyIRC.tempMin, dailyIRC.tempMax);
    lv_label_set_text_fmt(lblPercipM, "%.0f%% / %.1f", dailyIRC.pop*100, dailyIRC.rain / 25.4);

} // end Update()


//******************************************************************************
//! SetImgSrc()
//
//!
//******************************************************************************
void forecastWidgetT::SetImgSrc(const lv_img_dsc_t * imgSrcIP)
{
    if( imgSrcIP != nullptr )
    {
        lv_img_set_src(imgM, imgSrcIP);
    }
    else
    {
        lv_img_set_src(imgM, &unknown100);
    }

} // end SetImgSrc()
