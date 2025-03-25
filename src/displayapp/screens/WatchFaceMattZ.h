#pragma once

#include <lvgl/src/lv_core/lv_obj.h>
#include <chrono>
#include <cstdint>
#include <memory>
#include "displayapp/screens/Screen.h"
#include "components/datetime/DateTimeController.h"
#include "components/ble/SimpleWeatherService.h"
#include "components/ble/BleController.h"
#include "displayapp/widgets/StatusIcons.h"
#include "utility/DirtyValue.h"
#include "displayapp/apps/Apps.h"

namespace Pinetime {
  namespace Controllers {
    class Settings;
    class Battery;
    class Ble;
    class AlarmController;
    class NotificationManager;
    class HeartRateController;
    class MotionController;
  }

  namespace Style {
    static lv_style_t style_arc_bg;
    static lv_style_t style_arc_indic;
  }

  namespace Applications {
    namespace Screens {

      class WatchFaceMattZ : public Screen {
      public:
        WatchFaceMattZ(Controllers::DateTime& dateTimeController,
                         const Controllers::Battery& batteryController,
                         const Controllers::Ble& bleController,
                         const Controllers::AlarmController& alarmController,
                         Controllers::NotificationManager& notificationManager,
                         Controllers::Settings& settingsController,
                         Controllers::HeartRateController& heartRateController,
                         Controllers::MotionController& motionController,
                         Controllers::SimpleWeatherService& weather);
        ~WatchFaceMattZ() override;

        void Refresh() override;

      private:
        uint8_t displayedHour = -1;
        uint8_t displayedMinute = -1;

        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>> currentDateTime {};
        //Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>> currentDateSecond {};
        Utility::DirtyValue<uint32_t> stepCount {};
        Utility::DirtyValue<uint8_t> heartbeat {};
        Utility::DirtyValue<bool> heartbeatRunning {};
        Utility::DirtyValue<bool> notificationState {};
        Utility::DirtyValue<std::optional<Pinetime::Controllers::SimpleWeatherService::CurrentWeather>> currentWeather {};

        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::days>> currentDate;

        //bool arcDirection = false;

        lv_obj_t* label_time;
        lv_obj_t* label_time_ampm;
        lv_obj_t* label_day;
        lv_obj_t* label_date;
        lv_obj_t* label_dots;
        lv_obj_t* heartbeatIcon;
        lv_obj_t* heartbeatValue;
        lv_obj_t* stepIcon;
        lv_obj_t* stepValue;
        lv_obj_t* notificationIcon;
        lv_obj_t* weatherIcon;
        lv_obj_t* temperature;
        //v_obj_t* secondsArc;

        Controllers::DateTime& dateTimeController;
        Controllers::NotificationManager& notificationManager;
        Controllers::Settings& settingsController;
        Controllers::HeartRateController& heartRateController;
        Controllers::MotionController& motionController;
        Controllers::SimpleWeatherService& weatherService;

        lv_task_t* taskRefresh;
        Widgets::StatusIcons statusIcons;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::MattZ> {
      static constexpr WatchFace watchFace = WatchFace::MattZ;
      static constexpr const char* name = "Matt Z";

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::WatchFaceMattZ(controllers.dateTimeController,
                                             controllers.batteryController,
                                             controllers.bleController,
                                             controllers.alarmController,
                                             controllers.notificationManager,
                                             controllers.settingsController,
                                             controllers.heartRateController,
                                             controllers.motionController,
                                             *controllers.weatherController);
      };

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}
