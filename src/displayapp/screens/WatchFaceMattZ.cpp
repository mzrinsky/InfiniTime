#include "displayapp/screens/WatchFaceMattZ.h"

#include <lvgl/lvgl.h>
#include <cstdio>

#include "displayapp/InfiniTimeTheme.h"
#include "displayapp/screens/NotificationIcon.h"
#include "displayapp/screens/Symbols.h"
#include "displayapp/screens/WeatherSymbols.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"
#include "components/ble/SimpleWeatherService.h"
#include "components/settings/Settings.h"

using namespace Pinetime::Applications::Screens;

WatchFaceMattZ::WatchFaceMattZ(Controllers::DateTime& dateTimeController,
                                   const Controllers::Battery& batteryController,
                                   const Controllers::Ble& bleController,
                                   const Controllers::AlarmController& alarmController,
                                   Controllers::NotificationManager& notificationManager,
                                   Controllers::Settings& settingsController,
                                   Controllers::HeartRateController& heartRateController,
                                   Controllers::MotionController& motionController,
                                   Controllers::SimpleWeatherService& weatherService)
  : currentDateTime {{}},
    dateTimeController {dateTimeController},
    notificationManager {notificationManager},
    settingsController {settingsController},
    heartRateController {heartRateController},
    motionController {motionController},
    weatherService {weatherService},
    statusIcons(batteryController, bleController, alarmController) {

  // lv_style_init(&Pinetime::Style::style_arc_indic);
  // lv_style_set_line_color(&Pinetime::Style::style_arc_indic, LV_STATE_DEFAULT, Colors::pink);
  // lv_style_set_line_width(&Pinetime::Style::style_arc_indic, LV_STATE_DEFAULT, LV_DPX(8));
  // lv_style_set_line_rounded(&Pinetime::Style::style_arc_indic, LV_STATE_DEFAULT, true);

  // lv_style_init(&Pinetime::Style::style_arc_bg);
  // lv_style_set_line_color(&Pinetime::Style::style_arc_bg, LV_STATE_DEFAULT, Colors::blue);
  // lv_style_set_line_width(&Pinetime::Style::style_arc_bg, LV_STATE_DEFAULT, LV_DPX(8));
  // lv_style_set_line_rounded(&Pinetime::Style::style_arc_bg, LV_STATE_DEFAULT, true);
  // lv_style_set_pad_all(&Pinetime::Style::style_arc_bg, LV_STATE_DEFAULT, LV_DPX(5));


  statusIcons.Create();

  //lv_obj_t* iconContainer = statusIcons.GetObject();

  notificationIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(notificationIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::white);
  lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(false));
  //lv_label_set_text(notificationIcon, NotificationIcon::GetIcon(true));
  lv_obj_align(notificationIcon, nullptr, LV_ALIGN_IN_TOP_RIGHT, -55, 0);
  //lv_coord_t w = lv_obj_get_width_fit(weatherIcon);
  //lv_obj_align(temperature, nullptr, LV_ALIGN_IN_TOP_LEFT, w, 0);

  weatherIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(weatherIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::white);
  lv_obj_set_style_local_text_font(weatherIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &fontawesome_weathericons);
  lv_label_set_text(weatherIcon, "");
  lv_obj_align(weatherIcon, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);
  lv_obj_set_auto_realign(weatherIcon, true);

  temperature = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(temperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::white);
  lv_label_set_text(temperature, "");
  lv_coord_t w = lv_obj_get_width_fit(weatherIcon);
  lv_obj_align(temperature, nullptr, LV_ALIGN_IN_TOP_LEFT, w, 0);

  label_day = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(label_day, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Fonts::small);
  lv_obj_set_style_local_text_color(label_day, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::pink);
  lv_obj_align(label_day, lv_scr_act(), LV_ALIGN_CENTER, 0, -80);

  label_date = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_align(label_date, lv_scr_act(), LV_ALIGN_CENTER, 0, -50);
  lv_obj_set_style_local_text_color(label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::blue);

  label_time = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Fonts::xlarge);
  lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);

  label_dots = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(label_dots, ":");
  lv_obj_set_style_local_text_font(label_dots, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Fonts::xlarge);
  lv_obj_align(label_dots, lv_scr_act(), LV_ALIGN_CENTER, 0, -2);

  label_time_ampm = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(label_time_ampm, "AM");
  lv_obj_set_style_local_text_font(label_time_ampm, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Fonts::normal);
  lv_obj_set_style_local_text_color(label_time_ampm, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::pink);
  lv_obj_align(label_time_ampm, lv_scr_act(), LV_ALIGN_CENTER, 0, 56);

  heartbeatIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(heartbeatIcon, Symbols::heartBeat);
  lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::icon);
  lv_obj_align(heartbeatIcon, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

  heartbeatValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::icon);
  lv_label_set_text_static(heartbeatValue, "");
  lv_obj_align(heartbeatValue, heartbeatIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  stepValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::icon);
  lv_label_set_text_static(stepValue, "0");
  lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);

  stepIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::icon);
  lv_label_set_text_static(stepIcon, Symbols::shoe);
  lv_obj_align(stepIcon, stepValue, LV_ALIGN_OUT_LEFT_MID, -5, 0);

  // secondsArc = lv_arc_create(lv_scr_act(), nullptr);
  // lv_arc_set_adjustable(secondsArc, false);
  // lv_arc_set_bg_angles(secondsArc, 0, 360);
  // lv_arc_set_rotation(secondsArc, 270);
  // lv_arc_set_range(secondsArc, 0, 60);
  // lv_arc_set_value(secondsArc, 0);
  // lv_obj_set_size(secondsArc, 220, 220);
  // lv_obj_align(secondsArc, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);

  // lv_style_list_t* list;
  
  // lv_obj_clean_style_list(secondsArc, LV_ARC_PART_BG);
  // list = lv_obj_get_style_list(secondsArc, LV_ARC_PART_BG);
  // _lv_style_list_add_style(list, &Pinetime::Style::style_arc_bg);

  // lv_obj_clean_style_list(secondsArc, LV_ARC_PART_INDIC);
  // list = lv_obj_get_style_list(secondsArc, LV_ARC_PART_INDIC);
  // _lv_style_list_add_style(list, &Pinetime::Style::style_arc_indic);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceMattZ::~WatchFaceMattZ() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void WatchFaceMattZ::Refresh() {
  statusIcons.Update();

  notificationState = notificationManager.AreNewNotificationsAvailable();
  if (notificationState.IsUpdated()) {
    //lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(notificationState.Get()));
    lv_label_set_text(notificationIcon, NotificationIcon::GetIcon(notificationState.Get()));
  }

  // currentDateSecond = std::chrono::time_point_cast<std::chrono::seconds>(dateTimeController.CurrentDateTime());

  // if (currentDateSecond.IsUpdated()) {
  //   uint8_t sec = dateTimeController.Seconds();
  //   lv_arc_set_value(secondsArc, sec);
  //   //lv_arc_set_rotation(seconds_arc, 270);
  //   lv_obj_realign(secondsArc);
  // }

  currentDateTime = std::chrono::time_point_cast<std::chrono::minutes>(dateTimeController.CurrentDateTime());

  if (currentDateTime.IsUpdated()) {

    // if (arcDirection) {
    //   arcDirection = false;
    //   lv_style_reset(&Pinetime::Style::style_arc_indic);
    //   lv_style_set_line_color(&Pinetime::Style::style_arc_indic, LV_STATE_DEFAULT, Colors::blue);
    //   lv_style_set_line_width(&Pinetime::Style::style_arc_indic, LV_STATE_DEFAULT, LV_DPX(8));
    //   lv_style_set_line_rounded(&Pinetime::Style::style_arc_indic, LV_STATE_DEFAULT, true);
    
    //   lv_style_reset(&Pinetime::Style::style_arc_bg);
    //   lv_style_set_line_color(&Pinetime::Style::style_arc_bg, LV_STATE_DEFAULT, Colors::pink);
    //   lv_style_set_line_width(&Pinetime::Style::style_arc_bg, LV_STATE_DEFAULT, LV_DPX(8));
    //   lv_style_set_line_rounded(&Pinetime::Style::style_arc_bg, LV_STATE_DEFAULT, true);
    //   lv_style_set_pad_all(&Pinetime::Style::style_arc_bg, LV_STATE_DEFAULT, LV_DPX(5));
    // } else {
    //   arcDirection = true;
    //   lv_style_init(&Pinetime::Style::style_arc_indic);
    //   lv_style_set_line_color(&Pinetime::Style::style_arc_indic, LV_STATE_DEFAULT, Colors::pink);
    //   lv_style_set_line_width(&Pinetime::Style::style_arc_indic, LV_STATE_DEFAULT, LV_DPX(8));
    //   lv_style_set_line_rounded(&Pinetime::Style::style_arc_indic, LV_STATE_DEFAULT, true);
    
    //   lv_style_init(&Pinetime::Style::style_arc_bg);
    //   lv_style_set_line_color(&Pinetime::Style::style_arc_bg, LV_STATE_DEFAULT, Colors::blue);
    //   lv_style_set_line_width(&Pinetime::Style::style_arc_bg, LV_STATE_DEFAULT, LV_DPX(8));
    //   lv_style_set_line_rounded(&Pinetime::Style::style_arc_bg, LV_STATE_DEFAULT, true);
    //   lv_style_set_pad_all(&Pinetime::Style::style_arc_bg, LV_STATE_DEFAULT, LV_DPX(5));
    // }
    // lv_obj_invalidate(secondsArc);

    uint8_t hour = dateTimeController.Hours();
    uint8_t minute = dateTimeController.Minutes();

    if (settingsController.GetClockType() == Controllers::Settings::ClockType::H12) {
      char ampmChar[3] = "AM";
      if (hour == 0) {
        hour = 12;
      } else if (hour == 12) {
        ampmChar[0] = 'P';
      } else if (hour > 12) {
        hour = hour - 12;
        ampmChar[0] = 'P';
      }
      lv_label_set_text(label_time_ampm, ampmChar);
      lv_label_set_text_fmt(label_time, "%02d %02d", hour, minute);
      lv_obj_realign(label_time);
      lv_obj_realign(label_time_ampm);
    } else {
      lv_label_set_text_fmt(label_time, "%02d %02d", hour, minute);
      lv_obj_realign(label_time);
    }

    currentDate = std::chrono::time_point_cast<std::chrono::days>(currentDateTime.Get());
    if (currentDate.IsUpdated()) {
      uint16_t year = dateTimeController.Year() % 100;
      uint8_t day = dateTimeController.Day();
      if (settingsController.GetClockType() == Controllers::Settings::ClockType::H24) {
        lv_label_set_text_fmt(label_date,
                              "%s %d %s %d",
                              dateTimeController.DayOfWeekShortToString(),
                              day,
                              dateTimeController.MonthShortToString(),
                              year);
      } else {
        lv_label_set_text_fmt(label_day, "%s", dateTimeController.DayOfWeekShortToString());
        lv_label_set_text_fmt(label_date,
                              "%02d %02d %02d",
                              year, 
                              dateTimeController.Month(),
                              day);
      }
      lv_obj_realign(label_day);
      lv_obj_realign(label_date);
    }
  }

  heartbeat = heartRateController.HeartRate();
  heartbeatRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
      lv_label_set_text_fmt(heartbeatValue, "%d", heartbeat.Get());
    } else {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x1B1B1B));
      lv_label_set_text_static(heartbeatValue, "");
    }

    lv_obj_realign(heartbeatIcon);
    lv_obj_realign(heartbeatValue);
  }

  stepCount = motionController.NbSteps();
  if (stepCount.IsUpdated()) {
    lv_label_set_text_fmt(stepValue, "%lu", stepCount.Get());
    lv_obj_realign(stepValue);
    lv_obj_realign(stepIcon);
  }

  currentWeather = weatherService.Current();
  if (currentWeather.IsUpdated()) {
    auto optCurrentWeather = currentWeather.Get();
    if (optCurrentWeather) {
      int16_t temp = optCurrentWeather->temperature.Celsius();
      char tempUnit = 'C';
      if (settingsController.GetWeatherFormat() == Controllers::Settings::WeatherFormat::Imperial) {
        temp = optCurrentWeather->temperature.Fahrenheit();
        tempUnit = 'F';
      }
      lv_label_set_text_fmt(temperature, "%d°%c", temp, tempUnit);
      lv_label_set_text(weatherIcon, Symbols::GetSymbol(optCurrentWeather->iconId));
    } else {
      lv_label_set_text_static(temperature, "");
      lv_label_set_text(weatherIcon, "");
    }    
    //lv_obj_realign(temperature);
    lv_obj_realign(weatherIcon);
    lv_coord_t w = lv_obj_get_width_fit(weatherIcon);
    lv_obj_align(temperature, nullptr, LV_ALIGN_IN_TOP_LEFT, w, 0);
  }
}
